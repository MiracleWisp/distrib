#ifndef PA2_PARENT_H
#define PA2_PARENT_H

#include "io.h"
#include <sys/types.h>
#include <unistd.h>

pid_t process2pid[MAX_PROCESS_ID];

void parent_run();

void parent_send_stop();

void parent_receive_balance_histories();

#endif //PA2_PARENT_H
