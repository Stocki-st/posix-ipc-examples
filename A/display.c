#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "share.h"

extern int errno;
int main(int argc, char** argv)
{
    const char* filename = (argc >= 2) ? argv[1] : DEFAULT_FILENAME;
    char line[MAX_MSG_LEN];
    int fd = open (filename,  O_RDONLY);

    if(fd == -1) {
        perror(filename);
        return 1;
    }
    ssize_t rv = 0;
    unsigned char length = 0;

    while ((rv = read(fd,&length, 1)) == 1) {
    rv = read(fd,line, length);
        if(rv == -1) {
            break;
        }
        if(rv != (ssize_t) length) {
            fprintf(stderr, "Can't read full message, rv: %d", rv);
            close(fd);
            return 1;
        }
        line[rv] = '\0';
        printf("len: %d, msg: '%s'\n", rv, line);
    }

    if(rv == -1) {
    perror(filename);
        close(fd);
        return 1;
    }

    printf("done...\n");
    close(fd);
    return 0;
}
