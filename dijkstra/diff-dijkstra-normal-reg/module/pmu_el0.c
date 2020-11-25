/*
 * Enable user-mode ARM performance counter access.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/smp.h>


#if !defined(__arm__) && !defined(__aarch64__)
#error Module can only be compiled on ARM.
#endif

/* #define from /lib/modules/uname-r/source/arch/arm64/include/asm/perf_event.h in ASM Aarch 64 */
#define ARMV8_PMCR_MASK         0x3f
#define ARMV8_PMCR_E            (1 << 0) /*  Enable all counters */
#define ARMV8_PMCR_P            (1 << 1) /*  Reset all counters */
#define ARMV8_PMCR_C            (1 << 2) /*  Cycle counter reset */
#define ARMV8_PMCR_N_MASK       0x1f

#define ARMV8_PMUSERENR_EN_EL0  (1 << 0) /*  EL0 access enable */
#define ARMV8_PMUSERENR_CR      (1 << 2) /*  Cycle counter read enable */
#define ARMV8_PMUSERENR_ER      (1 << 3) /*  Event counter read enable */

#define ARMV8_PMCNTENSET_EL0_ENABLE (1<<31) /* *< Enable Perf count reg */

#define PERF_DEF_OPTS (1 | 16)


static int __init
init(void)
{
	u64 val=0;
	/*Enable user-mode access to counters. */
	asm volatile("msr pmuserenr_el0, %0" : : "r"((u64)ARMV8_PMUSERENR_EN_EL0|ARMV8_PMUSERENR_ER|ARMV8_PMUSERENR_CR));
	/*   Performance Monitors Count Enable Set register bit 30:0 disable, 31 enable. Can also enable other event counters here. */ 
	asm volatile("msr pmcntenset_el0, %0" : : "r" (ARMV8_PMCNTENSET_EL0_ENABLE));
	/* Enable counters */

	asm volatile("mrs %0, pmcr_el0" : "=r" (val));
	asm volatile("msr pmcr_el0, %0" : : "r" (val|ARMV8_PMCR_E));
	printk(KERN_INFO "ENABLE_ARM_PMU Initialized");
	return 0;
}


static void __exit
leave(void)
{
	printk(KERN_INFO "ENABLE_ARM_PMU Unloaded");
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Enables user-mode access to ARMv8 Aarch64 PMU counters");
MODULE_VERSION("1:0.0-dev");
module_init(init);
module_exit(leave);