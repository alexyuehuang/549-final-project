/* 
 * sort.c - Sorting an array
 */ 
#include <stdio.h>
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

char sort2_desc[] = "Merge Sort";
void sort2(int N, int A[N])
{

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
    /***********************************************************************************/   
}

