#include <iostream>
#include <arm_neon.h>
#include<cstdlib>
#include<sys/time.h>
using namespace std;

int num[]={16,64,128,256,512,1024,2048};
float **matrix;
int n;

void LU_neon(float **M ) {

    for (int k = 0; k < n; k++) {  
        float val = M[k][k];
        float32x4_t divsor=vld1q_dup_f32(&val);

        //掐头
        int head_n = (k + 1) % 4;
        head_n = 4 - head_n;
        for(int t = k + 1, c = 0; c < head_n && t < n; t ++, c ++){
            M[k][t]/=M[k][k];
        } 

        //中间
        for (int j = k + 1+head_n; j +4 <=n; j+=4) {
            // M[k][j] = M[k][j] / M[k][k];
            float32x4_t v =vld1q_f32(&M[k][j]);
            v=vdivq_f32(v,divsor);
            vst1q_f32(&M[k][j], v);
        }

        //去尾
        int tail_n = n % 4;
        int t = n - tail_n;
        if(k + 1 < t){
            for(; t < n; t++){
                    M[k][t]/=M[k][k];
            }
        }
        M[k][k] = 1;

        for (int i = k + 1; i < n; i++) {
            float val = M[i][k];
            float32x4_t vaik=vld1q_dup_f32(&val);  

            //掐头
            int head_n2 = (k + 1) % 4;
            head_n2 = 4 - head_n2;
            int c = 0;
            for(int t = k + 1; c < head_n2 && t < n; t ++, c ++){
                M[i][t] -= (M[k][t]*M[i][k]);
            }

            //中间
            for (int j = k + 1 +head_n2; j +4<= n; j+=4) {
                float32x4_t vakj=vld1q_f32(&M[k][j]);
                float32x4_t vaij=vld1q_f32(&M[i][j]);
                float32x4_t vx=vmulq_f32(vakj,vaik);
                vaij =vaij -vx;
                // M[i][j] = M[i][j] - M[i][k] * M[k][j];
                vst1q_f32(&M[i][j], vaij);
            }

            //去尾
            int tail_n2 = n % 4;
            int t2 = n - tail_n2;
            if(k + 1 >= t2){
                M[i][k] = 0;
                continue;
            }
            for(; t2 < n; t2++){
                    M[i][t2] -= (M[k][t2]*M[i][k]);
            }
            M[i][k] = 0;
        }
    }
}


void LU(float **M) {
    for (int k = 0; k < n; k++) {
        for (int j = k + 1; j < n; j++) {
            M[k][j] = M[k][j] / M[k][k];
        }
        M[k][k] = 1;
        for (int i = k + 1; i < n; i++) {
            for (int j = k + 1; j < n; j++) {
                M[i][j] = M[i][j] - M[i][k] * M[k][j];
            }
            M[i][k] = 0;
        }

    }
}

void generate(int i){
    if(i!= 0){
        for(int i=0;i<num[i-1];i++){
            delete matrix[i];
        }
        delete matrix;
    }

    matrix=new float*[n];
    for(int i=0;i<n;i++){
        matrix[i] =new float[n];
    }
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            matrix[i][j]= 0.8*i+1.3*j +1;
        }
    }
}

void print(){
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << matrix[i][j]<<" ";
        }
        cout << endl;
    }
}

void Delete(){
    for(int i=0;i<n;i++){
        delete matrix[i];
    }
    delete matrix;
}

int main()
{
    for(int i=0;i<7;i++){
        n=num[i];
        // generate(i);
        struct timeval time1, time2;
        gettimeofday(&time1, nullptr);
        for(int i=0;i<10;i++){
            generate(i);
            LU_neon(matrix);        
        }
        gettimeofday(&time2, nullptr);
        double elapsed_time = (time2.tv_sec - time1.tv_sec) * 1000. +
                    (time2.tv_usec - time1.tv_usec) / 1000.;
        cout<<"大小"<<"用时"<<elapsed_time/10<<"ms"<<endl;
    }
    cout<<"over\n";
    return 0;
}
