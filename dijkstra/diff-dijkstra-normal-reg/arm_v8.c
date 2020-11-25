//#include <linux/module.h>       /* Needed by all modules */
#include <linux/kernel.h>       /* Needed for KERN_ERR */
#include <linux/fs.h>
#include <linux/wait.h>
#include "arm_v8.h"
#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>


unsigned seconds=5;


void select_event(void)
{
	/*  GEM5 events are different than arm documented   ------->    look in gem5/arch/arm/ArmPMU*/
	pmu_enable_event_counter(0, 0x02);  // i cache refills
	asm volatile("ISB"); 
	printf("config event 0 refills in the instruction cache\n");
	pmu_enable_event_counter(1, 0x21);  // retired branches
	asm volatile("ISB"); 
	printf("config event 1 retired branches\n");
	pmu_enable_event_counter(2, 0x05);  // d cache refills
	asm volatile("ISB"); 
	printf("config event 2 refills in the data cache\n");
 	pmu_enable_event_counter(3, 0x08);  // retired instructions
 	asm volatile("ISB"); 
	printf("config event 3  retired instructions\n");
	pmu_enable_event_counter(4, 0x10);  // branch predictor misses
	asm volatile("ISB"); 
	printf("config event 4 branch predictor misses\n");
	pmu_enable_event_counter(5, 0x12);  // Predictable branch speculatively executed
	asm volatile("ISB"); 
	printf("config event 5 branches\n");
	//sleep( seconds );
	pmu_enable_config_counter(0);
	asm volatile("ISB"); 
	pmu_enable_config_counter(1);
	asm volatile("ISB"); 
	pmu_enable_config_counter(2);
	asm volatile("ISB"); 
	pmu_enable_config_counter(3);
	asm volatile("ISB"); 
	pmu_enable_config_counter(4);
	asm volatile("ISB"); 
	pmu_enable_config_counter(5);
	asm volatile("ISB"); 
	//sleep( seconds );
}

void cycle_counter_disable(void){
	pmu_cycle_counter_disable();  // Stop CCNT cycle counter 
}

void event_counters_disable(void){
	pmu_event_counters_disable(0);   // Stop event counter 0
	asm volatile("ISB"); 
	pmu_event_counters_disable(1);   // Stop event counter 1
	asm volatile("ISB"); 
	pmu_event_counters_disable(2);   // Stop event counter 2
	asm volatile("ISB"); 
	pmu_event_counters_disable(3);   // Stop event counter 3
	asm volatile("ISB"); 
	pmu_event_counters_disable(4);   // Stop event counter 4
	asm volatile("ISB"); 
	pmu_event_counters_disable(5);   // Stop event counter 5
	asm volatile("ISB"); 
}

void reset_event_counters(void)
{	
	pmu_reset_event_counters_all();  // Reset the configurable counters
}

void reset_cycle_counter(void)
{	
	pmu_reset_cycle_counter();  // Reset the CCNT (cycle counter)
}







