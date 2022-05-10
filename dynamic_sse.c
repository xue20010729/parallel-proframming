#include<pthread.h>
#include<stdlib.h>
#include<sys/time.h>
#include <stdio.h>
#include <semaphore.h>
#include <emmintrin.h>
#include <pmmintrin.h> //SSE3


// int num[]={16,64,128,256,512,1024,2048};
float **matrix;
int num=16;
int n;

typedef struct 
{
    int k;  //消去的轮次
    int t_id;      //线程id
}threadParam_t;

void generate(){
    matrix = (float**)malloc(sizeof(float*) * num);
    for (int i = 0; i < num; i++)
    {
        matrix[i] = (float*)malloc(sizeof(float*) * num);
    }

    for(int i=0;i<num;i++){
        for(int j=0;j<num;j++){
            matrix[i][j]= 0.8*i+1.3*j +1;
        }
    }
    
}

void print(){
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num; j++) {
            printf("%f ",matrix[i][j]);
        }
    printf("\n");
    }
}


void Delete(){
    for(int i=0;i<num;i++){
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
void* threadFunc_dynamic(void* param){
    threadParam_t* p = (threadParam_t* )param;
    int k= p->k;
    int t_id = p->t_id;
    int i = k +t_id +1; //获取自己的计算任务 ,就是算哪一行
    // for (int j = k + 1; j < n; j++) {
    //     matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
    // }
    __m128 vaik=_mm_set1_ps(matrix[i][k]);
    int j;
    for (j = k + 1; j +4< n; j+=4) {
        __m128 vakj=_mm_loadu_ps(matrix[k]+j);
        __m128 vaij=_mm_loadu_ps(matrix[i]+j);
        __m128 vx=_mm_mul_ps(vakj,vaik);
        vaij =vaij -vx;
        // M[i][j] = M[i][j] - M[i][k] * M[k][j];
        _mm_storeu_ps(matrix[i]+j, vaij);
    }
    for(;j<n;j++){
        matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
    }
    matrix[i][k]=0;

    pthread_exit(NULL);
}


void LU_thread_dynamic(){
    for (int k = 0; k < n; k++) {
        //主线程做除法操作
        for (int j = k + 1; j < n; j++) {
            matrix[k][j] = matrix[k][j] / matrix[k][k];
        }
        matrix[k][k] = 1;

        int work_count = n-1 -k; //工作线程数量
        pthread_t* handle =malloc(work_count*sizeof(pthread_t));
        threadParam_t* param = malloc(work_count*sizeof(threadParam_t));

        for (int t_id = 0; t_id < work_count; t_id++) {
            param[t_id].k = k;
            param[t_id].t_id = t_id;
        }

        for (int t_id = 0; t_id < work_count; t_id++) {
            pthread_create(&handle[t_id], NULL, threadFunc_dynamic, (void*) (param + t_id) );
        }

        for (int t_id = 0; t_id < work_count; t_id++) {
            pthread_join(handle[t_id], NULL);
        }

        free(handle);
        free(param);
    }
}

sem_t sem_main;
sem_t sem_workstart;
sem_t sem_workend;


int main()
{
    n=num;
    generate();
    // print();
    // LU();
    LU_thread_dynamic();
    print();
    Delete();


    return 0;
}
