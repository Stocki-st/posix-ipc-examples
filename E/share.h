#ifndef SHARE_H
#define SHARE_H

#define MAX_MSG_LEN 256
#define SHAREDMEM_NAME "/is211818_shm"
#define SHM_SEM_SYNC "/is211818_sem_sync"
#define SHM_SEM_NEW_MSG "/is211818_sem_new_msg"
#define SHM_SEM_READY "/is211818_sem_ready"
#define SHM_SEM_ALL_READY "/is211818_sem_all_ready"


struct shared_msg {
    int num_of_displays;
    int displays_ready;
    //length of msg
    //saved string also includes null terminator
    // --> length = strlen(msg) + 1
    int length;
    char msg[MAX_MSG_LEN];
};


#endif
