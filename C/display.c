#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>

#include "share.h"

int main(int argc, char** argv)
{
    char line[MAX_MSG_LEN];
//    struct mq_attr attr = {0,MAX_MSG_COUNT, MAX_MSG_LEN,0};
    int mq = mq_open (DEFAULT_NAME,  O_RDONLY);

    if(mq == -1) {
        perror(DEFAULT_NAME);
        return 1;
    }

    ssize_t rv = 0;
    unsigned int prio = 0;
    while((rv = mq_receive(mq, line, MAX_MSG_LEN, &prio)) != -1){
        if(prio ==MY_SHUTDOWN_PRIO){
            break;
        }
        line[rv] = '\0';
        printf("len: %ld, prio: %d, msg received: '%s' \n",strlen(line), prio, line);
    } 

    if(rv == -1) {
        perror(DEFAULT_NAME);
        mq_close(mq);
        return 1;
    }

    printf("done...\n");
    mq_close(mq);
    return 0;
}
