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
#include "arm_v8.h"

#include <unistd.h>   //for sleep()


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
		fPtr = fopen("output.dat", "w");
		asm volatile ("ISB");
		if(fPtr == NULL){
			/* File not created hence exit */
			printf("Unable to create file.\n");
			exit(EXIT_FAILURE);
		}	
		
		rand_status = 1;
		asm volatile ("ISB");
		rand_wait = 0;
		sprintf(command, "./monitor -m %u -t %u -d %u &", loop_monitor, timing_frame, div);
		system(command);
		while(1){
			if(monitor_status == 1 && monitor_wait == 0){ 
				break;
			}
		}
		
		
		asm volatile ("ISB");
		fprintf(fPtr, "random execution started ...\n");
		for(int i =0; i<loop_rand; i++) {
			random_execution();
		}
		//maybe it will work as when random execution stop we dont care
		pmu_cycle_counter_disable();
		pmu_event_counters_disable_all();
		rand_finish = 1;
		
		while(1){
			if(monitor_finish == 1) {
				break;
			}
		}
		rand_status = 0;

		fprintf(fPtr, "closing random execution ...\n");
		//system("pkill -f monitor");
		fclose(fPtr);
		return 0;
}
