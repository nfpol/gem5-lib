#include <stdio.h>
#include <stdlib.h>


int main()
{
    int randomnumber;
		srand(time(NULL));
		
		for(int i =0; i<15; i++) {
			randomnumber = rand() % 4+ 1;
			if (randomnumber==1){
					/*Run libflush example */
				printf("libflush");
				chdir("/home/attacks/armageddon/libflush/"); 
				system("m5 resetstats");
				system("./example/build/armv8/release/bin/example -s 400 -n  1000 -x 1 -z 10");
				system("m5 dumpstats");
				chdir("/home/attacks/gem5-lib/");	
			}
			else if (randomnumber==2){
				printf("basicmath");
				chdir("/home/nikos/gem5-lib/automotive/basicmath"); 
				system("m5 resetstats");
				system("basicmath_small > output_small.txt");
				system("m5 dumpstats");
				chdir("/home/attacks/gem5-lib/");	
			else if (randomnumber==3){
				printf("bitcount");
				chdir("/home/nikos/gem5-lib/automotive/bitcount"); 
				system("m5 resetstats");
				system("bitcnts 75000 > output_small.txt");
				system("m5 dumpstats");
				chdir("/home/attacks/gem5-lib/");	
			}
			else if (randomnumber==4){
				printf("sha");
				chdir("/home/nikos/gem5-lib/security/sha"); 
				system("m5 resetstats");
				system("./sha input_small.asc > output_small.txt");
				system("m5 dumpstats");
				chdir("/home/attacks/gem5-lib/");	
			}
			else{
				printf("nothing");
			}
			
		}
    
    return 0;
}