/* 
 * mm.c - Matrix Multiplication
 */ 
#include <stdio.h>
#include "mm.h"

// All of the functions waiting to be evaluated
trans_func_t func_list[MAX_FUNCS];
int func_counter = 0; 

char mult1_desc[] = "Ordinary Multiplication";
void mult1(int N, int A[N][N],int B[N][N], int C[N][N])
{
    int sum=0;
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            sum=0;
            for(int k=0;k<N;k++){
                sum+=A[i][k]*B[k][j];
            }
            C[i][j]=sum;
        }
    }
}

char mult2_desc[] = "Multiplication kij";
void mult2(int N, int A[N][N],int B[N][N], int C[N][N])
{
    int r=0;
    for(int k=0;k<N;k++){
        for(int i=0;i<N;i++){
            r=A[i][k];
            for(int j=0;j<N;j++){
                C[i][j]+=r*B[k][j];
            }
        }
    }
}

char mult3_desc[] = "Multiplication blocked";
void mult3(int N, int A[N][N],int B[N][N], int C[N][N])
{
    int b=8;//block size;
    for(int i=0; i<N;i+=b){
        for(int j=0;j<N;j+=b){
            for(int k=0; k<N;k+=b){
                for(int i1=i;i1<i+b;i1++){
                    for(int j1=j;j1<j+b;j1++){
                        for(int k1=k;k1<k+b;k1++){
                            C[i1][j1]+=A[i1][k1]*B[k1][j1];
                        }
                    }
                }
            }
        }
    }
}

char mult4_desc[] = "Multiplication Z-Layout";
void mult4(int N, int A[N][N],int B[N][N], int C[N][N])
{
    if(N==1){
        C[0][0]+=A[0][0]*B[0][0];
        return;
    }
    mult4(N/2,A, B, C);
    mult4(N/2,A[0][N/2], B[N/2][0],C);
    mult4(N/2,A[0][N/2], B[N/2][N/2], C[0][N/2]);
    mult4(N/2,A[0][0], B[0][N/2],C[0][N/2]);
    mult4(N/2,A[N/2][0], B[N/2][N/2], C[N/2][0]);
    mult4(N/2,A[0][0], B[N/2][0],C[N/2][0]);
    mult4(N/2,A[N/2][N/2], B[N/2][N/2], C[N/2][N/2]);
    mult4(N/2,A[N/2][0], B[0][N/2],C[N/2][N/2]);
}

/* 
 * registerTransFunction - Add the given function into the list
 *     of functions to be tested
 */
void registerTransFunction(void (*func)(int N, int A[N][N],int B[N][N], int C[N][N]), 
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
    registerTransFunction(mult1, mult1_desc);
    registerTransFunction(mult2, mult2_desc);
    registerTransFunction(mult3, mult3_desc);
    registerTransFunction(mult4, mult4_desc);
    /***********************************************************************************/   
}
