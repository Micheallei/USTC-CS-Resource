#include<iostream>
#include<vector>
#include<algorithm>
#include <fstream>
#include <time.h>
#include <cstring>
#include <stdlib.h>
#include<windows.h>
#include <map>
#include <string>
using namespace std;


struct Node{
    Node* p;
    int rank;
    int serial;//序号
};

struct Edge {
    int w;//权重
    int u;//左端点
    int v;//右端点
};

vector<Node> node_set(512);//存放分离数据集合的结点
int edge_count[512][513];//用于判断该边是否存在及已经有的边数,纵列多出来的一列用于存放边数
vector<Edge> Edge_heap;//存放生成的所有边
vector<Edge> result;//存放Kruskal算法结果

int N[4]={8,64,128,512};

void generate(int N){//生成边及其权重
    memset(edge_count, 0, sizeof(edge_count));
    node_set.clear();
    Edge_heap.clear();
    //memset(node_set,0,sizeof(node_set));
    int num=0;//为该顶点随机生成的边数
    for(int i=0;i<N;i++){
        num=1+rand()%(int(N/2));
        int temp=(num <= int(N/2)-edge_count[i][N]) ? num:int(N/2)-edge_count[i][N];
        for(int j=0; j<temp ;j++){//依次生成每一条边的权重，同时判断该边所选择的另一个节点是否冲突
            int v=rand()%N;//该边的另一条顶点
            if(edge_count[i][v]){
                j--;
                continue;
            }
            else if(edge_count[v][N]>=int(N/2)) continue;
            else{
                int length=1+rand()%20;//随机生成边的权重
                Edge_heap.push_back(Edge{length,i,v});
                edge_count[i][N]++;
                edge_count[v][N]++;
                edge_count[i][v]=1;
                edge_count[v][i]=1;
            }
        }
    }
}

//分离数据集合的操作
void Make_Set(Node *x){
    x->p=x;
    x->rank=0;
}

Node* Find_set(Node* x){
    if(x->p!=x){
        x->p = Find_set(x->p);
    }
    return x->p;
}

void Link(Node* x,Node* y){
    if(x->rank > y->rank){
        y->p=x;
    }
    else{
        x->p=y;
        if(x->rank == y->rank){
            y->rank ++;
        }
    }
}

void Union(Node* x,Node* y){
    Link(Find_set(x),Find_set(y));
}

bool cmp(Edge& a, Edge& b)
{
    return a.w > b.w;
}

void Mst_Kruskal(int N){
    result.clear();
    for(int i=0;i<N;i++){
        node_set[i].serial=i;
        Make_Set(&node_set[i]);
    }
    make_heap(Edge_heap.begin(),Edge_heap.end(),cmp);//生成最小优先队列
    while(Edge_heap.size()>0){
        pop_heap(Edge_heap.begin(),Edge_heap.end(),cmp);
        Edge l = Edge_heap.back();
        Edge_heap.pop_back();
        //cout<<l.w<<endl;
        if(Find_set(&node_set[l.u]) != Find_set(&node_set[l.v])){
            result.push_back(l);
            Union(&node_set[l.u],&node_set[l.v]);
        }
    }
}


int main(){
    
    srand((unsigned)time(NULL));//随机数生成
    string input_path[4]={"..\\..\\input\\input1.txt","..\\..\\input\\input2.txt","..\\..\\input\\input3.txt","..\\..\\input\\input4.txt"};
    string output_path[4]={"..\\..\\output\\result1.txt","..\\..\\output\\result2.txt","..\\..\\output\\result3.txt","..\\..\\output\\result4.txt"};
    string time_path="..\\..\\output\\time.txt";
    //string path3="..\\..\\input\\input3.txt";
    //string path4="..\\..\\input\\input4.txt";

    //clock计时参数的声明
    LARGE_INTEGER nFreq;
    LARGE_INTEGER t1;
    LARGE_INTEGER t2;
    double dt;
    double time;//记录算法运行时间，单位为us
    ofstream time_file(time_path); //打开文件备写
    if(!time_file) { //打开失败
        cout << "error opening file." << endl;
        return 1;
    }

    for(int i=0;i<4;i++){
        generate(N[i]);//随机生成数据
/*
        Edge_heap.push_back(Edge{4,0,1});
        Edge_heap.push_back(Edge{8,0,7});
        Edge_heap.push_back(Edge{8,1,2});
        Edge_heap.push_back(Edge{11,1,7});
        Edge_heap.push_back(Edge{7,2,3});
        Edge_heap.push_back(Edge{4,2,5});
        Edge_heap.push_back(Edge{2,2,8});
        Edge_heap.push_back(Edge{9,3,4});
        Edge_heap.push_back(Edge{14,3,5});
        Edge_heap.push_back(Edge{10,4,5});
        Edge_heap.push_back(Edge{2,5,6});
        Edge_heap.push_back(Edge{1,6,7});
        Edge_heap.push_back(Edge{6,6,8});
        Edge_heap.push_back(Edge{7,7,8});
*/       


        ofstream des_File(input_path[i]); //打开文件备写
        if(!des_File) { //打开失败
            cout << "error opening file." << endl;
            return 1;
        }
        for(auto iter=Edge_heap.begin();iter!=Edge_heap.end();iter++){
            des_File<<(*iter).u << " "<<(*iter).v << " "<<(*iter).w<<"\n";
        }
        des_File.close();


        QueryPerformanceFrequency(&nFreq);
        QueryPerformanceCounter(&t1);

        Mst_Kruskal(N[i]);//算法运行

        QueryPerformanceCounter(&t2);
        dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
        time=dt*1000000;

        time_file<<time<<endl;//把时间输入文件
        

        ofstream result_File(output_path[i]); //打开文件备写
        if(!result_File) { //打开失败
            cout << "error opening file." << endl;
            return 1;
        }
        int price=0;
        for(auto iter=result.begin();iter!=result.end();iter++){
            price+=(*iter).w;
        }
        result_File<<"总代价: "<<price<<endl;
        for(auto iter=result.begin();iter!=result.end();iter++){
            result_File<<(*iter).u << " "<<(*iter).v << " "<<(*iter).w<<"\n";
        }
        result_File.close();

    }
    time_file.close();
    return 0;
}
