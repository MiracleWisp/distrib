#ifndef PA1_LOGGER_H
#define PA1_LOGGER_H

#include <stdio.h>

static FILE *events_log_file;

void log_init();

void log_started();

void log_received_all_started();

void log_done();

void log_received_all_done();

void log_transfer_out(int money, int dest);

void log_transfer_in(int money, int src);

void log_close();

#endif //PA1_LOGGER_H
