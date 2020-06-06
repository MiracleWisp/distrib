#include "local_state.h"

void init_state(local_id local_pid, int mutex_enabled) {
    local_state = (LocalState) {
            local_pid,
            0,
            mutex_enabled,
            0
    };
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
