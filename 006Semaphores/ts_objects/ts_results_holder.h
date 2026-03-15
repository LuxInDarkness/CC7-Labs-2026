#ifndef TS_RESULTS_HOLDER_H
#define TS_RESULTS_HOLDER_H

#include <glib.h>

typedef struct Results {
    int32_t cars_parked;
    float total_wait_time;
    
    pthread_mutex_t mutex;
} Results;

void results_init(Results *r);
void results_update(Results *r, float time_waited);
void results_destroy(Results *r);

#endif // TS_RESULTS_HOLDER_H