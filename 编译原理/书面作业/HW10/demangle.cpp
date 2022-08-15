class A
{
public:
    A(int a) :m_a(a) {}
    int getMa() { return m_a; }
private:
    int m_a;
};
 
class B :public A
{
public:
    B(int a, int b) :A(a), m_b(b) {}
private:
    int m_b;
};
 
class C :public A
{
public:
    C(int a, int c) :A(a), m_c(c) {}
private:
    int m_c;
};
 
class D :public B, public C
{
public:
    D(int a, int b, int c, int d) :B(a+1, b), C(a, c), m_d(d) {}
    void func(){
    /*
        cout<<B::getMa()<<endl;
        cout<<C::getMa()<<endl;*/
    }
private:
    int m_d;
};


int main(){
    D d=D(1,2,3,4);
    d.B::getMa();
    d.func();
}