/* +++Date last modified: 05-Jul-1997 */

/*
**  BITCNTS.C - Test program for bit counting functions
**
**  public domain by Bob Stout & Auke Reitsma
*/

#include <stdio.h>
#include <stdlib.h>
#include "conio.h"
#include <limits.h>
#include <time.h>
#include <float.h>
#include "bitops.h"

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include "arm_v8.h"

#define FUNCS  7

extern void init_pmu(void);
extern void select_event(void);
extern void reset_event_counters(void);
extern void reset_cycle_counter(void);

static int CDECL bit_shifter(long int x);

int main(int argc, char *argv[])
{
	int diff2 = 0;
  int diff = 0;
  clock_t start, stop;
  double ct, cmin = DBL_MAX, cmax = 0;
  int i, cminix, cmaxix;
  long j, n, seed;
  int iterations;
	
	init_pmu();
	select_event();
	reset_event_counters();
	reset_cycle_counter();
	
  static int (* CDECL pBitCntFunc[FUNCS])(long) = {
    bit_count,
    bitcount,
    ntbl_bitcnt,
    ntbl_bitcount,
    /*            btbl_bitcnt, DOESNT WORK*/
    BW_btbl_bitcount,
    AR_btbl_bitcount,
    bit_shifter
  };
  static char *text[FUNCS] = {
    "Optimized 1 bit/loop counter",
    "Ratko's mystery algorithm",
    "Recursive bit count by nybbles",
    "Non-recursive bit count by nybbles",
    /*            "Recursive bit count by bytes",*/
    "Non-recursive bit count by bytes (BW)",
    "Non-recursive bit count by bytes (AR)",
    "Shift and count bits"
  };
  if (argc<2) {
    //fprintf(stderr,"Usage: bitcnts <iterations>\n");
    exit(-1);
	}
  iterations=atoi(argv[1]);
  
  puts("Bit counter algorithm benchmark\n");
  
  for (i = 0; i < FUNCS; i++) {
    start = clock();
    
    for (j = n = 0, seed = rand(); j < iterations; j++, seed += 13)
	 n += pBitCntFunc[i](seed);
    
    stop = clock();
    ct = (stop - start) / (double)CLOCKS_PER_SEC;
    if (ct < cmin) {
	 cmin = ct;
	 cminix = i;
    }
    if (ct > cmax) {
	 cmax = ct;
	 cmaxix = i;
    }
    
    //printf("%-38s> Time: %7.3f sec.; Bits: %ld\n", text[i], ct, n);
  }
	
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
	
  //printf("\nBest  > %s\n", text[cminix]);
  //printf("Worst > %s\n", text[cmaxix]);
	
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
	
	
  return 0;
}

static int CDECL bit_shifter(long int x)
{
  int i, n;
  
  for (i = n = 0; x && (i < (sizeof(long) * CHAR_BIT)); ++i, x >>= 1)
    n += (int)(x & 1L);
  return n;
}
