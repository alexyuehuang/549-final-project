/* 
 * sort.c - Sorting an array
 */ 
#include <stdio.h>
#include <limits.h> // INT_MIN
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
    for (i = 0; i < N - 1; i++) { 
        min_idx = i; 
        for (j = i + 1; j < N; j++) {
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
void merge(int *Arr, int start, int mid, int end) {

	// crawlers for both intervals and for temp
	int i = start, j = mid+1, k = 0;

	// traverse both arrays and in each iteration add smaller of both elements in temp 
	while(i <= mid && j <= end) {
		if(Arr[i] <= Arr[j]) {
			temp[k] = Arr[i];
			k += 1; i += 1;
		}
		else {
			temp[k] = Arr[j];
			k += 1; j += 1;
		}
	}

	// add elements left in the first interval 
	while(i <= mid) {
		temp[k] = Arr[i];
		k += 1; i += 1;
	}

	// add elements left in the second interval 
	while(j <= end) {
		temp[k] = Arr[j];
		k += 1; j += 1;
	}

	// copy temp to original interval
	for(i = start; i <= end; i += 1) {
		Arr[i] = temp[i - start];
	}
}

// Arr is an array of integer type
// start and end are the starting and ending index of current interval of Arr
void mergeSort(int *Arr, int start, int end) {
	if(start < end) {
		int mid = (start + end) / 2;
		mergeSort(Arr, start, mid);
		mergeSort(Arr, mid+1, end);
		merge(Arr, start, mid, end);
	}
}

char sort2_desc[] = "Merge Sort";
void sort2(int N, int A[N])
{
    mergeSort(A, 0, N-1);
}
///////////////////////////////////////////////////////////////////////////////
// k-way merge sort
#define K 9 // K has to be defined at compile time to keep the interface consistent

//int temp[MAXN]; //reusable temp array // already defined
void kMerge(int *Arr, int start, int end) {
    int N = end - start + 1;// total length
    int n = N / K; // length of each subarray
    int leftover = N % K; // note: n * K + leftover = N
    int ptrs[K] = {0}; // offsets at each subarray
    for(int idx = 0; idx < N; idx++){
        int min = INT_MAX;
        int min_i;
        int s = start;
        // the first leftover subarrays has n+1 elements and the rest has n
        // iterate all the arrays to find the smallest element and to put it in temp[idx]
        for(int i = 0; i < K; i++){
            //check ptrs[i] does not go out of range
            if(i < leftover) {
                if(ptrs[i] == n + 1) {
                    s = s + n + 1;
                    continue;
                }
            }else{
                if(ptrs[i] == n) {
                    s = s + n;
                    continue;
                }
            }

            // record the current array in
            if(min > Arr[s + ptrs[i]]){
                min = Arr[s + ptrs[i]];
                min_i = i;
            }
            if(i < leftover) {
                s = s + n + 1;
            }else{
                s = s + n;
            }
        }
        temp[idx] = min;
        ptrs[min_i]++;
    }
    // copy temp to original interval
	for(int i = start; i <= end; i++) {
		Arr[i] = temp[i - start];
	}
}

// Arr is an array of integer type
// start and end are the starting and ending index of current interval of Arr
void kMergeSort(int *Arr, int start, int end) {
	if(start < end) {
		int N = end - start + 1;// total length
        int n = N / K; // length of each subarray
        int leftover = N % K; // note: n * K + leftover = N
        // the first leftover subarrays has n+1 elements and the rest has n
        int s = start;
        for(int i = 0; i < K; i++){
            if(i < leftover) {
		        kMergeSort(Arr, s, s + n);
                s = s + n + 1;
            }else{
                
                kMergeSort(Arr, s, s + n - 1);
                s = s + n;
            }
        }
        
		kMerge(Arr, start, end);
	}
}

char sort3_desc[] = "K-Way Merge Sort";
void sort3(int N, int A[N]){
    kMergeSort(A, 0, N-1);
}

/* 
 * registerTransFunction - Add the given function into the list
 *     of functions to be tested
 */
void registerTransFunction(void (*func)(int N, int A[N]), 
                           char* desc)
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
    registerTransFunction(sort1, sort1_desc);
    registerTransFunction(sort2, sort2_desc);
    registerTransFunction(sort3, sort3_desc);
    /***********************************************************************************/   
}

