class A {
    virtual void FuncA() {} 
    int a;
};
 
 
class B : virtual public A {
    virtual void FuncA() {}     
    virtual void FuncB() {}
    int b;
};
 
 
class C : virtual public A {  
    virtual void FuncA() {} 
    virtual void FuncC() {}
    int c;
};
 
 
class D : public B, public C{
    void FuncA() {} 
    void FuncB() {}
    void FuncC() {}
    int d;
};
 
 
int main() {
    A a;
    D d;
    return 0;
}