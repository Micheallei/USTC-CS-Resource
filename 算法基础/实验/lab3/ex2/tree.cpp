#include "tree.h"
#include<iostream>
#include <fstream>
using namespace std;


int maxnum(int x,int y,int z){
    if(x>=y){
        if(x>=z) return x;
        else return z;
    }
    else if(z>=y) return z;
    else return y;
}

RB_Tree::RB_Tree(Node *root,Node* nil):root(root),nil(nil){}

void RB_Tree::inorder_RB_walk(Node* x,ofstream &f){
    if(x!=nil){
        inorder_RB_walk(x->left,f);
        f<<x->interval[0]<<" "<<x->interval[1]<<" "<<x->max<<endl;
        inorder_RB_walk(x->right,f);
    }
}

Node* RB_Tree::RB_search(Node* x,int k){
    while(x!=nil && k!=x->key){
        if(k < x->key){
            x=x->left;
        }
        else x=x->right;
    }
    return x;
}

Node* RB_Tree::RB_maximum(Node* x){
    while(x->right!=nil){
        x=x->right;
    }
    return x;
}

Node* RB_Tree::RB_minimum(Node* x){
    while(x->left!=nil){
        x=x->left;
    }
    return x;
}

Node* RB_Tree::RB_successor(Node* x){
    if(x->right!=nil){
        return RB_minimum(x->right);
    }
    Node* y=x->p;
    while(y!=nil && x==y->right){
        x=y;
        y=y->p;
    }
    return y;
}

Node* RB_Tree::RB_predecessor(Node* x){
    if(x->left!=nil){
        return RB_maximum(x->left);
    }
    Node* y=x->p;
    while(y!=nil && x==y->left){
        x=y;
        y=y->p;
    }
    return y;
}

void RB_Tree::left_rotate(Node* x){
    Node* y=x->right;
    x->right=y->left;
    if(y->left!=nil){
        y->left->p=x;
    }
    y->p=x->p;
    if(x->p==nil){//x原来为根节点
        root=y;
    }
    else if(x==x->p->left){
        x->p->left=y;
    }
    else{
        x->p->right=y;
    }
    y->left=x;
    x->p=y;
//更改x和y的max属性
    x->max=maxnum(x->interval[1],x->left->max,x->right->max);
    y->max=maxnum(y->interval[1],y->left->max,y->right->max);
}

void RB_Tree::right_rotate(Node* x){
    Node* y=x->left;
    x->left=y->right;
    if(y->right!=nil){
        y->right->p=x;
    }
    y->p=x->p;
    if(x->p==nil){//x原来为根节点
        root=y;
    }
    else if(x==x->p->left){
        x->p->left=y;
    }
    else{
        x->p->right=y;
    }
    y->right=x;
    x->p=y;

    x->max=maxnum(x->interval[1],x->left->max,x->right->max);
    y->max=maxnum(y->interval[1],y->left->max,y->right->max);
}


void RB_Tree::RB_insert(Node* z){
    Node* y=nil;
    Node* x=root;
    while(x!=nil){
        y=x;
        if(z->key < x->key){
            x=x->left;
        }
        else{
            x=x->right;
        }
    }
    z->p=y;
    if(y==nil){//对应树在插入前为空
        root=z;
    }
    else if(z->key < y->key){
        y->left=z;
    }
    else{
        y->right=z;
    }
    z->left=nil;
    z->right=nil;
    z->color=1;
//设置max属性并向上传播
    z->max=z->interval[1];//因为z左右子树都是nil结点
    Node* temp=z->p;
    while(temp!=nil){
        temp->max=maxnum(temp->interval[1],temp->left->max,temp->right->max);
        temp=temp->p;
    }
    RB_insert_fixup(z);
}

void RB_Tree::RB_insert_fixup(Node* z){
    Node* y;
    while(z->p->color==1){//z父节点为红
        if(z->p==z->p->p->left){
            y=z->p->p->right;//z父亲的兄弟
            if(y->color==1){//case 1
                z->p->color=0;
                y->color=0;
                z->p->p->color=1;
                z=z->p->p;
            }
            else{
                if(z==z->p->right){//case 2
                    z=z->p;
                    left_rotate(z);
                }
                z->p->color=0;//case 3
                z->p->p->color=1;
                right_rotate(z->p->p);
            }
        }
        else{//z的父亲是一个右儿子
            y=z->p->p->left;//z父亲的兄弟
            if(y->color==1){//case 1
                z->p->color=0;
                y->color=0;
                z->p->p->color=1;
                z=z->p->p;
            }
            else{
                if(z==z->p->left){//case 2
                    z=z->p;
                    right_rotate(z);
                }
                z->p->color=0;//case 3
                z->p->p->color=1;
                left_rotate(z->p->p);
            }
        }
    }
    root->color=0;
}


void RB_Tree::RB_transplant(Node* u,Node* v){
    if(u->p==nil){
        root=v;
    }
    else if(u==u->p->left){
        u->p->left=v;
    }
    else{
        u->p->right=v;
    }
    v->p=u->p;
}


void RB_Tree::RB_delete(Node* z){
    Node* y=z;
    Node* x;
    Node* temp;//用来往上传递对max属性的修改
    int y_original_color=y->color;
    if(z->left==nil){
        x=z->right;
        RB_transplant(z,z->right);
    }
    else if(z->right==nil){
        x=z->left;
        RB_transplant(z,z->left);
    }
    else{
        y=RB_minimum(z->right);
        y_original_color=y->color;
        x=y->right;
        if(y->p==z){
            x->p=y;
        }
        else{
            RB_transplant(y,y->right);
            y->right=z->right;
            y->right->p=y;
        }
        RB_transplant(z,y);
        y->left=z->left;
        y->left->p=y;
        y->color=z->color;

    }

//往上传播max的值
    temp=x->p;
    while(temp != nil){
        temp->max=maxnum(temp->interval[1],temp->left->max,temp->right->max);
        temp=temp->p;
    }

    if(y_original_color==0){
        RB_delete_fixup(x);
    }
}

void RB_Tree::RB_delete_fixup(Node* x){
    Node* w;//x的兄弟结点
    while(x!=root && x->color==0){
        if(x==x->p->left){
            w=x->p->right;
            if(w->color==1){//case 1
                w->color=0;
                x->p->color=1;
                left_rotate(x->p);
                w=x->p->right;
            }
            if(w->left->color==0 && w->right->color==0){//case 2
                w->color=1;
                x=x->p;
            }
            else{
                if(w->right->color==0){//case 3
                    w->left->color=0;
                    w->color=1;
                    right_rotate(w);
                    w=x->p->right;
                }
                //case 4
                w->color=x->p->color;
                x->p->color=0;
                w->right->color=0;
                left_rotate(x->p);
                x=root;
            }
        }
        else{
            w=x->p->left;
            if(w->color==1){//case 1
                w->color=0;
                x->p->color=1;
                right_rotate(x->p);
                w=x->p->left;
            }
            if(w->left->color==0 && w->right->color==0){//case 2
                w->color=1;
                x=x->p;
            }
            else{
                if(w->left->color==0){//case 3
                    w->right->color=0;
                    w->color=1;
                    left_rotate(w);
                    w=x->p->left;
                }
                //case 4
                w->color=x->p->color;
                x->p->color=0;
                w->left->color=0;
                right_rotate(x->p);
                x=root;
            }
        }
    }
    x->color=0;
}

Node* RB_Tree::interval_search(int i[2]){
    Node* x=root;
    while(x!=nil && !(x->interval[0]<=i[1] && i[0]<=x->interval[1])){
        if(x->left!=nil && x->left->max>=i[0]){
            x=x->left;
        }
        else x=x->right;
    }
    return x; 
}