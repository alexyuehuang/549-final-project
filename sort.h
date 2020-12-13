/* 
 * sort.h
 */

#define MAX_FUNCS 100

/* Maximum array dimension */
#define MAXN 1000000

typedef struct trans_func
{
  void (*func_ptr)(int N, int A[N]);
  char *description;
} trans_func_t;

void registerFunctions();

// Used in funnel sort
struct Buffer
{ // FIFO queue
  int *data;
  int nmemb; // max count of elements
  int head;
  int tail;
  int count; // current count of elements
};

struct Funnel
{
  struct Buffer **in; // input arrays represented as buffers
  int in_count;       // count of input arrays (buffers) [n^1/3]

  struct Buffer **buffers;
  struct Funnel **bottom; // left funnels
  int bb_count;           // count of bottom funnels = count of buffers

  struct Funnel *top; // right funnel
  struct Buffer *out; // output array
};