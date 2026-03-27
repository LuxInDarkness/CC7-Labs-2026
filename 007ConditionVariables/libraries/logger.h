#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <time.h>
#include <string.h>

#include "../ts_objects/queue.h"

typedef struct LogArgs {
    LineQueue * queue;
} LogArgs;

#define LOG_FILE "bridge_crossing.log"

FILE *open_log_file();
void log_message(FILE *log_fp, char * message);
void close_log_file(FILE *log_fp);
void *logger_work(void *arg);

#endif // LOGGER_H