#include "thread.h"
#include <stdarg.h>
#include <string.h>

static void log_event(const char *fmt, ...) {
    char buf[256];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timebuf[64];
    strftime(timebuf, sizeof(timebuf), "%a %b %d %H:%M:%S %Y", tm_info);

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    char full[350];
    snprintf(full, sizeof(full), "[%s] %s", timebuf, buf);

    FILE *lf = open_log_file();
    log_message(lf, full);
    close_log_file(lf);
}

Thread create_thread(int id, int burst_time, int arrival_time) {
    Thread t;
    t.id             = id;
    t.burst_time     = burst_time;
    t.remaining_time = burst_time;
    t.arrival_time   = arrival_time;
    t.state          = PENDING;
    t.last_preempted_time = -1;
    return t;
}

void thread_set_ready(Thread *t) {
    t->state = READY;
}

void thread_set_running(Thread *t) {
    t->state = RUNNING;
}

void thread_set_finished(Thread *t) {
    t->state    = FINISHED;
    t->remaining_time = 0;
}

int thread_consume(Thread *t, int slice) {
    t->remaining_time -= slice;
    if (t->remaining_time < 0) t->remaining_time = 0;
    return t->remaining_time;
}

void thread_log_arrived(const Thread *t, int current_time) {
    log_event("Process %d (Burst %d): Arrived at time %d",
              t->id, t->burst_time, t->arrival_time);
}

void thread_log_started(const Thread *t, int current_time, float waited) {
    log_event("Process %d (Burst %d): Started at time %d (waited %.2f seconds)",
              t->id, t->burst_time, current_time, waited);
}

void thread_log_resumed(const Thread *t, int current_time) {
    log_event("Process %d (Burst %d remaining): Resumed at time %d",
              t->id, t->remaining_time, current_time);
}

void thread_log_preempted(const Thread *t, int current_time) {
    log_event("Process %d (Burst %d remaining): Preempted at time %d",
              t->id, t->remaining_time, current_time);
}

void thread_log_completed(const Thread *t, int current_time,
                          float waited, float turnaround) {
    log_event("Process %d (Burst %d): Completed at time %d "
              "(waited %.2f, turnaround %.2f)",
              t->id, t->burst_time, current_time, waited, turnaround);
}

/* ── FIFO / SJF execution ────────────────────────────────────────────── */

/*
 * Runs the thread fully from current_time.
 * Handles idle gap if CPU was free before thread arrived.
 * Logs: arrived → started → completed.
 * Returns the new current_time.
 */
int thread_run_full(Thread *t, int current_time) {
    if (current_time < t->arrival_time)
        current_time = t->arrival_time;

    float waited     = (float)(current_time - t->arrival_time);
    float turnaround = waited + (float)t->burst_time;

    thread_set_ready(t);
    thread_log_arrived(t, current_time);
    thread_set_running(t);
    thread_log_started(t, current_time, waited);

    current_time += t->burst_time;
    thread_consume(t, t->burst_time);   /* clears remaining_time to 0 */
    thread_set_finished(t);
    thread_log_completed(t, current_time, waited, turnaround);

    return current_time;
}

/* ── Round Robin execution ───────────────────────────────────────────── */

/*
 * Runs one quantum slice for Round Robin.
 * `wait_acc` is the caller-owned accumulator for this thread's total wait.
 * Logs first-run start vs. resume, and preempt vs. complete.
 * Returns the new current_time.
 */
int thread_run_slice(Thread *t, int current_time, int quantum,
                     float *wait_acc) {
    /*
     * Wait for this slice = time since the thread last left the CPU.
     * For the very first slice, that reference point is arrival_time.
     * This way, time the thread spent running is never counted as wait.
     */
    int reference    = (t->last_preempted_time >= 0)
                       ? t->last_preempted_time
                       : t->arrival_time;
    *wait_acc       += (float)(current_time - reference);

    int slice = (t->remaining_time < quantum) ? t->remaining_time : quantum;

    if (t->state == READY || t->state == PENDING) {
        thread_set_ready(t);
        thread_log_arrived(t, current_time);
        thread_set_running(t);
        thread_log_started(t, current_time, *wait_acc);
    } else {
        thread_log_resumed(t, current_time);
    }

    current_time += slice;
    thread_consume(t, slice);

    if (t->remaining_time > 0) {
        t->last_preempted_time = current_time;  /* record when it left CPU */
        thread_log_preempted(t, current_time);
    } else {
        float turnaround = (float)(current_time - t->arrival_time);
        thread_set_finished(t);
        thread_log_completed(t, current_time, *wait_acc, turnaround);
    }

    return current_time;
}

/* ── SRTF execution ──────────────────────────────────────────────────── */

/*
 * Applies a pre-computed interval [time_before, time_after] to the thread.
 *   is_new_run          — true when this thread just got the CPU (context switch in)
 *   was_preempted_before — true when the previous holder was preempted (context switch out)
 *
 * The scheduler drives time and picks which thread runs each interval;
 * this function handles only the state/logging for that thread.
 */
void thread_apply_interval(Thread *t, int time_before, int time_after,
                           int is_new_run, int was_preempted_before) {
    int elapsed = time_after - time_before;

    if (is_new_run) {
        if (t->state == PENDING || t->state == READY) {
            thread_set_ready(t);
            thread_log_arrived(t, time_before);
            thread_set_running(t);
            thread_log_started(t, time_before,
                               (float)(time_before - t->arrival_time));
        } else {
            /* Was preempted earlier — resuming */
            thread_log_resumed(t, time_before);
        }
    }

    thread_consume(t, elapsed);

    if (t->remaining_time == 0) {
        float turnaround = (float)(time_after - t->arrival_time);
        float waited     = turnaround - (float)t->burst_time;
        thread_set_finished(t);
        thread_log_completed(t, time_after, waited, turnaround);
    }
    /* If preempted, the scheduler will call thread_log_preempted separately
       once it has confirmed a different thread will run next. */
}