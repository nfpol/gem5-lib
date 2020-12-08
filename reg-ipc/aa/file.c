#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "ipc.h"

/* IPC */
int server_fd;
void *addr;
struct shm_msg *client_msg;
struct shm_msg *client_monitor;
struct shm_msg *server_msg;
char msg[MSG_SIZE_MAX] = {0,};

int main(int argc, char **argv)
{
	int i;
	FILE * fPtr;
	fPtr = fopen("./monitor/output-reg.dat", "a");
	if(fPtr == NULL){
		/* File not created hence exit */
		printf("Unable to create file.\n");
		exit(EXIT_FAILURE);
	}	
	/* create shm */
    if((server_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, PERM_FILE)) == -1) {
        printf("shm_open error : %s\n", strerror(errno));
        return -1;
    }
	
	/* set size */
    if(ftruncate(server_fd, MSG_SIZE_MAX) == -1) {
        printf("ftruncate error : %s\n", strerror(errno));
        goto out;
    }
	
	/* mmap */
    addr = mmap(NULL, MSG_SIZE_MAX, PROT_READ | PROT_WRITE, MAP_SHARED, server_fd, 0);
    if(addr == MAP_FAILED) {
        printf("mmap error : %s\n", strerror(errno));
        goto out;
    }
    memset(addr, 0, MSG_SIZE_MAX);
	
	client_msg = (struct shm_msg *)((char*)addr + SHM_CLIENT_BUF_IDX_RAND);
	client_monitor = (struct shm_msg *)((char*)addr + SHM_CLIENT_BUF_IDX_MONITOR);
	server_msg = (struct shm_msg *)((char*)addr + SHM_SERVER_BUF_IDX);
	server_msg->status = 1;
		
	printf("file is running...\n");
	
	while(1) {
		/* read msg */
		while(1) {
			if(client_msg->status == 1) {
				server_msg->status = 0;
				memcpy(msg, client_msg->msg, client_msg->len);
				fprintf(fPtr, "%s", msg);
				printf("receive msg : %s\n", msg);
				client_msg->status = 0;
				server_msg->status = 1;
				break;
			}
			if(client_monitor->status == 1) {
				client_monitor->status = 0;
				memcpy(msg, client_monitor->msg, client_monitor->len);
				fprintf(fPtr, "%s", msg);
				printf("receive msg : %s\n", msg);
				client_monitor->status = 0;
				server_msg->status = 1;
				break;
			}
			sleep(0);
		}
		
		if(client_msg->len == (sizeof(END_MSG) + 16)) {
			printf("end msg!!\n");
			break;
		}
	}
	
	printf("file is closing...\n");

out:
	/* destroy shm */
    if(munmap(addr, MSG_SIZE_MAX) == -1) {
        printf("munmap error : %s\n", strerror(errno));
    }
	if(close(server_fd) == -1) {
        printf("close error : %s\n", strerror(errno));
    }
	if(shm_unlink(SHM_NAME) == -1) {
        printf("shm_unlink error : %s\n", strerror(errno));
    }
	return 0;
}

