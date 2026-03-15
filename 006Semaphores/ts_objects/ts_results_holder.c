#include <pthread.h>

#include "ts_results_holder.h"

void results_init(Results *r) {
    r->cars_parked = 0;
    r->total_wait_time = 0;
    pthread_mutex_init(&r->mutex, NULL);
}

void results_update(Results *r, float time_waited) {
    pthread_mutex_lock(&r->mutex);
   
    r->cars_parked += 1;
    r->total_wait_time += time_waited;

    pthread_mutex_unlock(&r->mutex);
}

void results_destroy(Results *r) {
    pthread_mutex_destroy(&r->mutex);
}