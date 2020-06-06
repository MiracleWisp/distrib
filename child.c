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

    child_process_message();

    log_received_all_done();

    child_send_balance_history();
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
    sprintf(msg.s_payload, log_started_fmt, get_lamport_time(), local_state.id, getpid(), getppid(),
            local_state.init_balance);
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
    sprintf(msg.s_payload, log_done_fmt, get_lamport_time(), local_state.id,
            local_state.history.s_history[local_state.history.s_history_len].s_balance);
    msg.s_header.s_payload_len = strlen(msg.s_payload);
    send_multicast(NULL, &msg);
}

void child_process_message() {
    bool stopped = false;
    int done_received = 0;
    while (!stopped || done_received != processes_count - 2) {
        Message msg;
        receive_any(NULL, &msg);
        update_local_time(msg.s_header.s_local_time);
        switch (msg.s_header.s_type) {
            case TRANSFER:
                child_process_transfer(&msg);
                break;
            case DONE:
                done_received++;
                break;
            case STOP:
                stopped = true;
                log_done();
                child_send_done();
                break;
        }
    }
}

void child_process_transfer(Message *msg) {
    TransferOrder *order = (TransferOrder *) &(msg->s_payload);

    if (order->s_src == local_state.id) {
        local_state.current_time++;
        for (timestamp_t time = get_lamport_time(); time <= MAX_T; time++) {
            local_state.history.s_history[time].s_balance -= order->s_amount;
        }
        msg->s_header.s_local_time = get_lamport_time();
        send(NULL, order->s_dst, msg);
        log_transfer_out(order->s_amount, order->s_dst);
    }

    if (order->s_dst == local_state.id) {
        for (timestamp_t time = msg->s_header.s_local_time; time < get_lamport_time(); time++) {
            local_state.history.s_history[time].s_balance_pending_in += order->s_amount;
        }
        for (timestamp_t time = get_lamport_time(); time <= MAX_T; time++) {
            local_state.history.s_history[time].s_balance += order->s_amount;
        }
        local_state.current_time++;
        Message ack = {
                {
                        MESSAGE_MAGIC,
                        0,
                        ACK,
                        get_lamport_time(),
                }
        };
        send(NULL, PARENT_ID, &ack);
        log_transfer_in(order->s_amount, order->s_src);
    }
    if (get_lamport_time() >= local_state.history.s_history_len) {
        local_state.history.s_history_len = get_lamport_time() + 1;
    }
}

void child_send_balance_history() {
    local_state.current_time++;
    local_state.history.s_history_len = get_lamport_time() + 1;
    size_t history_size =
            sizeof(BalanceHistory) - (MAX_T + 1 - local_state.history.s_history_len) * sizeof(BalanceState);

    Message msg = {
            .s_header = {
                    .s_magic = MESSAGE_MAGIC,
                    .s_type = BALANCE_HISTORY,
                    .s_local_time = get_lamport_time(),
                    .s_payload_len = history_size,
            }
    };
    memcpy(&msg.s_payload, &local_state.history, history_size);
    send(NULL, PARENT_ID, &msg);
}
