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
#include <string.h>

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


int main(int argc, char* argv[])
{
		char command[100];
		int div = 10;
		int timing_frame = 1;
		int loop_monitor = 60000;
		int loop_rand = 20;
		FILE* logfile = NULL;
    int randomnumber;
		srand(time(NULL));

		char* data = calloc(32, sizeof(data));
	 
		/* Parse arguments */
		static const char* short_options = "t:m:r:d:h:";
		
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
		
		FILE * fPtr;
		fPtr = fopen("./monitor/output-reg.dat", "w");
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
		
		client_msg = (struct shm_msg *)((char*)addr + HM_CLIENT_BUF_IDX_MONITOR);
		server_msg = (struct shm_msg *)((char*)addr + SHM_SERVER_BUF_IDX);
		server_msg->status = 1;
			
		printf("server is running...\n");
		fprintf(fPtr, "random execution started ...\n");
		sprintf(command, "nice --2 ./monitor/monitor -m %u -t %u -d %u &", loop_monitor, timing_frame, div);
		system(command);
		for(int i =0; i<loop_rand; i++) {
			randomnumber = rand() % 4+ 1;
			while(1) {
				if(client_msg->status == 1) {
					server_msg->status = 0;
					memcpy(msg, client_msg->msg, client_msg->len);
					//fprintf(fPtr, "%s", client_msg->msg);
					fprintf(fPtr, "%s", msg);
					client_msg->status = 0;
					server_msg->status = 1;
					break;
				}
			}
			if (randomnumber==1){
				fprintf(fPtr, "libflush ...\n");
				chdir("/home/nikos/armageddon/libflush/"); 
				system("./example/build/armv8/release/bin/example -s 400 -n  1000 -x 1 -z 10");
				chdir("/home/nikos/gem5-lib/");	
			}
			else if (randomnumber==2){
				fprintf(fPtr, "basicmath small ...\n");
				chdir("/home/nikos/gem5-lib/automotive/basicmath");
				system("./basicmath_small > output_small.txt");
				chdir("/home/nikos/gem5-lib/");
			}	
			else if (randomnumber==3){
				fprintf(fPtr, "bitcount small ...\n");
				chdir("/home/nikos/gem5-lib/automotive/bitcount"); 
				system("./bitcnts 75000 > output_small.txt");
				chdir("/home/nikos/gem5-lib/");	
			}
			else if (randomnumber==4){
				fprintf(fPtr, "sha small ...\n");
				chdir("/home/nikos/gem5-lib/security/sha"); 
				system("./sha input_small.asc > output_small.txt");
				chdir("/home/nikos/gem5-lib/");	
			}
			else {
				fprintf(fPtr, "wrong rand ...\n");
			}
			
		}
	
	fprintf(fPtr, "closing random execution ...\n");
	//system("pkill -f monitor");
	ipc_disconnect();
	printf("ipc disconnect successfull\n");
	fclose(fPtr);
	return 0;
}
