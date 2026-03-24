#ifndef STUDENT_H
#define STUDENT_H

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "../ts_objects/queue.h"
#include "../ts_objects/bridge.h"

typedef struct Student {
    int dir;
    float time_waited;
    char * name;
    int priority;
} Student;

typedef struct StArgs {
    int id;
    LineQueue * queue;
    Bridge * bridge;
} StArgs;

#endif // STUDENT_H