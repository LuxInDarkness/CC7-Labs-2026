#include "student.h"

void init_student(Student * student, int dir, char * name) {
    student->dir = dir;
    student->name = name;
    student->priority = 0;
    student->time_waited = 0.0;
}

void wait_for_bridge(Student * student, Bridge * bridge, LineQueue * queue) {
    int sleep_time = (rand() % 6);
    sleep(sleep_time);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    if (student->dir == LEFT)
        wait_left_bridge(&bridge);
    else if (student->dir == RIGHT)
        wait_right_bridge(&bridge);
    clock_gettime(CLOCK_MONOTONIC, &end);

    float wait_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9f;
    student->time_waited += wait_time;

    char log[MAX_LINE_LEN];
    snprintf(log, sizeof(log), "%s arrives wanting to go %s",
        student->name,
        student->dir == LEFT ? "LEFT" : "RIGHT");

    queue_push(&queue, log);
}

void cross_bridge(Student * student) {
    int sleep_time = (rand() % 3) + 1;
    sleep(sleep_time);
}

void exit_bridge(Student * student) {

}

void *student_work(void *arg) {
    StArgs * args = (StArgs *) arg;
    Student student;

    srand(time(NULL));
    int dir = rand() % 2;
    char * name = "Inge " + args->id;

    init_student(&student, dir, &name);

}
