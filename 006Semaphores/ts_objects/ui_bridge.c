#include "ui_bridge.h"
#include <string.h>

void ui_queue_init(UIEventQueue *q) {
    q->head  = 0;
    q->tail  = 0;
    q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
}

void ui_queue_push(UIEventQueue *q, UIEvent event) {
    pthread_mutex_lock(&q->mutex);
    if (q->count < UI_QUEUE_SIZE) {
        q->events[q->tail] = event;
        q->tail = (q->tail + 1) % UI_QUEUE_SIZE;
        q->count++;
    }
    pthread_mutex_unlock(&q->mutex);
}

int ui_queue_pop(UIEventQueue *q, UIEvent *out) {
    pthread_mutex_lock(&q->mutex);
    if (q->count == 0) {
        pthread_mutex_unlock(&q->mutex);
        return 0;
    }
    *out   = q->events[q->head];
    q->head = (q->head + 1) % UI_QUEUE_SIZE;
    q->count--;
    pthread_mutex_unlock(&q->mutex);
    return 1;
}

void ui_queue_destroy(UIEventQueue *q) {
    pthread_mutex_destroy(&q->mutex);
}