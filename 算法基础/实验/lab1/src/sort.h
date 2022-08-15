#ifndef SRC_SORT_H
#define SRC_SORT_H

#include<iostream>
using namespace std;

extern double time_count[6];
extern int time_cur;

int random_generate();
void insertion_sort(int A[], int length);
void heap_sort(int A[], int length);
void merge_sort(int A[],  int p, int r);
void counting_sort(int A[], int B[], int length);
void quick_sort(int A[], int p, int r);


#endif //SRC_SORT_H
