#include "sort.h"
#include <chrono>
using namespace std;
using namespace chrono;

void merge(int A[],int p,int q,int r){//非原址排序
    int n1=q-p+1;
    int n2=r-q;
    int *L = new int[n1+1];
    int *R = new int[n2+1];
    int i;
    for(i=0;i<n1;i++){//注意数组L和R下标从0开始
        L[i]=A[p+i];
    }
    for(i=0;i<n2;i++){
        R[i]=A[q+i+1];
    }
    L[n1]=40000;
    R[n2]=40000;
    i=0;
    int j=0;
    for(int k=p;k<r+1;k++){
        if(L[i]<=R[j]){
            A[k]=L[i];
            i++;
        }
        else{
            A[k]=R[j];
            j++;
        }
    }
    delete[] L;
    delete[] R;
}

void merge_sort(int A[],int p,int r){//真正的merge sort算法
    if(p<r){
        int q=(p+r)/2;
        merge_sort(A,p,q);
        merge_sort(A,q+1,r);
        merge(A,p,q,r);
    }
}
/*
void merge_sort(int A[],int p,int r){//为了计时而包装了一层函数
    auto start = system_clock::now();

    MERGE_SORT(A,p,r);

    auto end   = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    time_count[time_cur++]=(double)(duration.count());
}
*/
