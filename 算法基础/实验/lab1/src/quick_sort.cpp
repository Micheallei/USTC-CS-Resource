#include "sort.h"
#include <chrono>
using namespace std;
using namespace chrono;

int PARTITION(int A[],int p,int r){
    int x=A[r];//主元
    int i=p-1;
    int temp;
    for(int j=p;j<r;j++){
        if(A[j]<=x){
            i++;
            temp=A[i];
            A[i]=A[j];
            A[j]=temp;
        }
    }
    temp=A[i+1];
    A[i+1]=A[r];
    A[r]=temp;
    return i+1;
}


void quick_sort(int A[],int p,int r){
    if(p<r) {
        int q = PARTITION(A, p, r);
        quick_sort(A, p, q - 1);
        quick_sort(A, q + 1, r);
    }
}

/*
void quick_sort(int A[],int p,int r){

    auto start = system_clock::now();

    QUICK_SORT(A,p,r);

    auto end   = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    time_count[time_cur++]=(double)(duration.count());
    //cout<<"quick_sort time:"<<time_count[time_cur-1]<<endl;
    //cout<<"times:"<<time_cur<<endl;
}
*/



