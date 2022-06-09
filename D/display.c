#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <semaphore.h>
#include "share.h"

int main(int argc, char** argv)
{
    char line[MAX_MSG_LEN+1] = {0};

    printf("try to open shared mem '%s'\n",SHAREDMEM_NAME);

    int fd = shm_open(SHAREDMEM_NAME, O_RDWR, 0);
    if(fd == -1) {
        perror(SHAREDMEM_NAME);
        return 1;
    }
    printf("shared mem opened '%s' - fd: %d\n",SHAREDMEM_NAME, fd);

    if(ftruncate(fd, sizeof(struct shared_msg)) == -1) {
        perror(SHAREDMEM_NAME);
        close(fd);
        ///TODO: delete fd
        return 1;
    }

    struct shared_msg* shm = mmap(NULL, sizeof(struct shared_msg),PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    printf("mmap created\n");

    sem_t* sem_sync = sem_open(SHM_SEM_SYNC, O_RDWR);
    if(sem_sync == SEM_FAILED) {
        perror(SHM_SEM_SYNC);
        if(munmap(shm,sizeof(struct shared_msg)) == -1) {
            perror(SHAREDMEM_NAME);
        }
        close(fd);
        return 1;
    }

    sem_t* sem_new_msg = sem_open(SHM_SEM_NEW_MSG, O_RDWR);
    if(sem_new_msg == SEM_FAILED) {
        perror(SHM_SEM_NEW_MSG);
        if(munmap(shm,sizeof(struct shared_msg)) == -1) {
            perror(SHAREDMEM_NAME);
        }
        sem_close(sem_sync);
        close(fd);
        return 1;
    }

    sem_t* sem_ready = sem_open(SHM_SEM_READY,O_RDWR );
    if(sem_ready == SEM_FAILED) {
        perror(SHM_SEM_READY);
        if(munmap(shm,sizeof(struct shared_msg)) == -1) {
            perror(SHAREDMEM_NAME);
        }
        close(fd);
        sem_close(sem_sync);
        sem_close(sem_new_msg);
        return 1;
    }


    unsigned char length = 0;

    while (1) {
        //wait for notification that new msg is available
        sem_wait(sem_new_msg);
        {
            /*
            * Critical section
            */
            sem_wait(sem_sync);
            //read from shared mem
            memcpy(line, shm->msg,MAX_MSG_LEN);
            length = shm->length;
            sem_post(sem_sync);
        }
        // notify that reading is finished
        sem_post(sem_ready);


        if(strcmp(line,"quit") == 0) {
            break;
        }

        printf("msg: '%s' with length %d (null included)\n",  line, length);
    }

    printf("done...\n");
    munmap(shm, sizeof(struct shared_msg));
    close(fd);
    sem_close(sem_sync);
    sem_close(sem_new_msg);
    sem_close(sem_ready);
    sem_unlink(SHM_SEM_SYNC);
    sem_unlink(SHM_SEM_READY);
    sem_unlink(SHM_SEM_NEW_MSG);
    shm_unlink(SHAREDMEM_NAME);
    return 0;
}