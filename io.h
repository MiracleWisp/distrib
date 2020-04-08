#ifndef PA1_IO_H
#define PA1_IO_H

#include "ipc.h"

int processes_count;
int reader[MAX_PROCESS_ID][MAX_PROCESS_ID];
int writer[MAX_PROCESS_ID][MAX_PROCESS_ID];

void create_pipes();

void close_useless_pipes();

#endif //PA1_IO_H
