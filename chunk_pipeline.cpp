/*
本文件是块划分的流水线算法
*/
#include<stdlib.h>
#include<sys/time.h>
#include <stdio.h>
#include<cmath>
#include <mpi.h>

int size[]={16,64,128,256,512,1024,2048};
int n;

int max(int a,int b){
    return a>b?a:b;
}


int main(int argc,char* argv[])
{
    n=size[6];
    float ** matrix=new float*[n];
	float *buffer = new float[n*n];
	for(int i=0;i<n;i++){
		matrix[i]=buffer+i*n;
	}

    int rank,size;
    MPI_Status status;

    struct timeval time1, time2;
    gettimeofday(&time1, NULL);

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);


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
        if (r1<=k && r2>=k) {	
			for (int j = k + 1; j < n; j++){
                matrix[k][j] /= matrix[k][k];
            }
            matrix[k][k] = 1.0;
            if(rank!=size-1)
				MPI_Send(&matrix[k][0], n, MPI_FLOAT, rank+1, k, MPI_COMM_WORLD);
			for (int i = k + 1; i <= r2; i++) {
				for (int j = k + 1; j < n; j++)
					matrix[i][j] -= matrix[i][k] * matrix[k][j];
				matrix[i][k] = 0;
			}
		}
		else if (r1> k) {
			MPI_Recv(&matrix[k][0], n, MPI_FLOAT, rank-1, k, MPI_COMM_WORLD, &status);
			if(rank!=size-1)
				MPI_Send(&matrix[k][0], n, MPI_FLOAT, rank + 1, k, MPI_COMM_WORLD);
			for (int i = r1; i <= r2; i++) {
				for (int j = k + 1; j < n; j++)
					matrix[i][j] -= matrix[i][k] * matrix[k][j];
				matrix[i][k] = 0;
			}
		}
    }
    // 最终汇总一下
    if(rank!=0){
        MPI_Send(&matrix[r1][0],(r2-r1+1)*n,MPI_FLOAT,0,rank,MPI_COMM_WORLD);
    }else{
        for(int i=1;i<size;i++){
            int r1_ = i * average, r2_ = (i == size - 1) ? n - 1 : (i + 1)*average - 1;
            MPI_Recv(&matrix[r1_][0],n*(r2_-r1_+1),MPI_FLOAT,i,i,MPI_COMM_WORLD,&status);
        }
    }
    MPI_Finalize();

    if(rank==0){
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

    delete[] matrix;
    delete[] buffer;
    return 0;
}