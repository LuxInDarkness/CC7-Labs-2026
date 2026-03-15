#include <stdlib.h>

#include "ts_queue.h"

void queue_init(LineQueue *q) {
    q->head = q->tail = q->count = 0;
    q->done = false;
    q->producer_count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_empty, NULL);
}

// Call when a producer thread starts (before pushing lines)
void add_producer(LineQueue *q) {
    pthread_mutex_lock(&q->mutex);
    q->producer_count++;
    pthread_mutex_unlock(&q->mutex);
}

// Called by producer thread — blocks if queue is full
void queue_push(LineQueue *q, ParkingEvent event) {
    pthread_mutex_lock(&q->mutex);

    while (q->count == QUEUE_CAPACITY)  // queue full, wait
        pthread_cond_wait(&q->not_empty, &q->mutex);

    q->events[q->tail] = event;
    q->tail = (q->tail + 1) % QUEUE_CAPACITY;
    q->count++;

    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}

// Called by consumer threads — blocks until an event is available
// Returns false when queue is empty AND done (time to exit)
bool queue_pop(LineQueue *q, ParkingEvent *out_event) {
    pthread_mutex_lock(&q->mutex);

    while (q->count == 0 && !q->done)
        pthread_cond_wait(&q->not_empty, &q->mutex);

    if (q->count == 0 && q->done) {
        pthread_mutex_unlock(&q->mutex);
        return false;   // no more work, thread should exit
    }

    *out_event = q->events[q->head];
    q->head = (q->head + 1) % QUEUE_CAPACITY;
    q->count--;

    pthread_cond_signal(&q->not_empty);  // wake one waiting producer if any
    pthread_mutex_unlock(&q->mutex);
    return true;
}

// Call when a producer thread finishes (after pushing all events)
void remove_producer(LineQueue *q) {
    pthread_mutex_lock(&q->mutex);
    q->producer_count--;
    pthread_mutex_unlock(&q->mutex);
}

void mark_done(LineQueue *q) {
    pthread_mutex_lock(&q->mutex);
    if (q->producer_count == 0) {
        q->done = true;
        pthread_cond_broadcast(&q->not_empty);  // wake ALL waiting threads   
    }
    pthread_mutex_unlock(&q->mutex);
}

void queue_destroy(LineQueue *q) {
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->not_empty);
}