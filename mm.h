/* 
 * mm.h
 */ 

#define MAX_FUNCS 100

/* Maximum matrix dimension */
#define MAXN 10000

typedef struct trans_func{
  void (*func_ptr)(int N, int A[N][N],int B[N][N], int C[N][N]);
  char* description;
} trans_func_t;

void registerFunctions();