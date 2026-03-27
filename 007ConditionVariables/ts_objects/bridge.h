#ifndef BRIDGE_H
#define BRIDGE_H

#include <pthread.h>

#define RIGHT 0
#define LEFT 1
#define NO 2

typedef struct Bridge {
    int max_spaces;
    int crossing_right_students;
    int crossing_left_students;
    int waiting_right_students;
    int waiting_left_students;
    int dir_priority;
    int priority_change_amount;
    int cons_right_counter;
    int cons_left_counter;
    pthread_mutex_t mutex;
    pthread_cond_t right_space_available;
    pthread_cond_t left_space_available;
    pthread_cond_t is_empty;
} Bridge;

void init_bridge(Bridge * bridge, int spaces, int amount);
int wait_right_bridge(Bridge * bridge);
int wait_left_bridge(Bridge * bridge);
void signal_right_bridge(Bridge * bridge);
void signal_left_bridge(Bridge * bridge);
void broadcast_right_bridge(Bridge * bridge);
void broadcast_left_bridge(Bridge * bridge);
void destroy_bridge(Bridge * bridge);

#endif // BRIDGE_H