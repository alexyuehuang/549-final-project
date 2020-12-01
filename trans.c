/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
 //transpose function for submission
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int a,b,c,d,e,f,g,h;
    //case for 32*32: use 8*8 blockings and store values into local variables
    if(M==32){
        for(int i=0;i<4;i++){
        for(int j=0; j<4; j++){
            for(int m=0; m<8;m++){
                a=A[(8*i+m)][(8*j)];
                b=A[(8*i+m)][(8*j+1)];
                c=A[(8*i+m)][(8*j+2)];
                d=A[(8*i+m)][(8*j+3)];
                e=A[(8*i+m)][(8*j+4)];
                f=A[(8*i+m)][(8*j+5)];
                g=A[(8*i+m)][(8*j+6)];
                h=A[(8*i+m)][(8*j+7)];
                B[(8*j)][(8*i+m)]=a;
                B[(8*j+1)][(8*i+m)]=b;
                B[(8*j+2)][(8*i+m)]=c;
                B[(8*j+3)][(8*i+m)]=d;
                B[(8*j+4)][(8*i+m)]=e;
                B[(8*j+5)][(8*i+m)]=f;
                B[(8*j+6)][(8*i+m)]=g;
                B[(8*j+7)][(8*i+m)]=h;
            }
        }
    }
    }
    //case for 64*64, use 8*8 blocking, but this time divide into 8*4 blockings and load by parts
    if(M==64){
        for(int i=0;i<64;i+=8){
        for(int j=0; j<64; j+=8){
            for(int m=0; m<4;m++){
                a=A[(i+m)][(j)];
                b=A[(i+m)][(j+1)];
                c=A[(i+m)][(j+2)];
                d=A[(i+m)][(j+3)];
                B[(j)][(i+m)]=a;
                B[(j+1)][(i+m)]=b;
                B[(j+2)][(i+m)]=c;
                B[(j+3)][(i+m)]=d;
            }
            for(int m=0; m<4;m++){
                a=A[(i+m)][(j+4)];
                b=A[(i+m)][(j+5)];
                c=A[(i+m)][(j+6)];
                d=A[(i+m)][(j+7)];
                B[(j)][(i+m+4)]=a;
                B[(j+1)][(i+m+4)]=b;
                B[(j+2)][(i+m+4)]=c;
                B[(j+3)][(i+m+4)]=d;
            }
            //first line
            for(int m=0; m<4;m++){
                a=A[(i+4)][(j+m)];
                b=A[(i+5)][(j+m)];
                c=A[(i+6)][(j+m)];
                d=A[(i+7)][(j+m)];
                e=B[(j+m)][(i+4)];
                f=B[(j+m)][(i+5)];
                g=B[(j+m)][(i+6)];
                h=B[(j+m)][(i+7)];
                B[(j+m)][(i+4)]=a;
                B[(j+m)][(i+5)]=b;
                B[(j+m)][(i+6)]=c;
                B[(j+m)][(i+7)]=d;
                B[(j+m+4)][(i)]=e;
                B[(j+m+4)][(i+1)]=f;
                B[(j+m+4)][(i+2)]=g;
                B[(j+m+4)][(i+3)]=h;
            }
            i+=4;
            j+=4;
            for(int m=0; m<4;m++){
                a=A[(i+m)][(j)];
                b=A[(i+m)][(j+1)];
                c=A[(i+m)][(j+2)];
                d=A[(i+m)][(j+3)];
                B[(j)][(i+m)]=a;
                B[(j+1)][(i+m)]=b;
                B[(j+2)][(i+m)]=c;
                B[(j+3)][(i+m)]=d;
            }
            i-=4;
            j-=4;
        }
    }
    }
    //case for 61*67. Divide into blocks of 17*17. Optimized for this particular problem
    if(M==61){
        for(int i=0; i<61;i+=17){
        for (int j=0; j<67;j+=17){
            for(int l=0; ((l<17)&&((j+l)<67));++l){
                for(int k=0; ((k<17)&&((i+k)<61));++k){
                    a=A[(j+l)][(i+k)];
                    B[(i+k)][(j+l)]=a;
                }
            }
        }
    }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 
/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

