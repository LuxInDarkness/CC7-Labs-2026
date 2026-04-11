#ifndef RESULTS_HOLDER_H
#define RESULTS_HOLDER_H

#include <pthread.h>

typedef struct Results {
    int students_count;
    float total_wait_time;
    pthread_mutex_t mutex;
} Results;

void results_init(Results *r);
void results_update(Results *r, float wait_time);
void results_destroy(Results *r);

#endif // RESULTS_HOLDER_H