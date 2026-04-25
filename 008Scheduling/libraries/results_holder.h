#ifndef RESULTS_HOLDER_H
#define RESULTS_HOLDER_H

#include <pthread.h>

typedef struct Results {
    float waiting_time;
    float turnaround_time;
    int updates_count; // Number of times results have been updated
    float average_waiting_time;
    float average_turnaround_time;
    pthread_mutex_t mutex;
} Results;

void results_init(Results *r);
void results_update(Results *r, float wait_time, float turnaround_time);
void results_destroy(Results *r);

#endif // RESULTS_HOLDER_H