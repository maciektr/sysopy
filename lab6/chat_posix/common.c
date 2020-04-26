#include "common.h"

#include <mqueue.h>

int get_queue(char *name, int mode) {
    struct mq_attr attr;
    attr.mq_msgsize = MSG_MAX_SIZE;
    attr.mq_maxmsg = MSG_MAX_N;
    attr.mq_curmsgs = 0;
    attr.mq_flags = 0;
    return mq_open(name, mode | O_CREAT, QMOD, &attr);
}

// void set_msg(msg_t *buffer, int sender_id, order_t order, int integer_msg){
//     buffer->mtype = order;
//     buffer->sender_id = sender_id;
//     buffer->integer_msg = integer_msg;
// }