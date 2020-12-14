#ifndef _IPC_H
#define _IPC_H

/* msg */
#define MSG_SIZE_MAX (8 * 1024)
#define REPLY_MSG "reply"
#define END_MSG "\nend\n"

/* permission */
#define PERM_FILE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

/* shared memory */
#define SHM_NAME "/shm"
#define SHM_SERVER_BUF_IDX (0)
#define SHM_CLIENT_BUF_IDX_RAND (128)
#define SHM_CLIENT_BUF_IDX_MONITOR (2048)



struct shm_msg {
    int status;
    size_t len;
    char msg[MSG_SIZE_MAX];
};

#endif
