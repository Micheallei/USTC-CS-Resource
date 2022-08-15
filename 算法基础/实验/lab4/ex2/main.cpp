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
#include<cmath>
#include <queue>
using namespace std;

#define INT_MAX 50000//每条路径最多长 729*50 <50000

struct Edge {
    int w;//权重
    int u;//左端点编号
    int v;//右端点编号
    Edge* next;
};

struct Node{
    Edge* head;
    //int serial;//结点编号
    int d;
    int pi;//前驱节点编号
    int h;//存储对应结点的h函数值
};

class cmp{
public:
    bool operator() (Node *node1, Node *node2){
        return node1->d > node2->d;
    }
};

vector<Node> node_set(730);//存放邻接链表的结点集合
int edge_count[729][729];//用于判断该边是否存在
vector<Edge> Edge_set;//存放生成的所有边
//int D[729][729];//存放最终的最短路径值，初始化为-1
//vector<Edge> result;//存放Kruskal算法结果

int N[4]={27,81,243,729};
int m[2]={5,7};


void test(){
    node_set[0]=Node{NULL,0,0,0};
    node_set[1]=Node{NULL,0,0,0};
    node_set[2]=Node{NULL,0,0,0};
    node_set[3]=Node{NULL,0,0,0};
    node_set[4]=Node{NULL,0,0,0};

    Edge_set.push_back(Edge{3,0,1,NULL});
    Edge_set.push_back({8,0,2,NULL});
    Edge_set.push_back(Edge{-4,0,4,NULL});

    Edge_set.push_back({1,1,3,NULL});
    Edge_set.push_back(Edge{7,1,4,NULL});

    Edge_set.push_back({4,2,1,NULL});

    Edge_set.push_back(Edge{2,3,0,NULL});
    Edge_set.push_back(Edge{-5,3,2,NULL});

    Edge_set.push_back(Edge{6,4,3,NULL});

    Edge* temp=node_set[0].head=NULL;
    for(int j=2;j>=0;j--){
        node_set[0].head=&Edge_set[j];
        //cout<<node_set[i].head->v<<endl;
        node_set[0].head->next=temp;
        temp=node_set[0].head;
    }
    temp=node_set[1].head=NULL;
    for(int j=1;j>=0;j--){
        node_set[1].head=&Edge_set[3+j];
        //cout<<node_set[i].head->v<<endl;
        node_set[1].head->next=temp;
        temp=node_set[1].head;
    }

    temp=node_set[3].head=NULL;
    for(int j=1;j>=0;j--){
        node_set[3].head=&Edge_set[6+j];
        //cout<<node_set[i].head->v<<endl;
        node_set[3].head->next=temp;
        temp=node_set[3].head;
    }

    node_set[2].head=&Edge_set[5];
    node_set[4].head=&Edge_set[8];
}


void generate(int N , int m){//m分别为5或7
    memset(edge_count, 0, sizeof(edge_count));
    node_set.clear();
    Edge_set.clear();
    int num= int(log(N)/log(m));//为每个顶点随机生成的边数
    for(int i=0;i<N;i++){
        for(int j=0; j<num ;j++){//依次生成每一条边的权重，同时判断该边所选择的另一个节点是否冲突
            int v=rand()%N;//该边的另一个顶点
            if(edge_count[i][v]){
                j--;
                continue;
            }
            else{
                int length=rand()%51;//随机生成边的权重[0,50]
                Edge_set.push_back(Edge{length,i,v,NULL});
                edge_count[i][v]=1;
            }
        }
        //Edge_set.clear();
    }
    for(int i=0;i<N;i++){//使用尾插法构建邻接链表
        //node_set[i].serial=i;
        Edge* temp=node_set[i].head=NULL;
        for(int j=0;j<num;j++){
            node_set[i].head=&Edge_set[i*num+j];
            //cout<<node_set[i].head->v<<endl;
            node_set[i].head->next=temp;
            temp=node_set[i].head;
        }
    }
}

void Initialize_single_source(int s,int N){//s为源结点编号，N为结点总数, 复杂度为O(V)
    for(int i=0;i<N;i++){
        node_set[i].d=INT_MAX;
        node_set[i].pi=-1;
    }
    node_set[s].d=0;
}

void Relax(int u,int v,int w){//O(1)
    if(node_set[v].d > node_set[u].d + w){
        node_set[v].d = node_set[u].d +w;
        //cout<<"u:"<<u<<" v:"<<v<<" d:"<<node_set[v].d<<endl;
        node_set[v].pi=u;
    }
}

bool Bellman_Ford(int s,int N){//s为源结点的编号,N为结点总数,算完之后，每个结点的d和pi为最短路径相关的值,O(VE)
    Initialize_single_source(s,N);
    for(int i=0;i<N-1;i++){
        for(auto iter=Edge_set.begin();iter!=Edge_set.end();iter++){
            Relax((*iter).u, (*iter).v, (*iter).w);
        }
    }
    for(auto iter=Edge_set.begin();iter!=Edge_set.end();iter++){
        int u=(*iter).u;
        int v=(*iter).v;
        if(node_set[v].d > node_set[u].d + (*iter).w){
            return false;
        }
    }
    return true;
}




void Dijkstra(int s,int N){//O(ElgV)
    Initialize_single_source(s,N);
    priority_queue<int,vector<Node*>,cmp> tmpNode_set;//最小优先队列
    for (int i = 0; i < N; i++)  
        tmpNode_set.push(&node_set[i]); 
    while(!tmpNode_set.empty()){
        Node* u= tmpNode_set.top();//O(1)
        tmpNode_set.pop();//O(1)
        Edge* temp=u->head;
        while(temp!=NULL){
            Relax(temp->u,temp->v,temp->w);//O(1)
            temp=temp->next;
        }
    }
}

bool print_path(int u,int v,ofstream &f){//输出从u到v的最短路径上的所有结点
    if(u!=v ){
        if(node_set[v].pi==-1) return false;
        else{
            print_path(u,node_set[v].pi,f);
            f<<v<<",";
        }
    } 
    else {
        f<<"( "<<v<<",";
    }
    return true;
}

void Johnson(int N,ofstream &f){//O(VElgV)
    Edge* temp=node_set[N].head=NULL;

    //node_set[N].serial=N;
    for(int i=N-1;i>=0;i--){//初始化计算G'
        Edge_set.push_back(Edge{0,N,i,NULL});
        node_set[N].head=&Edge_set.back();
        node_set[N].head->next=temp;
        temp=node_set[N].head;
    }
    
    if(Bellman_Ford(N,N+1)==false){
        cout<<"the input graph contains a negative-weight cycle"<<endl;
    }
    else{
        for(int i=0;i<N+1;i++){
            node_set[i].h=node_set[i].d;//初始化每个结点的h函数
        }
        for(auto iter=Edge_set.begin();iter!=Edge_set.end();iter++){//重赋权重
            int u=(*iter).u;
            int v=(*iter).v;
            (*iter).w=(*iter).w + node_set[u].h - node_set[v].h;
        }
        for(int i=0;i<N;i++){
            Dijkstra(i,N+1);
            for(int j=0;j<N;j++){
                if(i!=j){
                    if(print_path(i,j,f)){
                        f<<"  "<<node_set[j].d + node_set[j].h -node_set[i].h<<" )"<<"\n";
                        //cout<<"  "<<node_set[j].d + node_set[j].h -node_set[i].h<<" )"<<"\n";
                    }
                    else{//两节点间不存在最短路径
                        f<<"no pass for node"<<i<<","<<j<<"\n";
                        //cout<<"no pass for node"<<i<<","<<j<<"\n";
                    }
                }
            }
        }
    }
}


int main(){
    Edge_set.reserve(3700);
    //tmpNode_set.reserve(730);

    srand((unsigned)time(NULL));//随机数生成

    string input_path1="..\\..\\input\\input11.txt";
    string input_path2="..\\..\\input\\input21.txt";
    string input_path3="..\\..\\input\\input31.txt";
    string input_path4="..\\..\\input\\input41.txt";
    string input_path5="..\\..\\input\\input12.txt";
    string input_path6="..\\..\\input\\input22.txt";
    string input_path7="..\\..\\input\\input32.txt";
    string input_path8="..\\..\\input\\input42.txt";

    string output_path1="..\\..\\output\\result11.txt";
    string output_path2="..\\..\\output\\result21.txt";
    string output_path3="..\\..\\output\\result31.txt";
    string output_path4="..\\..\\output\\result41.txt";
    string output_path5="..\\..\\output\\result12.txt";
    string output_path6="..\\..\\output\\result22.txt";
    string output_path7="..\\..\\output\\result32.txt";
    string output_path8="..\\..\\output\\result42.txt";
    string time_path="..\\..\\output\\time.txt";

    ofstream inputfile1(input_path1);
    ofstream inputfile2(input_path2);
    ofstream inputfile3(input_path3);
    ofstream inputfile4(input_path4);
    ofstream inputfile5(input_path5);
    ofstream inputfile6(input_path6);
    ofstream inputfile7(input_path7);
    ofstream inputfile8(input_path8);

    ofstream outputfile1(output_path1);
    ofstream outputfile2(output_path2);
    ofstream outputfile3(output_path3);
    ofstream outputfile4(output_path4);
    ofstream outputfile5(output_path5);
    ofstream outputfile6(output_path6);
    ofstream outputfile7(output_path7);
    ofstream outputfile8(output_path8);

    ofstream timefile(time_path);

    //clock计时参数的声明
    LARGE_INTEGER nFreq;
    LARGE_INTEGER t1;
    LARGE_INTEGER t2;
    double dt;
    double time_count;//记录算法运行时间，单位为us

    Edge* temp;

    generate(N[0],m[0]);
    for(int k=0;k<N[0];k++){//把每个结点遍历，输出以其为起点的边
        temp=node_set[k].head;
        while(temp!=NULL){
            inputfile1<<temp->u << " "<<temp->v << " "<<temp->w<<"\n";
            temp=temp->next;
        }
    }
    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    Johnson(N[0],outputfile1);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count=dt*1000000;
    timefile << time_count << endl;//把时间输入文件


    generate(N[1],m[0]);
    for(int k=0;k<N[1];k++){//把每个结点遍历，输出以其为起点的边
        temp=node_set[k].head;
        while(temp!=NULL){
            inputfile2<<temp->u << " "<<temp->v << " "<<temp->w<<"\n";
            temp=temp->next;
        }
    }
    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    Johnson(N[1],outputfile2);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count=dt*1000000;
    timefile<<time_count<<endl;//把时间输入文件

    generate(N[2],m[0]);
    for(int k=0;k<N[2];k++){//把每个结点遍历，输出以其为起点的边
        temp=node_set[k].head;
        while(temp!=NULL){
            inputfile3<<temp->u << " "<<temp->v << " "<<temp->w<<"\n";
            temp=temp->next;
        }
    }
    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    Johnson(N[2],outputfile3);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count=dt*1000000;
    timefile<<time_count<<endl;//把时间输入文件

    generate(N[3],m[0]);
    for(int k=0;k<N[3];k++){//把每个结点遍历，输出以其为起点的边
        temp=node_set[k].head;
        while(temp!=NULL){
            inputfile4<<temp->u << " "<<temp->v << " "<<temp->w<<"\n";
            temp=temp->next;
        }
    }
    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    Johnson(N[3],outputfile4);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count=dt*1000000;
    timefile<<time_count<<endl;//把时间输入文件

    generate(N[0],m[1]);
    for(int k=0;k<N[0];k++){//把每个结点遍历，输出以其为起点的边
        temp=node_set[k].head;
        while(temp!=NULL){
            inputfile5<<temp->u << " "<<temp->v << " "<<temp->w<<"\n";
            temp=temp->next;
        }
    }
    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    Johnson(N[0],outputfile5);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count=dt*1000000;
    timefile<<time_count<<endl;//把时间输入文件

    generate(N[1],m[1]);
    for(int k=0;k<N[1];k++){//把每个结点遍历，输出以其为起点的边
        temp=node_set[k].head;
        while(temp!=NULL){
            inputfile6<<temp->u << " "<<temp->v << " "<<temp->w<<"\n";
            temp=temp->next;
        }
    }
    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    Johnson(N[1],outputfile6);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count=dt*1000000;
    timefile<<time_count<<endl;//把时间输入文件

    generate(N[2],m[1]);
    for(int k=0;k<N[2];k++){//把每个结点遍历，输出以其为起点的边
        temp=node_set[k].head;
        while(temp!=NULL){
            inputfile7<<temp->u << " "<<temp->v << " "<<temp->w<<"\n";
            temp=temp->next;
        }
    }
    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    Johnson(N[2],outputfile7);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count=dt*1000000;
    timefile<<time_count<<endl;//把时间输入文件

    generate(N[3],m[1]);
    for(int k=0;k<N[3];k++){//把每个结点遍历，输出以其为起点的边
        temp=node_set[k].head;
        while(temp!=NULL){
            inputfile8<<temp->u << " "<<temp->v << " "<<temp->w<<"\n";
            temp=temp->next;
        }
    }
    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&t1);
    Johnson(N[3],outputfile8);
    QueryPerformanceCounter(&t2);
    dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
    time_count=dt*1000000;
    timefile<<time_count<<endl;//把时间输入文件


    inputfile1.close();
    inputfile2.close();
    inputfile3.close();
    inputfile4.close();
    inputfile5.close();
    inputfile6.close();
    inputfile7.close();
    inputfile8.close();
    outputfile1.close();
    outputfile2.close();
    outputfile3.close();
    outputfile4.close();
    outputfile5.close();
    outputfile6.close();
    outputfile7.close();
    outputfile8.close();
    timefile.close();
    
    
    return 0;
}
