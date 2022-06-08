#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "share.h"


int main(int argc, char** argv)
{
    if(argc<2) {
        fprintf(stderr, "Select display usage - possible arguments are: 'lower' or 'upper'");
        return 1;
    }
    // how to make this sane?
 char* pipe_name;
    if(strcmp(argv[1],"lower") == 0) {
        pipe_name =  PIPE_NAME_UPPER;
    }  else if(strcmp(argv[1],"upper") == 0) {
         pipe_name =  PIPE_NAME_LOWER;
    } else {
        fprintf(stderr, "Select display usage - possible arguments are: 'lower' or 'upper'");
        return 1;
    }

    printf("pipe name: %s", pipe_name);

    char line[MAX_MSG_LEN];
    printf("try to open pipe: %s", pipe_name);
    int fd = open (pipe_name, O_RDONLY);

    if(fd == -1) {
        perror(pipe_name);
        return 1;
    }
    ssize_t rv = 0;
    unsigned char length = 0;

    while ((rv = read(fd,&length, 1)) == 1) {
        rv = read(fd,&line, length);

        if(rv == -1) {
            break;
        }
        if(rv != (ssize_t) length) {
            fprintf(stderr, "Can't read full message, rv: %ld", rv);
            close(fd);
            return 1;
        }
        line[rv] = '\0';
        printf("len: %ld, msg: '%s'\n", rv, line);
    }

    if(rv == -1) {
        perror(pipe_name);
        close(fd);
        return 1;
    }
    printf("done...\n");
    close(fd);
    return 0;
}
