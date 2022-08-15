#include <iostream>
#include <fstream>
#include <time.h>
#include <cstring>
#include <stdlib.h>
#include<windows.h>
#include <map> 
#include<vector>
#include <string>
#include <algorithm>
#include "tree.h"
using namespace std;

int N[5]={20,40,60,80,100};//整数n
vector<int> data;//存放随机生成的n个互异正整数
vector<Node> tree_node;//存放以n个正整数作为结点关键字的红黑树结点


int random_generate(int n,int low, int high,ofstream &Write_file){//随机生成n个互异的正整数，范围在[low,high],放入data容器并以追加的方式写入文件
    data.clear();
    int temp;
    
    srand((unsigned)time(NULL));//随机数生成
    for(int i = 0; i < n;i++ ){
        temp=rand()%(high-low+1)+low;
        auto iter = find(data.begin(), data.end(), temp);
        if (iter != data.end()){
            i--;
            continue;
        }
        data.push_back(temp);
    }   

    Write_file<<n<<endl;
    for(int i=0;i<data.size();i++){
        Write_file <<data[i]<<" ";
    }
    Write_file<<endl;
    return 0;
}

int random_delete(int n,int low, int high,ofstream &Write_file){//随机生成范围在[low,high]的n个互异的下标存入data容器，并写入对应的结点关键字到文件，对应随机删除的结点
    data.clear();
    int temp;
    
    srand((unsigned)time(NULL));//随机数生成
    for(int i = 0; i < n;i++ ){
        temp=rand()%(high-low+1)+low;
        auto iter = find(data.begin(), data.end(), temp);
        if (iter != data.end()){
            i--;
            continue;
        }
        data.push_back(temp);
    }   

    Write_file<<4*n<<endl;
    for(int i=0;i<data.size();i++){
        Write_file <<tree_node[data[i]].key<<" ";
    }
    Write_file<<endl;
    return 0;
}


int main() {
    string path1="..\\..\\input\\input.txt";
    string path2="..\\..\\output\\inorder.txt";
    string path3="..\\..\\output\\time1.txt";
    string path4="..\\..\\output\\delete_data.txt";
    string path5="..\\..\\output\\time2.txt";
/*
//随机生成n个插入的关键字结点
    ofstream Write_file0(path1,ios::app); //以追加模式打开文件备写
    if(!Write_file0) {
        cout << "error opening destination file." << endl;
        return 1;
    }

    for(int i=0;i<5;i++){
        if(random_generate(N[i],1,32767,Write_file0)){
            cout<<"generate input.txt wrong"<<endl;
            return 1;
        }
    }
    Write_file0.close();
*/
    ifstream srcFile(path1,ios::in); //以文本模式打开in.txt备读
    if(!srcFile) { //打开失败
        cout << "error opening source file." << endl;
        return 1;
    }
    ofstream inorder(path2); //打开文件备写
    if(!inorder) {
        cout << "error opening destination file." << endl;
        return 1;
    }
    ofstream time1(path3); //打开文件备写
    if(!time1) {
        cout << "error opening destination file." << endl;
        return 1;
    }
    ofstream delete_data(path4); //打开文件备写
    if(!delete_data) {
        cout << "error opening destination file." << endl;
        return 1;
    }
    ofstream time2(path5); //打开文件备写
    if(!time2) {
        cout << "error opening destination file." << endl;
        return 1;
    }
    
//clock计时参数的声明
    LARGE_INTEGER nFreq;
    LARGE_INTEGER t1;
    LARGE_INTEGER t2;
    double dt;
    double time=0;//记录算法运行时间，单位为us
    
    int n;
    int temp;
    for(int i=0;i<5;i++){//进行五次红黑树实验要求的算法
        tree_node.clear();
        srcFile>>n;//读入一组数据，对应要生成的红黑树的各个结点的关键字
        for(int j=0;j<n;j++){
            srcFile>>temp;
            tree_node.push_back(Node{temp,0,NULL,NULL,NULL});
        }
        Node NIL=Node{-1,0,NULL,NULL,NULL};//哨兵结点
        RB_Tree tree=RB_Tree(&NIL,&NIL);//红黑树

//插入操作
        QueryPerformanceFrequency(&nFreq);
        QueryPerformanceCounter(&t1);
        for(int j=0;j<n;j++){//将n个结点依次插入
            tree.RB_insert(&tree_node[j]);
        }
        QueryPerformanceCounter(&t2);
        dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
        time=dt*1000000;//单位为us

        time1<<time<<endl;//把时间输入文件

//中序遍历
        
        inorder<<n<<endl;
        tree.inorder_RB_walk(tree.root,inorder);
        inorder<<endl;
        


//删除操作
        random_delete(n/4,0,n-1,delete_data);//随机产生n/4个删除的结点

        QueryPerformanceFrequency(&nFreq);
        QueryPerformanceCounter(&t1);
        for(int j=0;j<data.size();j++){
            tree.RB_delete(&tree_node[data[j]]);
        }
        QueryPerformanceCounter(&t2);
        dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
        time=dt*1000000;//单位为us

        time2<<time<<endl;//把时间输入文件
//删除后的中序遍历

        //Write_file1<<n<<endl;
        tree.inorder_RB_walk(tree.root,delete_data);
        delete_data<<endl;
        

    }

    srcFile.close();
    inorder.close();
    time1.close();
    delete_data.close();
    time2.close();    
    return 0;
}
