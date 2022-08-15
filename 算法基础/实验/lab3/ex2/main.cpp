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


vector<int> ldata;//存放区间左端点
vector<int> rdata;//存放区间右端点
vector<Node> tree_node;
vector<int> data;


void random_lgenerate(int n,int low, int high){//随机生成n个互异的正整数，范围在[low,high],并根据本实验要求对不合要求的值做了剔除
    ldata.clear();
    int temp;
    
    for(int i = 0; i < n;i++ ){
        temp=rand()%(high-low+1)+low;
        if(temp>=25 && temp<30){
            i--;
            continue;
        }
        auto iter = find(ldata.begin(), ldata.end(), temp);
        if (iter != ldata.end()){
            i--;
            continue;
        }
        ldata.push_back(temp);
    }
}

void random_rgenerate(int n,int low,int high){
    rdata.clear();
    int temp;
    
    for(int i = 0; i < n;i++ ){//右端点值大于左端点，且与(25,30)无重叠
        temp=rand()%(high-low+1)+low;
        if(temp > ldata[i] && temp<=25){
            rdata.push_back(temp);
        }
        else if(temp > ldata[i] && ldata[i]>=30){
            rdata.push_back(temp);
        }
        else{
            i--;
            continue;
        }
    }
}

int random_delete(int n,int low, int high,ofstream &Write_file){//随机生成范围在[low,high]的n个互异的下标存入data容器，并写入对应的结点关键字到文件，对应随机删除的结点
    data.clear();
    int temp;
    
    for(int i = 0; i < n;i++ ){
        temp=rand()%(high-low+1)+low;
        auto iter = find(data.begin(), data.end(), temp);
        if (iter != data.end()){
            i--;
            continue;
        }
        data.push_back(temp);
    }   

    for(int i=0;i<data.size();i++){
        Write_file <<tree_node[data[i]].interval[0]<<" "<<tree_node[data[i]].interval[1]<<endl;
    }
    return 0;
}


int main() {
    srand((unsigned)time(NULL));//随机数生成
    string path1="..\\..\\input\\input.txt";
    string path2="..\\..\\output\\inorder.txt";
    string path3="..\\..\\output\\delete_data.txt";
    string path4="..\\..\\output\\search.txt";

    int n=30;
/*
//随机生成30个插入的区间
    ofstream Write_file0(path1); //打开文件备写
    if(!Write_file0) {
        cout << "error opening destination file." << endl;
        return 1;
    }
    random_lgenerate(n,0,49);
    random_rgenerate(n,1,50);
    for(int i=0;i<n;i++){
        Write_file0<<ldata[i]<<" "<<rdata[i]<<endl;
    }
    Write_file0.close();
*/

    ifstream srcFile(path1,ios::in); //以文本模式打开in.txt备读
    if(!srcFile) { //打开失败
        cout << "error opening source file." << endl;
        return 1;
    }
    ofstream inorder_file(path2); //打开文件备写
    if(!inorder_file) {
        cout << "error opening destination file." << endl;
        return 1;
    }
    ofstream delete_data_file(path3); //打开文件备写
    if(!delete_data_file) {
        cout << "error opening destination file." << endl;
        return 1;
    }
    ofstream search_file(path4); //打开文件备写
    if(!search_file) {
        cout << "error opening destination file." << endl;
        return 1;
    }

    

    int ltemp,rtemp;

    tree_node.clear();
    //读入一组数据，对应要生成的红黑树的各个结点的关键字
    for(int j=0;j<n;j++){
        srcFile>>ltemp;
        srcFile>>rtemp;
        tree_node.push_back(Node{ltemp,ltemp,rtemp,rtemp,0,NULL,NULL,NULL});
    }
    Node NIL=Node{-1,0,0,0,0,NULL,NULL,NULL};//哨兵结点
    RB_Tree tree=RB_Tree(&NIL,&NIL);//红黑树

//插入操作
    for(int j=0;j<n;j++){//将n个结点依次插入
        tree.RB_insert(&tree_node[j]);
    }

//中序遍历
    tree.inorder_RB_walk(tree.root,inorder_file);
    //inorder_file<<endl;
    
//查找操作
    random_lgenerate(2,0,49);//生成查找的三个区间
    random_rgenerate(2,1,50);
    ltemp=rand()%(29-26+1)+26;
    rtemp=rand()%(29-26+1)+26;
    while(rtemp<ltemp){
        rtemp=rand()%(29-26+1)+26;
    }
    ldata.push_back(ltemp);
    rdata.push_back(rtemp);
    for(int i=0;i<3;i++){
        search_file<<ldata[i]<<" "<<rdata[i]<<endl;
    }
    search_file<<endl;
    int interval[2];
    Node* result;
    for(int i=0;i<3;i++){
        interval[0]=ldata[i];
        interval[1]=rdata[i];
        result=tree.interval_search(interval);
        if(result!=tree.nil){
            search_file<<result->interval[0]<<" "<<result->interval[1]<<endl;  
        }
        else{
            search_file<<"Null"<<endl;
        }
    }

//删除操作
    random_delete(3,0,n-1,delete_data_file);

    for(int j=0;j<data.size();j++){
        tree.RB_delete(&tree_node[data[j]]);
    }

//删除后的中序遍历

    //Write_file1<<n<<endl;
    tree.inorder_RB_walk(tree.root,delete_data_file);
    //delete_data_file<<endl;
        


    srcFile.close();
    inorder_file.close();
    delete_data_file.close();
    search_file.close();    

    return 0;
}
