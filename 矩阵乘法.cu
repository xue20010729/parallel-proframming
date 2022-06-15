#include <stdio.h>
#include<Windows.h>
#include <cuda_runtime.h>
#include <cublas_v2.h>
#include <iostream>
using namespace std;
#define N 1024
#define TILE_WIDTH 16

void print_matrix(double* matrix) {
    cout << endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << " " << matrix[i * N + j];
        }
        cout << endl;
    }
}

__global__ void matrixMulGPU(double* a, double* b, double* c)
{
    int val = 0;

    int row = blockIdx.x * blockDim.x + threadIdx.x;
    int col = blockIdx.y * blockDim.y + threadIdx.y;

    if (row < N && col < N)
    {
        for (int k = 0; k < N; ++k)
            val += a[row * N + k] * b[k * N + col];
        c[row * N + col] = val;
    }
}


__global__ void matrixMulGPU_pro(double* a, double* b, double* c)
{
    __shared__ double Mds[TILE_WIDTH][TILE_WIDTH];
    __shared__ double Nds[TILE_WIDTH][TILE_WIDTH];

    int bx = blockIdx.x;
    int by = blockIdx.y;
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    int Col = bx * TILE_WIDTH + tx;
    int Row = by * TILE_WIDTH + ty;

    int val = 0;

    for (int i = 0; i < N / TILE_WIDTH; i++)  //有多少个TILE_WIDTH，每个循环计算一个块的大小
    {
        Mds[ty][tx] = a[Row * N + (i * TILE_WIDTH + tx)];
        Nds[ty][tx] = b[Col + (i * TILE_WIDTH + ty) * N];
        __syncthreads();


        for (int k = 0; k < TILE_WIDTH; k++) //TILE_WIDTH相乘
            val += Mds[ty][k] * Nds[k][tx];
        __syncthreads();
    }

    c[Row * N + Col] = val;
}


void cublas_matrixMul()
{
    double* h_A = (double*)malloc(sizeof(double) * N * N);
    double* h_B = (double*)malloc(sizeof(double) * N * N);
    double* h_C = (double*)malloc(sizeof(double) * N * N);
    for (int i = 0; i < N * N; i++) {
        h_A[i] = i;
        h_B[i] = i;
    }

    double* g_A, * g_B, * g_C_cubals;
    cudaMalloc((void**)&g_A, sizeof(double) * N * N);
    cudaMalloc((void**)&g_B, sizeof(double) * N * N);
    cudaMalloc((void**)&g_C_cubals, sizeof(double) * N * N);

    cublasSetVector(N * N, sizeof(double), h_A, 1, g_A, 1);
    cublasSetVector(N * N, sizeof(double), h_B, 1, g_B, 1);
    cublasSetVector(N * N, sizeof(double), h_C, 1, g_C_cubals, 1);

    cublasHandle_t  handle;
    cublasCreate(&handle);
    double alpha = 1.0;
    double beta = 0.0;
    cublasDgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, N, N, N, &alpha, g_A, N, g_B, N, &beta, g_C_cubals, N);
    cublasDestroy(handle);

    //cublasGetVector(N * N, sizeof(double), g_C_cubals, 1, h_C, 1);
    //print_matrix(h_C);

    free(h_A);
    free(h_B);
    free(h_C);
    cudaFree(g_A);
    cudaFree(g_B);
    cudaFree(g_C_cubals);

}

void cal_sum_test()
{
    cout << endl;
    LARGE_INTEGER nFreq;
    LARGE_INTEGER nBeginTime;
    LARGE_INTEGER nEndTime;

    QueryPerformanceFrequency(&nFreq);
    double* h_A = (double*)malloc(sizeof(double) * N * N);
    double* h_B = (double*)malloc(sizeof(double) * N * N);
    double* h_C = (double*)malloc(sizeof(double) * N * N);
    for (int i = 0; i < N * N; i++) {
        h_A[i] = i;
        h_B[i] = i;
    }

    double* g_A, * g_B, * g_C_cubals,*g_C_my;
    cudaMalloc((void**)&g_A, sizeof(double) * N * N);
    cudaMalloc((void**)&g_B, sizeof(double) * N * N);
    cudaMalloc((void**)&g_C_cubals, sizeof(double) * N * N);
    cudaMalloc((void**)&g_C_my, sizeof(double) * N * N);

    cublasSetVector(N * N, sizeof(double), h_A, 1, g_A, 1);
    cublasSetVector(N * N, sizeof(double), h_B, 1, g_B, 1);
    cublasSetVector(N * N, sizeof(double), h_C, 1, g_C_cubals, 1);
    cublasSetVector(N * N, sizeof(double), h_C, 1, g_C_my, 1);

    QueryPerformanceCounter(&nBeginTime);

    cublasHandle_t  handle;
    cublasCreate(&handle);
    double alpha = 1.0;
    double beta = 0.0;
    cublasDgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, N, N, N, &alpha, g_A, N, g_B, N, &beta, g_C_cubals, N);
    cublasDestroy(handle);


    QueryPerformanceCounter(&nEndTime);
    cout << "    cublas_matrix_multiply " << (double)(nEndTime.QuadPart - nBeginTime.QuadPart) * 1.0 / nFreq.QuadPart << endl;


    QueryPerformanceCounter(&nBeginTime);

    dim3 threads_per_block_(16, 16, 1);
    dim3 number_of_blocks_((N / threads_per_block_.x) + 1, (N / threads_per_block_.y) + 1, 1);


    matrixMulGPU << < number_of_blocks_, threads_per_block_ >> > (g_A, g_B, g_C_my);

    cudaDeviceSynchronize();
    QueryPerformanceCounter(&nEndTime);
    cout << "    my_matrix_multiply     " << (double)(nEndTime.QuadPart - nBeginTime.QuadPart) * 1.0 / nFreq.QuadPart << endl;

    QueryPerformanceCounter(&nBeginTime);

    dim3 threads_per_block(TILE_WIDTH, TILE_WIDTH, 1);
    dim3 number_of_blocks(N / TILE_WIDTH, N / TILE_WIDTH, 1);
    

    matrixMulGPU_pro<< < number_of_blocks, threads_per_block >> > (g_A, g_B, g_C_my);

    cudaDeviceSynchronize();
    QueryPerformanceCounter(&nEndTime);
    cout<<"    my_matrix_multiply_pro "<< (double)(nEndTime.QuadPart - nBeginTime.QuadPart)*1.0/nFreq.QuadPart<<endl;


    free(h_A);
    free(h_B);
    free(h_C);
    cudaFree(g_A);
    cudaFree(g_B);
    cudaFree(g_C_cubals);
    cudaFree(g_C_my);
}






int main()
{
    cal_sum_test();
    return 0;
}
