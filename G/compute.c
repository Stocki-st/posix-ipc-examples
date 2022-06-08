#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#include "share.h"

#define PIPE_NAME PIPE_NAME_UPPER
///TODO: check if null terminated/max size
void make_uppercase(char *s) {
    for(int i = 0; s[i] != '\0'; i++) {
        s[i] = toupper((unsigned char)s[i]);
    }
}

void make_lowercase(char *s) {
    for(int i = 0; s[i] != '\0'; i++) {
        s[i] = tolower((unsigned char)s[i]);
    }
}


int main(int argc, char** argv)
{
    char line[MAX_MSG_LEN];
    int mq = mq_open (DEFAULT_NAME,  O_RDONLY);
    if(mq == -1) {
        perror(DEFAULT_NAME);
        return 1;
    }
    //delete existing pipe
    unlink(PIPE_NAME);
    //create pipe
    if(mkfifo(PIPE_NAME, 0666)) {
        perror(PIPE_NAME);
        mq_close(mq);
        return 1;
    }

    printf("try to open pipe: %s\n",PIPE_NAME);
    int fd = open (PIPE_NAME, O_WRONLY );
    if(fd == -1) {
        perror(PIPE_NAME);
        mq_close(mq);
        return 1;
    }

    ssize_t rv = 0;
    unsigned int prio = 0;
    while((rv = mq_receive(mq, line, MAX_MSG_LEN, &prio)) != -1) {
        if(prio == MY_SHUTDOWN_PRIO) {
            break;
        }
        line[rv] = '\0';
        int length = strlen(line);
        printf("received len: %d, prio: %d, msg received: '%s' \n", length, prio, line);

        make_uppercase(line);
        printf("converted to uppercase: %s\n", line);

        unsigned char lengthHeader = strlen(line);
        if(lengthHeader >= MAX_MSG_LEN) {
            lengthHeader = MAX_MSG_LEN - 1;
            line[lengthHeader] = 0;
        }

        if( write(fd, &lengthHeader, 1) != 1) {
            fprintf(stderr, "Can't write length header");
            close(fd);
            return 1;
        }

        int rv = write(fd, &line, strlen(line));
        if(rv == -1) {
            perror(PIPE_NAME);
            close(fd);
            return 1;
        }
    }

    if(rv == -1) {
        perror(DEFAULT_NAME);
        mq_close(mq);
        close(fd);
        return 1;
    }

    printf("done...\n");
    mq_close(mq);
    close(fd);
    return 0;
}
