#include <iostream>
#include <emmintrin.h>
#include <pmmintrin.h> //SSE3
#include<cstdlib>
#include<sys/time.h>
#include <immintrin.h>
using namespace std;


float **matrix;
int n;

// void LU_SSE(float **M){
// 	for(int k=0;k<n;k++){
// 		// float val[4] ={M[k][k],M[k][k],M[k][k],M[k][k]};
// 		// __m128 divsor =_mm_loadu_ps(val);
//         __m128 divsor =_mm_set1_ps(M[k][k]);
// 		int j;
// 		for (j = k + 1; j +4 < n; j+=4) {
// 			__m128 v =_mm_loadu_ps(M[k]+j);
// 			v=_mm_div_ps(v,divsor);
// 			_mm_storeu_ps(M[k]+j,v);
// 		}
// 		for(;j<n;j++){
// 			M[k][j] = M[k][j] / M[k][k];
// 		}
// 		M[k][k]=1;
// 		for (int i = k + 1; i < n; i++) {
//             // float val[4] = {M[i][k],M[i][k],M[i][k],M[i][k]};
//             __m128 vaik=_mm_set1_ps(M[i][k]);  
//             int j;  
//             for (j = k + 1; j +4< n; j+=4) {
//                 __m128 vakj=_mm_loadu_ps(M[k]+j);
//                 __m128 vaij=_mm_loadu_ps(M[i]+j);
//                 __m128 vx=_mm_mul_ps(vakj,vaik);
//                 vaij =vaij -vx;
//                 // M[i][j] = M[i][j] - M[i][k] * M[k][j];
//                 _mm_storeu_ps(M[i]+j, vaij);
//             }
//             for(;j<n;j++){
//                 M[i][j] = M[i][j] - M[i][k] * M[k][j];
//             }
//             M[i][k] = 0;
// 		}
// 	}

// }

void LU_AVX(float **M){
	for(int k=0;k<n;k++){
		// float val[4] ={M[k][k],M[k][k],M[k][k],M[k][k]};
		// __m128 divsor =_mm_loadu_ps(val);
        __m256 divsor =_mm256_set1_ps(M[k][k]);
		int j;
		for (j = k + 1; j +8 < n; j+=8) {
			__m256 v =_mm256_loadu_ps(M[k]+j);
			v=_mm256_div_ps(v,divsor);
			_mm256_storeu_ps(M[k]+j,v);
		}
		for(;j<n;j++){
			M[k][j] = M[k][j] / M[k][k];
		}
		M[k][k]=1;
		for (int i = k + 1; i < n; i++) {
            // float val[4] = {M[i][k],M[i][k],M[i][k],M[i][k]};
            __m256 vaik=_mm256_set1_ps(M[i][k]);  
            int j;  
            for (j = k + 1; j +8< n; j+=8) {
                __m256 vakj=_mm256_loadu_ps(M[k]+j);
                __m256 vaij=_mm256_loadu_ps(M[i]+j);
                __m256 vx=_mm256_mul_ps(vakj,vaik);
                vaij =vaij -vx;
                // M[i][j] = M[i][j] - M[i][k] * M[k][j];
                _mm256_storeu_ps(M[i]+j, vaij);
            }
            for(;j<n;j++){
                M[i][j] = M[i][j] - M[i][k] * M[k][j];
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

// void generate(int i){
//     if(i!= 0){
//         for(int i=0;i<num[i-1];i++){
//             delete matrix[i];
//         }
//         delete matrix;
//     }

//     matrix=new float*[n];
//     for(int i=0;i<n;i++){
//         matrix[i] =new float[n];
//     }
//     for(int i=0;i<n;i++){
//         for(int j=0;j<n;j++){
//             matrix[i][j]= 0.8*i+1.3*j +1;
//         }
//     }
// }

void generate(){
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
    // for(int i=0;i<7;i++){
    //     n=num[i];
    //     generate(i);
    //     struct timeval time1, time2;
    //     gettimeofday(&time1, nullptr);
    //     for(int j=0;j<10;j++){
    //         // generate(i);
    //         reset();
    //         LU_SSE(matrix);        
    //     }
    //     gettimeofday(&time2, nullptr);
    //     double elapsed_time = (time2.tv_sec - time1.tv_sec) * 1000. +
    //                 (time2.tv_usec - time1.tv_usec) / 1000.;
    //     cout<<"大小"<<"用时"<<elapsed_time/10<<"ms"<<endl;
    // }
    n=512;
    generate();
    LU_AVX(matrix);
    cout<<"over\n";
    // print();
    Delete();
    return 0;
}
