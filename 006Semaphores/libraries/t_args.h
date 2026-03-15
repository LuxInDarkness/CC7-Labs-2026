#ifndef T_ARGS_H
#define T_ARGS_H

#include "../ts_objects/ts_queue.h"
#include "../ts_objects/ts_results_holder.h"
#include "../ts_objects/ts_semaphore.h"
#include "../ts_objects/ui_bridge.h"

typedef struct {
    LineQueue *queue;
    Results   *results;
    Semaphore *semaphore; // Only used for car threads
    UIEventQueue *ui_queue;   // Only used for logger thread
    int car_number; // Only used for car threads
} ThreadArgs;

#endif /* T_ARGS_H */