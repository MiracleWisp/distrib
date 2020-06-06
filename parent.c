#include <wait.h>
#include "parent.h"
#include "logger.h"
#include "local_state.h"
#include "banking.h"
#include "string.h"

void parent_run() {
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


    for (size_t id = 1; id < processes_count; id++) {
        Message msg;
        if (id == local_state.id) {
            continue;
        }
        receive(NULL, id, &msg);
    }
    log_received_all_done();

    // Wait for the children to stop
    for (int id = 1; id < processes_count; id++) {
        waitpid(process2pid[id], NULL, 0);
    }
    log_close();
}

void transfer(void *parent_data, local_id src, local_id dst, balance_t amount) {
    local_state.current_time++;
    Message msg = {
            {
                    MESSAGE_MAGIC,
                    sizeof(TransferOrder),
                    TRANSFER,
                    get_lamport_time()
            }
    };
    TransferOrder order = {
            src,
            dst,
            amount
    };
    memcpy(&msg.s_payload, &order, sizeof(TransferOrder));
    send(NULL, src, &msg);
    //receive ACK
    receive_any(NULL, &msg);
    update_local_time(msg.s_header.s_local_time);
}

void parent_send_stop() {
    local_state.current_time++;

    Message msg = {
            {
                    MESSAGE_MAGIC,
                    0,
                    STOP,
                    get_lamport_time()
            }
    };
    send_multicast(NULL, &msg);
}
