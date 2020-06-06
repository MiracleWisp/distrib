#include "logger.h"
#include "common.h"
#include "pa2345.h"
#include <time.h>
#include <unistd.h>
#include "banking.h"
#include "local_state.h"
#include <sys/types.h>
#include <unistd.h>

void log_init() {
    events_log_file = fopen(events_log, "w");
}

void log_started() {
    pid_t pid = getpid();
    pid_t parent_pid = getppid();
    fprintf(events_log_file, log_started_fmt, get_lamport_time(), local_state.id, pid, parent_pid, 0);
}

void log_received_all_started() {
    fprintf(events_log_file, log_received_all_started_fmt, get_lamport_time(), local_state.id);
}

void log_done() {
    fprintf(events_log_file, log_done_fmt, get_lamport_time(), local_state.id, 0);
}

void log_received_all_done() {
    fprintf(events_log_file, log_received_all_done_fmt, get_lamport_time(), local_state.id);
}

void log_transfer_out(int money, int dest) {
    fprintf(events_log_file, log_transfer_out_fmt, get_lamport_time(), local_state.id, money, dest);
}

void log_transfer_in(int money, int src) {
    fprintf(events_log_file, log_transfer_in_fmt, get_lamport_time(), local_state.id, money, src);
}

void log_close() {
    fclose(events_log_file);
}
