#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "share.h"

extern int errno;
int main(int argc, char** argv)
{
    const char* filename = (argc >= 2) ? argv[1] : DEFAULT_FILENAME;
    unlink(filename);

    char line[MAX_MSG_LEN];

    if(mkfifo(filename, 0666)) {
        perror(filename);
        return 1;
    }

    printf("try to open %s\n",filename);
    int fd = open (filename, O_WRONLY );

    if(fd == -1) {
        perror(filename);
        return 1;
    }
    printf("opened named pipe '%s'\n",filename);
    while (printf("> "),fgets(line, MAX_MSG_LEN, stdin)) {
        int len = strlen(line);
        if(len > 0 && line[len - 1] == '\n') {
            line[len-1] = '\0';
            --len;
        }

        if(strcmp(line,"quit") == 0) {
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
        unsigned char lengthHeader = len;
        if( write(fd, &lengthHeader, 1) != 1) {
            fprintf(stderr, "Can't write length header");
            close(fd);
            return 1;
        }

        int rv = write(fd, &line, len);
        if(rv == -1) {
            perror(filename);
            close(fd);
            return 1;
        }
        if(rv != len) {
            fprintf(stderr, "Can't write file");
            close(fd);
            return 1;
        }
    }

    printf("done...\n");
    close(fd);
    return 0;
}
