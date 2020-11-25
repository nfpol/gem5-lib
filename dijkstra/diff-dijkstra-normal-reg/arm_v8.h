#ifndef ARM_V8_TIMING_H
#define ARM_V8_TIMING_H
#include <stdint.h>
#include <stdio.h>
#include<stdlib.h>


/* ARMV8 PMU related */                                                                        
#define ARMV8_PMCR_E            (1 << 0)   /* Enable all counters */
#define ARMV8_PMCR_P            (1 << 1)   /* Reset all counters */
#define ARMV8_PMCR_C            (1 << 2)   /* Cycle counter reset */

#define ARMV8_PMUSERENR_EN      (1 << 0) /* EL0 access enable */
#define ARMV8_PMUSERENR_CR      (1 << 2) /* Cycle counter read enable */
#define ARMV8_PMUSERENR_ER      (1 << 3) /* Event counter read enable */

#define ARMV8_PMCNTENSET_EL0_EN (1 << 31)  



inline void pmu_enable_event_counter(unsigned counter, uint32_t event_type)  
{
	/* Setup PMU counter to record specific event */
	/* evtCount is the event id */
	uint32_t event = event_type & 0x1F;
	uint32_t counter_set = (1 << counter);
	uint32_t r = 0;
	asm volatile("isb");
	if (counter == 0 ) asm volatile("msr pmevtyper0_el0, %0" : : "r" (event));
	else if ( counter == 1) asm volatile("msr pmevtyper1_el0, %0" : : "r" (event));
	else if ( counter == 2) asm volatile("msr pmevtyper2_el0, %0" : : "r" (event));
	else if ( counter == 3) asm volatile("msr pmevtyper3_el0, %0" : : "r" (event));
	else if ( counter == 4) asm volatile("msr pmevtyper4_el0, %0" : : "r" (event));
	else if ( counter == 5) asm volatile("msr pmevtyper5_el0, %0" : : "r" (event));
	else printf("wrong counter number\n");
	asm volatile("isb");
	/*   Performance Monitors Count Enable Set register bit counter_set */
	asm volatile("mrs %0, pmcntenset_el0" : "=r" (r));
	asm volatile("msr pmcntenset_el0, %0" : : "r" (r|counter_set));
	
}

inline void pmu_reset_cycle_counter(void) /* Resetting the cycle counter */
{
	uint32_t value = 0;
	asm volatile("MRS %0, PMCR_EL0" : "=r" (value));
	value |= ARMV8_PMCR_C;
	asm volatile ("ISB");
	asm volatile("MSR PMCR_EL0, %0" : : "r" (value));
}

inline void pmu_reset_event_counters_all(void) /* Resetting the event counters */
{
	uint32_t value = 0;
	asm volatile("MRS %0, PMCR_EL0" : "=r" (value));
	value |= ARMV8_PMCR_P;
	asm volatile ("ISB");
	asm volatile("MSR PMCR_EL0, %0" : : "r" (value));
}

inline uint64_t get_timing(void)
{
	uint64_t result = 0;
	asm volatile("isb");
	asm volatile("mrs %0, pmccntr_el0" : "=r" (result));
	return result;
}

inline uint32_t get_event_counter(unsigned counter)
{
	uint32_t ret;
	asm volatile("isb");
	if (counter==0) asm volatile("mrs %0, pmevcntr0_el0" : "=r" (ret));
	else if  (counter == 1) asm volatile("mrs %0, pmevcntr1_el0" : "=r" (ret));
	else if  (counter == 2) asm volatile("mrs %0, pmevcntr2_el0" : "=r" (ret));
	else if  (counter == 3) asm volatile("mrs %0, pmevcntr3_el0" : "=r" (ret));
	else if  (counter == 4) asm volatile("mrs %0, pmevcntr4_el0" : "=r" (ret));
	else if  (counter == 5) asm volatile("mrs %0, pmevcntr5_el0" : "=r" (ret));
	else printf("wrong counter number\n");
	return ret;
}


inline void pmu_disable(void)
{
  uint32_t value = 0;
  uint32_t mask = 0;

  /* Disable Performance Counter */
  asm volatile("MRS %0, PMCR_EL0" : "=r" (value));
  mask = 0;
  mask |= ARMV8_PMCR_E; /* Enable */
  mask |= ARMV8_PMCR_C; /* Cycle counter reset */
  mask |= ARMV8_PMCR_P; /* Reset all counters */
  asm volatile ("ISB");
  asm volatile("MSR PMCR_EL0, %0" : : "r" (value & ~mask));
}

inline void pmu_cycle_counter_disable(void)
{
	uint32_t value = 0;
	uint32_t mask = 0;
	/* Disable cycle counter register */
	asm volatile("MRS %0, PMCNTENSET_EL0" : "=r" (value));
	mask = 0;
	mask |= ARMV8_PMCNTENSET_EL0_EN;
	asm volatile ("ISB");
	asm volatile("MSR PMCNTENSET_EL0, %0" : : "r" (value & ~mask));
	//printf("CCNT cycle counter disabled\n");
}

inline void pmu_event_counters_disable(unsigned counter){
	uint32_t mask = 0;
	uint32_t value = 0;
	uint32_t counter_set = (1 << counter);
	/* Disable Performance event Counters */
	asm volatile("MRS %0, PMCNTENSET_EL0" : "=r" (value));
	mask = 0;
	mask |= counter_set;
	asm volatile("MSR PMCNTENSET_EL0, %0" : : "r" (value & ~mask));
	//printf("PMU event counter %zu disabled\n", counter_set);
}




#endif   /*ARM_V8_TIMING_H*/
