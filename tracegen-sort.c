/* 
 * tracegen-sort.c - Running the binary tracegen with valgrind produces
 * a memory trace of the registered sorting function. 
 * 
 * The beginning and end of each registered transpose function's trace
 * is indicated by reading from "marker" addresses. These two marker
 * addresses are recorded in file for later use.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <time.h>

#include "sort.h"

/* External variables declared in sort.c */
extern trans_func_t func_list[MAX_FUNCS];
extern int func_counter; 

/* Markers used to bound trace regions of interest */
volatile char MARKER_START, MARKER_END;

static int A_TEMP[MAXN];
static int A[MAXN];
static int N;

/* comparator function for qsort */
int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int validate(int fn, int N, int A[N], int A_sorted[N]) {
    qsort(A, N, sizeof(int), cmpfunc);
    for(int i = 0; i < N; i++){
        if(A[i] != A_sorted[i]){
            printf("Validation failed on function %d! Expected %d but got %d at A[%d]\n",fn,A[i],A_sorted[i],i);
            return 0;
        }
    }
    return 1;
}

/* 
 * initArray - Initialize the given array randomly
 */
void initArray(int N, int A[N])
{
    int i;
    srand(time(NULL));
    for (i = 0; i < N; i++){
        A[i] = rand();
    }
}

int main(int argc, char* argv[]){
    char c;
    int selectedFunc = -1;
    while( (c=getopt(argc,argv,"N:F:")) != -1){
        switch(c){
        case 'N':
            N = atoi(optarg);
            break;
        case 'F':
            selectedFunc = atoi(optarg);
            break;
        case '?':
        default:
            printf("./tracegen failed to parse its options.\n");
            exit(1);
        }
    }
  

    /*  Register transpose functions */
    registerFunctions();

    /* Fill A with data */
    initArray(N, A);
    
    /* Store initial A values in A_TEMP for correctness check */
    memcpy(A_TEMP, A, N*sizeof(A[0]));

    /* Record marker addresses */
    FILE* marker_fp = fopen("marker","w");
    assert(marker_fp);
    fprintf(marker_fp, "%llx %llx", 
            (unsigned long long int) &MARKER_START,
            (unsigned long long int) &MARKER_END );
    fclose(marker_fp);

    /* Invoke the transpose function */
    MARKER_START = 33;
    (*func_list[selectedFunc].func_ptr)(N, A);
    MARKER_END = 34;
    if (!validate(selectedFunc, N, A_TEMP, A))
        return selectedFunc + 1;

    return 0;
}


