#ifndef THREAD_H
#define THREAD_H

#include <stdio.h>
#include <time.h>
#include "logger.h"

typedef enum {
    PENDING,   /* has not arrived yet                  */
    READY,     /* arrived, waiting for CPU             */
    RUNNING,   /* currently on CPU                     */
    FINISHED   /* execution complete                   */
} ThreadState;

typedef struct Thread {
    int         id;
    int         burst_time;
    int         remaining_time;
    int         arrival_time;
    ThreadState state;
    int         last_preempted_time;
} Thread;

Thread create_thread(int id, int burst_time, int arrival_time);
void   thread_set_ready   (Thread *t);
void   thread_set_running (Thread *t);
void   thread_set_finished(Thread *t);

/* Consume `slice` units of CPU time; returns remaining_time after the slice */
int  thread_consume(Thread *t, int slice);

/* FIFO / SJF: run the thread to completion from current_time,
   log arrival+start and completion, return the new current_time */
int  thread_run_full(Thread *t, int current_time);

/* RR: run one quantum slice, log start/resume/preempt/complete,
   accumulate wait into *wait_acc, return new current_time */
int  thread_run_slice(Thread *t, int current_time, int quantum,
                      float *wait_acc);

/* SRTF: apply an already-computed elapsed interval to the thread,
   log preempt/start/resume/complete as needed,
   `prev_id` is the id of the thread that ran just before (-1 if none),
   `is_new_run` signals a context switch onto this thread */
void thread_apply_interval(Thread *t, int time_before, int time_after,
                           int is_new_run, int was_preempted_before);

void thread_log_arrived   (const Thread *t, int current_time);
void thread_log_started   (const Thread *t, int current_time, float waited);
void thread_log_resumed   (const Thread *t, int current_time);
void thread_log_preempted (const Thread *t, int current_time);
void thread_log_completed (const Thread *t, int current_time,
                           float waited, float turnaround);

#endif // THREAD_H