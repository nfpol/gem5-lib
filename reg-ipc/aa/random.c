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

static void print_help(char* argv[]) {
  fprintf(stdout, "Usage: %s [OPTIONS]\n", argv[0]);
  fprintf(stdout, "\t-t, -timing_frame <value>\t TIMING FRAME (default: t 1  --> 1/10 --> 0.1sec )\n");
  fprintf(stdout, "\t-m, -loop_monitor <value>\t  Loop for the moniotr (default:  m 1000)\n");
  fprintf(stdout, "\t-r, -loop_rand <value>\t How many random choices of benchmark attack to execute  (default: -r 20)\n");
  fprintf(stdout, "\t-d, -div <value>\t time divider (default:  div 10)\n");
  fprintf(stdout, "\t-h, -help\t\t Help page\n");
}

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


int main(int argc, char* argv[])
{
		char command[100];
		int div = 10;
		int timing_frame = 1;
		int loop_monitor = 1000;
		int loop_rand = 20;
		FILE* logfile = NULL;
    int randomnumber;
		srand(time(NULL));
		//FILE * fPtr;
		//fPtr = fopen("./monitor/output-reg.dat", "a");
		//if(fPtr == NULL){
		//		/* File not created hence exit */
		//			printf("Unable to create file.\n");
		//			exit(EXIT_FAILURE);
		//}	
			/* 1. Initialize */
		if(ipc_connect()) {
			printf("ipc connect error\n");
			return 0;
		}
		
		char* data = calloc(16, sizeof(data));
	  printf("ipc connect successfull\n");
			/* Parse arguments */
		static const char* short_options = "t:m:r:d:h:";
		/*static struct option long_options[] = {
			{"timing",           required_argument, NULL, 't'},
			{"loop_monitor",     required_argument, NULL, 'm'},
			{"loop_rand",        required_argument, NULL, 'r'},
			{"divider",          required_argument, NULL, 'd'},
			{ NULL,              0, NULL, 0}
		};*/ 
		/*call the server file */
		system("./home/nikos/gem5-lib/reg-ipc/aa/server");
		
		int c;
		while ((c = getopt_long(argc, argv, short_options, NULL)) != -1) {
			switch (c) {
				case 't':
					timing_frame = atoi(optarg);
					if (timing_frame <= 0) {
						fprintf(stderr, "Error: timing frame is negative.\n");
						return -1;
					}
					break;
				case 'm':
					loop_monitor = atoi(optarg);
					if (loop_monitor <= 0) {
						fprintf(stderr, "Error: loop_monitor is negative\n");
						return -1;
					}
					break;
				case 'r':
					loop_rand = atoi(optarg);
					if (loop_rand <= 0) {
						fprintf(stderr, "Error: loop_rand is negative\n");
						return -1;
					}
					break;
				case 'd':
					div = atoi(optarg);
					if (div <= 0) {
						fprintf(stderr, "Error: loop_rand is negative\n");
						return -1;
					}
					break;
				case 'h':
					print_help(argv);
					return 0;
				case '?':
				default:
					fprintf(stderr, "Error: Invalid option '-%c'\n", optopt);
					return -1; 
			}
		}
		sprintf(command, "./monitor/monitor -m %u -t %u -d %u &", loop_monitor, timing_frame, div);
		system(command);
		data = "starting random execution\n";
		ipc_send(data, 16);
		for(int i =0; i<loop_rand; i++) {
			randomnumber = rand() % 4+ 1;
			if (randomnumber==1){
					/*Run libflush example */
				data = "libflush\n";
				ipc_send(data, 16);
				//fprintf(fPtr, "libflush\n");
				//printf("libflush\n");
				chdir("/home/nikos/armageddon/libflush/"); 
				//system("m5 resetstats");
				system("./example/build/armv8/release/bin/example -s 400 -n  1000 -x 1 -z 10");
				//system("m5 dumpstats");
				chdir("/home/nikos/gem5-lib/");	
			}
			else if (randomnumber==2){
				data = "basicmath_small\n";
				ipc_send(data, 16);
				//fprintf(fPtr, "basicmath\n");
				//printf("basicmath\n");
				chdir("/home/nikos/gem5-lib/automotive/basicmath"); 
				//system("m5 resetstats");
				system("./basicmath_small > output_small.txt");
				//system("m5 dumpstats");
				chdir("/home/nikos/gem5-lib/");
			}	
			else if (randomnumber==3){
				data = "bitcount small\n";
				ipc_send(data, 16);
				//printf(""bitcount\n");
				//fprintf(fPtr, "bitcount\n");
				chdir("/home/nikos/gem5-lib/automotive/bitcount"); 
				//system("m5 resetstats");
				system("./bitcnts 75000 > output_small.txt");
				//system("m5 dumpstats");
				chdir("/home/nikos/gem5-lib/");	
			}
			else if (randomnumber==4){
				data = "sha small\n";
				ipc_send(data, 16);
				//printf("sha\n");
				//fprintf(fPtr, "sha\n");
				chdir("/home/nikos/gem5-lib/security/sha"); 
				//system("m5 resetstats");
				system("./sha input_small.asc > output_small.txt");
				//system("m5 dumpstats");
				chdir("/home/nikos/gem5-lib/");	
			}
			else {
				data = "wrong rand\n";
				ipc_send(data, 16);
				//fprintf(fPtr, "nothing");
				//printf("nothing");
			}
			
		}
	
	data = "closing random execution";
	ipc_send(data, 16);
	//fprintf(fPtr, "closing random execution\n");
	//printf("closing random execution\n");
	//system("pkill -f monitor");
	ipc_disconnect();
	printf("ipc disconnect successfull\n");
	//fclose(fPtr);
	return 0;
}
