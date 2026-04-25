#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <time.h>
#include <string.h>

#define LOG_FILE "scheduling.log"

FILE *open_log_file();
void log_message(FILE *log_fp, char * message);
void close_log_file(FILE *log_fp);

#endif // LOGGER_H