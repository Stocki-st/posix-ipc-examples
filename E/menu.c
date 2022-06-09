#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <semaphore.h>

#include "share.h"

int main(int argc, char** argv)
{
    shm_unlink(SHAREDMEM_NAME);

    char line[MAX_MSG_LEN];

    printf("try to open shared mem '%s'\n",SHAREDMEM_NAME);
    int fd = shm_open(SHAREDMEM_NAME, O_CREAT| O_RDWR,0666);
    if(fd == -1) {
        perror(SHAREDMEM_NAME);
        return 1;
    }

    printf("shared mem opened '%s' - fd: %d\n",SHAREDMEM_NAME, fd);

    if(ftruncate(fd, sizeof(struct shared_msg)) == -1) {
        perror(SHAREDMEM_NAME);
        close(fd);
        shm_unlink(SHAREDMEM_NAME);
        return 1;
    }

    struct shared_msg* shm = mmap(NULL, sizeof(struct shared_msg),PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    //semaphore
    sem_unlink(SHM_SEM_SYNC);
    sem_t* sem_sync = sem_open(SHM_SEM_SYNC,O_RDWR | O_CREAT, 0666,1 /*init value = 1*/);
    if(sem_sync == SEM_FAILED) {
        perror(SHM_SEM_SYNC);
        if(munmap(shm,sizeof(struct shared_msg)) == -1) {
            perror(SHAREDMEM_NAME);
        }
        close(fd);
        shm_unlink(SHAREDMEM_NAME);
        return 1;
    }

    sem_unlink(SHM_SEM_NEW_MSG);
    sem_t* sem_new_msg = sem_open(SHM_SEM_NEW_MSG,O_RDWR | O_CREAT, 0666,0 /*init value = 0*/);
    if(sem_new_msg == SEM_FAILED) {
        perror(SHM_SEM_NEW_MSG);
        if(munmap(shm,sizeof(struct shared_msg)) == -1) {
            perror(SHAREDMEM_NAME);
        }
        close(fd);
        shm_unlink(SHAREDMEM_NAME);
        sem_close(sem_sync);
        sem_unlink(SHM_SEM_SYNC);
        return 1;
    }

    sem_unlink(SHM_SEM_READY);
    sem_t* sem_ready = sem_open(SHM_SEM_READY,O_RDWR | O_CREAT, 0666,1 /*init value = 0*/);
    if(sem_ready == SEM_FAILED) {
        perror(SHM_SEM_READY);
        if(munmap(shm,sizeof(struct shared_msg)) == -1) {
            perror(SHAREDMEM_NAME);
        }
        close(fd);
        sem_close(sem_sync);
        sem_close(sem_new_msg);
        shm_unlink(SHAREDMEM_NAME);
        sem_unlink(SHM_SEM_SYNC);
        sem_unlink(SHM_SEM_NEW_MSG);
        return 1;
    }

    sem_t* sem_all_finished = sem_open(SHM_SEM_ALL_FINISHED, O_RDWR| O_CREAT, 0666,0);
    if( sem_all_finished == SEM_FAILED) {
        perror(SHM_SEM_ALL_FINISHED);
        if(munmap(shm,sizeof(struct shared_msg)) == -1) {
            perror(SHAREDMEM_NAME);
        }
        close(fd);
        sem_close(sem_sync);
        sem_close(sem_new_msg);
        sem_close(sem_ready);
        shm_unlink(SHAREDMEM_NAME);
        sem_unlink(SHM_SEM_SYNC);
        sem_unlink(SHM_SEM_NEW_MSG);
        sem_unlink(SHM_SEM_READY);
        return 1;
    }

    while (printf("> "),fgets(line, MAX_MSG_LEN, stdin)) {
        int len = strlen(line);
        if(len > 0 && line[len - 1] == '\n') {
            line[len-1] = '\0';
            --len;
        }

        if(strcmp(line,"sleep") == 0) {
            sleep(3);
            continue;
        }

        //make sure length is in allowed range
        if(len >= MAX_MSG_LEN) {
            len = MAX_MSG_LEN - 1;
            line[len] = 0;
        }

        // wait for reading finished notification
        sem_wait(sem_ready);
        {
            /*
            * Critical section
            */
            sem_wait(sem_sync);
            //write into shared mem
            memcpy(shm->msg, line, MAX_MSG_LEN);
            shm->length = len+1;
            shm->displays_ready = 0;
            sem_post(sem_sync);
        }
        // notify that new msg is ready to read
        sem_post(sem_new_msg);

        if(strcmp(line,"quit") == 0) {
            break;
        }
    }

    printf("done...\n");

    if(munmap(shm,sizeof(struct shared_msg)) == -1) {
        perror(SHAREDMEM_NAME);
    }
    sem_close(sem_new_msg);
    sem_close(sem_sync);
    sem_close(sem_ready);
    sem_close(sem_all_finished);
    sem_unlink(SHM_SEM_SYNC);
    sem_unlink(SHM_SEM_NEW_MSG);
    sem_unlink(SHM_SEM_READY);
    sem_unlink(SHM_SEM_ALL_FINISHED);
    close(fd);
    shm_unlink(SHAREDMEM_NAME);
    return 0;
}
