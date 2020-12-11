#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>


#include <unistd.h>   //for sleep()
#include "./monitor/arm_v8.h"

static void print_help(char* argv[] ) {
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
		int loop_monitor = 1000;
		int loop_rand = 20;
		uint64_t cycles = 0;
		FILE* logfile = NULL;
    int randomnumber;
		srand(time(NULL));
		FILE * fPtr;
		system("./delete-out");
		asm volatile("ISB");
		fPtr = fopen("output.dat", "w");
		if(fPtr == NULL){
				/* File not created hence exit */
					printf("Unable to create file.\n");
					exit(EXIT_FAILURE);
		}	
			/* Parse arguments */
		static const char* short_options = "t:m:r:d:h:";
		/*static struct option long_options[] = {
			{"timing",           required_argument, NULL, 't'},
			{"loop_monitor",     required_argument, NULL, 'm'},
			{"loop_rand",        required_argument, NULL, 'r'},
			{"divider",          required_argument, NULL, 'd'},
			{ NULL,              0, NULL, 0}
		};*/ 
	
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
		asm volatile("ISB");
		sprintf(command, "./monitor/monitor -m %u -t %u -d %u &", loop_monitor, timing_frame, div);
		system(command);
		asm volatile("ISB");
		for(int i =0; i<loop_rand; i++) {
			randomnumber = rand() % 4+ 1;
			if (randomnumber==1){
				pmu_cycle_counter_disable();
				pmu_event_counters_disable_all();
				cycles = get_timing();
				fprintf(fPtr, "libflush started execution at %lu CPU cycles\n", cycles);
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
				cycles = get_timing();
				pmu_enable_all_counters();
				fprintf(fPtr, "basicmath small started execution at %lu CPU cycles\n", cycles);
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
				cycles = get_timing();
				fprintf(fPtr, "bitcount small started execution at %lu CPU cycles\n", cycles);
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
				cycles = get_timing();
				fprintf(fPtr, "sha large started execution at %lu CPU cycles\n", cycles);
				asm volatile("ISB");
				pmu_enable_cycle_counter();
				pmu_enable_all_counters();
				chdir("/home/nikos/gem5-lib/security/sha");
				system("./sha input_large.asc > output_large.txt");
				chdir("/home/nikos/gem5-lib/");	
			}
			else {
				fprintf(fPtr, "nothing");
			}
			
		}
	pmu_cycle_counter_disable();
	pmu_event_counters_disable_all();
	fprintf(fPtr, "closing random execution\n");
	//system("pkill -f monitor");
	fclose(fPtr);
	return 0;
}
