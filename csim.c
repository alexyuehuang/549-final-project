#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <getopt.h>

// protocols
#define LRU 1
#define RR 2
#define FIFO 3
#define MRU 4
#define NMRU 5

#define SEED (time(NULL)) // define SEED as a constant to eliminate randomness

// argument inputs
int s = -1;
int E = -1;
int b = -1;
int r = -1;//replcement policy 1: LRU 2: RR
FILE * file = NULL;
char en = 0;

// simulate cache
long * * tags; // tags
char * * vs; // valid bits
// lru data structure: it maintains the property that for each set
// the line with the smallest lru value (1) is the most recently used one
// the line with the largest lru value is the least recrently used one
// the line with lru value 0 is unused so far
long * * lru; 

// store hit, miss, eviction
long hit = 0l;
long miss = 0l;
long eviction = 0l;

/* update the lru for LRU protocol
 * s_num: set number
 * idx: the index of the line used
 * The lru of the line used is set to 1
 * The lru of the rest of the lines are incremented by 1 if necessary
 */
void update_lru(long s_num, int idx){
   long value = lru[s_num][idx]; // old value of the entry
	for(int i = 0; i < E; ++i){
		if(i == idx){
			lru[s_num][i] = 1; // the lru of the most recently used one is set to 1
		}
      // for the non-zero entry, if it's less than value, increment
      //                         if value is 0, increment (we use a new line)
      else if((lru[s_num][i] < value || value == 0) && lru[s_num][i]){
			lru[s_num][i]++;
		}
	}
}

/* update the lru for FIFO protocol
 * This function will only be called when a new line enters the cache
 * s_num: set number
 * idx: the index of the line used
 * The lru of the line used is set to 1
 * The lru of the rest of the lines are incremented by 1 if necessary
 */
void update_fifo(long s_num, int idx){
	for(int i = 0; i < E; ++i){
		if(i == idx){
			lru[s_num][i] = 1; // the lru of the most recently used one is set to 1
		}
		// except idx, we increment all of the other values if it's not 0
      	else if(lru[s_num][i]){
			lru[s_num][i]++;
		}
	}
}

/* update for MRU protocol
 * s_num: set number
 * idx: the index of the line used
 */
void update_mru(long s_num, int idx){
	// lru[s_num][0] is used to record the most recent visited index
   	lru[s_num][0] = idx;
}

/* return the index of the line that we are going to use for LRU protocol
 * s_num: set number
 * if an lru entry is 0, it is a cold miss and the index is returned
 * otherwise return the index with the largest lru value (i.e. least recently
 * used line)
 */
int index_line_lru(long s_num){
	int idx = 0;
	long max_lru = 0;
	for(int i = 0; i < E; ++i){
		if(lru[s_num][i] == 0)
			return i;
		if(lru[s_num][i] > max_lru){
			max_lru = lru[s_num][i];
			idx = i;
		}
	}
	return idx;
}

/* return the index of the line that we are going to use for RR protocol
 * s_num: set number
 * if there is an empty line, it is a cold miss and the index is returned
 * otherwise return an random index
 */
int index_line_rr(long s_num){
	for(int i = 0; i < E; ++i){
		if(vs[s_num][i] == 0)
			return i;
	}
	return rand() % E;
}

/* return the index of the line that we are going to use for FIFO protocol (the same as LRU)
 * s_num: set number
 * if an lru entry is 0, it is a cold miss and the index is returned
 * otherwise return the index with the largest lru value (i.e. least recently
 * used line)
 */
int index_line_fifo(long s_num){
	int idx = 0;
	long max_lru = 0;
	for(int i = 0; i < E; ++i){
		if(lru[s_num][i] == 0)
			return i;
		if(lru[s_num][i] > max_lru){
			max_lru = lru[s_num][i];
			idx = i;
		}
	}
	return idx;
}

/* return the index of the line that we are going to use for MRU protocol
 * s_num: set number
 * if there is an empty line, it is a cold miss and the index is returned
 * otherwise return the most recently used index, stored in lru[s_num][0]
 * (see update_mru() function)
 */
int index_line_mru(long s_num){
	for(int i = 0; i < E; ++i){
		if(vs[s_num][i] == 0)
			return i;
	}
	return lru[s_num][0];
}

/* return the index of the line that we are going to use for NMRU protocol
 * s_num: set number
 * if there is an empty line, it is a cold miss and the index is returned
 * otherwise return an index that is not the most recently used index, stored in lru[s_num][0]
 */
int index_line_nmru(long s_num){
	if(E == 1) // no other choice. This is to avoid infinite loop below.
		return 0;

	for(int i = 0; i < E; ++i){
		if(vs[s_num][i] == 0)
			return i;
	}

	int idx;
	while(1){
		idx = rand() % E;
		if(idx != lru[s_num][0])
			return idx;
	}
}

/* simulate a cache load/store at address addr */
void move_cache(long addr){
	long tag = ((unsigned long)addr) >> (s + b); // tag
	long s_num = (((unsigned long)addr) << (64 - s - b)) >> (64 - s); // set
	for(int i = 0; i < E; ++i){
      // hit if valid bit is 1 and tag is the same
		if(vs[s_num][i] == 1 && tags[s_num][i] == tag) {
			hit ++;
			if(en)
				printf(" hit");
			if(r == LRU) //LRU replacement protocol
         		update_lru(s_num, i);
			else if(r == MRU || r == NMRU) //MRU or NMRU
				update_mru(s_num, i);
			break;
		}
      // miss if we reach the last line in the set
		if(i == E - 1){
			miss ++;
			if(en)
				printf(" miss");
			int idx; // index of the line we are going to use

			if(r == LRU) // LRU
				idx = index_line_lru(s_num); 
			else if(r == RR) // RR
				idx = index_line_rr(s_num);
			else if(r == FIFO) // FIFO
				idx = index_line_fifo(s_num);
			else if(r == MRU) // MRU
				idx = index_line_mru(s_num);
			else if(r == NMRU) // NMRU
				idx = index_line_nmru(s_num);

			if(vs[s_num][idx] == 0){ // a cold miss
				if(r == LRU) // LRU
            		update_lru(s_num, idx);
				else if(r == FIFO) // FIFO
					update_fifo(s_num, idx);
				else if(r == MRU || r == NMRU) // MRU or NMRU
					update_mru(s_num, idx);

			}else{ // eviction is needed
				eviction ++;
				if(en)
					printf(" eviction");
				if(r == LRU) // LRU
            		update_lru(s_num, idx);
				else if(r == FIFO) // FIFO
					update_fifo(s_num, idx);
				else if(r == MRU || r == NMRU) // MRU or NMRU
					update_mru(s_num, idx);
			}
			vs[s_num][idx] = 1;
			tags[s_num][idx] = tag;
		}
	}
}

/* parse the trace file */
void parse_file() {
	char buf[32];
	while (fgets(buf, sizeof(buf), file) != NULL) {
		if(buf[0] == 'I')
			continue;
		long addr_l = strtol(buf+3,NULL,16); //address
		if(buf[1] == 'L' || buf[1] == 'S'){
			 if(en){
				printf("L/S %lx ",addr_l);
			 }
			 move_cache(addr_l);
			 if(en)
				printf("\n");
		}
		if(buf[1] == 'M'){
			 if(en)
				printf("M %lx",addr_l);
			 move_cache(addr_l);
			 move_cache(addr_l);
			 if(en)
				printf("\n");
		}
	}
}

/* free memory for the program */
void free_memory() {
	int S = pow(2,s);
	for(int i = 0; i < S; ++i)  
		free(tags[i]);
   free(tags);
   for(int i = 0; i < S; ++i)  
		free(lru[i]);
   free(lru);
   for(int i = 0; i < S; ++i)  
		free(vs[i]);
   free(vs); 
}

/* Malloc memory for the program and initialized to 0*/
void malloc_memory() {
	int S = pow(2,s); // num of sets
	tags = (long * *)malloc(sizeof(long *) * S); 
	lru = (long * *)malloc(sizeof(long *) * S);  
   for(int i = 0; i < S; ++i) {
		tags[i]=(long *)malloc(sizeof(long) * E);
      memset((void *)tags[i], 0, sizeof(long) * E);
   }
	for(int i = 0; i < S; ++i) {  
		lru[i]=(long *)malloc(sizeof(long) * E);
      memset((void *)lru[i], 0, sizeof(long) * E);
   }
	vs = (char * *)malloc(sizeof(char *) * S);  
   for(int i = 0; i < S; ++i) {
		vs[i]=(char *)malloc(sizeof(char) * E);
      memset((void *)vs[i], 0, sizeof(char) * E);
   }
}

/* summarize the cache simulation statistics */
void printSummary(int hits, int misses, int evictions) {
    float miss_rate = (float)misses/(float)(hits + misses);
    printf("num of hits:\t%d\n"
	   "num of misses:\t%d\n"
       "miss rate:\t%f\n"
	   "evictions:\t%d\n",
	   hits, misses, miss_rate, evictions);
    FILE* output_fp = fopen("csim_results", "w");
    if(output_fp == NULL){
       printf("Opening file csim_results failed!");
    }
    fprintf(output_fp, "%d %d %f %d\n",
	    hits,
	    misses,
        miss_rate,
	    evictions);
    fclose(output_fp);
}

/* print out usage information */
void usage(char *exe){
   printf("Usage: %s [-v] [-h] -s <s> -E <E> -b <b> -t <tracefile>\n"
         "This program simulates the program specified in the tracefile on the cache with number\n"
         "of sets, associativity, block size specified by the arguments. This program will count\n"
         "number of hits, misses, and evictions, and calculate miss rate."
			"\t -v: optional verbose flag\n"
         "\t -h: print out usage information\n"
			"\t -s <s>: number of set index bits, i.e. 2^s sets\n"
			"\t -E <E>: associativity, i.e. number of lines per set\n"
			"\t -b <b>: number of block bits, i.e. block size 2^b\n"
			"\t -r <r>: replacement policy: one of LRU, RR, FIFO， MRU, NMRU\n"
			"\t -t <tracefile>: name of the valgrind trace to replay\n",
		exe);
}

/* parse the command line arguments and set up the flags */
void parse_arg(int argc, char * argv[]) {
   char c;
   while ((c = getopt(argc,argv,"hvs:E:b:t:r:")) != -1) {
      switch(c) {
         case 's':
            s = atoi(optarg);
            break;
         case 'E':
            E = atoi(optarg);
            break;
         case 'b':
            b = atoi(optarg);
            break;
         case 't':
            file = fopen(optarg, "r");
            break;
		 case 'r':
		 	if(!strcmp(optarg, "LRU")){
				r = LRU;
			}
			else if(!strcmp(optarg, "RR")){
				r = RR;
			}
			else if(!strcmp(optarg, "FIFO")){
				r = FIFO;
			}
			else if(!strcmp(optarg, "MRU")){
				r = MRU;
			}
			else if(!strcmp(optarg, "NMRU")){
				r = NMRU;
			}
		 	break;
         case 'v':
            en = 1;
            break;
         case 'h':
            usage(argv[0]);
            exit(0);
         default:
            usage(argv[0]);
            exit(1);
      }
   }
   if(s == -1 || E == -1 || b == -1 || r == -1 || file == NULL){
		usage(argv[0]);
		exit(1);
	}
}

/* main function of the cache simulator */
int main(int argc, char * argv[]) {
    parse_arg(argc, argv);
    malloc_memory();
	srand(SEED); //randomness is used in RR
    parse_file();
    free_memory();
    printSummary(hit, miss, eviction);
    return 0;
}

