#ifndef PA2_CHILD_H
#define PA2_CHILD_H

void child_run();

void child_send_started();

void child_send_done();

void child_process_message();

void child_process_transfer(Message *msg);

void child_send_balance_history();

#endif //PA2_CHILD_H
