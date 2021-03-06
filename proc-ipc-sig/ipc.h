#ifndef _IPC_H
#define _IPC_H

/* msg */
#define MSG_SIZE_MAX (4 * 1024)
#define REPLY_MSG "reply"
#define END_MSG "\nend\n"

/* permission */
#define PERM_FILE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

/* shared memory */
#define SHM_NAME "/shm"
#define SHM_SERVER_BUF_IDX (0)
#define SHM_CLIENT_BUF_IDX_MONITOR (1024)



struct shm_msg {
    int status;
    int wait;
    int finish;
    size_t len;
    char msg[MSG_SIZE_MAX];
};

#endif
