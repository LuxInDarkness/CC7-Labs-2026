#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <time.h>
#include <string.h>

#include "actions.h"
#include "t_args.h"
#include "../ts_objects/ui_bridge.h"
#define LOG_FILE "parking_lot.log"

FILE *open_log_file();
void log_message(FILE *log_fp, ParkingEvent event);
void close_log_file(FILE *log_fp);
void *logger_work(void *arg);

#endif // LOGGER_H