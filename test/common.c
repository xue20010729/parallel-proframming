#include<stdlib.h>
#include<sys/time.h>
#include <stdio.h>

#define  NUM_THREADS 4

int num[]={16,64,128,256,512,1024,2048};
float **matrix;
// int num=16;
int n;

typedef struct 
{
    int t_id;      //线程id
}threadParam_t;

pthread_barrier_t barrier_Division;
pthread_barrier_t barrier_Elimination;


void generate(int i){
    if(i!= 0){
        for(int i=0;i<num[i-1];i++){
            free(matrix[i]);
        }
        free(matrix);
    }

    matrix = (float**)malloc(sizeof(float*) * n);
    for (int i = 0; i < n; i++)
    {
        matrix[i] = (float*)malloc(sizeof(float*) * n);
    }

    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            matrix[i][j]= 0.8*i+1.3*j +1;
        }
    }
    
}

void reset(){
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            matrix[i][j]= 0.8*i+1.3*j +1;
        }
    }
}


void print(){
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%f ",matrix[i][j]);
        }
    printf("\n");
    }
}


void Delete(){
    for(int i=0;i<n;i++){
        free(matrix[i]);
    }
    free(matrix);
}

void LU() {
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
}



int main()
{
    for(int i=0;i<7;i++){
        n=num[i];
        generate(i);
        struct timeval time1, time2;
        gettimeofday(&time1, NULL);
        for(int j=0;j<10;j++){
            // generate(i);
            reset();
            LU();        
        }
        gettimeofday(&time2, NULL);
        double elapsed_time = (time2.tv_sec - time1.tv_sec) * 1000. +
                    (time2.tv_usec - time1.tv_usec) / 1000.;
        // cout<<"大小"<<"用时"<<elapsed_time/10<<"ms"<<endl;
        printf("大小%d用时%fms\n",n,elapsed_time/10);
    }
    printf("over\n");
    Delete();


    return 0;
}
