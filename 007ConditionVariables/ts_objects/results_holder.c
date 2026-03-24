#include "results_holder.h"

void results_init(Results *r) {
    r->students_count = 0;
    r->total_wait_time = 0.0;
    pthread_mutex_init(&r->mutex, NULL);
}

void results_update(Results *r, float wait_time) {
    pthread_mutex_lock(&r->mutex);
    
    r->students_count++;
    r->total_wait_time += wait_time;

    pthread_mutex_unlock(&r->mutex);
}

void results_destroy(Results *r) {
    pthread_mutex_destroy(&r->mutex);
}