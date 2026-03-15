#ifndef UI_BRIDGE_H
#define UI_BRIDGE_H

#include <pthread.h>

typedef enum UIEventType {
    UI_EVENT_ARRIVED, // car joins the queue
    UI_EVENT_PARKED,  // car moves from queue to a space
    UI_EVENT_LEFT,    // car leaves a space
    UI_EVENT_DONE     // simulation finished
} UIEventType;

typedef struct UIEvent {
    UIEventType type;
    int         car_number;
    float       wait_time;
} UIEvent;

#define UI_QUEUE_SIZE 128

typedef struct UIEventQueue {
    UIEvent         events[UI_QUEUE_SIZE];
    int             head;
    int             tail;
    int             count;
    pthread_mutex_t mutex;
} UIEventQueue;

void ui_queue_init(UIEventQueue *q);
void ui_queue_push(UIEventQueue *q, UIEvent event);
int  ui_queue_pop(UIEventQueue *q, UIEvent *out); // returns 1 if got event
void ui_queue_destroy(UIEventQueue *q);

#endif // UI_BRIDGE_H