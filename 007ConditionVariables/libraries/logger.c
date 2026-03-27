#include "logger.h"

FILE *open_log_file() {
    FILE *log_fp = fopen(LOG_FILE, "a");
    if (!log_fp) {
        perror("Failed to open log file");
        return NULL;
    }
    return log_fp;
}

void log_message(FILE *log_fp, char * message) {
    if (log_fp) {
        fprintf(log_fp, "%s", message);
    }
}

void close_log_file(FILE *log_fp) {
    if (log_fp) {
        fclose(log_fp);
    }
}

void *logger_work(void *arg) {
    LogArgs *args = (LogArgs *)arg;
    FILE *log_fp = open_log_file();

    char log[MAX_LINE_LEN];

    while (queue_pop(args->queue, log)) {
        log_message(log_fp, log);
    }

    close_log_file(log_fp);
    return NULL;
}