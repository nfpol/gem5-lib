#include <stdio.h>


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
	bool execute_loop = true;
	
	
	init_pmu();
	select_event();
	reset_event_counters();
	reset_cycle_counter();

	while(execute_loop == true){
		reset_event_counters(); //reset event counters
		sleep(0.01);  //sleep 10ms
		event_counters_disable();
		cycle_counter_disable();
		printf("\nPerformance monitor results\n\n");
		printf("i cache refills= %u\n", get_event_counter(0)); /*get_event_counter(0)*/
		printf("retired branches= %u\n", get_event_counter(1) );
		printf("d cache refills= %u\n", get_event_counter(2) );
		printf("retired instructions = %u\n", get_event_counter(3) );
		printf("branch predictor misses = %u\n", get_event_counter(4) );
		printf("Predictable branch speculatively executed = %u\n", get_event_counter(5) );
		printf("CPU cycles event counter = %u\n", get_event_counter(6) );
		printf("CPU cycles = %u\n", get_timing());
	}	
  exit(0);
  

}
