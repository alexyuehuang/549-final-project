/* 
 * trans.h
 */

#define MAX_TRANS_FUNCS 100

/* Maximum array dimension */
#define MAXN 256

typedef struct trans_func{
  void (*func_ptr)(int M,int N,int[N][M],int[M][N]);
  char* description;
} trans_func_t;

void registerFunctions();