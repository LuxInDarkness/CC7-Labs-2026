#include "thread.h"

Thread create_thread(int id, int burst_time, int arrival_time) {
    Thread t;
    t.id = id;
    t.burst_time = burst_time;
    t.remaining_time = burst_time; // Initially, remaining time is equal to burst time
    t.arrival_time = arrival_time;
    return t;
}

void update_remaining_time(Thread *t, int time) {
    if (t->remaining_time > 0) {
        t->remaining_time -= time;
        if (t->remaining_time < 0) {
            t->remaining_time = 0; // Ensure remaining time does not go negative
        }
    }
}
