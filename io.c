#include <unistd.h>
#include "io.h"

void close_useless_pipes() {
    for (int src = 0; src < processes_count; src++) {
        for (int dest = 0; dest < processes_count; dest++) {
            if (src != local_pid) {
                close(writer[src][dest]);
            }
            if (dest != local_pid) {
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
                int pipedes[2];
                pipe(pipedes);
                reader[source][destination] = pipedes[0];
                writer[source][destination] = pipedes[1];
            }
        }
    }
}
