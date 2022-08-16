import numpy as np
import cvxopt #用于求解线性规划
from cvxopt import matrix,solvers
from process_data import load_and_process_data
from evaluation import get_micro_F1,get_macro_F1,get_acc


#根据指定类别main_class生成1/-1标签
def svm_label(labels,main_class):#(m,)
    new_label=[]
    for i in range(len(labels)):
        if labels[i]==main_class:
            new_label.append(1)
        else:
            new_label.append(-1)
    return np.array(new_label)

# 实现线性回归
class SupportVectorMachine:

    '''参数初始化 
    lr: 梯度更新的学习率
    Lambda: L2范数的系数
    epochs: 更新迭代的次数
    '''
    def __init__(self,kernel,C,Epsilon):
        self.kernel=kernel
        self.C = C
        self.Epsilon=Epsilon

    '''KERNEL用于计算两个样本x1,x2的核函数'''
    def KERNEL(self, x1, x2, kernel='Gauss', d=2, sigma=1):
        #d是多项式核的次数,sigma为Gauss核的参数
        K = 0
        if kernel == 'Gauss':
            K = np.exp(-(np.sum((x1 - x2) ** 2)) / (2 * sigma ** 2))
        elif kernel == 'Linear':
            K = np.dot(x1,x2)
        elif kernel == 'Poly':
            K = np.dot(x1,x2) ** d
        else:
            print('No support for this kernel')
        return K

    '''
    根据训练数据train_data,train_label（均为np数组）求解svm,并对test_data进行预测,返回预测分数，即svm使用符号函数sign之前的值
    train_data的shape=(train_num,train_dim),train_label的shape=(train_num,) train_num为训练数据的数目，train_dim为样本维度
    预测结果的数据类型应为np数组，shape=(test_num,1) test_num为测试数据的数目
    '''
    def fit(self,train_data,train_label,test_data):
        '''
        需要你实现的部分
        '''
        m=train_data.shape[0]#训练集数据个数
        P=[]
        for i in range(m):
            P.append([])
            for j in range(m):
                P[i].append(train_label[i]*train_label[j]*self.KERNEL(x1=train_data[i],x2=train_data[j],kernel=self.kernel))
        #print("P:",len(P)," , ",len(P[0]))
        P=matrix(np.array(P)*1.0)
        q=matrix(np.ones((m,1))*(-1))
        G=matrix(np.vstack((np.identity(m)*(-1.0),np.identity(m)*(1.0))))
        h=matrix(np.vstack((np.zeros((m,1))*1.0,np.ones((m,1))*self.C*1.0)))
        
        A=matrix(train_label.reshape(-1,1).T*1.0)
        
        b=matrix(0.0)
        sol=solvers.qp(P,q,G,h,A,b)
        
        self.alpha=np.array(sol['x'],ndmin=2)
        #print("self.alpha: ",self.alpha.shape)
        self.w=self.alpha * train_label.reshape(-1,1) #不是真正的w,因为没有加上kernel
        #print("w shape: ",self.w.shape)
        index=0
        while(self.Epsilon>self.alpha[index] or self.alpha[index]>self.C-self.Epsilon):
            #大于0且小于C的α对应的点为支持向量
            index+=1
        #计算b
        value=0
        for i in range(m):
            value+=self.w[i]*self.KERNEL(x1=train_data[index],x2=train_data[i],kernel=self.kernel)
        self.b=train_label[index]-value
    
        #预测结果，返回未通过符号函数的结果
        preds=[]
        for i in range(test_data.shape[0]):#对每一个测试数据
            f_x=0
            for j in range(m):
                f_x += self.w[j]*self.KERNEL(x1=test_data[i],x2=train_data[j],kernel=self.kernel)
            f_x += self.b
            preds.append(f_x)
        return np.array(preds).astype(float).reshape(-1,1)
        


def main():
    # 加载训练集和测试集
    Train_data,Train_label,Test_data,Test_label=load_and_process_data()
    Train_label=[label[0] for label in Train_label]
    Test_label=[label[0] for label in Test_label]
    train_data=np.array(Train_data)
    test_data=np.array(Test_data)
    test_label=np.array(Test_label).reshape(-1,1)
    #类别个数
    num_class=len(set(Train_label))


    #kernel为核函数类型，可能的类型有'Linear'/'Poly'/'Gauss'
    #C为软间隔参数；
    #Epsilon为拉格朗日乘子阈值，低于此阈值时将该乘子设置为0
    #kernel='Linear' 
    kernel='Gauss'
    C = 1
    Epsilon=10e-5
    #生成SVM分类器
    SVM=SupportVectorMachine(kernel,C,Epsilon)

    predictions = []
    #one-vs-all方法训练num_class个二分类器
    for k in range(1,num_class+1):
        #将第k类样本label置为1，其余类别置为-1
        train_label=svm_label(Train_label,k)
        # 训练模型，并得到测试集上的预测结果
        prediction=SVM.fit(train_data,train_label,test_data)
        predictions.append(prediction)
    predictions=np.array(predictions)
    #one-vs-all, 最终分类结果选择最大score对应的类别
    pred=np.argmax(predictions,axis=0)+1

    # 计算准确率Acc及多分类的F1-score
    print("Acc: "+str(get_acc(test_label,pred)))
    print("macro-F1: "+str(get_macro_F1(test_label,pred)))
    print("micro-F1: "+str(get_micro_F1(test_label,pred)))


main()
