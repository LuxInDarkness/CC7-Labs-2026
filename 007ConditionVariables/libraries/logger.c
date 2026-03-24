#include "logger.h"

#define LOG_FILE "parking_lot.log"

static const char *action_strings[] = {
    [ARRIVED] = "Arrived at parking lot",
    [PARKED]  = "Parked successfully",
    [LEFT]    = "Leaving parking lot"
};

FILE *open_log_file() {
    FILE *log_fp = fopen(LOG_FILE, "a");
    if (!log_fp) {
        perror("Failed to open log file");
        return NULL;
    }
    return log_fp;
}

void log_message(FILE *log_fp, ParkingEvent event) {
    if (log_fp) {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);

        char buf[64];
        strftime(buf, sizeof(buf), "%a %b %e %H:%M:%S %Y", t);

        if (event.action == PARKED) {
            fprintf(log_fp, "[%s] Car %d: %s (Waited: %.2f seconds)\n", buf, event.car_number, action_strings[event.action], event.wait_time);
        } else {
            fprintf(log_fp, "[%s] Car %d: %s\n", buf, event.car_number, action_strings[event.action]);
        }
    }
}

void close_log_file(FILE *log_fp) {
    if (log_fp) {
        fclose(log_fp);
    }
}

void *logger_work(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    FILE *log_fp = open_log_file();
    UIEventQueue *ui_q   = args->ui_queue;
    ParkingEvent event;

    while (queue_pop(args->queue, &event)) {
        log_message(log_fp, event);

        // Push to UI
        UIEvent ui_event = { .car_number = event.car_number, .wait_time = event.wait_time };
        switch (event.action) {
            case ARRIVED: ui_event.type = UI_EVENT_ARRIVED; break;
            case PARKED:  ui_event.type = UI_EVENT_PARKED;  break;
            case LEFT:    ui_event.type = UI_EVENT_LEFT;    break;
        }
        ui_queue_push(ui_q, ui_event);
    }

    // Signal UI that simulation is done
    ui_queue_push(ui_q, (UIEvent){ .type = UI_EVENT_DONE });

    close_log_file(log_fp);
    return NULL;
}