#ifndef TS_RESULTS_HOLDER_H
#define TS_RESULTS_HOLDER_H

#include <glib.h>

typedef struct {
    int32_t errors_count;
    GHashTable *ip_counts;
    GHashTable *url_counts;
    char* most_visited_url;

    pthread_mutex_t mutex;
} Results;

void results_init(Results *r);
void results_update(Results *r, int32_t error_code, char *ip, char *url);
void results_destroy(Results *r);

#endif // TS_RESULTS_HOLDER_H