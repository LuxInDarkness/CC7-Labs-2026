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

void wait_right_bridge(Bridge * bridge) {
    pthread_mutex_lock(&bridge->mutex);

    while(bridge->crossing_left_students > 0 || bridge->dir_priority == LEFT || bridge->crossing_right_students >= bridge->max_spaces) {
        bridge->waiting_right_students++;
        pthread_cond_wait(&bridge->is_empty, &bridge->mutex);
        bridge->waiting_right_students--;
    }

    bridge->cons_right_counter++;
    bridge->cons_left_counter = 0;
    bridge->crossing_right_students++;

    if (bridge->cons_right_counter >= bridge->priority_change_amount)
        bridge->dir_priority = LEFT;

    pthread_mutex_unlock(&bridge->mutex);
}