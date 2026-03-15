#include <time.h>

#include "car.h"

void arrived(int car_number, LineQueue *queue) {
    ParkingEvent event = { .car_number = car_number, .action = ARRIVED, .wait_time = 0 };
    queue_push(queue, event);
}

void parked(int car_number, LineQueue *queue, Semaphore *semaphore, float sleep_time, float *out_wait_time) {
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);
    wait_semaphore(semaphore);
    clock_gettime(CLOCK_MONOTONIC, &end);

    float wait_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9f;

    ParkingEvent event = { .car_number = car_number, .action = PARKED, .wait_time = wait_time };
    queue_push(queue, event);
    g_usleep(sleep_time * 1000000); // Simulate the time taken to park
    *out_wait_time = wait_time;
}

void left(int car_number, Semaphore *semaphore, LineQueue *queue) {
    ParkingEvent event = { .car_number = car_number, .action = LEFT, .wait_time = 0 };
    signal_semaphore(semaphore); // Signal that a parking space is now available
    queue_push(queue, event);
}

void *car_work(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    int car_number = args->car_number;
    float sleep_time = ((rand() % 4000) + 1000) / 1000.0; // Simulate wait time between 1 and 5 seconds
    float wait_time = 0;

    arrived(car_number, args->queue);
    parked(car_number, args->queue, args->semaphore, sleep_time, &wait_time);
    left(car_number, args->semaphore, args->queue);
    results_update(args->results, wait_time);

    return NULL;
}