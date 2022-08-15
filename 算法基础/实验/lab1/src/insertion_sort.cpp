#include "sort.h"
#include <chrono>
using namespace std;
using namespace chrono;

void insertion_sort(int A[],int length){//原址排序

    //auto start = system_clock::now();

    int i=0,j=0;
    int key;
    for(j=1;j<length;j++){
        key=A[j];
        i=j-1;
        while(i>=0 && A[i]>key){
            A[i+1]=A[i];
            i--;
        }
        A[i+1]=key;
    }
/*
    auto end   = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    time_count[time_cur++]=(double)(duration.count());
*/
};
