#ifndef STUDENT_H
#define STUDENT_H

#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

#include "../ts_objects/queue.h"
#include "../ts_objects/bridge.h"
#include "../ts_objects/results_holder.h"

typedef struct Student {
    int dir;
    float time_waited;
    char * name;
    int priority;
    int bridge_count;
} Student;

typedef struct StArgs {
    int id;
    LineQueue * queue;
    Bridge * bridge;
    Results * results;
} StArgs;

void *student_work(void *arg);

#endif // STUDENT_H