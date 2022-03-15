#include<iostream>
#include<cstdlib>
#include<sys/time.h>
using namespace std;

int main(){
    int length=131072;
    long array[length];
    struct timeval time1, time2;                                        
    gettimeofday(&time1, nullptr);

    for(int i=0;i<100;i++){
        for(int i=0;i<length;i++){
            array[i]=rand()%100+1;
        }
        for(int m=length;m>1;m/=2){
            for(int i=0;i<m/2;i++){
                array[i]=array[2*i]+array[2*i+1];
            }
        }
    }
    
    gettimeofday(&time2, nullptr);                                      
    double elapsed_time = (time2.tv_sec - time1.tv_sec) * 1000. +       
                (time2.tv_usec - time1.tv_usec) / 1000.;                
    cout<<"用时"<<elapsed_time<<"ms"<<endl;
    return 0;
}