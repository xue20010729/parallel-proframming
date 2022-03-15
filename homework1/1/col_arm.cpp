#include<iostream>
#include<cstdlib>
#include<sys/time.h>
using namespace std;

int main(){
    int length[6]={16,32,64,128,256,384};

    for(int k=0;k<6;k++){
        double matrix[length[k]][length[k]];
        for(int i=0;i<length[k];i++){
            for(int j=0;j<length[k];j++){
                matrix[i][j]= i*length[k]+j;
            }
        }
        double v[length[k]];
        for(int i=0;i<length[k];i++){
            v[i]=i;
        }
        double sum[length[k]];
        struct timeval time1, time2;                                        
        gettimeofday(&time1, nullptr);
        for(int m=0;m<50;m++){
            for(int i=0;i<length[k];i++){       //这是列
                sum[i]=0;
                for(int j=0;j<length[k];j++){
                    sum[i]+=matrix[j][i] * v[j];
                }
            }
        }
        
        gettimeofday(&time2, nullptr);                                      
        double elapsed_time = (time2.tv_sec - time1.tv_sec) * 1000. +       
                    (time2.tv_usec - time1.tv_usec) / 1000.;                
        cout<<"用时"<<elapsed_time/50<<"ms"<<endl;
    }

    
    return 0;
}
