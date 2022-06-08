#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "share.h"


int main(int argc, char** argv)
{
    char line[MAX_MSG_LEN];

    struct mq_attr attr = {0,MAX_MSG_COUNT, MAX_MSG_LEN,0};
    mqd_t mq = mq_open(DEFAULT_NAME,O_CREAT | O_WRONLY, 0666, &attr);
    if(mq == -1) {
        perror(DEFAULT_NAME);
        return 1;
    }
    printf("created message queue '%s'\n", DEFAULT_NAME);

    if(mq_getattr(mq,&attr) == -1) {
        perror(DEFAULT_NAME);
        return 1;
    }

    while (printf("> "),fgets(line, MAX_MSG_LEN, stdin)) {
        int len = strlen(line);
        if(len > 0 && line[len - 1] == '\n') {
            line[len-1] = '\0';
            --len;
        }

        if(strcmp(line,"quit") == 0) {
           char x = 0;
           mq_send(mq,&x,strlen(&x),MY_SHUTDOWN_PRIO);
           break;
        }

        if(strcmp(line,"sleep") == 0) {
            sleep(3);
            continue;
        }

        if(len >= MAX_MSG_LEN) {
            len = MAX_MSG_LEN - 1;
            line[len] = 0;
        }
        if(mq_send(mq, line, len, MY_FLUFFY_DEFAULT_PRIO) == -1) {
            perror(DEFAULT_NAME);
            mq_close(mq);
            return 1;
        }
    }

    printf("done...\n");
    mq_close(mq);
    return 0;
}
