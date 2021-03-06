#include "ipc.h"
#include "banking.h"
#include "local_state.h"
#include "io.h"
#include <stdio.h>

void qput(Queue *queue, QueueElement element) {
    queue->queue[queue->size] = element;
    queue->size++;

}

int get_index(Queue *queue) {
    int min = 0;
    for (int i = 1; i < queue->size; i++) {
        if (queue->queue[i].time < queue->queue[min].time) {
            min = i;
        } else {
            if (queue->queue[i].time == queue->queue[min].time &&
                queue->queue[i].process_id < queue->queue[min].process_id) {
                min = i;
            }
        }
    }
    return min;
}

QueueElement qpeek(Queue *queue) {
    int min = get_index(queue);
    return queue->queue[min];
}

void qpop(Queue *queue) {
    int min = get_index(queue);
    queue->size--;
    queue->queue[min] = queue->queue[queue->size];
}


int request_cs(const void *self) {
    local_state.current_time++;

    local_state.request_time = get_lamport_time();

    Message request_msg = {
            .s_header = {
                    .s_magic = MESSAGE_MAGIC,
                    .s_type = CS_REQUEST,
                    .s_local_time = get_lamport_time(),
                    .s_payload_len = 0,
            },
            .s_payload = "",
    };

    send_multicast(NULL, &request_msg);
    int replies_left = processes_count - 2;

    while (1) {
        if (replies_left == 0) {
            break;
        }

        Message received_msg;
        local_id peer = receive_any(NULL, &received_msg);
        update_local_time(received_msg.s_header.s_local_time);

        switch (received_msg.s_header.s_type) {
            case CS_REPLY:
                replies_left--;
                break;

            case CS_REQUEST:
                if (local_state.request_time < received_msg.s_header.s_local_time ||
                    (local_state.request_time == received_msg.s_header.s_local_time && local_state.id < peer)) {
                    local_state.deferred[peer] = 1;
                } else {
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
                }
                break;

            case DONE:
                local_state.done_received++;
                break;
        }
    }

    return 0;
}

int release_cs(const void *self) {
    local_state.current_time++;
    Message message = {
            .s_header = {
                    .s_magic = MESSAGE_MAGIC,
                    .s_local_time = get_lamport_time(),
                    .s_type = CS_REPLY,
                    .s_payload_len = 0,
            }
    };

    for (int i = 0; i < MAX_PROCESS_ID; i++) {
        if (local_state.deferred[i]) {
            send(NULL, i, &message);
            local_state.deferred[i] = 0;
        }
    }

    local_state.request_time = -1;

    return 0;
}


