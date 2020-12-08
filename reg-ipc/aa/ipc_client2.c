#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ipc.h"

#include <unistd.h>   //for sleep()


/* IPC */
static int ipc_fd;
static void *addr;
static struct shm_msg *client_msg;
static struct shm_msg *server_msg;

static int 
ipc_connect(void)
{
	/* get shm */
	if((ipc_fd = shm_open(SHM_NAME, O_RDWR, PERM_FILE)) == -1) {
			printf("shm_open error : %s\n", strerror(errno));
			return -1;
	}

	/* mmap */
	addr = mmap(NULL, MSG_SIZE_MAX, PROT_READ | PROT_WRITE, MAP_SHARED, ipc_fd, 0);
	if(addr == MAP_FAILED) {
			printf("mmap error : %s\n", strerror(errno));
			goto out;
	}

	client_msg = (struct shm_msg *)((char*)addr + SHM_CLIENT_BUF_IDX_RAND);
	server_msg = (struct shm_msg *)((char*)addr + SHM_SERVER_BUF_IDX);
	
	return 0;

out:
	/* close shm */
    if(munmap(addr, MSG_SIZE_MAX) == -1) {
        printf("munmap error : %s\n", strerror(errno));
	}
    if(close(ipc_fd) == -1) {
        printf("close error : %s\n", strerror(errno));
    }
	
	return -1;
}


static void
ipc_disconnect(void)
{
	char *data = calloc(16, sizeof(data));
	/* send end msg */
	client_msg->status = 0;
	client_msg->len = sizeof(END_MSG) + 16; // be careful to choose the right size
	strncpy(client_msg->msg, END_MSG, client_msg->len);
	memcpy(client_msg->msg + sizeof(END_MSG), data, 16); // todo
	client_msg->status = 1;
	
	/* close shm */
	if(munmap(addr, MSG_SIZE_MAX) == -1) {
		printf("munmap error : %s\n", strerror(errno));
	}

	if(close(ipc_fd) == -1) {
		printf("close error : %s\n", strerror(errno));
	}
}

static int
ipc_send(char* data, size_t size)
{
	/* prepare msg */
	client_msg->status = 0;
	client_msg->len = size;
	
	/* send msg */
	while(1) {
		if(server_msg->status == 1) {	
			memcpy(client_msg->msg, data, client_msg->len);
			client_msg->status = 1;
			break;
		}
		sleep(0);
	}
	
	return 0;
}


int main()
{
	srand(time(NULL));
	/* 1. Initialize */
	if(ipc_connect()) {
		printf("ipc connect error\n");
		return 0;
	}
	printf("ipc connect successfull\n");
	
	/* Send some shit */
	char* data = calloc(16, sizeof(data));
	for (int i = 0; i < 10; i++){
		data = "libflush P1";
		ipc_send(data, 16);
		sleep(rand()%3);
	}
	
	ipc_disconnect();
	printf("ipc disconnect successfull\n");
	
	return 0;
}