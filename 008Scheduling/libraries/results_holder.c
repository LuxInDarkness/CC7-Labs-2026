#include "results_holder.h"

void results_init(Results *r) {
    r->waiting_time = 0;
    r->turnaround_time = 0;
    r->average_waiting_time = 0.0;
    r->average_turnaround_time = 0.0;
    r->updates_count = 0;
    pthread_mutex_init(&r->mutex, NULL);
}

void results_update(Results *r, float wait_time, float turnaround_time) {
    pthread_mutex_lock(&r->mutex);
    
    r->waiting_time += wait_time;
    r->turnaround_time += turnaround_time;

    r->updates_count++;

    r->average_waiting_time = r->waiting_time / r->updates_count;
    r->average_turnaround_time = r->turnaround_time / r->updates_count;

    pthread_mutex_unlock(&r->mutex);
}

void results_destroy(Results *r) {
    pthread_mutex_destroy(&r->mutex);
}