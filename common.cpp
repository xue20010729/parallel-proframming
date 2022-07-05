#include<stdlib.h>
#include<sys/time.h>
#include <stdio.h>

int mpisize=4;
int size[]={16,64,128,256,512,1024,2048};
int n;

int max(int a,int b){
    return a>b?a:b;
}

int main(int argc, char* argv[])
{
    for(int index=0;index<7;index++){
        n=size[index];
        // float matrix[n][n];
        float ** matrix=new float*[n];
        float *buffer = new float[n*n];
        for(int i=0;i<n;i++){
            matrix[i]=buffer+i*n;
        }
        int rank;
        struct timeval time1, time2;
        gettimeofday(&time1, NULL);

        for(int time=0;time<10;time++){
            for(int i=0;i<n;i++){
                for(int j=0;j<n;j++){
                    matrix[i][j]= 0.8*i+1.3*j +1;
                }
            }

            for (int k = 0; k < n; k++) {
                for (int j = k + 1; j < n; j++) {
                    matrix[k][j] = matrix[k][j] / matrix[k][k];
                }
                matrix[k][k] = 1;
                for (int i = k + 1; i < n; i++) {
                    for (int j = k + 1; j < n; j++) {
                        matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
                    }
                    matrix[i][k] = 0;
                }
            }
            // printf("%d\n",time);
        }
        delete[] matrix;
        delete[] buffer;
        
        gettimeofday(&time2, NULL);
        double elapsed_time = (time2.tv_sec - time1.tv_sec) * 1000. +
                    (time2.tv_usec - time1.tv_usec) / 1000.;
        // cout<<"大小"<<"用时"<<elapsed_time/10<<"ms"<<endl;
        printf("大小%d用时%fms\n",n,elapsed_time/10);

        // for (int i = 0; i < n; i++) {
        //     for (int j = 0; j < n; j++) {
        //         printf("%f ",matrix[i][j]);
        //     }
        //     printf("\n");
        // }
    }
    

    printf("over\n");
	
    return 0;
}