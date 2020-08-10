#include <unistd.h>
#include <stdbool.h>
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
        update_local_time(msg.s_header.s_local_time);
    }
    log_received_all_started();

    if (local_state.mutex_enabled) {
        request_cs(NULL);
    }

    char str[128];
    for (int i = 1; i <= local_state.id * 5; ++i) {
        memset(str, 0, sizeof(str));
        sprintf(str, log_loop_operation_fmt, local_state.id, i, local_state.id * 5);
        print(str);
    }

    if (local_state.mutex_enabled) {
        release_cs(NULL);
    }

    child_send_done();

    child_process_message();

    log_received_all_done();
}

void child_send_started() {
    local_state.current_time++;
    Message msg = {
            {
                    MESSAGE_MAGIC,
                    0,
                    STARTED,
                    get_lamport_time()
            }
    };
    sprintf(msg.s_payload, log_started_fmt, get_lamport_time(), local_state.id, getpid(), getppid(), 0);
    msg.s_header.s_payload_len = strlen(msg.s_payload);
    send_multicast(NULL, &msg);
}

void child_send_done() {
    local_state.current_time++;
    Message msg = {
            {
                    MESSAGE_MAGIC,
                    0,
                    DONE,
                    get_lamport_time()
            }
    };
    sprintf(msg.s_payload, log_done_fmt, get_lamport_time(), local_state.id, 0);
    msg.s_header.s_payload_len = strlen(msg.s_payload);
    send_multicast(NULL, &msg);
}

void child_process_message() {
    while (local_state.done_received != processes_count - 2) {
        Message msg;
        int peer = receive_any(NULL, &msg);
        update_local_time(msg.s_header.s_local_time);
        switch (msg.s_header.s_type) {
            case DONE:
                local_state.done_received++;
                break;
            case CS_REQUEST:
                local_state.current_time++;
                Message message = {
                        .s_header = {
                                .s_magic = MESSAGE_MAGIC,
                                .s_local_time = get_lamport_time(),
                                .s_type = CS_REPLY,
                                .s_payload_len = 0,
                        }
                };
                send(NULL, peer, &message);
                break;
        }
    }
}
