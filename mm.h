/* 
 * mm.h
 */ 

#define MAX_MM_FUNCS 100

/* Maximum matrix dimension */
#define MAXN 10000

typedef struct mm_func{
  void (*func_ptr)(int N, const int A[N][N], const int B[N][N], int C[N][N]);
  char* description;
} mm_func_t;

void registerFunctions();
