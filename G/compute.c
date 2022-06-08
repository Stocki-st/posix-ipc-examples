#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdlib.h>

#include "share.h"


//converts a null-terminated char* to uppercase
void make_uppercase(char *s) {
    for(int i = 0; s[i] != '\0'; i++) {
        s[i] = toupper((unsigned char)s[i]);
    }
}

//converts a null-terminated char* to lowercase
void make_lowercase(char *s) {
    for(int i = 0; s[i] != '\0'; i++) {
        s[i] = tolower((unsigned char)s[i]);
    }
}


int generate_pipe(char* pipe_name) {
    //delete existing pipe
    unlink(pipe_name);
    //create pipe
    if(mkfifo(pipe_name, 0666)) {
        perror(pipe_name);
        return 1;
    }

    // open pipe
    printf("try to open pipe: %s\n",pipe_name);
    int fd = open(pipe_name, O_WRONLY);
    if(fd == -1) {
        perror(pipe_name);
        return 1;
    }
    return fd;
}
// Todo if time: extract writing into function
//int write_to_pipe(int fd, char* msg)

int main(int argc, char** argv)
{
    char line[MAX_MSG_LEN];

    // open message queue
    int mq = mq_open (DEFAULT_NAME,  O_RDONLY);
    if(mq == -1) {
        perror(DEFAULT_NAME);
        return 1;
    }

    //get named pipe for upper case
    int fd_upper;
    if((fd_upper = generate_pipe(PIPE_NAME_UPPER)) == 1) {
        mq_close(mq);
        return 1;
    }

    //get named pipe for lower case
    int fd_lower;
    if((fd_lower = generate_pipe(PIPE_NAME_LOWER)) == 1) {
        mq_close(mq);
        free(fd_upper);
        return 1;
    }

    ssize_t rv = 0;
    unsigned int prio = 0;

    // read from message queue
    while((rv = mq_receive(mq, line, MAX_MSG_LEN, &prio)) != -1) {
        if(prio == MY_SHUTDOWN_PRIO) {
            break;
        }
        line[rv] = '\0';
        int length = strlen(line);
        printf("received len: %d, prio: %d, msg received: '%s' \n", length, prio, line);

        char upper[MAX_MSG_LEN];
        char lower[MAX_MSG_LEN];
        strncpy(upper, line, MAX_MSG_LEN);
        strncpy(lower, line, MAX_MSG_LEN);

        make_uppercase(upper);
        make_lowercase(lower);

        printf("converted to uppercase: %s\n", upper);
        printf("converted to lowercase: %s\n", lower);


        // write into uppercase pipe
        unsigned char lengthHeader = strlen(upper);
        if(lengthHeader >= MAX_MSG_LEN) {
            lengthHeader = MAX_MSG_LEN - 1;
            upper[lengthHeader] = 0;
        }
        if( write(fd_upper, &lengthHeader, 1) != 1) {
            fprintf(stderr, "Can't write length header");
            mq_close(mq);
            close(fd_lower);
            close(fd_upper);
            return 1;
        }
        int rv = write(fd_upper, &upper, strlen(upper));
        if(rv == -1) {
            perror(PIPE_NAME_UPPER);
            mq_close(mq);
            close(fd_lower);
            close(fd_upper);
            return 1;
        }

        // write into lowercase pipe
        lengthHeader = strlen(lower);
        if(lengthHeader >= MAX_MSG_LEN) {
            lengthHeader = MAX_MSG_LEN - 1;
            lower[lengthHeader] = 0;
        }
        if( write(fd_lower, &lengthHeader, 1) != 1) {
            fprintf(stderr, "Can't write length header");
            mq_close(mq);
            close(fd_lower);
            close(fd_upper);
            return 1;
        }
        rv = write(fd_lower, &lower, strlen(lower));
        if(rv == -1) {
            perror(PIPE_NAME_LOWER);
            mq_close(mq);
            close(fd_lower);
            close(fd_upper);
            return 1;
        }

    }

    if(rv == -1) {
        perror(DEFAULT_NAME);
        mq_close(mq);
        close(fd_lower);
        close(fd_upper);
        return 1;
    }

    // close ressources
    mq_close(mq);
    close(fd_lower);
    close(fd_upper);

    //remove pipes
    unlink(PIPE_NAME_LOWER);
    unlink(PIPE_NAME_UPPER);

    printf("done...\n");
    return 0;
}
