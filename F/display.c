#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "share.h"


int main(int argc, char** argv)
{
    char line[MAX_MSG_LEN];
    printf("try to open pipe: %s", PIPE_NAME);
    int fd = open (PIPE_NAME, O_RDONLY);

    if(fd == -1) {
        perror(PIPE_NAME);
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
           // close(fd);
          //  return 1;
        }
        line[rv] = '\0';
        printf("len: %ld, msg: '%s'\n", rv, line);
    }

    if(rv == -1) {
        perror(PIPE_NAME);
        close(fd);
        return 1;
    }
    /*


        while ((rv = read(fd,line, MAX_MSG_LEN )) == 1) {
                printf("len: %ld, msg: '%s'\n", rv, line);
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
            perror(PIPE_NAME);
            close(fd);
            return 1;
        }*/

    printf("done...\n");
    close(fd);
    return 0;
}
