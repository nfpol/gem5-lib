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
		
	init_pmu();
	select_event();
	reset_event_counters();
	reset_cycle_counter();
	printf("Performance monitor results\n");
	printf("i cache refills---|---retired branches---|---d cahce refills---|---branch predictor misses---|---predictable branch speculatively executed---|---CPU cycles event counter---|---CPU cycles ccnt");
	while(i < 1000){
		reset_event_counters(); //reset event counters
		sleep(0.01);  //sleep 10ms
		//sleep(1);
		event_counters_disable();
		cycle_counter_disable();
		printf("%u   ", get_event_counter(0)); /*get_event_counter(0)*/
		printf("%u   ", get_event_counter(1) );
		printf("%u   ", get_event_counter(2) );
		printf("%u   ", get_event_counter(3) );
		printf("%u   ", get_event_counter(4) );
		printf("%u   ", get_event_counter(5) );
		printf("%u   ", get_event_counter(6) );
		printf("%u\n", get_timing());
		i++;
	}
	
		
  exit(0);
  

}
