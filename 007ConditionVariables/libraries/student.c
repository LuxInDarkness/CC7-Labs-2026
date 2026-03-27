#include "student.h"

void init_student(Student * student, int dir, char * name) {
    student->dir = dir;
    student->name = name;
    student->priority = 0;
    student->time_waited = 0.0;
}

void log_push(LineQueue * queue, char * message) {
    char log[MAX_LINE_LEN];
    time_t now = time(NULL);
    struct tm * t = localtime(&now);
    snprintf(log, sizeof(log), "%02d:%02d:%02d | %s",
        t->tm_hour, t->tm_min, t->tm_sec, message);
    queue_push(queue, log);
}

void wait_for_bridge(Student * student, Bridge * bridge, LineQueue * queue) {
    int sleep_time = (rand() % 6);
    sleep(sleep_time);

    char log[MAX_LINE_LEN];
    snprintf(log, sizeof(log), "%s arrives wanting to go %s\n",
        student->name,
        student->dir == LEFT ? "LEFT" : "RIGHT");
    log_push(queue, log);

    int bridge_count = 0;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    if (student->dir == LEFT)
        bridge_count = wait_left_bridge(bridge);
    else if (student->dir == RIGHT)
        bridge_count = wait_right_bridge(bridge);
    clock_gettime(CLOCK_MONOTONIC, &end);

    float wait_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9f;
    student->time_waited += wait_time;
    student->bridge_count = bridge_count;
}

void cross_bridge(Student * student, LineQueue * queue, Bridge * bridge) {
    char log[MAX_LINE_LEN];
    snprintf(log, sizeof(log), "%s crosses to the %s (on bridge: %d)\n",
        student->name,
        student->dir == LEFT ? "LEFT" : "RIGHT",
        student->bridge_count);
    log_push(queue, log);
    int sleep_time = (rand() % 3) + 1;
    sleep(sleep_time);
}

void exit_bridge(Student * student, Bridge * bridge, LineQueue * queue) {
    pthread_mutex_lock(&bridge->mutex);

    if (student->dir == RIGHT) {
        bridge->crossing_right_students--;
        if (bridge->crossing_right_students == 0) {
            bridge->dir_priority = NO;
            pthread_cond_broadcast(&bridge->left_space_available);
        } else {
            pthread_cond_signal(&bridge->right_space_available);
        }
    } else {
        bridge->crossing_left_students--;
        if (bridge->crossing_left_students == 0) {
            bridge->dir_priority = NO;
            pthread_cond_broadcast(&bridge->right_space_available);
        } else {
            pthread_cond_signal(&bridge->left_space_available);
        }
    }

    int bridge_count = student->dir == LEFT ? bridge->crossing_left_students : bridge->crossing_right_students;

    pthread_mutex_unlock(&bridge->mutex);

    char log[MAX_LINE_LEN];
    snprintf(log, sizeof(log), "%s exits bridge (on bridge: %d)\n",
        student->name,
        bridge_count);
    log_push(queue, log);
}

void *student_work(void *arg) {
    StArgs * args = (StArgs *) arg;
    LineQueue * queue = args->queue;
    Bridge * bridge = args->bridge;
    Results * results = args->results;

    Student student;

    int dir = rand() % 2;
    char name[20];
    snprintf(name, sizeof(name), "Inge %d", args->id);

    init_student(&student, dir, name);
    wait_for_bridge(&student, bridge, queue);
    cross_bridge(&student, queue, bridge);
    exit_bridge(&student, bridge, queue);

    results_update(results, student.time_waited);

    return NULL;
}
