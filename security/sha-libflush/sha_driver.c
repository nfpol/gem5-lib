/* NIST Secure Hash Algorithm */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "sha.h"


#include <stdint.h>
#include <errno.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include "arm_v8.h"
extern void init_pmu(void);
extern void select_event(void);
extern void reset_event_counters(void);
extern void reset_cycle_counter(void);

int main(int argc, char **argv)
{
    FILE *fin;
    SHA_INFO sha_info;
		int diff2 = 0;
		int diff = 0;
		
			
		init_pmu();
		select_event();
		reset_event_counters();
		reset_cycle_counter();

    if (argc < 2) {
	fin = stdin;
	sha_stream(&sha_info, fin);
	sha_print(&sha_info);
    } else {
			
				diff2 = get_timing();   //time before the attack

				/*Run libflush example */
				chdir("/home/attacks/armageddon/libflush/"); 
				system("./example/build/armv8/release/bin/example -s 400 -n  1000 -x 1 -z 10");
				chdir("/home/attacks/gem5-lib/automotive/bitcount-libflush/");	

				/* Run attack crypto_side_channel_attacl */
				/*
				chdir("/home/attacks/crypto-side-channel-attack/build/aes-attack/one-round-attack/real-security-daemon/"); 
				system("Te0=$(nm ../../../lib/libcrypto.so.1.0.0 | grep Te0 ) && Te0=$(echo $Te0 | cut -c9-16)");
				system("Te1=$(nm ../../../lib/libcrypto.so.1.0.0 | grep Te1 ) && Te1=$(echo $Te1 | cut -c9-16)");
				system("Te2=$(nm ../../../lib/libcrypto.so.1.0.0 | grep Te2 ) && Te2=$(echo $Te2 | cut -c9-16)");
				system("Te3=$(nm ../../../lib/libcrypto.so.1.0.0 | grep Te2 ) && Te3=$(echo $Te3 | cut -c9-16)");
				system("LD_PRELOAD=../../../lib/libcrypto.so.1.0.0 ./security_daemon &");
				system("./attacker 1 1 210  $Te0 $Te1 $Te2 $Te3 ../../../lib/libcrypto.so.1.0.0");
				chdir("/home/attacks/gem5-lib/automotive/bitcount-libflush/");
				*/

  diff2 = get_timing() - diff2;
	while (--argc) {
	    fin = fopen(*(++argv), "rb");
	    if (fin == NULL) {
		printf("error opening %s for reading\n", *argv);
	    } else {
		sha_stream(&sha_info, fin);
		sha_print(&sha_info);
		fclose(fin);
	    }
	}
    }
		
		cycle_counter_disable();
		event_counters_disable();

		printf("\nPerformance monitor results\n\n");
		printf("i cache refills= %u\n", get_event_counter(0)); /*get_event_counter(0)*/
		printf("retired branches= %u\n", get_event_counter(1) );
		printf("d cache refills= %u\n", get_event_counter(2) );
		printf("retired instructions = %u\n", get_event_counter(3) );
		printf("branch predictor misses = %u\n", get_event_counter(4) );
		printf("Predictable branch speculatively executed = %u\n", get_event_counter(5) );
		printf("CPU cycles = %u\n", get_timing());
		diff = get_timing() - diff2;
		printf("CPU cycles  attack = %u\n", diff2);
    return(0);
}
