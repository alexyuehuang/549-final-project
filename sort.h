/* 
 * sort.h
 */

#define MAX_FUNCS 100

/* Maximum array dimension */
#define MAXN 1000000

typedef struct trans_func{
  void (*func_ptr)(int N, int A[N]);
  char* description;
} trans_func_t;

void registerFunctions();