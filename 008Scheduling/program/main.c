#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define MIN_THREADS 5
#define MAX_THREADS 15
#define MAX_BURST_TIME 10
#define MAX_ARRIVAL_TIME 100

int main() {
    srand(time(NULL)); // Seed the random number generator

    int num_threads = rand() % (MAX_THREADS - MIN_THREADS + 1) + MIN_THREADS; // Random number of threads between 5 and 15

    int burst_time[num_threads];
    for (int i = 0; i < num_threads; i++) {
        burst_time[i] = rand() % MAX_BURST_TIME + 1; // Random burst time between 1 and 10
    }

    int arrival_time[num_threads];
    for (int i = 0; i < num_threads; i++) {
        arrival_time[i] = rand() % (MAX_ARRIVAL_TIME + 1); // Random arrival time between 0 and 100
    }

    return 0;
}