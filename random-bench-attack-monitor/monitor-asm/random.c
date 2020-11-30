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


int main()
{		
	FILE * fPtr;
  	fPtr = fopen("output-asm.dat", "w");
  	if(fPtr == NULL){
       		/* File not created hence exit */
        	printf("Unable to create file.\n");
        	exit(EXIT_FAILURE);
       }
       fprintf(fPtr, "Performance monitor results\n\n");
       fprintf(fPtr, "i cache refills---|---retired branches---|---d cache refills---|---branch predictor misses---|---predictable branch speculatively executed---|---CPU cycles event counter---|---CPU cycles ccnt\n");
	for(int i =0; i<150; i++) {
			
		/* resetstats */
  		__asm__ __volatile__ ("mov x0, #0; mov x1, #0; .inst 0XFF000110 | (0x40 << 16);");
		sleep(0.1); //sleep 0.1s
			/* dumpstats */
     		__asm__ __volatile__ ("mov x0, #0; mov x1, #0; .inst 0xFF000110 | (0x41 << 16);");
     		fprintf(fPtr, "dump statistics\n");
			
	}
	
	fprintf(fPtr, "PMU monitor is closing ....\n");
	fclose(fPtr);
    
    return 0;
}
