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
    }

    log_received_all_started();

    bank_robbery(NULL, processes_count - 1);

    parent_send_stop();

    for (size_t id = 1; id < processes_count; id++) {
        Message msg;
        if (id == local_state.id) {
            continue;
        }
        receive(NULL, id, &msg);
    }
    log_received_all_done();


    parent_receive_balance_histories();

    // Wait for the children to stop
    for (int id = 1; id < processes_count; id++) {
        waitpid(process2pid[id], NULL, 0);
    }
    log_close();
}

void transfer(void *parent_data, local_id src, local_id dst, balance_t amount) {
    Message msg = {
            {
                    MESSAGE_MAGIC,
                    sizeof(TransferOrder),
                    TRANSFER,
                    get_physical_time()
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
}

void parent_send_stop() {
    Message msg = {
            {
                    MESSAGE_MAGIC,
                    0,
                    STOP,
                    get_physical_time()
            }
    };
    send_multicast(NULL, &msg);
}

void parent_receive_balance_histories() {
    AllHistory history;
    history.s_history_len = processes_count - 1;
    for (size_t child_id = 1; child_id <= processes_count - 1; child_id++) {
        Message msg;
        receive_any(NULL, &msg);
        BalanceHistory *received_history = (BalanceHistory *) &msg.s_payload;
        history.s_history[received_history->s_id - 1] = *received_history;
    }

    print_history(&history);
}
