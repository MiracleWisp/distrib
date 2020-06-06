#include "local_state.h"

void init_state(local_id local_pid, balance_t init_balance) {
    local_state = (LocalState) {
            local_pid,
            init_balance,
            {
                    local_pid,
                    1
            },
            0
    };
    for (int i = 0; i < MAX_T; i++) {
        local_state.history.s_history[i] = (BalanceState) {
                init_balance,
                i,
                0
        };
    }
}

timestamp_t get_lamport_time() {
    return local_state.current_time;
}

void update_local_time(timestamp_t time) {
    if (time > local_state.current_time) {
        local_state.current_time = time;
    }
    local_state.current_time++;
}
