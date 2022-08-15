#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include<windows.h>
#include <iomanip>
#include <ccomplex>
#include <cmath>

#define PI       3.1415926
using namespace std;


vector<complex<double>> RECURSIVE_FFT(vector<double> &a){
    int n=a.size();
    //cout<<a[0]<<endl;
    vector<complex<double>> y = vector<complex<double>>(n);
    vector<double> a0,a1;

    complex<double> w_n {cos(2*PI/(double)n),sin(2*PI/(double)n)};
    complex<double> w {1.0,0.0};
    if(n==1){
        y[0].real(a[0]);
        y[0].imag(0.0);
        //cout<<y[0];
        return y;
    }
    for(int i=0;i<n;i+=2){
        a0.push_back(a[i]);
        a1.push_back(a[i+1]);
    }
    vector<complex<double>> y0=RECURSIVE_FFT(a0);
    vector<complex<double>> y1=RECURSIVE_FFT(a1);

    for(int k=0;k<=(n/2-1);k++){
        y[k]=y0[k]+w*y1[k];
        y[k+n/2]=y0[k]-w*y1[k];
        w=w*w_n;
    }
    return y;
}


int main() {
    ifstream srcFile("D:\\ex\\ex2\\input\\2_2_input.txt",ios::in); //以文本模式打开in.txt备读
    if(!srcFile) { //打开失败
        cout << "error opening source file." << endl;
        return 1;
    }

    ofstream result_File("D:\\ex\\ex2\\output\\result.txt");
    ofstream time_File("D:\\ex\\ex2\\output\\time.txt");

    //clock计时参数的声明
    LARGE_INTEGER nFreq;
    LARGE_INTEGER t1;
    LARGE_INTEGER t2;
    double dt;
    double time=0;//记录算法运行时间，单位为us

    int n;//共有6个量级
    double num;//临时存放从文件读入的整数

    vector<double> A;//存放每个量级下的参数a0,a1,a2,...,an
    vector<complex<double>> result;//存放每次的结果

    for(int i=0;i<6;i++){//依次对每个量级FFT初始化
        A.clear();
        srcFile>>n;
        for(int j=0;j<n;j++){
            srcFile >> num;
            //cout<<num<<endl;
            A.push_back(num);
        }

        QueryPerformanceFrequency(&nFreq);
        QueryPerformanceCounter(&t1);

        result=RECURSIVE_FFT(A);//FFT计算

        QueryPerformanceCounter(&t2);
        dt =(t2.QuadPart -t1.QuadPart)/(double)nFreq.QuadPart;
        time=dt*1000000;

        time_File<<time<<endl;//把时间输入文件

        //输出FFT结果
        //result_File<<n<<endl;
        cout<<n<<endl;
        for(int k=0;k<n;k++){
            cout<<"A(w"<<k<<"):  real: "<<result[k].real()<<"  imag: "<<result[k].imag()<<endl;
            result_File<<fixed << setprecision(4)<<result[k].real()<<" ";
        }
        result_File<<endl;

    }

    return 0;
}