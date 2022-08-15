#include<iostream>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include "sort.h"
using namespace std;

int random_generate(){
    ofstream Write_file("..\\..\\input\\input.txt",ios::out); //以文本模式打开out.txt备写
    if(!Write_file) {
        cout << "error opening destination file." << endl;
        return 0;
    }

    int n=262144;
    srand((unsigned)time(NULL));//随机数生成
    for(int i = 0; i < n;i++ ){
        Write_file << rand() << endl;
    }
    Write_file.close();
    return 1;
}
