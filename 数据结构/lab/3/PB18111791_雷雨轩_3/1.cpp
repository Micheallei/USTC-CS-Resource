//ʵ��1�������Ĵ��������
#include<iostream>
using namespace std;
#define MAXQSIZE 100

typedef enum PointerTag { Link, Thread };					//Link = 0, ָ��;thread=1,����
typedef struct BiThrNode {									//����������������
	char data;
	struct BiThrNode* lchild, * rchild, * parent;
	PointerTag LTag, RTag;
}BiThrNode, * BiThrTree;

typedef struct {					
	BiThrTree * base;
	int front;
	int rear;
}SqQueue;													//����ѭ��������Ϊ��������������ĸ���

void InitQueue(SqQueue& Q) {								//��ʼ���ն���
	Q.base = new BiThrTree[MAXQSIZE];
	Q.front = Q.rear = 0;
}

void enqueue(SqQueue& Q, BiThrTree e) {						//���
	if ((Q.rear + 1) % MAXQSIZE == Q.front) exit(0);
	Q.base[Q.rear] = e;
	Q.rear = (Q.rear + 1) % MAXQSIZE;
}

BiThrTree dequeue(SqQueue& Q) {								//����
	if (Q.front == Q.rear) exit(0);
	BiThrTree e = Q.base[Q.front];
	Q.front = (Q.front + 1) % MAXQSIZE;
	return e;
}

void CreateBiThrTree(BiThrTree& T) {						//�ɲ���������������Ķ�������
															//����ָ��ָ�����Һ��ӣ���û���Һ��ӣ���Ӧָ��ΪNULL
	char ch;
	SqQueue Q;												//������������
	InitQueue(Q);
	cin >>noskipws>> ch;
	if (ch == ' ') {										//��һ���ڵ�Ϊ�գ��򷵻ؿ���
		T = NULL;
	}
	else {													//�������ڵ㲢ʹ���ڵ����
		T = new BiThrNode;
		T->data = ch;
		T->parent = NULL;
		T->LTag = Link;
		T->RTag = Link;
		enqueue(Q, T);
	}
	while (Q.rear != Q.front) {								//��ǰ���в�Ϊ��
		BiThrTree p = dequeue(Q);							//��ͷԪ�س��ӣ���Ϊ��������������������ݵĸ�ĸ	
		cin >> noskipws >> ch;
		if (ch == ' ') {									//����Ϊ��
			p->lchild = NULL;
			p->LTag = Thread;
		}
		else {												//���Ӳ�Ϊ��
			p->lchild = new BiThrNode;
			p->lchild->data = ch;
			p->lchild->parent = p;
			p->LTag = Link;
			enqueue(Q, p->lchild);							//�������
		}
		cin >> noskipws >> ch;
		if (ch == ' ') {									//�Һ���Ϊ��
			p->rchild = NULL;
			p->RTag = Thread;
		}
		else {												//�Һ��Ӳ�Ϊ��
			p->rchild = new BiThrNode;
			p->rchild->data = ch;
			p->rchild->parent = p;
			p->RTag = Link;
			enqueue(Q, p->rchild);							//�Һ������
		}
	}
}

void PostThreading(BiThrTree T, BiThrTree &pre) {			//ʵ�ֺ����������ĺ���
	if (T) {
		PostThreading(T->lchild,pre);						//������������������
		PostThreading(T->rchild,pre);						//������������������
		if (!T->lchild) { T->LTag = Thread; T->lchild = pre; }		//��ǰ�ڵ�T��ǰ������
		if (!pre->rchild) { pre->RTag = Thread; pre->rchild = T; }	//pre�ĺ������
		pre = T;											//��ǰ�ڵ��������������ǰ����Ϊ��ǰ�ڵ�
	}
}

void PostOrderThreading(BiThrTree &Thrt,BiThrTree T) {
																	//�������������T,���������������
																	//���������������һ��ͷ��㣬��lchildָ����������ڵ㣬
																	//rchildָ���������з��ʵ����һ���ڵ㣨ҲΪ���ڵ㣩;
																	//��Ӧ�ģ��������������е�һ���ڵ�lchildָ��ͷ��㣻���һ���ڵ�(��)���������
	Thrt = new BiThrNode;											//��ͷ���
	Thrt->LTag = Link; Thrt->RTag = Thread;
	Thrt->rchild = Thrt;											//��ʼΪ��������ָ���ָ
	if(!T)	Thrt->lchild = Thrt;									//��������Ϊ�գ�����ָ���ָ
	else {
		Thrt->lchild = T;
		BiThrTree pre = Thrt;										//preָ����������е�ǰ�ڵ��ǰ��
		PostThreading(T,pre);
		if (!pre->rchild) {											//β���(�����ڵ�������������������ָ��ͷ���Thrt)
			pre->rchild = Thrt;
			pre->RTag = Thread;
		}
		Thrt->rchild = pre;
	}
}

int PrintEle(char a) {												//�����ķ��ʺ���
	cout << a;
	return 1;
}

void PreOrderTraverse(BiThrTree T, int (* Visit)(char)) {
																	//��������������������ĵݹ��㷨
	if (T) {
		Visit(T->data);												//���ʸ��ڵ�
		if (T->LTag == 0) {											//���������ڣ��������������
			PreOrderTraverse(T->lchild, Visit);
		}
		if (T->RTag == 0) {											//���������ڣ��������������
			PreOrderTraverse(T->rchild, Visit);
		}	
	}
}

void InOrderTraverse(BiThrTree T, int (*Visit)(char)) {
																	//��������������������ĵݹ��㷨
	if (T) {
		if (T->LTag == 0) {											//���������ڣ��������������
			InOrderTraverse(T->lchild, Visit);
		}
		Visit(T->data);												//���ʸ��ڵ�
		if (T->RTag == 0) {											//���������ڣ��������������
			InOrderTraverse(T->rchild, Visit);		
		}
	}
}

void PostOrderTraverse(BiThrTree Thrt, int (*Visit)(char)) {
																	//��������������������ķǵݹ��㷨
	int flag = 0;
	BiThrTree p = Thrt->lchild;
	if (p == Thrt) return;											//����
	do {
		while (p->LTag == Link) p = p->lchild;						//�ߵ����������
		if (p->RTag == Link) { p = p->rchild; continue; }			//����˽ڵ����Һ��ӣ�������ǰѭ������ʼ��������Ը��Һ���Ϊ��������
		while (p->RTag == Thread) { Visit(p->data); p = p->rchild; }	//û���Һ��ӣ�����ʵ�ǰ�ڵ㲢ָ������
		Visit(p->data);													//�˻ص����Һ��ӵĽڵ㣬��ʱ�Ըýڵ�Ϊ�������������������Ѻ��������ϣ����Է��ʸýڵ�
		while ((p->parent->rchild == p) || (p->parent->lchild == p && p->parent->RTag == Thread)) {//�Ҹø��ڵ�ĺ��:��˫���Һ��ӻ���������˫�����Һ��ӣ�����Ϊ˫��
			p = p->parent;
			if (p == Thrt->lchild) {									//���ص����ڵ㣬���˳�ѭ��
				flag = 1;
				break;
			}
			Visit(p->data);
		}
		if (flag) break;
		if(p->parent->lchild == p && p->RTag == Link){					//����˫��������˫�����Һ��ӣ���λ��˫�׵��Һ��ӣ�������Ϊ���������������
			p = p->parent->rchild;
		}
	} while (p != Thrt->lchild);
	Visit(p->data);														//���ʸ��ڵ�
}


int main() {
	BiThrTree T,Thrt;
	CreateBiThrTree(T);													//�������������������������
	PostOrderThreading(Thrt, T);										//����������������,����ͷ���Thrt
	PreOrderTraverse(Thrt->lchild, PrintEle);							//�������
	cout << endl;
	InOrderTraverse(Thrt->lchild, PrintEle);							//�������
	cout << endl;
	PostOrderTraverse(Thrt, PrintEle);									//�������
	cout << endl;
	return 0;
}