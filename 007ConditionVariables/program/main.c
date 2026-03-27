#include "../libraries/logger.h"
#include "../libraries/student.h"
#include "../ts_objects/results_holder.h"

LineQueue queue;
Results results;
Bridge bridge;

#define STUDENTS 50
#define SPACES 4
#define PRIORITY_CHANGE 8

int main() {
    init_bridge(&bridge, SPACES, PRIORITY_CHANGE);
    queue_init(&queue);
    results_init(&results);

    LogArgs log_args = { &queue };
    pthread_t logger_thread;
    if (pthread_create(&logger_thread, NULL, logger_work, &log_args) != 0) {
        printf("Failed to create logger thread.\n");
        return 1;
    }

    srand(time(NULL));

    pthread_t student_threads[STUDENTS];
    StArgs st_args[STUDENTS];

    for (int i = 0; i < STUDENTS; i++) {
        st_args[i].queue = &queue;
        st_args[i].results = &results;
        st_args[i].bridge = &bridge;
        st_args[i].id = i + 1;
        
        if (pthread_create(&student_threads[i], NULL, student_work, &st_args[i]) != 0) {
            printf("Failed to create student thread.\n");
            return 1;
        }
    }

    for (int i = 0; i < STUDENTS; i++) {
        pthread_join(student_threads[i], NULL);
    }

    // Signal logger thread that no more events will be added
    mark_done(&queue);

    // Wait for logger thread to finish
    pthread_join(logger_thread, NULL);

    // Print final results
    printf("Average wait time for students before crossing: %.2f seconds\n", results.total_wait_time / results.students_count);

    // Destroy ts objects
    queue_destroy(&queue);
    destroy_bridge(&bridge);
    results_destroy(&results);

    return 0;
}