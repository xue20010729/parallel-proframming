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

sem_t sem_main;
sem_t sem_workerstart[NUM_THREADS];
sem_t sem_workerend[NUM_THREADS];


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


void* threadFunc_static(void* param){
    threadParam_t* p = (threadParam_t* )param;
    int t_id = p->t_id;

    for (int k = 0; k < n; k++)
    {
        sem_wait(&sem_workerstart[t_id]);

        for(int i =k +1+t_id; i<n ;i+= NUM_THREADS){
            for (int j = k + 1; j < n; j++) {
                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
            }   
            matrix[i][k]= 0;
        }

        sem_post(&sem_main);      // 唤醒主线程
        sem_post(&sem_workerend[t_id]);        //阻塞，等待主线程唤醒进入下一轮
    }
    pthread_exit(NULL);
}


void LU_thread_static(){
    //初始化信号量
    sem_init(&sem_main, 0, 0);
    for(int i=0;i<NUM_THREADS;i++){
        sem_init(&sem_workerstart[i], 0, 0);
        sem_init(&sem_workerend[i], 0, 0);
    }


    //创建线程
    pthread_t handles[NUM_THREADS];         // 创建对应的 Handle
    threadParam_t param[NUM_THREADS];     // 创建对应的线程数据结构


    for(int t_id = 0;t_id<NUM_THREADS;t_id++){
        param[t_id].t_id = t_id;
        pthread_create(&handles[t_id], NULL, threadFunc_static, (void*) (param + t_id) );
    }

    for (int k = 0; k < n; k++) {
        //主线程做除法操作
        for (int j = k + 1; j < n; j++) {
            matrix[k][j] = matrix[k][j] / matrix[k][k];
        }
        matrix[k][k] = 1;

        //开始唤醒工作线程
        for(int t_id = 0;t_id<NUM_THREADS;t_id++){
            sem_post(&sem_workerstart[t_id]);
        }

        //主线程睡眠（等待所有的工作线程完成此轮消去任务）
        for(int t_id = 0;t_id<NUM_THREADS;t_id++){
            sem_wait(&sem_main);
        }

        // 主线程再次唤醒工作线程进入下一轮次的消去任务
        for(int t_id = 0;t_id<NUM_THREADS;t_id++){
            sem_post(&sem_workerend[t_id]);
        }
    }

    for (int t_id = 0; t_id < NUM_THREADS; t_id++) {
        pthread_join(handles[t_id], NULL);
    }

    sem_destroy(&sem_main);
    for(int i=0;i<NUM_THREADS;i++){
        sem_destroy(&sem_workerstart[i]);
        sem_destroy(&sem_workerend[i]);
    }

    return ;
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
            LU_thread_static();        
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
