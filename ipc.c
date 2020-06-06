#define _XOPEN_SOURCE 600 /* needed for timespec in <time.h> */

#include "ipc.h"
#include "io.h"
#include "local_state.h"
#include <unistd.h>
#include <asm/errno.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include "stdio.h"
#include "string.h"

int send(void *self, local_id dst, const Message *msg) {
    int fd = writer[local_state.id][dst];
    write(fd, msg, sizeof(MessageHeader) + msg->s_header.s_payload_len);
    return 0;
}


int send_multicast(void *self, const Message *msg) {
    for (int dest = 0; dest < processes_count; dest++) {
        if (dest != local_state.id) {
            send(self, dest, msg);
        }
    }
    return 0;
}

int receive(void *self, local_id from, Message *msg) {
    int fd = reader[from][local_state.id];
    unsigned int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags & !O_NONBLOCK);
    read(fd, &msg->s_header, sizeof(MessageHeader));
    read(fd, &msg->s_payload, (size_t) msg->s_header.s_payload_len);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    return 0;
}

int receive_any(void *self, Message *msg) {
    while (1) {
        for (int src = 0; src < processes_count; ++src) {
            if (src == local_state.id) {
                continue;
            }
            int fd = reader[src][local_state.id];
            int read_count = read(fd, &msg->s_header, sizeof(MessageHeader));
            switch (read_count) {
                case -1:
                    if (errno == EAGAIN) {
//                        printf("(pipe empty)\n");
                    } else {
                        perror("read");
                    }
                    break;
                case 0:
//                    printf("End of conversaпtion\n");
                    break;
                default:
                    if (msg->s_header.s_payload_len) {
                        read(fd, &msg->s_payload, (size_t) msg->s_header.s_payload_len);
                    }
                    return 0;
            }
        }
        nanosleep((const struct timespec[]) {{0, 1000L}}, NULL);
    }
}
