
#include<iostream>
#include<fstream>
#include<stdio.h>
using namespace std;
#define INFINITY 100			//最大值
#define MAX_VERTEX_NUM 20		//最大顶点个数
#define STACK_INIT_SIZE 100   //栈的初始空间分配量


//辅助栈
typedef struct {
	int* base;
	int* top;
	int stacksize;
}SqStack;

void InitStack(SqStack& S) {
	S.base = new int[STACK_INIT_SIZE];
	S.top = S.base;
	S.stacksize = STACK_INIT_SIZE;
}

void Push(SqStack& S, int e) {
	*S.top = e;
	S.top++;
}

int Pop(SqStack& S) {
	S.top--;
	int e = *S.top;
	return e;
}


typedef struct QNode {//辅助循环队列
	int* base;
	int front;
	int rear;
}SqQueue;

void InitQueue(SqQueue& Q) {
	Q.base = new int[MAX_VERTEX_NUM];
	Q.front = Q.rear = 0;
}

void EnQueue(SqQueue& Q, int e) {//入队
	Q.base[Q.rear] = e;
	Q.rear = (Q.rear + 1) % MAX_VERTEX_NUM;
}

int DeQueue(SqQueue& Q) {
	int e;
	e = Q.base[Q.front];
	Q.front = (Q.front + 1) % MAX_VERTEX_NUM;
	return e;
}

typedef struct ArcCell {
	int adj; //顶点关系类型，有权图代表边权
	//int* info;//该弧相关信息的指针
}ArccCell, AdjMatrix[MAX_VERTEX_NUM][MAX_VERTEX_NUM];

typedef struct {
	int vex[MAX_VERTEX_NUM];//顶点向量,为-1表示该顶点不存在
	AdjMatrix arcs; //邻接矩阵
	int vexnum, arcnum;	//图的当前顶点数和弧数
}MGraph;

void CreateUDN(MGraph& G) {//采用邻接矩阵表示法，构造无向网，默认边权为1
	FILE* f;
	fopen_s(&f, "test_2.txt", "r");
	int i, j;
	for (i = 0; i < MAX_VERTEX_NUM; i++) {//初始化邻接矩阵
		for (j = 0; j < MAX_VERTEX_NUM; j++) {
			G.arcs[i][j].adj = INFINITY; //INFINITY表示该弧不存在
		}
	}
	fscanf_s(f, "%d", &G.vexnum);
	for (i = 0; i < G.vexnum; i++) {
		G.vex[i] = i;
	}
	for (; i < MAX_VERTEX_NUM; i++) {
		G.vex[i] = -1;	//-1表示不存在编号为i的顶点
	}
	while (fscanf_s(f, "%d %d", &i, &j) != EOF) {
		G.arcs[i][j].adj = 1;	//默认边权为1
		G.arcs[j][i].adj = 1;
		G.arcnum++;
	}
}


int FirstAdjVex(MGraph& G, int v) {//给定无向图及其中一个顶点编号（从0开始），输出该顶点的第一个邻接点编号，若没有邻接顶点，返回-1
	int i = 0;
	while ((i < G.vexnum) && (G.arcs[v][i].adj == INFINITY)) {
		i++;
	}
	if (i == G.vexnum) return -1;
	return i;
}

int NextAdjVex(MGraph& G, int v, int w) {//返回v是图G顶点，w是v的一个邻接顶点，返回v相对于w的下一个邻接顶点，若无，返回-1；
	int i = w + 1;
	while ((i < G.vexnum) && (G.arcs[v][i].adj == INFINITY)) {
		i++;
	}
	if (i == G.vexnum) return -1;
	return i;
}



typedef struct ArcNode {
	int adjvex;	//该弧所指向的顶点的位置
	struct ArcNode* nextarc;//指向下一条弧的指针
	int info; //该弧权值
}ArcNode;

typedef struct VNode {
	int data; //顶点信息
	ArcNode* firstarc;//指向第一条依附于该顶点的弧的指针
}VNode, AdjList[MAX_VERTEX_NUM];

typedef struct {
	AdjList vertices;
	int vexnum, arcnum; //图的当前顶点数和弧数
	//int kind; //图的种类
}ALGraph;

void CreateUDN(ALGraph& G) {
	//采用邻接表的存储结构，构造无向网G
	FILE* f;
	fopen_s(&f, "test_2.txt", "r");
	fscanf_s(f, "%d", &G.vexnum);
	int i, j;
	G.arcnum = 0;
	ArcNode* temp1, * temp2;
	for (i = 0; i < G.vexnum; i++) {//初始化顶点信息
		G.vertices[i].data = i;
		G.vertices[i].firstarc = NULL;
	}
	for (; i < MAX_VERTEX_NUM; i++) {
		G.vertices[i].data = -1;
		G.vertices[i].firstarc = NULL;
	}
	while (fscanf_s(f, "%d %d", &i, &j) != EOF) {//完成弧在每个单链表头结点后的插入(无向图一条弧出现两次)
		temp1 = new ArcNode;
		temp1->info = 1;
		temp1->adjvex = j;
		temp1->nextarc = G.vertices[i].firstarc;
		G.vertices[i].firstarc = temp1;
		temp2 = new ArcNode;
		temp2->info = 1;
		temp2->adjvex = i;
		temp2->nextarc = G.vertices[j].firstarc;
		G.vertices[j].firstarc = temp2;
		G.arcnum++;
	}
}

int FirstAdjVex(ALGraph& G, int v) {	//给定无向图及其中一个顶点编号（从0开始），输出该顶点的第一个邻接点编号，若没有邻接顶点，返回-1
	if (!G.vertices[v].firstarc) return -1;
	return G.vertices[v].firstarc->adjvex;
}

int NextAdjVex(ALGraph& G, int v, int w) {//返回v是图G顶点，w是v的一个邻接顶点，返回v相对于w的下一个邻接顶点，若无，返回-1；
	ArcNode* temp;
	temp = G.vertices[v].firstarc;
	while ((temp->adjvex != w)) {
		temp = temp->nextarc;
	}
	temp = temp->nextarc;
	if (temp == NULL) return -1;
	return temp->adjvex;
}


int count_1 = 0;
int visited[MAX_VERTEX_NUM] = { 0 };//visited[i]是深度优先搜索时访问第i个顶点的次序号
int min[MAX_VERTEX_NUM];	//作为中间量，记录当前所找到的顶点i及其子孙的回边所指向的顶点中访问次序号最小的顶点的访问次序号
int low[MAX_VERTEX_NUM];	//low[i]表示顶点i及其子孙的回边所连接的顶点中访问次序号最小的顶点的访问次序号
int result[MAX_VERTEX_NUM] = { 0 };	//记录找到的关节点的顶点编号
int num = 0;
void DFSArticul(MGraph& G, int v0) {
	//从第v0个顶点出发深度优先查找关节点
	SqStack S;
	InitStack(S);
	int v1;
	visited[v0] = min[v0] = ++count_1;	//初始化根节点
	while (1) {	//用辅助栈模拟递归过程
		int flag = 0;
		for (v1 = FirstAdjVex(G, v0); v1 >= 0; v1 = NextAdjVex(G, v0, v1)) {
			if (visited[v1] == 0) {//v1未曾访问，是v0的孩子
				//mark[v0] = 1;
				Push(S, v0);
				v0 = v1;
				visited[v0] = min[v0] = ++count_1;
				flag = 1; break;
			}
			else if (visited[v1] < min[v0]) {
				min[v0] = visited[v1];	//v1已访问，是祖先节点,则更新当前找到的min[v0]
			}
		}//for
		if (flag) continue;
		low[v0] = min[v0];	//若能执行到此步骤，说明上面for循环里每一次都执行的else if块，即当前v0已没有孩子节点，为叶子
		if (S.base == S.top) break;	//栈空，表明从初始v0出发深度优先搜索执行完毕
		v1 = v0;
		v0 = Pop(S);
		if (low[v1] < min[v0]) min[v0] = low[v1];	//更新当前找到的min[v0]
		if (low[v1] >= visited[v0]) { result[num++] = v0; } //输出关节点v0	
	}//while	
}

void FindArticul(MGraph& G) {
	//连通图以邻接矩阵做存储结构，查找并输出G上全部关节点，按顶点编号升序输出
	for (int i = 0; i < MAX_VERTEX_NUM; i++) {
		min[i] = INFINITY;
		low[i] = INFINITY;
	}
	count_1 = 1;
	visited[0] = 1;//以0作为根节点
	int v;
	v = FirstAdjVex(G, 0);
	DFSArticul(G, v);	//从第v顶点出发深度优先查找关节点
	if (count_1 < G.vexnum) {	//生成树的根至少有两棵子树，则根为关节点
		result[num++] = 0;//输出顶点编号0；
		while ((v = NextAdjVex(G, 0, v)) >= 0) {
			if (visited[v] == 0) DFSArticul(G, v);
		}//while
	}//if
}//FindArticul

void bubble_sort() {//冒泡排序
	int i = 0;
	int j = 0;
	int temp;
	for (j = 0; j < num - 1; j++) {
		for (i = 0; i < num - 1 - j; i++) {
			if (result[i] > result[i + 1]) {
				temp = result[i];
				result[i] = result[i + 1];
				result[i + 1] = temp;
			}
		}
	}
}


int PathMatrix[MAX_VERTEX_NUM][MAX_VERTEX_NUM] = { 0 };//P[v]存放顶点0到顶点v的最短路径上经过的顶点顺序
int final[MAX_VERTEX_NUM] = { 0 };//为1表示从顶点0到该顶点的最短路径已经找到
int num_1[MAX_VERTEX_NUM] = { 0 };//num[v]存放从顶点0到该顶点v最短路径长度

void ShorttestPath(ALGraph& G, int v0) {//最短路径算法
	int i, j;
	for (i = 0; i < G.vexnum; i++) {	//初始化最短路径
		for (j = 0; j < G.vexnum; j++) {
			PathMatrix[i][j] = -1;
		}
	}
	SqQueue Q;
	InitQueue(Q);
	final[0] = 1;	//起始点顶点0已被访问
	int v1 = 0;
	EnQueue(Q, v1);
	final[v1] = 1;
	num_1[v1]++;
	PathMatrix[0][0] = 0;
	int v2;
	while (Q.front!=Q.rear) {//考虑到边权默认为1，所以按非递归层次遍历的方式来找最短路径
		v1 = DeQueue(Q);
		for (v2 = FirstAdjVex(G, v1); v2 >= 0; v2 = NextAdjVex(G, v1, v2)) {
			if (!final[v2]) {//如果顶点v2还未找到最短路径
				for (i = 0; i < num_1[v1]; i++) {//回溯v2父母节点v1的最短路径，即v2最短路径为v1最短路径加上v1->v2边
					PathMatrix[v2][num_1[v2]] = PathMatrix[v1][i];
					num_1[v2]++;
				}
				PathMatrix[v2][num_1[v2]++] = v2;
				final[v2] = 1;
				EnQueue(Q,v2);
			}
		}//for
	}//while
}

int main() {
	//关节点
	MGraph G1;
	CreateUDN(G1);
	FindArticul(G1);
	bubble_sort();
	int temp = -1;
	for (int i = 0; i < num; i++) {
		if (temp == result[i]) continue;//考虑到若顶点i去掉后会多生成n个连通片，则算法执行过程中，会记录顶点i n次，所以需把重复点去掉
		cout << result[i] << " ";
		temp = result[i];
	}
	cout << endl;
	//最短路径
	ALGraph G2;
	CreateUDN(G2);
	ShorttestPath(G2, 0);
	int i = 0;
	int j = 0;
	for (i = 1; i < G2.vexnum; i++) {
		cout << num_1[i] - 1 << " " << PathMatrix[i][0];
		for (j = 1; j < num_1[i]; j++) {
			cout << "->" << PathMatrix[i][j];
		}
		cout << endl;
	}
	return 0;
}
