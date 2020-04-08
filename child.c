#include <unistd.h>
#include "logger.h"
#include "local_state.h"
#include "string.h"
#include "child.h"
#include "stdio.h"
#include "io.h"
#include "pa2345.h"


void child_run() {
    log_started();
    child_send_started();

    //wait for STARTED msgs from all other processes
    for (size_t id = 1; id < processes_count; id++) {
        Message msg;
        if (id == local_state.id) {
            continue;
        }
        receive(NULL, id, &msg);
    }
    log_received_all_started();

    //send DONE msg
    log_done();
    child_send_done();

    //wait for DONE from all other processes
    for (size_t id = 1; id < processes_count; id++) {
        Message msg;
        if (id == local_state.id) {
            continue;
        }
        receive(NULL, id, &msg);
    }
    log_received_all_done();
}

void child_send_started() {
    Message msg = {
            {
                    MESSAGE_MAGIC,
                    0,
                    STARTED,
            }
    };
    sprintf(msg.s_payload, log_started_fmt, get_physical_time(), local_state.id, getpid(), getppid(),
            local_state.init_balance);
    msg.s_header.s_payload_len = strlen(msg.s_payload);
    send_multicast(NULL, &msg);
}

void child_send_done() {
    Message msg = {
            {
                    MESSAGE_MAGIC,
                    0,
                    DONE,
            }
    };
    sprintf(msg.s_payload, log_done_fmt, get_physical_time(), local_state.id,
            local_state.history.s_history[local_state.history.s_history_len].s_balance);
    msg.s_header.s_payload_len = strlen(msg.s_payload);
    send_multicast(NULL, &msg);
}
