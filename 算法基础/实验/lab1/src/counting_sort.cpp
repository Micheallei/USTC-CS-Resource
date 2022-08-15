#include "sort.h"
#include <chrono>
using namespace std;
using namespace chrono;

void counting_sort(int A[],int B[],int length){//B中存放排序输出,length为A数组长度

    //auto start = system_clock::now();

    int k=32767;//最大取值
    int *C = new int[k+1];
    int i,j;
    for(i=0;i<=k;i++){
        C[i]=0;
    }
    for(j=0;j<length;j++){
        C[A[j]]=C[A[j]]+1;
    }
    for(i=1;i<=k;i++){
        C[i]=C[i]+C[i-1];
    }
    for(j=length-1;j>=0;j--){
        B[C[A[j]]-1]=A[j];//B数组下标从0开始
        C[A[j]]=C[A[j]]-1;
    }
    delete[] C;
/*
    auto end   = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    time_count[time_cur++]=(double)(duration.count());
*/
}

