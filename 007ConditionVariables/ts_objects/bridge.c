#include "bridge.h"

void init_bridge(Bridge * bridge, int spaces, int amount) {
    bridge->max_spaces = spaces;
    bridge->crossing_right_students = 0;
    bridge->crossing_left_students = 0;
    bridge->waiting_right_students = 0;
    bridge->waiting_left_students = 0;
    bridge->dir_priority = NO;
    bridge->priority_change_amount = amount;
    bridge->cons_right_counter = 0;
    bridge->cons_left_counter = 0;
    pthread_mutex_init(&bridge->mutex, NULL);
    pthread_cond_init(&bridge->is_empty, NULL);
    pthread_cond_init(&bridge->right_space_available, NULL);
    pthread_cond_init(&bridge->left_space_available, NULL);
}

int wait_right_bridge(Bridge * bridge) {
    pthread_mutex_lock(&bridge->mutex);

    while (bridge->crossing_left_students > 0 ||
           bridge->dir_priority == LEFT ||
           bridge->crossing_right_students >= bridge->max_spaces) {
        bridge->waiting_right_students++;
        pthread_cond_wait(&bridge->right_space_available, &bridge->mutex);
        bridge->waiting_right_students--;
    }

    bridge->crossing_right_students++;
    bridge->cons_right_counter++;
    bridge->cons_left_counter = 0;

    if (bridge->dir_priority == RIGHT)
        bridge->dir_priority = NO;           // priority consumed, back to neutral
    else if (bridge->cons_right_counter >= bridge->priority_change_amount)
        bridge->dir_priority = LEFT;

    int count = bridge->crossing_right_students;
    pthread_mutex_unlock(&bridge->mutex);
    return count;
}

int wait_left_bridge(Bridge * bridge) {
    pthread_mutex_lock(&bridge->mutex);

    while (bridge->crossing_right_students > 0 ||
           bridge->dir_priority == RIGHT ||
           bridge->crossing_left_students >= bridge->max_spaces) {
        bridge->waiting_left_students++;
        pthread_cond_wait(&bridge->left_space_available, &bridge->mutex);
        bridge->waiting_left_students--;
    }

    bridge->crossing_left_students++;
    bridge->cons_left_counter++;
    bridge->cons_right_counter = 0;

    if (bridge->dir_priority == LEFT)
        bridge->dir_priority = NO;           // priority consumed, back to neutral
    else if (bridge->cons_left_counter >= bridge->priority_change_amount)
        bridge->dir_priority = RIGHT;

    int count = bridge->crossing_left_students;
    pthread_mutex_unlock(&bridge->mutex);
    return count;
}

void signal_right_bridge(Bridge * bridge) {
    pthread_cond_signal(&bridge->right_space_available);
}

void signal_left_bridge(Bridge * bridge) {
    pthread_cond_signal(&bridge->left_space_available);
}

void broadcast_right_bridge(Bridge * bridge) {
    pthread_cond_broadcast(&bridge->right_space_available);
}

void broadcast_left_bridge(Bridge * bridge) {
    pthread_cond_broadcast(&bridge->left_space_available);
}

void destroy_bridge(Bridge * bridge) {
    pthread_mutex_destroy(&bridge->mutex);
    pthread_cond_destroy(&bridge->is_empty);
    pthread_cond_destroy(&bridge->right_space_available);
    pthread_cond_destroy(&bridge->left_space_available);
}