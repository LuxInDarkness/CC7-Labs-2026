#ifndef processor_h
#define processor_h

#include "../ts_objects/ts_queue.h"
#include "../ts_objects/ts_results_holder.h"
#include "../libraries/log_parser.h"

typedef struct {
    LineQueue *queue;
    Results   *results;
} ThreadArgs;

#endif /* processor_h */