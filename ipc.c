#include "ipc.h"
#include "io.h"
#include <unistd.h>

int send(void *self, local_id dst, const Message *msg) {
    int fd = writer[local_pid][dst];
    write(fd, &msg->s_header, sizeof(MessageHeader));
    write(fd, &msg->s_payload, msg->s_header.s_payload_len);
    return 0;
}


int send_multicast(void *self, const Message *msg) {
    for (int dest = 0; dest < processes_count; dest++) {
        if (dest != local_pid) {
            send(self, dest, msg);
        }
    }
    return 0;
}

int receive(void *self, local_id from, Message *msg) {
    int fd = writer[from][local_pid];
    read(fd, &msg->s_header, sizeof(MessageHeader));
    read(fd, &msg->s_payload, (size_t) &msg->s_header.s_payload_len);
    return 0;
}

int receive_any(void *self, Message *msg) {
    return 0;
}
