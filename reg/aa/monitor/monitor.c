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

extern void init_pmu(void);
extern void select_event(void);
extern void reset_event_counters(void);
extern void reset_cycle_counter(void);
extern void event_counters_disable(void);
extern void cycle_counter_disable(void);

             
int main(int argc, char* argv[]) {
 /* Define parameters */
	double timing_frame = 0;
	int time = 0;
	int div = 0;
	int loop = 0;
	FILE* logfile = NULL;
  int i,j,k = 0;
  FILE * fPtr;
  fPtr = fopen("output.dat", "w");
  if(fPtr == NULL){
        /* File not created hence exit */
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
  }
	
	/* Parse arguments */
  static const char* short_options = "t:lm:d";
	
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
      case 'lm':
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
				/*
      case 'h':
        print_help(argv);
        return 0;
      case '?':
      default:
        fprintf(stderr, "Error: Invalid option '-%c'\n", optopt);
        return -1; */
    }
  }
		
	init_pmu();
	select_event();
	reset_event_counters();
	reset_cycle_counter();
	timing_frame = (time/div);
	printf("%f", timing_frame);
	printf("Performance monitor results\n");
	fprintf(fPtr, "i cache refills---|---retired branches---|---d cache refills---|---branch predictor misses---|---predictable branch speculatively executed---|---CPU cycles event counter---|---CPU cycles ccnt\n");
	while(i < loop){
		reset_event_counters(); //reset event counters
		pmu_reset_cycle_counter();
		sleep(timing_frame);  //sleep 10ms
		//sleep(1);
		event_counters_disable();
		cycle_counter_disable();
		fprintf(fPtr, "%u                            ", get_event_counter(0));
		fprintf(fPtr, "%u                            ", get_event_counter(1) );
		fprintf(fPtr, "%u                            ", get_event_counter(2) );
		fprintf(fPtr, "%u                            ", get_event_counter(3) );
		fprintf(fPtr, "%u                            ", get_event_counter(4) );
		fprintf(fPtr, "%u                            ", get_event_counter(5) );
		fprintf(fPtr, "%u                            ", get_event_counter(6) );
		fprintf(fPtr, "%lu                            \n", get_timing());
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
	

	printf("PMU monitor is closing ....\n");
	fclose(fPtr);
	
		
  exit(0);
  

}
