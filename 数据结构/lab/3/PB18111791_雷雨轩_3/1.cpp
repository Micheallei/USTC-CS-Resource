//实验1二叉树的创建与遍历
#include<iostream>
using namespace std;
#define MAXQSIZE 100

typedef enum PointerTag { Link, Thread };					//Link = 0, 指针;thread=1,线索
typedef struct BiThrNode {									//线索化的三叉链表
	char data;
	struct BiThrNode* lchild, * rchild, * parent;
	PointerTag LTag, RTag;
}BiThrNode, * BiThrTree;

typedef struct {					
	BiThrTree * base;
	int front;
	int rear;
}SqQueue;													//构造循环队列作为层序遍历构建树的辅助

void InitQueue(SqQueue& Q) {								//初始化空队列
	Q.base = new BiThrTree[MAXQSIZE];
	Q.front = Q.rear = 0;
}

void enqueue(SqQueue& Q, BiThrTree e) {						//入队
	if ((Q.rear + 1) % MAXQSIZE == Q.front) exit(0);
	Q.base[Q.rear] = e;
	Q.rear = (Q.rear + 1) % MAXQSIZE;
}

BiThrTree dequeue(SqQueue& Q) {								//出队
	if (Q.front == Q.rear) exit(0);
	BiThrTree e = Q.base[Q.front];
	Q.front = (Q.front + 1) % MAXQSIZE;
	return e;
}

void CreateBiThrTree(BiThrTree& T) {						//由层序遍历构建出来的二叉树，
															//左右指针指向左右孩子，若没左右孩子，相应指针为NULL
	char ch;
	SqQueue Q;												//构建辅助队列
	InitQueue(Q);
	cin >>noskipws>> ch;
	if (ch == ' ') {										//第一个节点为空，则返回空树
		T = NULL;
	}
	else {													//创建根节点并使根节点入队
		T = new BiThrNode;
		T->data = ch;
		T->parent = NULL;
		T->LTag = Link;
		T->RTag = Link;
		enqueue(Q, T);
	}
	while (Q.rear != Q.front) {								//当前队列不为空
		BiThrTree p = dequeue(Q);							//队头元素出队，此为接下来层序遍历输入数据的父母	
		cin >> noskipws >> ch;
		if (ch == ' ') {									//左孩子为空
			p->lchild = NULL;
			p->LTag = Thread;
		}
		else {												//左孩子不为空
			p->lchild = new BiThrNode;
			p->lchild->data = ch;
			p->lchild->parent = p;
			p->LTag = Link;
			enqueue(Q, p->lchild);							//左孩子入队
		}
		cin >> noskipws >> ch;
		if (ch == ' ') {									//右孩子为空
			p->rchild = NULL;
			p->RTag = Thread;
		}
		else {												//右孩子不为空
			p->rchild = new BiThrNode;
			p->rchild->data = ch;
			p->rchild->parent = p;
			p->RTag = Link;
			enqueue(Q, p->rchild);							//右孩子入队
		}
	}
}

void PostThreading(BiThrTree T, BiThrTree &pre) {			//实现后序线索化的函数
	if (T) {
		PostThreading(T->lchild,pre);						//对左子树后序线索化
		PostThreading(T->rchild,pre);						//对右子树后序线索化
		if (!T->lchild) { T->LTag = Thread; T->lchild = pre; }		//当前节点T的前驱线索
		if (!pre->rchild) { pre->RTag = Thread; pre->rchild = T; }	//pre的后继线索
		pre = T;											//当前节点完成线索化，则前驱变为当前节点
	}
}

void PostOrderThreading(BiThrTree &Thrt,BiThrTree T) {
																	//后序遍历二叉树T,并将其后续线索化
																	//在线索链表上添加一个头结点，其lchild指向二叉树根节点，
																	//rchild指向后序遍历中访问的最后一个节点（也为根节点）;
																	//相应的，二叉树后序序列第一个节点lchild指向头结点；最后一个节点(根)视情况而定
	Thrt = new BiThrNode;											//建头结点
	Thrt->LTag = Link; Thrt->RTag = Thread;
	Thrt->rchild = Thrt;											//开始为空树，右指针回指
	if(!T)	Thrt->lchild = Thrt;									//若二叉树为空，则左指针回指
	else {
		Thrt->lchild = T;
		BiThrTree pre = Thrt;										//pre指向遍历过程中当前节点的前驱
		PostThreading(T,pre);
		if (!pre->rchild) {											//尾结点(即根节点若无右子树，则其后继指向头结点Thrt)
			pre->rchild = Thrt;
			pre->RTag = Thread;
		}
		Thrt->rchild = pre;
	}
}

int PrintEle(char a) {												//遍历的访问函数
	cout << a;
	return 1;
}

void PreOrderTraverse(BiThrTree T, int (* Visit)(char)) {
																	//先序遍历后续线索化树的递归算法
	if (T) {
		Visit(T->data);												//访问根节点
		if (T->LTag == 0) {											//左子树存在，先序遍历左子树
			PreOrderTraverse(T->lchild, Visit);
		}
		if (T->RTag == 0) {											//右子树存在，先序遍历右子树
			PreOrderTraverse(T->rchild, Visit);
		}	
	}
}

void InOrderTraverse(BiThrTree T, int (*Visit)(char)) {
																	//中序遍历后续线索化树的递归算法
	if (T) {
		if (T->LTag == 0) {											//左子树存在，中序遍历左子树
			InOrderTraverse(T->lchild, Visit);
		}
		Visit(T->data);												//访问根节点
		if (T->RTag == 0) {											//右子树存在，中序遍历右子树
			InOrderTraverse(T->rchild, Visit);		
		}
	}
}

void PostOrderTraverse(BiThrTree Thrt, int (*Visit)(char)) {
																	//后序遍历后续线索化树的非递归算法
	int flag = 0;
	BiThrTree p = Thrt->lchild;
	if (p == Thrt) return;											//空树
	do {
		while (p->LTag == Link) p = p->lchild;						//走到树的最左端
		if (p->RTag == Link) { p = p->rchild; continue; }			//最左端节点有右孩子，结束当前循环，开始后序遍历以该右孩子为根的子树
		while (p->RTag == Thread) { Visit(p->data); p = p->rchild; }	//没有右孩子，则访问当前节点并指向其后继
		Visit(p->data);													//退回到有右孩子的节点，此时以该节点为根的子树的左右子树已后序遍历完毕，所以访问该节点
		while ((p->parent->rchild == p) || (p->parent->lchild == p && p->parent->RTag == Thread)) {//找该根节点的后继:是双亲右孩子或是左孩子且双亲无右孩子，则后继为双亲
			p = p->parent;
			if (p == Thrt->lchild) {									//若回到根节点，则退出循环
				flag = 1;
				break;
			}
			Visit(p->data);
		}
		if (flag) break;
		if(p->parent->lchild == p && p->RTag == Link){					//若是双亲左孩子且双亲有右孩子，则定位到双亲的右孩子，对以其为根的子树后序遍历
			p = p->parent->rchild;
		}
	} while (p != Thrt->lchild);
	Visit(p->data);														//访问根节点
}


int main() {
	BiThrTree T,Thrt;
	CreateBiThrTree(T);													//按层序遍历输入来构建二叉树
	PostOrderThreading(Thrt, T);										//后续线索化二叉树,生成头结点Thrt
	PreOrderTraverse(Thrt->lchild, PrintEle);							//先序遍历
	cout << endl;
	InOrderTraverse(Thrt->lchild, PrintEle);							//中序遍历
	cout << endl;
	PostOrderTraverse(Thrt, PrintEle);									//后序遍历
	cout << endl;
	return 0;
}