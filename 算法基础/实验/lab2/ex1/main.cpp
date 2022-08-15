#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include<windows.h>
#include <iomanip>


using namespace std;

//存放矩阵链的维度
vector<long long> A;

//存放代价矩阵
long long m[30][30]={0};

//存放分割点矩阵
int s[30][30]={0};


void MATRIX_CHAIN_ORDER(){
    int j=0;
    long long temp=0;
    long long q=0;
    int n=A.size()-1;
    //cout<<n<<endl;
    for(int l=2;l<n+1;l++){//矩阵链长度从2到n
        for(int i=1;i<=n-l+1;i++){
            j=i+l-1;
            m[i][j]=-1;//取负来代替无穷
            //cout<<j<<endl;
            for(int k=i;k<j;k++){
                temp=A[i-1];
                temp*=A[k];
                temp*=A[j];
                //cout<<temp;
                q=m[i][k]+m[k+1][j]+temp;
                //cout<<q<<endl;
                if(m[i][j]<0){
                    m[i][j]=q;
                    s[i][j]=k;
                }
                else if(q<m[i][j]){
                    m[i][j]=q;
                    s[i][j]=k;
                }
            }
        }
    }
    //cout<<"1"<<endl;
}

void PRINT_OPTIMAL_PARENS(int i, int j, ofstream &result_File){
    if(i==j){
        cout<<"A"<<i;
        result_File<<"A"<<i<<" ";
    }
    else{
        cout<<"(";
        result_File<<"( ";
        PRINT_OPTIMAL_PARENS(i,s[i][j],result_File);
        PRINT_OPTIMAL_PARENS(s[i][j]+1,j,result_File);
        cout<<")";
        result_File<<") ";
    }
}

int main() {
    ifstream srcFile("..\\..\\input\\2_1_input.txt",ios::in); //以文本模式打开in.txt备读
    if(!srcFile) { //打开失败
        cout << "error opening source file." << endl;
        return 1;
    }

    ofstream result_File("..\\..\\output\\result.txt");
    ofstream time_File("..\\..\\output\\time.txt");

    //clock计时参数的声明
    LARGE_INTEGER nFreq;
    LARGE_INTEGER t1;
    LARGE_INTEGER t2;
    double dt;
    double time;//记录算法运行时间，单位为us

    int n;//共有5个量级
    long long num;//临时存放从文件读入的整数
    for(int i=0;i<5;i++){//依次对每个量级的矩阵各个维度初始化并求矩阵链
        A.clear();
        memset(m,0, 30*30*sizeof(long long));
        memset(s,0,30*30*sizeof(int));
        srcFile>>n;
        for(int j=0;j<=n;j++){
            srcFile >> num;
            //cout<<num<<endl;
            A.push_back(num);
        }

        QueryPerformanceFrequency(&nFreq);
        QueryPerformanceCounter(&t1);

        MATRIX_CHAIN_ORDER();//矩阵链乘计算

        QueryPerformanceCounter(&t2);
        dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
        time=dt*1000000;

        time_File<<time<<endl;//把时间输入文件

        //输出m矩阵
        cout<<"m matrix"<<endl;
        for(int k=1;k<n+1;k++){
            for(int j=1;j<n+1;j++){
                cout<<setw(17)<<m[k][j];
            }
            cout<<endl;
        }
        //输出s矩阵
        cout<<"s matrix"<<endl;
        for(int k=1;k<n;k++){
            for(int j=2;j<n+1;j++){
                cout<<setw(4)<<s[k][j];
            }
            cout<<endl;
        }


        cout<<"multi times  "<<m[1][n]<<endl;
        result_File<<m[1][n]<<endl;
        cout<<"the best scheme:  ";
        PRINT_OPTIMAL_PARENS(1,n,result_File);
        result_File<<endl;
        cout<<endl;
        cout<<endl;
    }
    return 0;
}
