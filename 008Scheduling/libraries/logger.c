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
    printf("%s\n", message);
    if (log_fp) {
        fprintf(log_fp, "%s\n", message);
    }
}

void close_log_file(FILE *log_fp) {
    if (log_fp) {
        fclose(log_fp);
    }
}