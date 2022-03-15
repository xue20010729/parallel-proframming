#include<iostream>
#include<cstdlib>
#include<sys/time.h>
using namespace std;

int main(){
    int length=131072;
    long sum1=0,sum2=0,sum=0;
    int array[length];

    struct timeval time1, time2;                                        
    gettimeofday(&time1, nullptr);

    for(int j=0;j<100;j++){
        
        for(int i=0;i<length;i++){
           array[i]=rand()%100+1;
        }
        for(int i=0;i<length;i+=2){
            sum1+=array[i];
            sum2+=array[i+1];
        }
        sum=sum1+sum2;
        sum1=0;
        sum2=0;
    }
    gettimeofday(&time2, nullptr);                                      
    double elapsed_time = (time2.tv_sec - time1.tv_sec) * 1000. +       
                (time2.tv_usec - time1.tv_usec) / 1000.;                
    cout<<"用时"<<elapsed_time<<"ms"<<endl;
    return 0;
}
