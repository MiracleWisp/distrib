#ifndef PA2_LOCAL_STATE_H
#define PA2_LOCAL_STATE_H

#include "ipc.h"
#include "banking.h"

typedef struct {
    timestamp_t time;
    local_id process_id;
} QueueElement;

typedef struct {
    QueueElement queue[64];
    int size;
} Queue;

typedef struct {
    local_id id;
    timestamp_t current_time;
    int mutex_enabled;
    int done_received;
    timestamp_t request_time;
    int deferred[MAX_PROCESS_ID];
} LocalState;

LocalState local_state;

void init_state(local_id local_pid, int mutex_enabled);

void update_local_time(timestamp_t time);

#endif //PA2_LOCAL_STATE_H
