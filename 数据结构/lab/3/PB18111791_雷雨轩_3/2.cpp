
//ʵ��2���ʽ��
#include<iostream>
#include<stack>
#include<string>
#include<sstream>
using namespace std;


typedef struct {  //ÿ��Ҷ�ڵ����ַ�����Ӧ�ȼ�������Ϊ3��+,-Ϊ1�� * /Ϊ2
	string s;
	int grade;
}node;

typedef struct BiTNode {
	node data;
	struct BiTNode* lchild, * rchild;
}BiTNode, *BiTree;

stack<int> stk;


void createBiTree(BiTree& T) {
	//�������������������нڵ��ֵ
	//������������ʾ�Ķ�����T
	node ch;
	cin >> ch.s;
	if ((ch.s == "+") || (ch.s == "-") || (ch.s == "/") || (ch.s == "*")) {	//���������������˹���˽ڵ��⣬��Ҫ�ݹ�Ĺ���ýڵ�����Ӻ��Һ���
		T = new BiTNode;
		T->data = ch;
		if ((ch.s == "+") || (ch.s == "-")) T->data.grade = 1;
		else if ((ch.s == "/") || (ch.s == "*")) T->data.grade = 2;
		createBiTree(T->lchild);
		createBiTree(T->rchild);
	}
	else {		//��������������ֻ��Ҫ����ýڵ㼴��
		T = new BiTNode;
		T->data = ch;
		T->data.grade = 3;
		T->lchild = NULL;
		T->rchild = NULL;
	}
}

void PrintEle(string S) {  //��ӡ���ڵ��ֵ��������Ӧջ����
	cout << S<<' ';
	int a1, b1;
	if (S == "+") {
		a1 = stk.top(); stk.pop();
		b1 = stk.top(); stk.pop();
		stk.push(b1 + a1);
	}
	else if (S == "-") {
		a1 = stk.top(); stk.pop();
		b1 = stk.top(); stk.pop();
		stk.push(b1 - a1);
	}
	else if (S == "*") {
		a1 = stk.top(); stk.pop();
		b1 = stk.top(); stk.pop();
		stk.push(b1 * a1);
	}
	else if (S == "/") {
		a1 = stk.top(); stk.pop();
		b1 = stk.top(); stk.pop();
		stk.push(b1/a1);
	}
	else {
		istringstream(S) >> a1;
		stk.push(a1);
	}
}

void PrintEle1(string S) {
	cout << S;
}

void PostOrderTraverse(BiTree T, void (*Visit)(string)) {
	//����������ʽ��������沨��ʽ������ֵ������������ջstk��
	if (T) {
		PostOrderTraverse(T->lchild, Visit);
		PostOrderTraverse(T->rchild, Visit);
		Visit(T->data.s);
	}
}

void InOrderTraverse(BiTree T, void (*Visit)(string)) {
	//����������ʽ���������׺���ʽ,�ж�������
	if (T) {
		if (T->lchild && T->lchild->lchild) cout << '(';//����ǰ�ڵ��������������2����ȣ��൱�ڻ��м���ʽ�����������
		InOrderTraverse(T->lchild , Visit);
		if(T->lchild && T->lchild->lchild) cout << ')';
		Visit(T->data.s);
		if (T->rchild && T->rchild->rchild) cout << '(';//����ǰ�ڵ��������������2����ȣ��൱�ڻ��м���ʽ�����������
		InOrderTraverse(T->rchild, Visit);
		if (T->rchild && T->rchild->rchild) cout << ')';
	}
}

void InOrderTraverse1(BiTree T, void (*Visit)(string)) {
	//����������ʽ���������׺���ʽ,�޶�������
	int flag1 = 0, flag2 = 0;
	if (T->lchild) {			//�����Ӵ���(��ýڵ�Ϊ�����)
		if (T->lchild->data.grade < T->data.grade) {  //������������ȼ������������ڵ㣬������
			flag1 = 1; cout << '(';
		}
		InOrderTraverse1(T->lchild, Visit);		//���ڵ�Ϊ����������û�к��ӣ�����Ҫ�ٽ��ж��Ըò�����Ϊ���������������
		if (flag1) cout << ')';
	}
	Visit(T->data.s);
	if (T->rchild) {				//���Һ��Ӵ���(��ýڵ�Ϊ�����)
		if (T->rchild->data.grade <= T->data.grade) { //�Һ�����������ȼ����ڻ�����������ڵ㣬������
			flag2 = 1; cout << '(';
		}
		InOrderTraverse1(T->rchild, Visit);
		if (flag2) cout << ')';
	}
}

int main() {
	BiTree T;
	createBiTree(T);
	cout << "��׺���ʽ: ";
	InOrderTraverse(T, PrintEle1);
	cout << endl << "�޶�������: ";
	InOrderTraverse1(T, PrintEle1);
	cout << endl<<"�沨��ʽ: ";
	PostOrderTraverse(T, PrintEle);
	cout << endl << "��ֵ: ";
	cout << stk.top() << endl;
	return 0;
}
