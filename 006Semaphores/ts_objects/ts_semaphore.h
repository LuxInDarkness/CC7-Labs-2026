#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <pthread.h>

typedef struct WaiterNode {
    pthread_cond_t cond;
    struct WaiterNode *next;
} WaiterNode;

typedef struct Semaphore {
    int spaces;
    pthread_mutex_t mutex;
    WaiterNode *head;  // front of the wait queue
    WaiterNode *tail;  // back of the wait queue
} Semaphore;

void init_semaphore(Semaphore *sem, int value);
void wait_semaphore(Semaphore *sem);
void signal_semaphore(Semaphore *sem);
void destroy_semaphore(Semaphore *sem);

#endif // SEMAPHORE_H