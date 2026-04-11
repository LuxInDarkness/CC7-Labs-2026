#ifndef THREAD_H
#define THREAD_H

typedef struct {
    int id;
    int burst_time;
    int remaining_time;
    int arrival_time;
} Thread;

#endif // THREAD_H