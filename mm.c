/*
 * mm.c - Matrix Multiplication
 * 
 * C = A.B
 * the target matrix C might contain garbage bytes originally
 */
#include "mm.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

// All of the functions waiting to be evaluated
mm_func_t func_list[MAX_MM_FUNCS];
int func_counter = 0;

char mult1_desc[] = "Ordinary Multiplication";
void mult1(int N, const int A[N][N], const int B[N][N], int C[N][N]) {
  int sum = 0;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      sum = 0;
      for (int k = 0; k < N; k++) {
        sum += A[i][k] * B[k][j];
      }
      C[i][j] = sum;
    }
  }
}

char mult2_desc[] = "Multiplication kij";
void mult2(int N, const int A[N][N], const int B[N][N], int C[N][N]) {
  int r = 0;
  memset(C, 0, sizeof(int) * N * N);
  for (int k = 0; k < N; k++) {
    for (int i = 0; i < N; i++) {
      r = A[i][k];
      for (int j = 0; j < N; j++) {
        C[i][j] += r * B[k][j];
      }
    }
  }
}

char mult3_desc[] = "Multiplication blocked";
void mult3(int N, const int A[N][N], const int B[N][N], int C[N][N]) {
  int b = 8; // block size;
  assert(N % b == 0); // TODO currently only works when N is divisible by b
  memset(C, 0, sizeof(int) * N * N);
  for (int i = 0; i < N; i += b) {
    for (int j = 0; j < N; j += b) {
      for (int k = 0; k < N; k += b) {
        for (int i1 = i; i1 < i + b; i1++) {
          for (int j1 = j; j1 < j + b; j1++) {
            for (int k1 = k; k1 < k + b; k1++) {
              C[i1][j1] += A[i1][k1] * B[k1][j1];
            }
          }
        }
      }
    }
  }
}


// this doesn't compile
// also, this is just standard n^3 divide and conquer mm
/* char mult4_desc[] = "Multiplication Z-Layout"; */
/* void mult4(int N, const int A[N][N],const int B[N][N], int C[N][N]) */
/* { */
/*     if(N==1){ */
/*         C[0][0]+=A[0][0]*B[0][0]; */
/*         return; */
/*     } */
/*     mult4(N/2,A, B, C); */
/*     mult4(N/2,A[0][N/2], B[N/2][0],C); */
/*     mult4(N/2,A[0][N/2], B[N/2][N/2], C[0][N/2]); */
/*     mult4(N/2,A[0][0], B[0][N/2],C[0][N/2]); */
/*     mult4(N/2,A[N/2][0], B[N/2][N/2], C[N/2][0]); */
/*     mult4(N/2,A[0][0], B[N/2][0],C[N/2][0]); */
/*     mult4(N/2,A[N/2][N/2], B[N/2][N/2], C[N/2][N/2]); */
/*     mult4(N/2,A[N/2][0], B[0][N/2],C[N/2][N/2]); */
/* } */

static inline int interleave(int x, int y, const int leave)
{	
  const int m = (1<<leave)-1;
  int ans = ((y&m) << leave) | ((x&m)), s = leave<<1;
  for (x >>= leave, y >>= leave; x|y ; s += 2, x >>= 1, y >>= 1) {
    ans |= ((y&1) << (1+s)) | ((x&1) << s);
  }
  return ans;
}

static void orig2morton(const int n, int *mortonA, const int A[n][n], const int block) {
  const int leave = log2(block);
  for(int y = 0; y < n; ++y) {
    for (int x = 0; x < n; ++x) {
      mortonA[interleave(x, y, leave)] = A[y][x];
    }
  }
}

/*
 * transform mortonz back to row-major
 */
static void morton2orig(const int n, const int *mortonA, int A[n][n], const int block) {
  const int leave = log2(block);
  for (int y = 0; y < n; ++y) {
    for (int x = 0; x < n; ++x) {
      A[y][x] = mortonA[interleave(x, y, leave)];
    }
  }
}

static const int threshold_bf = 8; // block size for morton z layout
/* #define MAXNN (MAXN*MAXN) */


/*
 * brute force O(n^3) sequantial matrix multiplication
 * C += A * B
 */
void bf(const int *A, const int *B, int *C, const int n) {
  for(int i = 0; i < n; ++i) {
    for(int j = 0; j < n; ++j) {
      for(int k = 0; k < n; ++k) 
        C[i*n+j] += A[i*n+k] * B[k*n+j];
      /* int s = (int)0; */
      /* for(int k = 0; k < n; k += 4) { */
      /*   s += A[i*n+k] * B[k*n+j] \ */
      /*        + A[i*n+k+2] * B[(k+2)*n+j] \ */
      /*        + A[i*n+k+1] * B[(k+1)*n+j] \ */
      /*        + A[i*n+k+3] * B[(k+3)*n+j]; */
      /* } */
      /* C[i*n+j] += s; */
    }
  }
}

static void dac(int n, const int* mortonA /* N*N */, const int *mortonB /* N*N */, int *mortonC /* N*N */)
{
  if (n <= threshold_bf) return bf(mortonA, mortonB, mortonC, n);

  const int halfn = n>>1, z = halfn*halfn;

  const int *A1 = mortonA;
  const int *A2 = mortonA + z;
  const int *A3 = mortonA + z*2;
  const int *A4 = mortonA + z*3;

  const int *B1 = mortonB;
  const int *B2 = mortonB + z*2;
  const int *B3 = mortonB + z;
  const int *B4 = mortonB + z*3;

  int *C1 = mortonC;
  int *C2 = mortonC + z;
  int *C3 = mortonC + z*2;
  int *C4 = mortonC + z*3;

  dac(halfn, A1, B1, C1);
  dac(halfn, A1, B3, C2);
  dac(halfn, A3, B1, C3);
  dac(halfn, A3, B3, C4);

  dac(halfn, A2, B2, C1);
  dac(halfn, A2, B4, C2);
  dac(halfn, A4, B2, C3);
  dac(halfn, A4, B4, C4);
}

void mult_dac(int N, const int A[N][N], const int B[N][N], int C[N][N])
{
  if (N <= threshold_bf) mult2(N, A, B, C);
  else
  {
    assert(N % threshold_bf == 0);
    int mortonA[N*N], mortonB[N*N], mortonC[N*N];
    /* static int mortonA[MAXNN], mortonB[MAXNN], mortonC[MAXNN]; */
    memset(mortonC, 0, sizeof(int)*N*N);
    orig2morton(N, mortonA, A, threshold_bf);
    orig2morton(N, mortonB, B, threshold_bf);
    dac(N, mortonA, mortonB, mortonC);
    morton2orig(N, mortonC, C, threshold_bf);
  }
}
char mult_dac_desc[] = "n^3 dac z layout";

/*
 * registerTransFunction - Add the given function into the list
 *     of functions to be tested
 */
void registerTransFunction(void (*func)(int N, const int A[N][N],
                                        const int B[N][N], int C[N][N]),
                           char *desc) {
  func_list[func_counter].func_ptr = func;
  func_list[func_counter].description = desc;
  func_counter++;
}

/*
 * registerFunctions - All functions registered will be tested
 */
void registerFunctions() {
  /****************************** Register functions
   * *********************************/
  registerTransFunction(mult1, mult1_desc);
  registerTransFunction(mult2, mult2_desc);
  registerTransFunction(mult3, mult3_desc);
  /* registerTransFunction(mult4, mult4_desc); */
  registerTransFunction(mult_dac, mult_dac_desc);
  /***********************************************************************************/
}
