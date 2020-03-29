#ifndef PA1_LOGGER_H
#define PA1_LOGGER_H

#include <stdio.h>
#include <time.h>
#include "io.h"
#include "common.h"
#include "pa1.h"

static FILE *events_log_file;

void log_init() {
    events_log_file = fopen(events_log, "w");
}

void log_started() {
    pid_t pid = getpid();
    pid_t parent_pid = getppid();
    fprintf(events_log_file, log_started_fmt, local_pid, pid, parent_pid);
}

void log_received_all_started() {
    fprintf(events_log_file, log_received_all_started_fmt, local_pid);
}

void log_done() {
    fprintf(events_log_file, log_done_fmt, local_pid);
}

void log_received_all_done() {
    fprintf(events_log_file, log_received_all_done_fmt, local_pid);
}

void log_close() {
    fclose(events_log_file);
}


#endif //PA1_LOGGER_H
