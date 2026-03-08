#include <pthread.h>

#include "ts_results_holder.h"

void results_init(Results *r) {
    r->errors_count = 0;
    r->ip_counts = g_hash_table_new(g_str_hash, g_str_equal);
    r->url_counts = g_hash_table_new(g_str_hash, g_str_equal);
    r->most_visited_url = NULL;
    pthread_mutex_init(&r->mutex, NULL);
}

void results_update(Results *r, int32_t error_code, char *ip, char *url) {
    pthread_mutex_lock(&r->mutex);
    // Update error count
    if (error_code >= 400 && error_code < 600) {
        r->errors_count++;
    }
    // Update IP and URL counts
    char *ip_key  = g_strdup(ip);
    char *url_key = g_strdup(url);

    g_hash_table_insert(r->ip_counts, ip_key, GINT_TO_POINTER(g_hash_table_lookup(r->ip_counts, ip_key) ? GPOINTER_TO_INT(g_hash_table_lookup(r->ip_counts, ip_key)) + 1 : 1));
    int32_t url_count = g_hash_table_lookup(r->url_counts, url_key) ? GPOINTER_TO_INT(g_hash_table_lookup(r->url_counts, url_key)) + 1 : 1;
    g_hash_table_insert(r->url_counts, url_key, GINT_TO_POINTER(url_count));
    // Update most visited URL
    if (!r->most_visited_url || url_count > GPOINTER_TO_INT(g_hash_table_lookup(r->url_counts, r->most_visited_url))) {
        free(r->most_visited_url);
        r->most_visited_url = strdup(url);
    }
    pthread_mutex_unlock(&r->mutex);
}

void results_destroy(Results *r) {
    free(r->most_visited_url);
    g_hash_table_destroy(r->ip_counts);
    g_hash_table_destroy(r->url_counts);
    r->ip_counts  = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    r->url_counts = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    pthread_mutex_destroy(&r->mutex);
}