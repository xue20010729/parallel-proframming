#include <stdio.h>
#include<Windows.h>
#include <cuda_runtime.h>
#include <cublas_v2.h>
#include <iostream>
using namespace std;
#define N 3

//void cal_sum_test()
//{
//    LARGE_INTEGER nFreq;
//    LARGE_INTEGER nBeginTime;
//    LARGE_INTEGER nEndTime;
//
//    QueryPerformanceFrequency(&nFreq);
//
//
//    QueryPerformanceCounter(&nEndTime);
//    cout<<" GPU time (global memory):"<< (double)(nEndTime.QuadPart - nBeginTime.QuadPart)*1.0/nFreq.QuadPart<<endl;
//
//
//}

void print_matrix(double *matrix){
    cout << endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout <<" " << matrix[i * N + j] ;
        }
        cout << endl;
    }
}
void test()
{
    double* h_A = (double*)malloc(sizeof(double) * N * N);
    double* h_B = (double*)malloc(sizeof(double) * N * N);
    double* h_C = (double*)malloc(sizeof(double) * N * N);
    for (int i = 0; i < N * N; i++) {
        h_A[i] = i;
        h_B[i] = i;
    }

    double* g_A, * g_B, * g_C;
    cudaMalloc((void**)&g_A, sizeof(double) * N * N);
    cudaMalloc((void**)&g_B, sizeof(double) * N * N);
    cudaMalloc((void**)&g_C, sizeof(double) * N * N);

    cublasSetVector(N * N, sizeof(double), h_A, 1, g_A, 1);
    cublasSetVector(N * N, sizeof(double), h_B, 1, g_B, 1);
    cublasSetVector(N * N, sizeof(double), h_C, 1, g_C, 1);

    cublasHandle_t  handle;
    cublasCreate(&handle);
    double alpha = 1.0;
    double beta = 0.0;
    cublasDgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, N, N, N, &alpha, g_A, N, g_B, N, &beta, g_C, N);
    cublasDestroy(handle);

    cublasGetVector(N * N, sizeof(double), g_C, 1, h_C, 1);
    print_matrix(h_C);

    free(h_A);
    free(h_B);
    free(h_C);
    cudaFree(g_A);
    cudaFree(g_B);
    cudaFree(g_C);

}


int main()
{
    test();
    return 0;
}
