#ifndef _IPC_H
#define _IPC_H

/* msg */
#define MSG_SIZE_MAX (8 * 1024)
#define REPLY_MSG "reply"
#define END_MSG "\nend of execution\n"

/* permission */
#define PERM_FILE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

/* shared memory */
#define SHM_NAME "/shm"
#define SHM_CLIENT_BUF_IDX_RAND (0)
#define SHM_CLIENT_BUF_IDX_MONITOR (1024)
#define SHM_SERVER_BUF_IDX (4096)


struct shm_msg {
    int status;
    size_t len;
    char msg[MSG_SIZE_MAX];
};

#endif
