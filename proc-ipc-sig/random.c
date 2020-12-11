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
#include "./monitor/arm_v8.h"

#include <unistd.h>   //for sleep()

/* IPC */
int server_fd;
static int ipc_fd;
static void *addr;
static struct shm_msg *client_msg;
static struct shm_msg *server_msg;
static int randomnumber = 0;
FILE * fPtr;

static void print_help(char* argv[]) {
  fprintf(stdout, "Usage: %s [OPTIONS]\n", argv[0]);
  fprintf(stdout, "\t-t, -timing_frame <value>\t TIMING FRAME (default: t 1  --> 1/10 --> 0.1sec )\n");
  fprintf(stdout, "\t-m, -loop_monitor <value>\t  Loop for the moniotr (default:  m 1000)\n");
  fprintf(stdout, "\t-r, -loop_rand <value>\t How many random choices of benchmark attack to execute  (default: -r 20)\n");
  fprintf(stdout, "\t-d, -div <value>\t time divider (default:  div 10)\n");
  fprintf(stdout, "\t-h, -help\t\t Help page\n");
}

static void random_execution(void){
	randomnumber = rand() % 4+ 1;
	if (randomnumber==1){
		pmu_cycle_counter_disable();
		pmu_event_counters_disable_all();
		fprintf(fPtr, "libflush ...\n");
		asm volatile("ISB");
		pmu_enable_cycle_counter();
		pmu_enable_all_counters();
		chdir("/home/nikos/armageddon/libflush/"); 
		system("./example/build/armv8/release/bin/example -s 400 -n  1000 -x 1 -z 10");
		chdir("/home/nikos/gem5-lib/");	
	}
	else if (randomnumber==2){
		pmu_cycle_counter_disable();
		pmu_event_counters_disable_all();
		fprintf(fPtr, "basicmath small ...\n");
		asm volatile("ISB");
		pmu_enable_cycle_counter();
		pmu_enable_all_counters();
		chdir("/home/nikos/gem5-lib/automotive/basicmath");
		system("./basicmath_small > output_small.txt");
		chdir("/home/nikos/gem5-lib/");
	}	
	else if (randomnumber==3){
		pmu_cycle_counter_disable();
		pmu_event_counters_disable_all();
		fprintf(fPtr, "bitcount small ...\n");
		asm volatile("ISB");
		pmu_enable_cycle_counter();
		pmu_enable_all_counters();
		chdir("/home/nikos/gem5-lib/automotive/bitcount"); 
		system("./bitcnts 75000 > output_small.txt");
		chdir("/home/nikos/gem5-lib/");	
	}
	else if (randomnumber==4){
		pmu_cycle_counter_disable();
		pmu_event_counters_disable_all();
		fprintf(fPtr, "sha small ...\n");
		asm volatile("ISB");
		pmu_enable_cycle_counter();
		pmu_enable_all_counters();
		chdir("/home/nikos/gem5-lib/security/sha"); 
		system("./sha input_small.asc > output_small.txt");
		chdir("/home/nikos/gem5-lib/");	
	}
	else {
		fprintf(fPtr, "wrong rand ...\n");
	}
				

}

int main(int argc, char* argv[])
{
		char command[100];
		int div = 10;
		int timing_frame = 1;
		int loop_monitor = 1000;
		int loop_rand = 20;
		FILE* logfile = NULL;
		srand(time(NULL));

		char* data = calloc(32, sizeof(data));
		system("./delete-out");	 
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
		
		//FILE * fPtr;
		fPtr = fopen("./output.dat", "w");
		asm volatile ("ISB");
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
		
		client_msg = (struct shm_msg *)((char*)addr + SHM_CLIENT_BUF_IDX_MONITOR);
		server_msg = (struct shm_msg *)((char*)addr + SHM_SERVER_BUF_IDX);
		server_msg->status = 1;
		server_msg->wait = 1;
		server_msg->finish = 0;
		client_msg->status = 0;
		client_msg->wait = 1;
		client_msg->finish = 0;
			
		printf("server is running...\n");
		
		asm volatile ("ISB");
		server_msg->wait = 0;
		sprintf(command, "nice --2 ./monitor/monitor -m %u -t %u -d %u &", loop_monitor, timing_frame, div);
		system(command);
		
		while(1){
			printf("waiting");
			if(client_msg->status == 1 && client_msg->wait == 0){
				printf("monitor status %u and monitor wait %u \n", client_msg->status, client_msg->wait); 
				break;
			}
		}
		
		
		asm volatile ("ISB");
		fprintf(fPtr, "random execution started ...\n");
		for(int i =0; i<loop_rand; i++) {
			random_execution();
		}
		printf("hell yeah_v3\n");
		server_msg->finish = 1;
		printf("hell yeah_v4\n");
		while(1){
			printf("oh my god\n");
			if(client_msg->finish == 1) break;
		}
		printf("hell yeah_v5\n");
		//maybe it will work as when random execution stop we dont care
		pmu_cycle_counter_disable();
		pmu_event_counters_disable_all();
		fprintf(fPtr, "closing random execution ...\n");
		//system("pkill -f monitor");
		
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
		printf("oh yes\n");
		
		fclose(fPtr);
		return 0;
}
