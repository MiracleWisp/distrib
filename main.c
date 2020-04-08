#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "ipc.h"
#include "logger.h"
#include "io.h"
#include "banking.h"
#include "local_state.h"
#include "child.h"
#include "parent.h"

void transfer(void *parent_data, local_id src, local_id dst,
              balance_t amount) {
    // student, please implement me
}

int main(int argc, char **argv) {
    //read options
    int opt;
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                processes_count = strtol(optarg, NULL, 10) + 1;
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }

    create_pipes();
    log_init();

    // Spawn processes
    for (int id = 1; id < processes_count; id++) {
        pid_t child_pid = fork();
        if (!child_pid) {
            //child
            local_state.id = id;
            init_state(id, strtol(argv[id + 2], NULL, 10));
            break;
        } else {
            //parent
            local_state.id = PARENT_ID;
            process2pid[id] = child_pid;
        }
    }

    close_useless_pipes();
    printf("%d\n", local_state.id);
    if (local_state.id == PARENT_ID) {
        parent_run();
    } else {
        child_run();
    }

    return 0;
}
