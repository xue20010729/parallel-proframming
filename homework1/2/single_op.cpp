#include<iostream>
#include<cstdlib>
#include<sys/time.h>
using namespace std;

int main(){

    int length[5]={2048,8192,32768,131072,262144};

    long long sum=0;


    for(int j=0;j<5;j++){
        int array[length[j]];
        struct timeval time1, time2;
        gettimeofday(&time1, nullptr);
        for(int k=0;k<1000;k++){
            for(int i=0;i<length[j];i++){
                array[i]=rand()%100+1;
            }
            for(int i=0;i<length[j];i++){
                sum+=array[i];
            }
            sum=0;
        }
        gettimeofday(&time2, nullptr);                                      
        double elapsed_time = (time2.tv_sec - time1.tv_sec) * 1000. +       
                (time2.tv_usec - time1.tv_usec) / 1000.;                
        cout<<"用时"<<elapsed_time/1000<<"ms"<<endl;
    }
    return 0;

}
