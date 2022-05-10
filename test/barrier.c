#include<pthread.h>
#include<stdlib.h>
#include<sys/time.h>
#include <stdio.h>
#include <semaphore.h>

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


void* threadFunc_static_pro(void* param){
    threadParam_t* p = (threadParam_t* )param;
    int t_id = p->t_id;

    // t_id 为 0 的线程做除法操作，其它工作线程先等待
    // 这里只采用了一个工作线程负责除法操作，同学们可以尝试采用多个工作线程完成除法操作
    // 比信号量更简洁的同步方式是使用 barrier
    for (int k = 0; k < n; k++)
    {
        if(t_id==0){
            for (int j = k + 1; j < n; j++) {
                matrix[k][j] = matrix[k][j] / matrix[k][k];
            }
            matrix[k][k] = 1;
        }

        pthread_barrier_wait(&barrier_Division);


        //循环划分任务（同学们可以尝试多种任务划分方式）
        for(int i =k +1+t_id; i<n ;i+= NUM_THREADS){
            for (int j = k + 1; j < n; j++) {
                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
            }   
            matrix[i][k]= 0;
        }

        pthread_barrier_wait(&barrier_Elimination);
    }
    pthread_exit(NULL);
}


void LU_thread_static_barrier(){
    pthread_barrier_init(&barrier_Division,NULL,NUM_THREADS);
    pthread_barrier_init(&barrier_Elimination,NULL,NUM_THREADS);

    pthread_t handles[NUM_THREADS];
    threadParam_t param[NUM_THREADS];

    for (int t_id = 0; t_id < NUM_THREADS; t_id++) {
        param[t_id].t_id = t_id;
        pthread_create(&handles[t_id], NULL, threadFunc_static_pro, (void*) (param + t_id));
    }    

    for (int t_id = 0; t_id < NUM_THREADS; t_id++) {
        pthread_join(handles[t_id], NULL);
    }


    pthread_barrier_destroy(&barrier_Division);
    pthread_barrier_destroy(&barrier_Elimination);

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
            LU_thread_static_barrier();        
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
