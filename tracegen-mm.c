/* 
 * tracegen-mm.c - Running the binary tracegen with valgrind produces
 * a memory trace of the registered transpose function specified. 
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

#include "mm.h"

#define MAX_RAND 128 
// prevent overflow

/* External variables declared in trans.c */
extern mm_func_t func_list[MAX_MM_FUNCS];
extern int func_counter; 

/* Markers used to bound trace regions of interest */
volatile char MARKER_START, MARKER_END;

/* static int A_TEMP[MAXN][MAXN]; */
static int A[MAXN][MAXN];
static int B[MAXN][MAXN];
static int C[MAXN][MAXN];
static int N = -1;

/* 
 * correct - baseline mm function used to evaluate correctness 
 * C = A.B
 */
void correct(int N, const int A[N][N], const int B[N][N], int C[N][N])
{
  int i,j,k;
  for (i = 0; i < N; ++i)
    for (j = 0; j < N; ++j)
      for (k = C[i][j] = 0; k < N; ++k)
        C[i][j] += A[i][k] * B[k][j];
}

/*
 * A,B: input matrices
 * V: output matrix to validate
 */
int validate(int fn, int N, int A[N][N], int B[N][N], int V[N][N]) {
  int T[N][N]; // truth
  correct(N, A, B, T);
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
      if (V[i][j] != T[i][j])
        return !printf(
            "Validation failed on function '%s'!\n Expected %d but got "
            "%d at V[%d][%d]\n",
            func_list[fn].description, T[i][j], V[i][j], i, j);
  return 1;
}

/*
 * initMatrix - Initialize the given matrix randomly
 */
void initMatrix(int N, int A[N][N], int B[N][N], int C[N][N]) {
  int i, j;
  srand(time(NULL));
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      A[i][j] = rand() % MAX_RAND;
      B[i][j] = rand() % MAX_RAND;
      C[i][j] = rand() % MAX_RAND;
    }
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
        printf("./tracegen-mm failed to parse its options.\n");
        exit(1);
    }
  }

  /*  Register transpose functions */
  registerFunctions();

  if (N == -1 || selectedFunc == -1 || selectedFunc >= func_counter) {
    printf("./tracegen-mm failed to parse its options.\n");
    exit(1);
  }

  /* Fill A with data */
  initMatrix(N, A, B, C);

  /* Store initial A values in A_TEMP for correctness check */
  /* memcpy(A_TEMP, A, M*N*sizeof(A[0][0])); */

  /* Record marker addresses */
  FILE* marker_fp = fopen("marker","w");
  assert(marker_fp);
  fprintf(marker_fp, "%llx %llx", 
      (unsigned long long int) &MARKER_START,
      (unsigned long long int) &MARKER_END );
  fclose(marker_fp);

  /* Invoke the transpose function */
  MARKER_START = 33;
  (*func_list[selectedFunc].func_ptr)(N, A, B, C);
  MARKER_END = 34;
  if (!validate(selectedFunc,N,A,B,C))
    return selectedFunc + 1;

  return 0;
}

