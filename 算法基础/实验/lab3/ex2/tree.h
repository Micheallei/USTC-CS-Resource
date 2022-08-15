#ifndef SRC_TREE_H
#define SRC_TREE_H

#include<iostream>
#include <fstream>
using namespace std;

class Node
{
public:
    int key;
    int interval[2];//0对应low,1对应high
    int max;//以x为根的子树中所有区间的端点的最大值
    int color;//0是黑，1是红
    Node* left;
    Node* right;
    Node* p;
};

class RB_Tree
{
public:
    Node* root;
    Node* nil;

    RB_Tree(Node* root,Node* nil);
    void inorder_RB_walk(Node* x,ofstream &f);//中序遍历
    Node* RB_search(Node* x,int k);//从树根开始查找
    Node* RB_maximum(Node* x);//找最大
    Node* RB_minimum(Node* x);//找最小
    Node* RB_successor(Node* x);//找后继
    Node* RB_predecessor(Node* x);//找前驱
    void left_rotate(Node* x);//左旋
    void right_rotate(Node* x);//右旋
    //插入
    void RB_insert(Node* z);
    void RB_insert_fixup(Node* z);
    //删除
    void RB_transplant(Node* u,Node* v);
    void RB_delete(Node* z);
    void RB_delete_fixup(Node* x);

    Node* interval_search(int i[2]);
};





#endif //SRC_TREE_H
