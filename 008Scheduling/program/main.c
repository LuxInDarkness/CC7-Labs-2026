#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "../libraries/thread.h"
#include "../libraries/scheduler.h"
#include "../libraries/results_holder.h"

#define MIN_THREADS      5
#define MAX_THREADS      15
#define MAX_BURST_TIME   10
#define MAX_ARRIVAL_TIME 100
#define TIME_QUANTUM     2

static void load_threads(Thread *src, int n) {
    reset_scheduler();
    for (int i = 0; i < n; i++)
        add_thread(src[i]);
}

int main(void) {
    srand((unsigned)time(NULL));

    /* Generate dataset */
    int n = rand() % (MAX_THREADS - MIN_THREADS + 1) + MIN_THREADS;
    Thread threads[MAX_THREADS];
    for (int i = 0; i < n; i++) {
        int bt = rand() % MAX_BURST_TIME   + 1;
        int at = rand() % (MAX_ARRIVAL_TIME + 1);
        threads[i] = create_thread(i, bt, at);
    }

    /* Print dataset summary */
    printf("Dataset: %d threads\n", n);
    printf("Burst Times:   [");
    for (int i = 0; i < n; i++)
        printf("%d%s", threads[i].burst_time, i < n - 1 ? ", " : "");
    printf("]\nArrival Times: [");
    for (int i = 0; i < n; i++)
        printf("%d%s", threads[i].arrival_time, i < n - 1 ? ", " : "");
    printf("]\n");

    Results res;

    /* FIFO */
    printf("\n--- FIFO Scheduling ---\n");
    results_init(&res);
    init_scheduler(n, FIFO, 0);
    load_threads(threads, n);
    fifo_scheduler(&res);
    print_results("FIFO", &res);
    results_destroy(&res);

    /* Round Robin */
    printf("\n--- Round Robin Scheduling (Quantum %d) ---\n", TIME_QUANTUM);
    results_init(&res);
    init_scheduler(n, RR, TIME_QUANTUM);
    load_threads(threads, n);
    rr_scheduler(&res);
    print_results("Round Robin", &res);
    results_destroy(&res);

    /* SJF */
    printf("\n--- SJF Scheduling ---\n");
    results_init(&res);
    init_scheduler(n, SJF, 0);
    load_threads(threads, n);
    sjf_scheduler(&res);
    print_results("SJF", &res);
    results_destroy(&res);

    /* SRTF */
    printf("\n--- SRTF Scheduling ---\n");
    results_init(&res);
    init_scheduler(n, SRTF, 0);
    load_threads(threads, n);
    srtf_scheduler(&res);
    print_results("SRTF", &res);
    results_destroy(&res);

    return 0;
}