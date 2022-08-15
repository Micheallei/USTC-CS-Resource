
//实验2表达式树
#include<iostream>
#include<stack>
#include<string>
#include<sstream>
using namespace std;


typedef struct {  //每个叶节点存放字符与相应等级，常数为3；+,-为1； * /为2
	string s;
	int grade;
}node;

typedef struct BiTNode {
	node data;
	struct BiTNode* lchild, * rchild;
}BiTNode, *BiTree;

stack<int> stk;


void createBiTree(BiTree& T) {
	//按先序次序输入二叉树中节点的值
	//构造二叉链表表示的二叉树T
	node ch;
	cin >> ch.s;
	if ((ch.s == "+") || (ch.s == "-") || (ch.s == "/") || (ch.s == "*")) {	//若是运算符，则除了构造此节点外，还要递归的构造该节点的左孩子和右孩子
		T = new BiTNode;
		T->data = ch;
		if ((ch.s == "+") || (ch.s == "-")) T->data.grade = 1;
		else if ((ch.s == "/") || (ch.s == "*")) T->data.grade = 2;
		createBiTree(T->lchild);
		createBiTree(T->rchild);
	}
	else {		//若是运算数，则只需要构造该节点即可
		T = new BiTNode;
		T->data = ch;
		T->data.grade = 3;
		T->lchild = NULL;
		T->rchild = NULL;
	}
}

void PrintEle(string S) {  //打印树节点的值并进行相应栈操作
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
	//后序遍历表达式树，输出逆波兰式，并求值，计算结果放在栈stk中
	if (T) {
		PostOrderTraverse(T->lchild, Visit);
		PostOrderTraverse(T->rchild, Visit);
		Visit(T->data.s);
	}
}

void InOrderTraverse(BiTree T, void (*Visit)(string)) {
	//中序遍历表达式树，输出中缀表达式,有多余括号
	if (T) {
		if (T->lchild && T->lchild->lchild) cout << '(';//若当前节点的左子树仍至少2的深度，相当于还有计算式，则添加括号
		InOrderTraverse(T->lchild , Visit);
		if(T->lchild && T->lchild->lchild) cout << ')';
		Visit(T->data.s);
		if (T->rchild && T->rchild->rchild) cout << '(';//若当前节点的右子树仍至少2的深度，相当于还有计算式，则添加括号
		InOrderTraverse(T->rchild, Visit);
		if (T->rchild && T->rchild->rchild) cout << ')';
	}
}

void InOrderTraverse1(BiTree T, void (*Visit)(string)) {
	//中序遍历表达式树，输出中缀表达式,无多余括号
	int flag1 = 0, flag2 = 0;
	if (T->lchild) {			//若左孩子存在(则该节点为运算符)
		if (T->lchild->data.grade < T->data.grade) {  //左孩子运算符优先级低于子树根节点，加括号
			flag1 = 1; cout << '(';
		}
		InOrderTraverse1(T->lchild, Visit);		//若节点为操作数，则没有孩子，不需要再进行对以该操作数为根的子树中序遍历
		if (flag1) cout << ')';
	}
	Visit(T->data.s);
	if (T->rchild) {				//若右孩子存在(则该节点为运算符)
		if (T->rchild->data.grade <= T->data.grade) { //右孩子运算符优先级低于或等于子树根节点，加括号
			flag2 = 1; cout << '(';
		}
		InOrderTraverse1(T->rchild, Visit);
		if (flag2) cout << ')';
	}
}

int main() {
	BiTree T;
	createBiTree(T);
	cout << "中缀表达式: ";
	InOrderTraverse(T, PrintEle1);
	cout << endl << "无多余括号: ";
	InOrderTraverse1(T, PrintEle1);
	cout << endl<<"逆波兰式: ";
	PostOrderTraverse(T, PrintEle);
	cout << endl << "求值: ";
	cout << stk.top() << endl;
	return 0;
}
