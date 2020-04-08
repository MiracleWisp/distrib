#ifndef PA2_LOCAL_STATE_H
#define PA2_LOCAL_STATE_H

#include "ipc.h"
#include "banking.h"

typedef struct {
    local_id id;
    balance_t init_balance;
    BalanceHistory history;
} LocalState;

LocalState local_state;

void init_state(local_id local_pid, balance_t init_balance);

#endif //PA2_LOCAL_STATE_H
