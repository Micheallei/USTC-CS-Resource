#include "sort.h"
#include <chrono>
using namespace std;
using namespace chrono;

int PARENT(int i){
    return (i-1)/2;
}

int LEFT(int i){
    return 2*i+1;
}

int RIGHT(int i){
    return 2*i+2;
}


void MAX_HEAPIFY(int A[],int i,int heap_size){
    int l=LEFT(i);
    int r=RIGHT(i);
    int largest;
    int temp;
    if(l<=heap_size && A[l]>A[i])
        largest=l;
    else largest=i;

    if(r<=heap_size && A[r]>A[largest])
        largest=r;

    if(largest!=i){
        temp=A[i];
        A[i]=A[largest];
        A[largest]=temp;
        MAX_HEAPIFY(A,largest,heap_size);
    }
}

//已知一个长度为length的数组A，将其建为最大堆
int BUILD_MAX_HEAP(int A[],int length){
    int heap_size=length-1;//堆中有效元素的的最大序号
    for(int i=(heap_size-1)/2;i>=0;i--) {//i从第一个内部节点开始
        MAX_HEAPIFY(A, i, heap_size);
    }
    return heap_size;
}


void heap_sort(int A[],int length){
    //auto start = system_clock::now();

    int heap_size=BUILD_MAX_HEAP(A,length);
    int temp;
    for(int i=length-1;i>=1;i--){//最大值总在A[0]
        temp=A[i];
        A[i]=A[0];
        A[0]=temp;
        heap_size--;
        MAX_HEAPIFY(A,0,heap_size);
    }
/*
    auto end   = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    time_count[time_cur++]=(double)(duration.count());
    */
    //cout<<"quick_sort time:"<<time_count[time_cur-1]<<endl;
    //cout<<"times:"<<time_cur<<endl;
}


