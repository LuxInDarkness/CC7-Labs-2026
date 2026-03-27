#ifndef TS_QUEUE_H
#define TS_QUEUE_H

#define QUEUE_CAPACITY 100
#define MAX_LINE_LEN 100

#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>

typedef struct LineQueue {
    char lines[QUEUE_CAPACITY][MAX_LINE_LEN];
    int head;           // next slot to read
    int tail;           // next slot to write
    int count;
    int producer_count;  // number of producer threads still running
    bool done;          // set to true when no more lines will be added

    pthread_mutex_t mutex;
    pthread_cond_t not_empty;  // signals waiting threads that a line arrived
    pthread_cond_t not_full;
} LineQueue;

void queue_init(LineQueue *q);
void queue_push(LineQueue *q, char * log);
bool queue_pop(LineQueue *q, char * out_log);
void add_producer(LineQueue *q);
void remove_producer(LineQueue *q);
void mark_done(LineQueue *q);
void queue_destroy(LineQueue *q);

#endif // TS_QUEUE_H