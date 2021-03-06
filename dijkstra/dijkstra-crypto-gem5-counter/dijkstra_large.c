#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include "arm_v8.h"



#define NUM_NODES                          100
#define NONE                               9999

extern void init_pmu(void);
extern void select_event(void);
extern void reset_event_counters(void);
extern void reset_cycle_counter(void);
extern void event_counters_disable(void);
extern void cycle_counter_disable(void);


struct _NODE
{
  int iDist;
  int iPrev;
};
typedef struct _NODE NODE;

struct _QITEM
{
  int iNode;
  int iDist;
  int iPrev;
  struct _QITEM *qNext;
};
typedef struct _QITEM QITEM;

QITEM *qHead = NULL;

             
             
             
int AdjMatrix[NUM_NODES][NUM_NODES];

int g_qCount = 0;
NODE rgnNodes[NUM_NODES];
int ch;
int iPrev, iNode;
int i, iCost, iDist;


void print_path (NODE *rgnNodes, int chNode)
{
  if (rgnNodes[chNode].iPrev != NONE)
    {
      //print_path(rgnNodes, rgnNodes[chNode].iPrev);
    }
  //printf (" %d", chNode);
  fflush(stdout);
}


void enqueue (int iNode, int iDist, int iPrev)
{
  QITEM *qNew = (QITEM *) malloc(sizeof(QITEM));
  QITEM *qLast = qHead;
  
  if (!qNew) 
    {
      //fprintf(stderr, "Out of memory.\n");
      exit(1);
    }
  qNew->iNode = iNode;
  qNew->iDist = iDist;
  qNew->iPrev = iPrev;
  qNew->qNext = NULL;
  
  if (!qLast) 
    {
      qHead = qNew;
    }
  else
    {
      while (qLast->qNext) qLast = qLast->qNext;
      qLast->qNext = qNew;
    }
  g_qCount++;
  //               ASSERT(g_qCount);
}


void dequeue (int *piNode, int *piDist, int *piPrev)
{
  QITEM *qKill = qHead;
  
  if (qHead)
    {
      //                 ASSERT(g_qCount);
      *piNode = qHead->iNode;
      *piDist = qHead->iDist;
      *piPrev = qHead->iPrev;
      qHead = qHead->qNext;
      free(qKill);
      g_qCount--;
    }
}


int qcount (void)
{
  return(g_qCount);
}

int dijkstra(int chStart, int chEnd) 
{
  

  
  for (ch = 0; ch < NUM_NODES; ch++)
    {
      rgnNodes[ch].iDist = NONE;
      rgnNodes[ch].iPrev = NONE;
    }

  if (chStart == chEnd) 
    {
      //printf("Shortest path is 0 in cost. Just stay where you are.\n");
    }
  else
    {
      rgnNodes[chStart].iDist = 0;
      rgnNodes[chStart].iPrev = NONE;
      
      enqueue (chStart, 0, NONE);
      
     while (qcount() > 0)
	{
	  dequeue (&iNode, &iDist, &iPrev);
	  for (i = 0; i < NUM_NODES; i++)
	    {
	      if ((iCost = AdjMatrix[iNode][i]) != NONE)
		{
		  if ((NONE == rgnNodes[i].iDist) || 
		      (rgnNodes[i].iDist > (iCost + iDist)))
		    {
		      rgnNodes[i].iDist = iDist + iCost;
		      rgnNodes[i].iPrev = iNode;
		      enqueue (i, iDist + iCost, iNode);
		    }
		}
	    }
	}
      
      //printf("Shortest path is %d in cost. ", rgnNodes[chEnd].iDist);
      //printf("Path is: ");
      //print_path(rgnNodes, chEnd);
      //printf("\n");
    }
}

int main(int argc, char *argv[]) {
  int diff2 = 0;
  int diff = 0;
  int i,j,k;
  FILE *fp;
  
  if (argc<2) {
    //fprintf(stderr, "Usage: dijkstra <filename>\n");
    //fprintf(stderr, "Only supports matrix size is #define'd.\n");
  }

  /* open the adjacency matrix file */
  fp = fopen (argv[1],"r");
	

  /* make a fully connected matrix */
  for (i=0;i<NUM_NODES;i++) {
    for (j=0;j<NUM_NODES;j++) {
      /* make it more sparce */
      fscanf(fp,"%d",&k);
			AdjMatrix[i][j]= k;
    }
  }
  

	/*Run libflush example */
	chdir("/home/nikos/gem5-lib/libflush/libflush-gem5-reg/"); 
	system("./example/build/armv8/release/bin/example -s 400 -n  1000 -x 1 -z 10");
	chdir("/home/nikos/gem5-lib/dijkstra/dijkstra-crypto-gem5-counter/");		
	
	/* Run attack crypto_side_channel_attacl */
	/*
  chdir("/home/nikos/crypto-side-channel-attack/build/aes-attack/one-round-attack/real-security-daemon/"); 
  system("Te0=$(nm ../../../lib/libcrypto.so.1.0.0 | grep Te0 ) && Te0=$(echo $Te0 | cut -c9-16)");
  system("Te1=$(nm ../../../lib/libcrypto.so.1.0.0 | grep Te1 ) && Te1=$(echo $Te1 | cut -c9-16)");
  system("Te2=$(nm ../../../lib/libcrypto.so.1.0.0 | grep Te2 ) && Te2=$(echo $Te2 | cut -c9-16)");
  system("Te3=$(nm ../../../lib/libcrypto.so.1.0.0 | grep Te2 ) && Te3=$(echo $Te3 | cut -c9-16)");
  system("LD_PRELOAD=../../../lib/libcrypto.so.1.0.0 ./security_daemon &");
  system("./attacker 1 1 210  $Te0 $Te1 $Te2  $Te3 ../../../lib/libcrypto.so.1.0.0");
  chdir("/home/nikos/gem5-lib/dijkstra/dijkstra-crypto-gem5-counter/");	
  */


  /* finds 10 shortest paths between nodes */
  for (i=0,j=NUM_NODES/2;i<100;i++,j++) {
			j=j%NUM_NODES;
      dijkstra(i,j);
  }
	
	
  exit(0);
  

}
