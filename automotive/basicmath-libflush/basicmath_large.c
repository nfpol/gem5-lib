#include "snipmath.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include "arm_v8.h"



extern void init_pmu(void);
extern void select_event(void);
extern void reset_event_counters(void);
extern void reset_cycle_counter(void);
/* The printf's may be removed to isolate just the math calculations */

int main(void)
{	
	int cycles_normal = 0;
	int diff = 0;
	chdir("/home/nikos/XKCP/support/Kernel-PMU/");
  system("./load-module");
	chdir("/home/nikos/lib-gem5/automotive/basicmath-libflush/");
	char user_input[5];
	double  a1 = 1.0, b1 = -10.5, c1 = 32.0, d1 = -30.0;
	double  x[3];
	double X;
	int     solutions;
	int i;
	unsigned long l = 0x3fed0169L;
	struct int_sqrt q;
	long n = 0;
	
	init_pmu();
	select_event();
	reset_event_counters();
	reset_cycle_counter();

	//printf("Enter five letters:\n ");
	//fgets(user_input, 5, stdin);
	//printf("You entered %s! \n", user_input);
	
	/* solve soem cubic functions */
	printf("********* CUBIC FUNCTIONS ***********\n");
	/* should get 3 solutions: 2, 6 & 2.5   */
	SolveCubic(a1, b1, c1, d1, &solutions, x);  
	printf("Solutions:");
	for(i=0;i<solutions;i++)
		printf(" %f",x[i]);
	printf("\n");

	a1 = 1.0; b1 = -4.5; c1 = 17.0; d1 = -30.0;
	/* should get 1 solution: 2.5           */
	SolveCubic(a1, b1, c1, d1, &solutions, x);  
	printf("Solutions:");
	for(i=0;i<solutions;i++)
		printf(" %f",x[i]);
	printf("\n");

	a1 = 1.0; b1 = -3.5; c1 = 22.0; d1 = -31.0;
	SolveCubic(a1, b1, c1, d1, &solutions, x);
	printf("Solutions:");
	for(i=0;i<solutions;i++)
		printf(" %f",x[i]);
	printf("\n");

	a1 = 1.0; b1 = -13.7; c1 = 1.0; d1 = -35.0;
	SolveCubic(a1, b1, c1, d1, &solutions, x);
	printf("Solutions:");
	for(i=0;i<solutions;i++)
		printf(" %f",x[i]);
	printf("\n");

	a1 = 3.0; b1 = 12.34; c1 = 5.0; d1 = 12.0;
	SolveCubic(a1, b1, c1, d1, &solutions, x);
	printf("Solutions:");
	for(i=0;i<solutions;i++)
		printf(" %f",x[i]);
	printf("\n");

	a1 = -8.0; b1 = -67.89; c1 = 6.0; d1 = -23.6;
	SolveCubic(a1, b1, c1, d1, &solutions, x);
	printf("Solutions:");
	for(i=0;i<solutions;i++)
		printf(" %f",x[i]);
	printf("\n");

	a1 = 45.0; b1 = 8.67; c1 = 7.5; d1 = 34.0;
	SolveCubic(a1, b1, c1, d1, &solutions, x);
	printf("Solutions:");
	for(i=0;i<solutions;i++)
		printf(" %f",x[i]);
	printf("\n");

	a1 = -12.0; b1 = -1.7; c1 = 5.3; d1 = 16.0;
	SolveCubic(a1, b1, c1, d1, &solutions, x);
	printf("Solutions:");
	for(i=0;i<solutions;i++)
		printf(" %f",x[i]);
	printf("\n");

  /* Now solve some random equations */
	for(a1=1;a1<10;a1+=1) {
		for(b1=10;b1>0;b1-=.25) {
			for(c1=5;c1<15;c1+=0.61) {
				for(d1=-1;d1>-5;d1-=.451) {
					SolveCubic(a1, b1, c1, d1, &solutions, x);  
					printf("Solutions:");
					for(i=0;i<solutions;i++)
						printf(" %f",x[i]);
					printf("\n");
				}
			}
		}
	}
	

	cycles_normal = get_event_counter(6);
	
	printf("Cycles CPU before exec  of attack = %u\n", cycles_normal);
	
	/*Run libflush example */
  chdir("/home/nikos/armageddon/libflush/"); 
  system("./example/build/armv8/release/bin/example -s 400 -n  1000 -x 1 -z 10");
  chdir("/home/nikos/gem5-lib/automotive/basicmath-libflush/");	
	
	/* Run attack crypto_side_channel_attacl */
	/*
  chdir("/home/nikos/crypto-side-channel-attack/build/aes-attack/one-round-attack/real-security-daemon/"); 
  system("Te0=$(nm ../../../lib/libcrypto.so.1.0.0 | grep Te0 ) && Te0=$(echo $Te0 | cut -c9-16)");
  system("Te1=$(nm ../../../lib/libcrypto.so.1.0.0 | grep Te1 ) && Te1=$(echo $Te1 | cut -c9-16)");
  system("Te2=$(nm ../../../lib/libcrypto.so.1.0.0 | grep Te2 ) && Te2=$(echo $Te2 | cut -c9-16)");
  system("Te3=$(nm ../../../lib/libcrypto.so.1.0.0 | grep Te2 ) && Te3=$(echo $Te3 | cut -c9-16)");
  system("LD_PRELOAD=../../../lib/libcrypto.so.1.0.0 ./security_daemon &");
  system("./attacker 1 1 210  $Te0 $Te1 $Te2 $Te3 ../../../lib/libcrypto.so.1.0.0");
  chdir("/home/nikos/gem5-lib/automotive/basicmath-libflush/");
  */
  	
  	
  cycles_normal = get_event_counter(6) - cycles_normal;
	printf("********* INTEGER SQR ROOTS ***********\n");


	printf("********* INTEGER SQR ROOTS ***********\n");
	/* perform some integer square roots */
	for (i = 0; i < 100000; i+=2)
	{
		usqrt(i, &q);
			// remainder differs on some machines
		// printf("sqrt(%3d) = %2d, remainder = %2d\n",
		printf("sqrt(%3d) = %2d\n", i, q.sqrt);
	}
	printf("\n");
	for (l = 0x3fed0169L; l < 0x3fed4169L; l++)
	{
		usqrt(l, &q);
		//printf("\nsqrt(%lX) = %X, remainder = %X\n", l, q.sqrt, q.frac);
		printf("sqrt(%lX) = %X\n", l, q.sqrt);
	}


	printf("********* ANGLE CONVERSION ***********\n");
	/* convert some rads to degrees */
	/*   for (X = 0.0; X <= 360.0; X += 1.0) */
	for (X = 0.0; X <= 360.0; X += .001)
	printf("%3.0f degrees = %.12f radians\n", X, deg2rad(X));
	puts("");
	/*   for (X = 0.0; X <= (2 * PI + 1e-6); X += (PI / 180)) */
	for (X = 0.0; X <= (2 * PI + 1e-6); X += (PI / 5760))
		printf("%.12f radians = %3.0f degrees\n", X, rad2deg(X));
	
	
	cycle_counter_disable();
	event_counters_disable();
	
	printf("\nPerformance monitor results\n\n");
	printf("refills in the instruction cache = %u\n", get_event_counter(12)); /*get_event_counter(0)*/
	printf("retired branches = %u\n", get_event_counter(1) );
	printf("refills in the data cache = %u\n", get_event_counter(2) );
	printf("retired instructions = %u\n", get_event_counter(3) );
	printf("branch predictor misses = %u\n", get_event_counter(4) );
	printf("Predictable branch speculatively executed = %u\n", get_event_counter(5) );
	
	
	printf("retired loads = %u\n", get_event_counter(7)); /*get_event_counter(0)*/
	printf("retired stores = %u\n", get_event_counter(11) );
	printf("retired loads + stores = %u\n", get_event_counter(8) );
	printf("retired branches = %u\n", get_event_counter(9) );
	printf("sw increment = %u\n", get_event_counter(10) );
	printf("CPU cycles = %u\n", get_event_counter(6));
	diff = get_event_counter(6) - cycles_normal;
	printf("CPU cycles difference attack - nomimal = %u\n", diff);
	return 0;
}
