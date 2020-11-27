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

             
int main(void) {
  int i,j,k = 0;
  FILE * fPtr;
  fPtr = fopen("output.dat", "w");
  if(fPtr == NULL){
        /* File not created hence exit */
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
  }
		
	init_pmu();
	select_event();
	reset_event_counters();
	reset_cycle_counter();
	printf("Performance monitor results\n");
	fprintf(fPtr, "i cache refills---|---retired branches---|---d cache refills---|---branch predictor misses---|---predictable branch speculatively executed---|---CPU cycles event counter---|---CPU cycles ccnt\n");
	while(i < 1000){
		reset_event_counters(); //reset event counters
		sleep(0.01);  //sleep 10ms
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
		fprintf(fPtr, "%u                            \n", get_timing());
		i++;
	}
	

	printf("PMU monitor is closing ....\n");
	fclose(fPtr);
	
		
  exit(0);
  

}
