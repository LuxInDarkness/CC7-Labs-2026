#ifndef TS_QUEUE_H
#define TS_QUEUE_H

#define MAX_LINE_LEN 1024
#define QUEUE_CAPACITY 2048

#include <stdbool.h>

typedef struct {
    char lines[QUEUE_CAPACITY][MAX_LINE_LEN];
    int head;           // next slot to read
    int tail;           // next slot to write
    int count;
    bool done;          // set to true when no more lines will be added

    pthread_mutex_t mutex;
    pthread_cond_t  not_empty;  // signals waiting threads that a line arrived
} LineQueue;

void queue_init(LineQueue *q);
void queue_push(LineQueue *q, const char *line);
bool queue_pop(LineQueue *q, char *out_line);
void queue_mark_done(LineQueue *q);
void queue_destroy(LineQueue *q);

#endif // TS_QUEUE_H