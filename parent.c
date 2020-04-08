#include <wait.h>
#include "parent.h"
#include "stdio.h"
#include "logger.h"
#include "local_state.h"

void parent_run() {
    printf("in parent\n");

    //wait for STARTED msgs from all other processes
    for (size_t id = 1; id < processes_count; id++) {
        Message msg;
        if (id == local_state.id) {
            continue;
        }
        receive(NULL, id, &msg);
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
