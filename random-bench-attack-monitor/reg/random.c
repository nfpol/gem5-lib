#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>


#include <unistd.h>   //for sleep()

int main()
{
    		int randomnumber;
		srand(time(NULL));
		//FILE * fPtr;
  		//fPtr = fopen("./monitor/output-reg.dat", "w");
  		//if(fPtr == NULL){
       		/* File not created hence exit */
        		//printf("Unable to create file.\n");
        		//exit(EXIT_FAILURE);
		//}
		
		for(int i =0; i<15; i++) {
			randomnumber = rand() % 4+ 1;
			if (randomnumber==1){
					/*Run libflush example */
				//fprintf(fPtr, "libflush\n");
				printf("libflush\n");
				chdir("/home/nikos/armageddon/libflush/"); 
				//system("m5 resetstats");
				system("./example/build/armv8/release/bin/example -s 400 -n  1000 -x 1 -z 10");
				//system("m5 dumpstats");
				chdir("/home/nikos/gem5-lib/");	
			}
			else if (randomnumber==2){
				//fprintf(fPtr, "basicmath\n");
				printf("basicmath\n");
				chdir("/home/nikos/gem5-lib/automotive/basicmath"); 
				//system("m5 resetstats");
				system("./basicmath_small > output_small.txt");
				//system("m5 dumpstats");
				chdir("/home/nikos/gem5-lib/");
			}	
			else if (randomnumber==3){
				printf("bitcount\n");
				//fprintf(fPtr, "bitcount\n");
				chdir("/home/nikos/gem5-lib/automotive/bitcount"); 
				//system("m5 resetstats");
				system("./bitcnts 75000 > output_small.txt");
				//system("m5 dumpstats");
				chdir("/home/nikos/gem5-lib/");	
			}
			else if (randomnumber==4){
				printf("sha\n");
				//fprintf(fPtr, "sha\n");
				chdir("/home/nikos/gem5-lib/security/sha"); 
				//system("m5 resetstats");
				system("./sha input_small.asc > output_small.txt");
				//system("m5 dumpstats");
				chdir("/home/nikos/gem5-lib/");	
			}
			else {
				//fprintf(fPtr, "nothing");
				printf("nothing");
			}
			
		}
	//fprintf(fPtr, "closing random execution\n");
	printf("closing random execution\n");
	//fclose(fPtr);
	return 0;
}
