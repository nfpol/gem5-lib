#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include "arm_v8.h"

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

	client_msg = (struct shm_msg *)((char*)addr + SHM_CLIENT_BUF_IDX_MONITOR);
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
	char *data = calloc(100, sizeof(data));
	/* send end msg */
	client_msg->status = 0;
	client_msg->len = sizeof(END_MSG) + 100; // be careful to choose the right size
	strncpy(client_msg->msg, END_MSG, client_msg->len);
	memcpy(client_msg->msg + sizeof(END_MSG), data, 100); // todo
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

            
int main(int argc, char* argv[]) {
 /* Define parameters */
	float timing_frame = 0;
	int time = 1;
	int div = 10;
	int loop = 6000;
	FILE* logfile = NULL;
  	int i,j,k = 0;
  //FILE * fPtr;
  //fPtr = fopen("output-reg.dat", "a");
  //if(fPtr == NULL){
  //      /* File not created hence exit */
  //      printf("Unable to create file.\n");
  //      exit(EXIT_FAILURE);
  //}
	
	char* data = calloc(100, sizeof(data));
	
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
	printf("ipc connect successfull\n");
		
	init_pmu();
	select_event();
	reset_event_counters();
	reset_cycle_counter();
	timing_frame = (time/(double)div);
	printf("Performance monitor results\n");
	//data = "PMU monitor is starting monitoring counters\n\n";
	//ipc_send(data, 300);
	//data = "i cache refills---|---retired branches---|---d cache refills---|---branch predictor misses---|---predictable branch speculatively executed---|---CPU cycles event counter---|---CPU cycles ccnt\n";
	//ipc_send(data, 300);
	//fprintf(fPtr, "PMU monitor is starting monitoring counters\n\n");
	//fprintf(fPtr, "i cache refills---|---retired branches---|---d cache refills---|---branch predictor misses---|---predictable branch speculatively executed---|---CPU cycles event counter---|---CPU cycles ccnt\n");
	while(i < loop){
		reset_event_counters(); //reset event counters
		pmu_reset_cycle_counter();
		sleep(timing_frame);  //sleep 10ms
		//sleep(1);
		event_counters_disable();
		cycle_counter_disable();
		
		snprintf(data, 100, "%u                            ", get_event_counter(0)); /*
		ipc_send(data, sizeof(data));
		snprintf(data, sizeof(data), "%u                            ", get_event_counter(1));
		ipc_send(data, sizeof(data));
		snprintf(data, sizeof(data), "%u                            ", get_event_counter(2));
		ipc_send(data, sizeof(data));
		snprintf(data, sizeof(data), "%u                            ", get_event_counter(3));
		ipc_send(data, sizeof(data));
		snprintf(data, sizeof(data), "%u                            ", get_event_counter(4));
		ipc_send(data, sizeof(data));
		snprintf(data, sizeof(data), "%u                            ", get_event_counter(5));
		ipc_send(data, sizeof(data));
		snprintf(data, sizeof(data), "%u                            ", get_event_counter(6));
		ipc_send(data, sizeof(data));
		snprintf(data, sizeof(data), "%lu                            \n", get_timing());
		ipc_send(data, sizeof(data)); */
		//f(fPtr, "%u                            ", get_event_counter(0));
		//f(fPtr, "%u                            ", get_event_counter(1) );
		//f(fPtr, "%u                            ", get_event_counter(2) );
		//f(fPtr, "%u                            ", get_event_counter(3) );
		//f(fPtr, "%u                            ", get_event_counter(4) );
		//f(fPtr, "%u                            ", get_event_counter(5) );
		//f(fPtr, "%u                            ", get_event_counter(6) );
		//f(fPtr, "%lu                            \n", get_timing());
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
	
	data = "PMU monitor is closing ....\n";
	ipc_send(data, sizeof(data));
	printf("PMU monitor is closing ....\n");
	//fprintf(fPtr, "PMU monitor is closing\n");
	//fclose(fPtr);
	
		
  exit(0);
  

}
