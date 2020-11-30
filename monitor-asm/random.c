#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   //for sleep()


int main()
{		
		for(int i =0; i<150; i++) {
			
			/* resetstats */
  			__asm__ __volatile__ ("mov x0, #0; mov x1, #0; .inst 0XFF000110 | (0x40 << 16);");
			sleep(0.1); //sleep 0.1s
			/* dumpstats */
	     		__asm__ __volatile__ ("mov x0, #0; mov x1, #0; .inst 0xFF000110 | (0x41 << 16);");
			
		}
    
    return 0;
}
