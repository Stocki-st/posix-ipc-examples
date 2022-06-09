#ifndef SHARE_H
#define SHARE_H

#define MAX_MSG_LEN 256
#define SHAREDMEM_NAME "/is211818_shm"
#define SHM_SEM_SYNC "/is211818_sem_sync"
#define SHM_SEM_NEW_MSG "/is211818_sem_new_msg"
#define SHM_SEM_READY "/is211818_sem_ready"


struct shared_msg {
    //length of msg
    //saved string also includes null terminator
    // --> length = strlen(msg) + 1
    int length;
    char msg[MAX_MSG_LEN];
};


#endif
