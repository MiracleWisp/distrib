#include "local_state.h"

void init_state(local_id local_pid, balance_t init_balance) {
    local_state = (LocalState) {
            local_pid,
            init_balance,
            {
                    local_pid,
                    1
            }
    };
    for (int i = 0; i < MAX_T; i++) {
        local_state.history.s_history[i] = (BalanceState) {
                init_balance,
                i,
                0
        };
    }
}
