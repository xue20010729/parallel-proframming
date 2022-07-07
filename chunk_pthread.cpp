/*
本文件是块划分+pthread
*/
#include<stdlib.h>
#include<sys/time.h>
#include <stdio.h>
#include <mpi.h>
#include <pthread.h>
#include<cmath>

int size[]={16,64,128,256,512,1024};
const int n=2048;
float matrix[n][n];
const int NUM_THREADS=4;

pthread_barrier_t	barrier;

typedef struct
{
    int t_id;
	int rank;
    int r1;
    int r2;
    int k;
}threadParam_t;

int max(int a,int b){
    return a>b?a:b;
}
void *threadFunc(void *param)
{
    threadParam_t *p = (threadParam_t*)param;
    int t_id = p->t_id;
	int rank = p->rank;
    int r1 = p->r1;
    int r2 = p->r2;
    int k =p->k;
    for (int i = max(k + 1, r1)+t_id; i <= r2; i+=NUM_THREADS) {
        for (int j = k + 1; j < n; j++){
            matrix[i][j] -= matrix[i][k] * matrix[k][j];
        }
        matrix[i][k] = 0;
        if (i == k + 1 && rank != 0) 
            MPI_Send(&matrix[i][0], n, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);	//减法结果广播回0号进程
    }
    pthread_barrier_wait(&barrier);
    pthread_exit(NULL);
}

int main(int argc,char* argv[])
{
    // n=size[0];

    int rank,size;
    MPI_Status status;

    struct timeval time1, time2;
    gettimeofday(&time1, NULL);

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
    pthread_barrier_init(&barrier,NULL,NUM_THREADS);

    int average = n/size;
    int r1 = rank * average, r2 = (rank == size - 1) ? n - 1 : (rank + 1)*average - 1;
    if (rank == 0) {	//0号进程实现初始化
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                matrix[i][j]= 0.8 * i + 1.3 * j + 1;
            }
        }
        for(int i=1;i<size;i++){
            MPI_Send(&matrix[0][0],n*n,MPI_FLOAT,i,i,MPI_COMM_WORLD);
        }
	}else{
        MPI_Recv(&matrix[0][0],n*n,MPI_FLOAT,0,rank,MPI_COMM_WORLD,&status);
    }

    // 开始消去
    for(int k=0;k<n;k++){
        if (rank == 0) {	//0号进程负责除法部分
			for (int j = k + 1; j < n; j++){
                matrix[k][j] /= matrix[k][k];
            }
            matrix[k][k] = 1.0;
			for (int j = 1; j < size; j++)
				MPI_Send(&matrix[k][0], n, MPI_FLOAT, j, k + 1, MPI_COMM_WORLD);
		}
		else{
			MPI_Recv(&matrix[k][0], n, MPI_FLOAT, 0, k + 1, MPI_COMM_WORLD, &status);
        }
        if (r2 >= k + 1) {		//负责k+1行之后的各进程并发进行减法
			// for (int i = max(k + 1, r1); i <= r2; i++) {
			// 	for (int j = k + 1; j < n; j++){
            //         matrix[i][j] -= matrix[i][k] * matrix[k][j];
            //     }
            //     matrix[i][k] = 0;
			// 	if (i == k + 1 && rank != 0) 
			// 		MPI_Send(&matrix[i][0], n, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);	//减法结果广播回0号进程
			// }
            pthread_t handles[NUM_THREADS];
            threadParam_t param[NUM_THREADS];

            for (int t_id = 0; t_id < NUM_THREADS; t_id++) {
                param[t_id].t_id = t_id;
                param[t_id].rank = rank;
                param[t_id].r1 = r1;
                param[t_id].r2 = r2;
                param[t_id].k = k;
                pthread_create(&handles[t_id], NULL, threadFunc, (void*) (param + t_id));
            }    

            for (int t_id = 0; t_id < NUM_THREADS; t_id++) {
                pthread_join(handles[t_id], NULL);
            }

            pthread_barrier_destroy(&barrier);
		}

		if (rank == 0 && k + 1 > r2&&k+1<n){
			MPI_Recv(&matrix[k + 1][0], n, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        }		

    }
    MPI_Finalize();

    if(rank==0){
        // for (int i = 0; i < n; i++) {
        //     for (int j = 0; j < n; j++) {
        //         printf("%f ",matrix[i][j]);
        //     }
        //     printf("\n");
        // }
        // printf("over\n");
        gettimeofday(&time2, NULL);
        double elapsed_time = (time2.tv_sec - time1.tv_sec) * 1000. +
                    (time2.tv_usec - time1.tv_usec) / 1000.;
        // cout<<"大小"<<"用时"<<elapsed_time/10<<"ms"<<endl;
        printf("大小%d用时%fms\n",n,elapsed_time);

        printf("over\n");
        float ** matrix_copy=new float*[n];
        float *buffer_copy = new float[n*n];
        for(int i=0;i<n;i++){
            matrix_copy[i]=buffer_copy+i*n;
        }
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                matrix_copy[i][j]= 0.8 * i + 1.3 * j + 1;
            }
        }
        for (int k = 0; k < n; k++) {
            for (int j = k + 1; j < n; j++) {
                matrix_copy[k][j] = matrix_copy[k][j] / matrix_copy[k][k];
            }
            matrix_copy[k][k] = 1;
            for (int i = k + 1; i < n; i++) {
                for (int j = k + 1; j < n; j++) {
                    matrix_copy[i][j] = matrix_copy[i][j] - matrix_copy[i][k] * matrix_copy[k][j];
                }
                matrix_copy[i][k] = 0;
            }
        }
        bool success=true;
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                if(fabs(matrix_copy[i][j] - matrix[i][j])>1e-6){
                    success=false;
                }
            }
        }
        if(success){
            printf("成功\n");
        }else{
            printf("失败\n");
        }
        delete[] matrix_copy;
        delete[] buffer_copy;
    }

    return 0;
}