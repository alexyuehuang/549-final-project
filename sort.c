/* 
 * sort.c - Sorting an array
 */
#include <stdio.h>
#include <limits.h> // INT_MAX
#include <math.h>
#include <stdlib.h> // malloc & free
#include <string.h>
#include "sort.h"

// All of the functions waiting to be evaluated
trans_func_t func_list[MAX_FUNCS];
int func_counter = 0;

char sort1_desc[] = "Selection Sort";
void sort1(int N, int A[N])
{
    int i, j;
    int min_idx;
    int tmp;
    for (i = 0; i < N - 1; i++)
    {
        min_idx = i;
        for (j = i + 1; j < N; j++)
        {
            if (A[j] < A[min_idx])
                min_idx = j;
        }
        // swap
        tmp = A[i];
        A[i] = A[min_idx];
        A[min_idx] = tmp;
    }
}
/////////////////////////////////////////////////////////////////////////
// merge sort (modified based on https://www.interviewbit.com/tutorial/merge-sort-algorithm/)
int temp[MAXN]; //reusable temp array
void merge(int *Arr, int start, int mid, int end)
{

    // crawlers for both intervals and for temp
    int i = start, j = mid + 1, k = 0;

    // traverse both arrays and in each iteration add smaller of both elements in temp
    while (i <= mid && j <= end)
    {
        if (Arr[i] <= Arr[j])
        {
            temp[k] = Arr[i];
            k += 1;
            i += 1;
        }
        else
        {
            temp[k] = Arr[j];
            k += 1;
            j += 1;
        }
    }

    // add elements left in the first interval
    while (i <= mid)
    {
        temp[k] = Arr[i];
        k += 1;
        i += 1;
    }

    // add elements left in the second interval
    while (j <= end)
    {
        temp[k] = Arr[j];
        k += 1;
        j += 1;
    }

    // copy temp to original interval
    for (i = start; i <= end; i += 1)
    {
        Arr[i] = temp[i - start];
    }
}

// Arr is an array of integer type
// start and end are the starting and ending index of current interval of Arr
void mergeSort(int *Arr, int start, int end)
{
    if (start < end)
    {
        int mid = (start + end) / 2;
        mergeSort(Arr, start, mid);
        mergeSort(Arr, mid + 1, end);
        merge(Arr, start, mid, end);
    }
}

char sort2_desc[] = "Merge Sort";
void sort2(int N, int A[N])
{
    mergeSort(A, 0, N - 1);
}
///////////////////////////////////////////////////////////////////////////////
// k-way merge sort
#define K 9 // K has to be defined at compile time to keep the interface consistent

//int temp[MAXN]; //reusable temp array // already defined
/* Note that this kMerge takes O(kN). Using heap/binary tree is a better option
 * But for the sake of IO complexity, it is okay to use this algorithm.
 */
void kMerge(int *Arr, int start, int end)
{
    int N = end - start + 1; // total length
    int n = N / K;           // length of each subarray
    int leftover = N % K;    // note: n * K + leftover = N
    int ptrs[K] = {0};       // offsets at each subarray
    for (int idx = 0; idx < N; idx++)
    {
        int min = INT_MAX;
        int min_i;
        int s = start;
        // the first leftover subarrays has n+1 elements and the rest has n
        // iterate all the arrays to find the smallest element and to put it in temp[idx]
        for (int i = 0; i < K; i++)
        {
            //check ptrs[i] does not go out of range
            if (i < leftover)
            {
                if (ptrs[i] == n + 1)
                {
                    s = s + n + 1;
                    continue;
                }
            }
            else
            {
                if (ptrs[i] == n)
                {
                    s = s + n;
                    continue;
                }
            }

            // record the current array in
            if (min > Arr[s + ptrs[i]])
            {
                min = Arr[s + ptrs[i]];
                min_i = i;
            }
            if (i < leftover)
            {
                s = s + n + 1;
            }
            else
            {
                s = s + n;
            }
        }
        temp[idx] = min;
        ptrs[min_i]++;
    }
    // copy temp to original interval
    for (int i = start; i <= end; i++)
    {
        Arr[i] = temp[i - start];
    }
}

// Arr is an array of integer type
// start and end are the starting and ending index of current interval of Arr
void kMergeSort(int *Arr, int start, int end)
{
    if (start < end)
    {
        int N = end - start + 1; // total length
        int n = N / K;           // length of each subarray
        int leftover = N % K;    // note: n * K + leftover = N
        // the first leftover subarrays has n+1 elements and the rest has n
        int s = start;
        for (int i = 0; i < K; i++)
        {
            if (i < leftover)
            {
                kMergeSort(Arr, s, s + n);
                s = s + n + 1;
            }
            else
            {

                kMergeSort(Arr, s, s + n - 1);
                s = s + n;
            }
        }

        kMerge(Arr, start, end);
    }
}

char sort3_desc[] = "K-Way Merge Sort";
void sort3(int N, int A[N])
{
    kMergeSort(A, 0, N - 1);
}

////////////////////////////////////////////////
// enqueue the element at the tail location
void buffer_enqueue(struct Buffer* buffer, int el)
{
    buffer->data[buffer->tail] = el;
    buffer->tail = (buffer->tail + 1) % buffer->nmemb; // loop back to the beginning if full
    buffer->count++;
}

// dequeue the element at the head location
int buffer_dequeue(struct Buffer* buffer)
{
    int head = buffer->head;
    buffer->head = (buffer->head + 1) % buffer->nmemb;
    buffer->count--;
    return buffer->data[head];
}


int buffer_empty(struct Buffer *buffer)
{
    return buffer->count == 0;
}


int buffer_full(struct Buffer *buffer)
{
    return (buffer->count == buffer->nmemb);
}

/* param: an array of buffers and the length of the array
 * return true if all of the buffers are non empty
 */
int buffers_nonempty(struct Buffer **in, int in_count)
{
    for (int i=0; i < in_count; i++) {
        if (buffer_empty(in[i])) {
           return 0;
        }
    }
    return 1;
}

// return true if all of the buffers are empty
int buffers_empty(struct Buffer **in, int in_count)
{
    for (int i=0; i < in_count; i++) {
        if (!buffer_empty(in[i])) {
           return 0;
        }
    }
    return 1;
}

// return the element at the head
int buffer_head(struct Buffer *buffer)
{
    return buffer->data[buffer->head];
}

// find the (non-empty) buffer with the smallest element (at the head)
int get_best_buffer_num(struct Buffer **in, int in_count)
{
    int i,j, best;
    for (i=0; i<in_count; i++) {        
        if (!buffer_empty(in[i])) {
            best = i;
            break;
        }
    }
    for (j=i+1; j<in_count; j++) {        
        if (!buffer_empty(in[j]) && buffer_head(in[j]) < buffer_head(in[best])) {
            best = j;
        }
    }
    return best;
}

// create a buffer with given parameters (data is given)
struct Buffer *buffer_create(int *data, int nmemb, int head, int tail, int count)
{
    struct Buffer *buffer = (struct Buffer *)
               malloc(sizeof(struct Buffer));
    buffer->data = data;
    buffer->nmemb = nmemb;
    buffer->head = head;
    buffer->tail = tail;
    buffer->count = count;
    return buffer;
}

// new a buffer with size nmemb (i.e. we malloc data)
struct Buffer *buffer_new(int nmemb)
{ 
    int *data = (int *) malloc(nmemb * sizeof(int));
    return buffer_create(data, nmemb, 0, 0, 0);
}

/* create a new funnel, given in buffers and out buffer
 * in: in buffers
 * out: out buffer
 * in_count: number of in buffers
 */
struct Funnel *funnel_new(struct Buffer **in, struct Buffer *out, int in_count)
{
    struct Funnel *funnel = (struct Funnel *)malloc(sizeof(struct Funnel));
    funnel->in = in;
    funnel->in_count = in_count;
    funnel->out = out;
    
    if (in_count < 5) { // base case
        funnel->bb_count = 0;
        funnel->bottom = NULL;
        funnel->top = NULL;
        return funnel;
    }
    int i;
    double root = sqrt(in_count);
    struct Buffer **curr = in;
    int froot = floor(root);

    if (froot*froot < (int)in_count) { // if in_count isn't square
        int croot = ceil(root);
        int big_count = in_count/(double)croot; // count buffers of size croot
        int rest = in_count - big_count * croot;
        if(rest > 0){ //put the rest in one funnel
			big_count++;
		}
        
        int big_buff_nmemb   = 2 * pow(croot, 3);
        struct Buffer **buffers = (struct Buffer**)
                  malloc(sizeof(struct Buffer *)*big_count); // intermediate buffers

        struct Funnel **bfunnels = (struct Funnel **)
                  malloc(sizeof(struct Funnel *)*big_count); // bottom funnels

        for (i=0; i<big_count; i++) {
            buffers[i] = buffer_new(big_buff_nmemb);
			if(i == big_count-1 && rest > 0){
				bfunnels[i] = funnel_new(curr, buffers[i], rest);
			}
			else
            	bfunnels[i] = funnel_new(curr, buffers[i], croot);             
            curr += croot;
        }


        funnel->bb_count = big_count;
        funnel->buffers = buffers;
        funnel->bottom = bfunnels;

        struct Funnel *tfunnel = (struct Funnel *)
                  malloc(sizeof(struct Funnel *));
        tfunnel = funnel_new(buffers, out, big_count);
        funnel->top = tfunnel;
    }
    else { // if in_count is a square
        funnel->bb_count = froot;
        struct Buffer **buffers = (struct Buffer**)
                  malloc(sizeof(struct Buffer *)*froot); // intermediate buffers

        struct Funnel **bfunnels = (struct Funnel **)
                  malloc(sizeof(struct Funnel *)*froot); // bottom funnels
        int buff_nmemb = 2 * pow(froot, 3);
        
        for (i=0; i<froot; i++) {
            buffers[i] = buffer_new(buff_nmemb);
            bfunnels[i] = funnel_new(curr, buffers[i], froot);
            curr += froot;
        }
        
        funnel->buffers = buffers;
        funnel->bottom = bfunnels;

        struct Funnel *tfunnel = (struct Funnel *)
                  malloc(sizeof(struct Funnel *));
        tfunnel = funnel_new(buffers, out, froot);
        funnel->top = tfunnel;
    }
    return funnel;
}

/* divide data in count (+1) buffers
 * data: data array
 * nmemb: length of the array
 * count: number of buffers
 * len: length of a buffer
 * extra: 1, then +1;
 */
struct Buffer **buffers_create(int *data, int nmemb, int count, int len, int extra)
{
    struct Buffer **buffers = (struct Buffer **)malloc(sizeof(struct Buffer*)*(count + extra)); 
	int i;
    int *p = data;
    for (i=0; i < count; i++) {
        buffers[i] = buffer_create(p, len, 0, 0, len); //full
        p += len;
    }
    // if extra == 1, put the rest of the elements in one buffer
    if (extra == 1) {
        int rest = nmemb - len * count;
        buffers[i] = buffer_create(p, rest, 0, 0, rest);
    }
    
    return buffers;
}

// create a funnel by simply giving data
struct Funnel *funnel_create(int *data, int nmemb, int count, int len, int extra)
{
    struct Buffer **in_buffers = buffers_create(data, nmemb, count, len, extra);
    struct Buffer *out_buffer = buffer_new(nmemb);
    return funnel_new(in_buffers, out_buffer, count+extra);
}


//TODO
// Currently, the recursive fill() call only supports 3 layers of funnels
// Because the base case we use is 5 input buffers for the smallest funnel,
// If the array size is less than ~10,000,000, the algorithm works fine (all 3 layers of funnels
// can be expanded and work as expected.)
// If the array size is greater than 10,000,000, the right funnel (top funnel) will not recursively
// call the smaller funnels. Instead, it will directly copy elements using trivial merging for k arrays
// as k-way merge sort does.
void funnel_left_fill(struct Funnel *funnel);
void funnel_right_fill(struct Funnel *funnel, struct Funnel *fa_funnel);
void funnel_left_fill_r(struct Funnel *funnel);
void funnel_right_fill_r(struct Funnel *funnel, struct Funnel *fa_funnel);

void funnel_left_fill_r(struct Funnel *funnel){
    if(funnel->bb_count == 0){
        funnel_left_fill(funnel);
        return;
    }
    for(int i = 0; i < funnel->bb_count; i++){
		funnel_left_fill_r(funnel->bottom[i]);
	}
	funnel_right_fill_r(funnel->top, funnel);
}

void funnel_right_left_fill_r(struct Funnel *funnel, struct Funnel *fa_funnel, int i, int ic);
void funnel_right_right_fill_r(struct Funnel *funnel, struct Funnel *fa_funnel, struct Funnel *gfa_funnel, int ic);

void funnel_right_fill_r(struct Funnel *funnel, struct Funnel *fa_funnel){
    if(funnel->bb_count == 0){
        funnel_right_fill(funnel, fa_funnel);
        return;
    }
	for(int i = 0; i < funnel->bb_count; i++){
		// similar to funnel_right_fill()
		funnel_right_left_fill_r(funnel->bottom[i], fa_funnel, i, funnel->bottom[0]->in_count);
	}
	funnel_right_right_fill_r(funnel->top, funnel, fa_funnel, funnel->bottom[0]->in_count);
}

void funnel_right_left_fill_r(struct Funnel *funnel, struct Funnel *fa_funnel, int idx, int ic){
		// We copy until the output buffer is full
		//if(funnel->bb_count == 0){
        	while (!buffer_full(funnel->out) && !buffers_empty(funnel->in, funnel->in_count)) {
           		int best_num = get_best_buffer_num(funnel->in, funnel->in_count);
           		buffer_enqueue(funnel->out, buffer_dequeue(funnel->in[best_num])); 
				if (buffer_empty(funnel->in[best_num])) {
					funnel_left_fill_r(fa_funnel->bottom[best_num + idx * ic]);
				}
			}
			return;
		//}
		//for(int i = 0; i < funnel->bb_count; i++){
			// similar to funnel_right_fill()
		//	funnel_right_left_fill_r(funnel->bottom[i], fa_funnel, i, funnel->bottom[0]->in_count);
		//}
		//funnel_right_right_fill_r(funnel->top, funnel, fa_funnel, funnel->bottom[0]->in_count);
}

// base case for right funnel
void funnel_right_right_fill_r(struct Funnel *funnel, struct Funnel *fa_funnel, struct Funnel *gfa_funnel, int ic){
	//if(funnel->bb_count == 0){
		// We copy until the output buffer is full
        while (!buffer_full(funnel->out) && !buffers_empty(funnel->in, funnel->in_count)) {
           	int best_num = get_best_buffer_num(funnel->in, funnel->in_count);
           	buffer_enqueue(funnel->out, buffer_dequeue(funnel->in[best_num])); 
			if (buffer_empty(funnel->in[best_num])) {
				funnel_right_left_fill_r(fa_funnel->bottom[best_num], gfa_funnel, best_num, ic);
			}
		}
		return;
	//}
	//for(int i = 0; i < funnel->bb_count; i++){
		// similar to funnel_right_fill()
	//	funnel_right_left_fill_r(funnel->bottom[i], fa_funnel, i, funnel->bottom[0]->in_count);
	//}
	//funnel_right_right_fill_r(funnel->top, funnel, fa_funnel, funnel->bottom[0]->in_count);
}

// base case for left funnel
void funnel_left_fill(struct Funnel *funnel){
	// if not all input buffers are empty
	if(!buffers_empty(funnel->in, funnel->in_count)){
		// We copy until the output buffer is full or we used up all elements
        while (!buffer_full(funnel->out) && !buffers_empty(funnel->in, funnel->in_count)) {
           	int best_num = get_best_buffer_num(funnel->in, funnel->in_count);
           	buffer_enqueue(funnel->out, buffer_dequeue(funnel->in[best_num])); 
		}
	}
}

// base case for right funnel
void funnel_right_fill(struct Funnel *funnel, struct Funnel *fa_funnel){
		// We copy until the output buffer is full
        while (!buffer_full(funnel->out) && !buffers_empty(funnel->in, funnel->in_count)) {
           	int best_num = get_best_buffer_num(funnel->in, funnel->in_count);
           	buffer_enqueue(funnel->out, buffer_dequeue(funnel->in[best_num])); 
			if (buffer_empty(funnel->in[best_num])) {
				funnel_left_fill_r(fa_funnel->bottom[best_num]);
			}
		}
		
}

int cmp_int(const void * x, const void * y)
{
    return (*(int *)x - *(int *)y);
}
void fsort(int nmemb, int *data){
	if(nmemb < 10){
		qsort(data, nmemb, sizeof(int), cmp_int);
		return;
	}
	int n = pow(nmemb, 1/3.0); // n = nmemb^(1/3), count of subarrays
	int len = nmemb / n; // nmemb in each part
	int *p = data;
	int i;
	for (i=0; i<n; i++) {
		fsort(len, p);
		p += len;
	}
	int extra = 0;
	int rest = nmemb - n * len;
	if (rest > 0) {
		fsort(rest, p);
		extra = 1;
	}
	struct Funnel *funnel = funnel_create(data, nmemb, n, len, extra);

	funnel_left_fill_r(funnel);
	
	//if(nmemb >= 1000)
	//	print_funnel(funnel, 0);
    //    printf("=====================================\n");
	// note that memcpy will change data, which is also the input arrays
	memcpy(data, funnel->out->data, nmemb*sizeof(int));

	//extern int *arr_ref, arr_size_ref;
	//printf("(");
	//for(int i = 0; i<arr_size_ref; i++){
	//	printf("%d ",arr_ref[i]);
	//}
	//printf(")\n");
}
////////////////////////////////////////////////
char sort4_desc[] = "Funnel Sort";
void sort4(int N, int A[N])
{
    fsort(N, A);
}

/* 
 * registerTransFunction - Add the given function into the list
 *     of functions to be tested
 */
void registerTransFunction(void (*func)(int N, int A[N]),
                           char *desc)
{
    func_list[func_counter].func_ptr = func;
    func_list[func_counter].description = desc;
    func_counter++;
}

/*
 * registerFunctions - All functions registered will be tested
 */
void registerFunctions()
{
    /****************************** Register functions *********************************/
    //registerTransFunction(sort1, sort1_desc);
    //registerTransFunction(sort2, sort2_desc);
    registerTransFunction(sort3, sort3_desc);
    registerTransFunction(sort4, sort4_desc);
    /***********************************************************************************/
}
