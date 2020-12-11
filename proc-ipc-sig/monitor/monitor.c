#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include "arm_v8.h"
#include <string.h>
#include <unistd.h>   //for sleep()

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../ipc.h"

/* IPC */
static int ipc_fd;
static void *addr;
static struct shm_msg *client_msg;
static struct shm_msg *server_msg;

extern void init_pmu(void);
extern void select_event(void);
extern void reset_event_counters(void);
extern void reset_cycle_counter(void);
extern void event_counters_disable(void);
extern void cycle_counter_disable(void);

 
static void print_help(char* argv[]) {
  fprintf(stdout, "Usage: %s [OPTIONS]\n", argv[0]);
  fprintf(stdout, "\t-t, -time <value>\t TIME (default: t 1  --> 1/10 --> 0.1sec )\n");
  fprintf(stdout, "\t-m, -loop_monitor <value>\t  Loop for the moniotr (default:  m 1000)\n");
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

	client_msg = (struct shm_msg *)((char*)addr + define SHM_CLIENT_BUF_IDX_MONITOR);
	server_msg = (struct shm_msg *)((char*)addr + SHM_SERVER_BUF_IDX);
	client_msg->status = 0;
	client_msg->wait = 1;
	client_msg->finish = 0;
	
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
	/* close shm */
	if(munmap(addr, MSG_SIZE_MAX) == -1) {
		printf("munmap error : %s\n", strerror(errno));
	}

	if(close(ipc_fd) == -1) {
		printf("close error : %s\n", strerror(errno));
	}
	client_msg->finish = 1;
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

            
int main(int argc, char* argv[]) {
 /* Define parameters */
	float timing_frame = 0;
	int time = 1;
	int div = 10;
	int loop = 60000;
	FILE* logfile = NULL;
  int i,j,k = 0;
 	//char buffer [256];
	
	//char* data = calloc(256, sizeof(data));
	
	/* Parse arguments */
  	static const char* short_options = "t:m:d:h:";
	
	int c;
  	while ((c = getopt_long(argc, argv, short_options, NULL)) != -1) {
		switch (c) {
      case 't':
        time = atoi(optarg);
        if (time <= 0) {
          fprintf(stderr, "Error: timing frame is negative.\n");
          return -1;
        }
        break;
      case 'm':
        loop = atoi(optarg);
        if (loop <= 0) {
          fprintf(stderr, "Error: loop is negative\n");
          return -1;
        }
        break;
			case 'd':
        div = atoi(optarg);
        if (div <= 0) {
          fprintf(stderr, "Error: loop is negative\n");
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
 	
 	/* 1. Initialize */
	if(ipc_connect()) {
		printf("ipc connect error\n");
		return 0;
	}
	client_msg->status = 1;
	client_msg->wait = 1;
	client_msg->finish = 0;
	printf("ipc connect successfull\n");
	FILE * fPtr;
	fPtr = fopen("./output-reg.dat", "w");
	asm volatile ("ISB");
	if(fPtr == NULL){
		/* File not created hence exit */
		printf("Unable to create file.\n");
		exit(EXIT_FAILURE);
	}	
	init_pmu();
	select_event();
	reset_event_counters();
	reset_cycle_counter();
	timing_frame = (time/(double)div);
	printf("Performance monitor results\n");
	//data = "PMU monitor is starting monitoring counters\n\n";
	//ipc_send(data, 256);
	//data = "i cache refills---|---retired branches---|---d cache refills---|---branch predictor misses---|---predictable branch speculatively executed---|---CPU cycles event counter---|---CPU cycles ccnt\n";
	//ipc_send(data, 256);
	
	while(1) {
		if (server_msg->status == 1 && server_msg->wait == 0) break;
	}
	
	
	while(i < loop && server_msg->finish == 0){
		
		reset_event_counters(); //reset event counters
		//pmu_reset_cycle_counter();
		
		if (i == 0) client_msg->wait = 0;
		
		sleep(timing_frame);  //sleep 0.1s (default)
		event_counters_disable();
		cycle_counter_disable();
		
		
		//snprintf(buffer, 256, "%u     %u     %u     %u     %u     %u     %u     %lu\n", get_event_counter(0), get_event_counter(1), get_event_counter(2), get_event_counter(3), get_event_counter(4), get_event_counter(5), get_event_counter(6), get_timing());
		//data = buffer;
		//ipc_send(data, 256);

		i++;
		pmu_enable_config_counter(0);
		pmu_enable_config_counter(1);
		pmu_enable_config_counter(2);
		pmu_enable_config_counter(3);
		pmu_enable_config_counter(4);
		pmu_enable_config_counter(5);
		pmu_enable_config_counter(6);
		pmu_enable_all_counters();
	}
	
	//data = "\nPMU monitor is closing ....\n";
	//ipc_send(data, 256);
	printf("PMU monitor is closing ....\n");
	fprintf(fPtr, "PMU monitor is closing\n");
	fclose(fPtr);
	ipc_disconnect();
		
  exit(0);
  

}
