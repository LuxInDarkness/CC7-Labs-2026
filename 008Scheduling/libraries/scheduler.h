#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thread.h"
#include "results_holder.h"

#define MAX_THREADS 15

enum SchedulerAlgorithm {
    FIFO,
    RR,
    SJF,
    SRTF
};

void init_scheduler (int num_threads, enum SchedulerAlgorithm algorithm,
                     int time_quantum);
void reset_scheduler(void);           /* clears thread list, keeps config */
void add_thread     (Thread thread);

void fifo_scheduler (Results *res);
void rr_scheduler   (Results *res);
void sjf_scheduler  (Results *res);
void srtf_scheduler (Results *res);

void print_results  (const char *algo_name, Results *res);

#endif // SCHEDULER_H