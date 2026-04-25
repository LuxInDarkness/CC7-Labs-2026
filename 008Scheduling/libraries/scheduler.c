#include "scheduler.h"

static Thread threads[MAX_THREADS];
static enum SchedulerAlgorithm current_algorithm;
static int g_time_quantum;
static int count_threads;

void init_scheduler(int num_threads, enum SchedulerAlgorithm algorithm,
                    int time_quantum) {
    current_algorithm = algorithm;
    g_time_quantum    = time_quantum;
    count_threads     = 0;
}

void reset_scheduler(void) {
    count_threads = 0;
}

void add_thread(Thread thread) {
    threads[count_threads++] = thread;
}

/* ── Comparison helpers ──────────────────────────────────────────────── */

static int cmp_arrival(const void *a, const void *b) {
    return ((Thread *)a)->arrival_time - ((Thread *)b)->arrival_time;
}

/* ── FIFO ────────────────────────────────────────────────────────────── */

void fifo_scheduler(Results *res) {
    qsort(threads, count_threads, sizeof(Thread), cmp_arrival);

    int current_time = 0;
    for (int i = 0; i < count_threads; i++) {
        Thread *t = &threads[i];
        if (current_time < t->arrival_time)
            current_time = t->arrival_time;

        float wait       = (float)(current_time - t->arrival_time);
        float turnaround = wait + (float)t->burst_time;

        current_time = thread_run_full(t, current_time);
        results_update(res, wait, turnaround);
    }
}

/* ── Round Robin ─────────────────────────────────────────────────────── */

void rr_scheduler(Results *res) {
    Thread local[MAX_THREADS];
    memcpy(local, threads, count_threads * sizeof(Thread));
    qsort(local, count_threads, sizeof(Thread), cmp_arrival);

    float wait_acc[MAX_THREADS];
    memset(wait_acc, 0, sizeof(wait_acc));

    int queue[MAX_THREADS * 200];
    int head = 0, tail = 0;
    int current_time  = local[0].arrival_time;
    int next_arrival  = 0;
    int done          = 0;

    /* Seed queue with all threads already present at time 0 */
    while (next_arrival < count_threads &&
           local[next_arrival].arrival_time <= current_time) {
        queue[tail++] = next_arrival++;
    }

    while (done < count_threads) {
        /* CPU idle — jump forward */
        if (head == tail) {
            current_time = local[next_arrival].arrival_time;
            while (next_arrival < count_threads &&
                   local[next_arrival].arrival_time <= current_time) {
                queue[tail++] = next_arrival++;
            }
        }

        int     idx = queue[head++];
        Thread *t   = &local[idx];

        current_time = thread_run_slice(t, current_time, g_time_quantum,
                                        &wait_acc[idx]);

        /* Enqueue threads that arrived during this slice */
        while (next_arrival < count_threads &&
               local[next_arrival].arrival_time <= current_time) {
            queue[tail++] = next_arrival++;
        }

        if (t->state == FINISHED) {
            float turnaround = (float)(current_time - t->arrival_time);
            results_update(res, wait_acc[idx], turnaround);
            done++;
        } else {
            queue[tail++] = idx;   /* re-queue for next slice */
        }
    }
}

/* ── SJF (non-preemptive) ────────────────────────────────────────────── */

void sjf_scheduler(Results *res) {
    Thread local[MAX_THREADS];
    memcpy(local, threads, count_threads * sizeof(Thread));

    int done_flags[MAX_THREADS];
    memset(done_flags, 0, sizeof(done_flags));

    int current_time = 0;
    for (int done = 0; done < count_threads; done++) {
        /* Find the arrived, undone thread with the shortest burst */
        int best = -1;
        for (int i = 0; i < count_threads; i++) {
            if (done_flags[i]) continue;
            if (local[i].arrival_time > current_time) continue;
            if (best == -1 || local[i].burst_time < local[best].burst_time)
                best = i;
        }
        if (best == -1) {
            /* CPU idle — advance to earliest pending thread */
            int next = -1;
            for (int i = 0; i < count_threads; i++) {
                if (!done_flags[i] &&
                    (next == -1 || local[i].arrival_time < local[next].arrival_time))
                    next = i;
            }
            current_time = local[next].arrival_time;
            best = next;
        }

        Thread *t    = &local[best];
        float wait       = (float)(current_time - t->arrival_time);
        float turnaround = wait + (float)t->burst_time;

        current_time = thread_run_full(t, current_time);
        results_update(res, wait, turnaround);
        done_flags[best] = 1;
    }
}

/* ── SRTF (preemptive SJF) ───────────────────────────────────────────── */

void srtf_scheduler(Results *res) {
    Thread local[MAX_THREADS];
    memcpy(local, threads, count_threads * sizeof(Thread));
    qsort(local, count_threads, sizeof(Thread), cmp_arrival);

    int current_time = local[0].arrival_time;
    int done         = 0;
    int prev         = -1;   /* index of thread that ran in the last interval */

    while (done < count_threads) {
        /* Pick arrived thread with shortest remaining time */
        int best = -1;
        for (int i = 0; i < count_threads; i++) {
            if (local[i].state == FINISHED) continue;
            if (local[i].arrival_time > current_time) continue;
            if (best == -1 ||
                local[i].remaining_time < local[best].remaining_time)
                best = i;
        }
        if (best == -1) {
            /* CPU idle — jump to next arrival */
            for (int i = 0; i < count_threads; i++) {
                if (local[i].state != FINISHED &&
                    (best == -1 ||
                     local[i].arrival_time < local[best].arrival_time))
                    best = i;
            }
            current_time = local[best].arrival_time;
        }

        /* Context switch out */
        if (prev != -1 && prev != best && local[prev].state != FINISHED) {
            thread_log_preempted(&local[prev], current_time);
        }

        /* Find the next event: either this thread finishes or a new one arrives */
        int next_event = current_time + local[best].remaining_time;
        for (int i = 0; i < count_threads; i++) {
            if (local[i].state == FINISHED) continue;
            if (local[i].arrival_time > current_time &&
                local[i].arrival_time < next_event)
                next_event = local[i].arrival_time;
        }

        int is_new_run = (prev != best);
        thread_apply_interval(&local[best], current_time, next_event,
                              is_new_run, prev != -1 && prev != best);
        current_time = next_event;

        if (local[best].state == FINISHED) {
            float turnaround = (float)(current_time - local[best].arrival_time);
            float waited     = turnaround - (float)local[best].burst_time;
            results_update(res, waited, turnaround);
            done++;
            prev = -1;
        } else {
            prev = best;
        }
    }
}

void print_results(const char *algo_name, Results *res) {
    printf("\n=== %s Results ===\n", algo_name);
    printf("Avg Waiting Time:    %.2f seconds\n", res->average_waiting_time);
    printf("Avg Turnaround Time: %.2f seconds\n", res->average_turnaround_time);
}