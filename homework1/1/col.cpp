#include<iostream>
#include<cstdlib>
#include<sys/time.h>
using namespace std;
int main(){
    const int length=384;
    double matrix[length][length];
    for(int i=0;i<length;i++){
        for(int j=0;j<length;j++){
            matrix[i][j]= i*length+j;
        }
    }

    double v[length];
    for(int i=0;i<length;i++){
        v[i]=i;
    }
////////////////////////////////////////////////
    double sum[length];
    struct timeval time1, time2;                                        
    gettimeofday(&time1, nullptr);
    for(int k=0;k<50;k++){
        for(int i=0;i<length;i++){       //这是列
            sum[i]=0;
            for(int j=0;j<length;j++){
                sum[i]+=matrix[j][i] * v[j];
            }
        }
    }
    
    gettimeofday(&time2, nullptr);                                      
    double elapsed_time = (time2.tv_sec - time1.tv_sec) * 1000. +       
                (time2.tv_usec - time1.tv_usec) / 1000.;                
    cout<<"用时"<<elapsed_time<<"ms"<<endl;
    return 0;
}
