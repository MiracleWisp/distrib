#include <unistd.h>
#include <fcntl.h>
#include "io.h"
#include "local_state.h"

void close_useless_pipes() {
    for (int src = 0; src < processes_count; src++) {
        for (int dest = 0; dest < processes_count; dest++) {
            if (src != local_state.id) {
                close(writer[src][dest]);
            }
            if (dest != local_state.id) {
                close(reader[src][dest]);
            }
        }
    }
}

void create_pipes() {
    for (int source = 0; source < processes_count; source++) {
        for (int destination = 0; destination < processes_count;
             destination++) {
            if (source != destination) {
                int fds[2];
                pipe(fds);
                // Enable non-blocking mode
                for (int i = 0; i < 2; ++i) {
                    unsigned int flags = fcntl(fds[i], F_GETFL, 0);
                    fcntl(fds[i], F_SETFL, flags | O_NONBLOCK);
                }

                reader[source][destination] = fds[0];
                writer[source][destination] = fds[1];
            }
        }
    }
}
