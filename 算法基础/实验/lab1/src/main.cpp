#include <iostream>
#include <fstream>
#include "sort.h"
#include <cstring>
#include<windows.h>
//#include <chrono>
using namespace std;
//using namespace chrono;

#define num_3 8
#define num_6 64
#define num_9 512
#define num_12 4096
#define num_15 32768
#define num_18 262144


int data3 [num_3];
int data6 [num_6];
int data9 [num_9];
int data12 [num_12];
int data15 [num_15];
int data18 [num_18];

int data3_sorted [num_3];
int data6_sorted [num_6];
int data9_sorted [num_9];
int data12_sorted [num_12];
int data15_sorted [num_15];
int data18_sorted [num_18];

double time_count[6] = {0};
int time_cur = 0;


void array_clean(){
//每个排序算法可能需要原址排序，所以需复制一个新的数组;并且每个算法结束后，需重新清空该数组得到一个新的待排序数组
    memcpy(data3_sorted, data3, sizeof(int) * num_3);
    memcpy(data6_sorted, data6, sizeof(int) * num_6);
    memcpy(data9_sorted, data9, sizeof(int) * num_9);
    memcpy(data12_sorted, data12, sizeof(int) * num_12);
    memcpy(data15_sorted, data15, sizeof(int) * num_15);
    memcpy(data18_sorted, data18, sizeof(int) * num_18);
    time_cur =0;
}


void file_print(string pathname){//每次调用完一个算法后，讲排序数据输出到对应文件
    int i=0;
    string path_3="..\\..\\output\\"+pathname+"\\result_3.txt";
    string path_6="..\\..\\output\\"+pathname+"\\result_6.txt";
    string path_9="..\\..\\output\\"+pathname+"\\result_9.txt";
    string path_12="..\\..\\output\\"+pathname+"\\result_12.txt";
    string path_15="..\\..\\output\\"+pathname+"\\result_15.txt";
    string path_18="..\\..\\output\\"+pathname+"\\result_18.txt";
    string time_path="..\\..\\output\\"+pathname+"\\time.txt";
    //string path_3="result.txt";
    ofstream Write_file_3(path_3);
    ofstream Write_file_6(path_6);
    ofstream Write_file_9(path_9);
    ofstream Write_file_12(path_12);
    ofstream Write_file_15(path_15);
    ofstream Write_file_18(path_18);
    ofstream Write_time(time_path);

    for(i=0;i<num_3;i++){
        Write_file_3<<data3_sorted[i]<<endl;
    }
    for(i=0;i<num_6;i++){
        Write_file_6<<data6_sorted[i]<<endl;
    }
    for(i=0;i<num_9;i++){
        Write_file_9<<data9_sorted[i]<<endl;
    }
    for(i=0;i<num_12;i++){
        Write_file_12<<data12_sorted[i]<<endl;
    }
    for(i=0;i<num_15;i++){
        Write_file_15<<data15_sorted[i]<<endl;
    }
    for(i=0;i<num_18;i++){
        Write_file_18<<data18_sorted[i]<<endl;
    }

    Write_time<<"单位:us"<<endl;
    for(i=0;i<6;i++){
        Write_time<<time_count[i]<<endl;
    }
    Write_file_3.close();
    Write_file_6.close();
    Write_file_9.close();
    Write_file_12.close();
    Write_file_15.close();
    Write_file_18.close();
    Write_time.close();
}

int main() {
/*
    if(!random_generate()){
        cout<<"generate input.txt wrong"<<endl;
        return 0;
    }
*/

//读取input.txt并将其放入数组中
    ifstream srcFile("..\\..\\input\\input.txt",ios::in); //以文本模式打开in.txt备读
    if(!srcFile) { //打开失败
        cout << "error opening source file." << endl;
        return 0;
    }
    int i=0;
    for(i=0;i<num_18;i++){
        srcFile>>data18[i];
    }
    srcFile.close();
    for(i=0;i<num_15;i++){
        data15[i]=data18[i];
    }
    for(i=0;i<num_12;i++){
        data12[i]=data18[i];
    }
    for(i=0;i<num_9;i++){
        data9[i]=data18[i];
    }
    for(i=0;i<num_6;i++){
        data6[i]=data18[i];
    }
    for(i=0;i<num_3;i++){
        data3[i]=data18[i];
    }

    //clock计时参数的声明
    LARGE_INTEGER nFreq;
    LARGE_INTEGER t1;
    LARGE_INTEGER t2;
    double dt;
//原址排序的算法

    array_clean();
//快速排序
    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    quick_sort(data3_sorted,0,num_3-1);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    quick_sort(data6_sorted,0,num_6-1);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    quick_sort(data9_sorted,0,num_9-1);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    quick_sort(data12_sorted,0,num_12-1);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    quick_sort(data15_sorted,0,num_15-1);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    quick_sort(data18_sorted,0,num_18-1);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    file_print("quick_sort");






    array_clean();
//插入排序
    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    insertion_sort(data3_sorted,num_3);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;
    //dt结果乘以1000,将显示时间调整到ms级别显示，乘以1000000，将显示时间调整到us级别显示

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    insertion_sort(data6_sorted,num_6);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    insertion_sort(data9_sorted,num_9);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    insertion_sort(data12_sorted,num_12);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    insertion_sort(data15_sorted,num_15);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    insertion_sort(data18_sorted,num_18);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    file_print("insertion_sort");



    array_clean();
//堆排序
    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    heap_sort(data3_sorted,num_3);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    heap_sort(data6_sorted,num_6);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    heap_sort(data9_sorted,num_9);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    heap_sort(data12_sorted,num_12);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    heap_sort(data15_sorted,num_15);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    heap_sort(data18_sorted,num_18);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    file_print("heap_sort");

//非原址排序的算法
    array_clean();
//归并排序
    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    merge_sort(data3_sorted,0,num_3-1);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    merge_sort(data6_sorted,0,num_6-1);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    merge_sort(data9_sorted,0,num_9-1);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    merge_sort(data12_sorted,0,num_12-1);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    merge_sort(data15_sorted,0,num_15-1);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    merge_sort(data18_sorted,0,num_18-1);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    file_print("merge_sort");


    array_clean();
//计数排序
    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    counting_sort(data3,data3_sorted,num_3);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    counting_sort(data6,data6_sorted,num_6);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    counting_sort(data9,data9_sorted,num_9);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    counting_sort(data12,data12_sorted,num_12);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    counting_sort(data15,data15_sorted,num_15);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    counting_sort(data18,data18_sorted,num_18);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count[time_cur++]=dt*1000000;

    file_print("counting_sort");

    return 0;
}
