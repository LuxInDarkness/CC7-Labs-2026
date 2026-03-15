#include "ts_semaphore.h"

void init_semaphore(Semaphore *sem, int value) {
    sem->spaces = value;
    sem->head = NULL;
    sem->tail = NULL;
    pthread_mutex_init(&sem->mutex, NULL);
}

void wait_semaphore(Semaphore *sem) {
    pthread_mutex_lock(&sem->mutex);

    if (sem->spaces > 0 && sem->head == NULL) {
        // No one waiting and space available, take it immediately
        sem->spaces--;
        pthread_mutex_unlock(&sem->mutex);
        return;
    }

    // Enqueue yourself
    WaiterNode node;
    pthread_cond_init(&node.cond, NULL);
    node.next = NULL;

    if (sem->tail) sem->tail->next = &node;
    else           sem->head = &node;
    sem->tail = &node;

    // Wait on YOUR condition variable
    pthread_cond_wait(&node.cond, &sem->mutex);

    pthread_cond_destroy(&node.cond);
    pthread_mutex_unlock(&sem->mutex);
}

void signal_semaphore(Semaphore *sem) {
    pthread_mutex_lock(&sem->mutex);
    sem->spaces++;

    if (sem->head != NULL) {
        // Wake the first waiter in line
        WaiterNode *next = sem->head;
        sem->head = sem->head->next;
        if (sem->head == NULL) sem->tail = NULL;

        sem->spaces--; // Reserve the space for this specific thread
        pthread_cond_signal(&next->cond);
    }

    pthread_mutex_unlock(&sem->mutex);
}

void destroy_semaphore(Semaphore *sem) {
    pthread_mutex_destroy(&sem->mutex);
}