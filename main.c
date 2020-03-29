#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "ipc.h"
#include "logger.h"
#include "io.h"

int main(int argc, char **argv) {

    //read options
    int opt;
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                processes_count = atoi(optarg) + 1;
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }

    create_pipes();
    log_init();
    pid_t process2pid[processes_count];
    // Spawn processes
    for (int id = 1; id < processes_count; id++) {
        pid_t child_pid = fork();
        if (!child_pid) {
            //child
            local_pid = id;
            break;
        } else {
            //parent
            local_pid = PARENT_ID;
            process2pid[id] = child_pid;
        }
    }
    close_useless_pipes();
    if (local_pid != PARENT_ID) {
        log_started();
        Message msg = {
                {
                        MESSAGE_MAGIC,
                        0,
                        STARTED,
                }
        };
        sprintf(msg.s_payload, log_started_fmt, local_pid, getpid(), getppid());
        msg.s_header.s_payload_len = strlen(msg.s_payload);
        send_multicast(NULL, &msg);
    }

    for (size_t id = 1; id < processes_count; id++) {
        Message msg;
        if (id == local_pid) {
            continue;
        }
        receive(NULL, id, &msg);
    }

    log_received_all_started();

    if (local_pid != PARENT_ID) {
        log_done();
        Message msg = {
                {
                        MESSAGE_MAGIC,
                        0,
                        DONE,
                }
        };
        sprintf(msg.s_payload, log_done_fmt, local_pid);
        msg.s_header.s_payload_len = strlen(msg.s_payload);
        send_multicast(NULL, &msg);
    }

    for (size_t id = 1; id < processes_count; id++) {
        Message msg;
        if (id == local_pid) {
            continue;
        }
        receive(NULL, id, &msg);
    }
    log_received_all_done();

    if (local_pid == PARENT_ID) {
        // Wait for the children to stop
        for (int id = 1; id < processes_count; id++) {
            waitpid(process2pid[id], NULL, 0);
        }
        log_close();
    }

    return 0;
}
