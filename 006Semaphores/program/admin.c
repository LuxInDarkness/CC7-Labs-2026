#include "admin.h"

LineQueue queue;
Results results;
Semaphore semaphore;
UIEventQueue ui_queue;

void admin(int cars, int spaces) {
    queue_init(&queue);
    results_init(&results);
    init_semaphore(&semaphore, spaces);

    ThreadArgs logger_args = { &queue, &results, NULL, &ui_queue, 0 };

    // Create logger thread
    pthread_t logger_thread;
    if (pthread_create(&logger_thread, NULL, logger_work, &logger_args) != 0) {
        printf("Failed to create logger thread.\n");
        return 1;
    }

    srand(time(NULL)); // Seed random number generator

    // Create car threads
    pthread_t car_threads[cars];
    ThreadArgs car_args[cars];

    for (int i = 0; i < cars; i++) {
        car_args[i].queue = &queue;
        car_args[i].results = &results;
        car_args[i].semaphore = &semaphore;
        car_args[i].ui_queue = NULL;
        car_args[i].car_number = i + 1;

        if (pthread_create(&car_threads[i], NULL, car_work, &car_args[i]) != 0) {
            printf("Failed to create car thread.\n");
            return 1;
        }
    }

    // Wait for car threads to finish
    for (int i = 0; i < cars; i++) {
        pthread_join(car_threads[i], NULL);
    }

    // Signal logger thread that no more events will be added
    mark_done(&queue);

    // Wait for logger thread to finish
    pthread_join(logger_thread, NULL);

    // Print final results
    printf("Total cars parked: %d\n", results.cars_parked);
    printf("Average wait time: %.2f seconds\n", results.cars_parked > 0 ? results.total_wait_time / results.cars_parked : 0);

    // Destroy ts objects
    queue_destroy(&queue);
    destroy_semaphore(&semaphore);
    results_destroy(&results);

}