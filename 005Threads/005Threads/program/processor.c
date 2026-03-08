#include <stdio.h>
#include <pthread.h>

#include "processor.h"

#define THREAD_COUNT 5

LineQueue queue;
Results results;

void *worker(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    char line[MAX_LINE_LEN];
    char ip[64], path[256];

    while (queue_pop(args->queue, line)) {
        // --- process the line ---
        int32_t error_code = parse_error_code(line);
        parse_ip(line, ip, sizeof(ip));
        parse_url(line, path, sizeof(path));

        // --- update shared results ---
        results_update(args->results, error_code, ip, path);
    }
    return NULL;
}

int main() {
    queue_init(&queue);
    results_init(&results);

    ThreadArgs args = { &queue, &results };

    pthread_t threads[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++)
        pthread_create(&threads[i], NULL, worker, &args);

    // Producer: read file and push lines
    FILE *fp = fopen("access.log", "r");
    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), fp))
        queue_push(&queue, line);
    fclose(fp);

    queue_mark_done(&queue);  // signal all threads to finish up

    for (int i = 0; i < THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);

    printf("Total Unique IPs: %d\nMost Visited URL: %s (%d times)\nHTTP Errors: %d\n",
           g_hash_table_size(results.ip_counts), results.most_visited_url,
           GPOINTER_TO_INT(g_hash_table_lookup(results.url_counts, results.most_visited_url)),
           results.errors_count);

    queue_destroy(&queue);
    results_destroy(&results);
    return 0;
}
