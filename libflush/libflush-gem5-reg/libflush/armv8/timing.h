/* See LICENSE file for license and copyright information */

#ifndef ARM_V8_TIMING_H
#define ARM_V8_TIMING_H

#include <stdint.h>
#include "memory.h"

#define ARMV8_PMCR_E            (1 << 0) /* Enable all counters */
#define ARMV8_PMCR_P            (1 << 1) /* Reset all counters */
#define ARMV8_PMCR_C            (1 << 2) /* Cycle counter reset */

#define ARMV8_PMUSERENR_EN      (1 << 0) /* EL0 access enable */
#define ARMV8_PMUSERENR_CR      (1 << 2) /* Cycle counter read enable */
#define ARMV8_PMUSERENR_ER      (1 << 3) /* Event counter read enable */

#define ARMV8_PMCNTENSET_EL0_EN (1 << 31) /* Performance Monitors Count Enable Set register */

inline uint64_t
arm_v8_get_timing(void)
{
	uint32_t ret;
	//unsigned counter = 1;
	uint32_t counter_set = 1 & 0x1F;   /*  Mask to leave only bits 4:0 */
	//printf(" counter_set [ %u ]\n", counter_set);
	asm volatile("MSR PMSELR_EL0, %0" : : "r" (counter_set)); /* specify counter number to read */
	asm volatile("ISB");   /* Synchronize context */
	asm volatile("MRS %0, PMXEVCNTR_EL0" : "=r"(ret)); /* read specified counter */
	return ret;
}

inline void
arm_v8_timing_init(void)
{
	uint32_t value = 0;
	//unsigned counter = 0;
	uint32_t event = 0x11;
	uint32_t counter_set_2 = (1 << 1);;
	uint32_t counter_set = 1 & 0x1F;      /*Mask to leave only bits 4:0*/
	/* Enabling PMU user access */
	asm volatile("ISB");
	asm volatile("MRS %0, PMUSERENR_EL0" : "=r" (value));
	//printf("Current value of PMUSERENR_EL0 is %zu\n", value);
	value |= ARMV8_PMUSERENR_EN; 
	value |= ARMV8_PMUSERENR_CR; 
	value |= ARMV8_PMUSERENR_ER;
	//asm volatile("MSR PMUSERENR_EL0, %0" : : "r" (value));   //works only in kernel node probably
	//printf("Configure PMUSERENR_EL0 with %zu\n", value);
	
	/* Enabling the PMU */
	value = 0;
	asm volatile("MRS %0, PMCR_EL0" : "=r" (value));  /* read previous value and add the new config after */
	//printf("Current value of PMCR_EL0 is %zu\n", value);
	value |= ARMV8_PMCR_E; 
	value |= ARMV8_PMCR_C; 
	value |= ARMV8_PMCR_P;
	asm volatile("ISB");
	asm volatile("MSR PMCR_EL0, %0" : : "r" (value));
	
	 /* Configuring an event counter */
	//printf("counter_set is %zu\n", counter_set);
	asm volatile("MSR PMSELR_EL0, %0" : : "r" (counter_set));  /*write the counter number (0-5) to the SEL,bits[4:0] you wish to configure.*/
	asm volatile("ISB");
	asm volatile("MSR  PMXEVTYPER_EL0, %0" : : "r" (event));   /* write the event number (from the event list) to evtCount,bits[7;0], in order to select the event being monitored by the counter.*/
	//printf("event is %zu\n", event);
	
	 /* Enabling a configured event counter  */
	value = 0;
	asm volatile("MRS %0, PMCNTENSET_EL0" : "=r" (value));
	value |= counter_set_2;              /* enable selected counter */
	value |= ARMV8_PMCNTENSET_EL0_EN;   /* enabling the cycle counter */
	asm volatile("ISB");
	asm volatile("MSR PMCNTENSET_EL0, %0" : : "r" (value));
	//printf("enable counter %zu\n", counter);
	
	
	
	
}

inline void
arm_v8_timing_terminate(void)
{
	//unsigned counter = 0;
	uint32_t value = 0;
	uint32_t mask = 0;
	uint32_t counter_set = (1 << 1);


	/* Disable Performance Counter */
	asm volatile("MRS %0, PMCR_EL0" : "=r" (value));
	mask = 0;
	mask |= ARMV8_PMCR_E; /* Enable */
	mask |= ARMV8_PMCR_C; /* Cycle counter reset */
	mask |= ARMV8_PMCR_P; /* Reset all counters */
	asm volatile("ISB");
	asm volatile("MSR PMCR_EL0, %0" : : "r" (value & ~mask));
	
	mask = 0;
	value = 0;

	/* Disable Performance event Counters */
	asm volatile("MRS %0, PMCNTENSET_EL0" : "=r" (value));
	mask = 0;
	mask |= counter_set;
	//asm volatile("MSR PMCNTENSET_EL0, %0" : : "r" (value & ~mask));
	//printf("PMU event counter %zu disabled\n", counter_set);
}

inline void
arm_v8_reset_timing(void)
{
	/* Resetting the event counters */
	uint32_t value = 0;
	asm volatile("MRS %0, PMCR_EL0" : "=r" (value));
	value |= ARMV8_PMCR_P;
	asm volatile("ISB");
	asm volatile("MSR PMCR_EL0, %0" : : "r" (value));
}

#endif  /*ARM_V8_TIMING_H*/
