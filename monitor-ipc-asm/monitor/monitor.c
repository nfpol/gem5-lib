#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include "arm_v8.h"
#include <getopt.h>
#include <inttypes.h>
#include <unistd.h>   //for sleep()

extern void init_pmu(void);
extern void select_event(void);
extern void reset_event_counters(void);
extern void reset_cycle_counter(void);
extern void event_counters_disable(void);
extern void cycle_counter_disable(void);

 
static void print_help(char* argv[]) {
  fprintf(stdout, "Usage: %s [OPTIONS]\n", argv[0]);
  fprintf(stdout, "\t-t, -time <value>\t TIME (default: t 1  --> 1/10 --> 0.1sec )\n");
  fprintf(stdout, "\t-m, -loop_monitor <value>\t  Loop for the moniotr (default:  m 1000)\n");
  fprintf(stdout, "\t-d, -div <value>\t time divider (default:  div 10)\n");
  fprintf(stdout, "\t-h, -help\t\t Help page\n");
}

            
int main(int argc, char* argv[]) {
 /* Define parameters */
	float timing_frame = 0;
	int time = 1;
	int div = 10;
	int loop = 1000;
	FILE* logfile = NULL;
  int i,j,k = 0;
  FILE * fPtr;
  fPtr = fopen("./monitor/output-reg.dat", "w");
  if(fPtr == NULL){
        /* File not created hence exit */
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
  }
	
	/* Parse arguments */
  static const char* short_options = "t:m:d:h:";
	static struct option long_options[] = {
	  {"timing_frame",        required_argument, NULL, 't'},
	  {"timing_divider",        required_argument, NULL, 'd'},
	  {"monitor_loop",        required_argument, NULL, 'm'},
	  {"help",            no_argument,       NULL, 'h'},
	  { NULL,             0, NULL, 0}
  };
	int c;
  while ((c = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
		switch (c) {
      case 't':
        time = atoi(optarg);
        if (time <= 0) {
          fprintf(stderr, "Error: timing frame is negative.\n");
          return -1;
        }
        break;
      case 'm':
        loop = atoi(optarg);
        if (loop <= 0) {
          fprintf(stderr, "Error: loop is negative\n");
          return -1;
        }
        break;
			case 'd':
        div = atoi(optarg);
        if (div <= 0) {
          fprintf(stderr, "Error: loop is negative\n");
          return -1;
        }
        break;
      case 'h':
        print_help(argv);
        return 0;
      case '?':
      default:
        fprintf(stderr, "Error: Invalid option '-%c'\n", optopt);
        return -1; 
    }
  }
	asm volatile("ISB");
	init_pmu();
	select_event();
	__asm__ __volatile__ ("mov x0, #0; mov x1, #0; .inst 0XFF000110 | (0x40 << 16);");
	reset_event_counters();
	reset_cycle_counter();
	asm volatile("ISB");
	timing_frame = (time/(double)div);
	printf("Performance monitor results\n");
	fprintf(fPtr, "PMU monitor is starting monitoring counters\n\n");
	while(i < loop){
		/* resetstats */
  	//__asm__ __volatile__ ("mov x0, #0; mov x1, #0; .inst 0XFF000110 | (0x40 << 16);");
		sleep(timing_frame);  //sleep 10ms
		/* dumpstats */
	  __asm__ __volatile__ ("mov x0, #0; mov x1, #0; .inst 0xFF000110 | (0x41 << 16);");
	}
	

	printf("PMU monitor is closing ....\n");
	fprintf(fPtr, "PMU monitor is closing\n");
	fclose(fPtr);
  exit(0);
  

}
