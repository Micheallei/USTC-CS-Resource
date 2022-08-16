import numpy as np
import math
from collections import Counter
from process_data import load_and_process_data
from evaluation import get_micro_F1,get_macro_F1,get_acc

class NaiveBayes:
    '''参数初始化
    Pc: P(c) 每个类别c的概率分布
    Pxc: P(c|x) 每个特征的条件概率
    '''
    def __init__(self):
        self.Pc={}
        self.Pxc={}

    '''
    通过训练集计算先验概率分布p(c)和条件概率分布p(x|c)
    建议全部取log，避免相乘为0
    '''
    def fit(self,traindata,trainlabel,featuretype):
        '''
        需要你实现的部分
        '''
        for label in range(1,4):
            D_c = len(trainlabel[trainlabel==label])
            self.Pc[label]= (D_c+1)/(trainlabel.shape[0]+3)

        for i,type_attr in enumerate(featuretype):
            if type_attr==0:#离散型
                self.Pxc[i]={}#第i个属性的条件概率
                for label in range(1,4):
                    self.Pxc[i][label]={}
                    D_c = len(trainlabel[trainlabel==label])
                    data_index=np.where(trainlabel==label)
                    temp=traindata[:,i].reshape(-1,1)[data_index]#选择标签值为label的数据的对应属性那一列
                    for value in range(1,4):
                        self.Pxc[i][label][value]= (len(temp[temp==value])+1)/(D_c+3)

            elif type_attr==1:#连续型
                self.Pxc[i]={}#第i个属性的条件概率
                for label in range(1,4):
                    self.Pxc[i][label]=[]#存放 标签为label的样本在 第i个属性上取值的均值和方差
                    data_index=np.where(trainlabel==label)
                    data=traindata[:,i].reshape(-1,1)[data_index]#选择标签值为label的数据的对应属性那一列
                    #print("data: ",data.shape)
                    mean=np.sum(data,axis=0)/(data.shape[0])
                    std=np.std(data, axis=0)
                    self.Pxc[i][label].append(mean)
                    self.Pxc[i][label].append(std)


    '''
    根据先验概率分布p(c)和条件概率分布p(x|c)对新样本进行预测
    返回预测结果,预测结果的数据类型应为np数组，shape=(test_num,1) test_num为测试数据的数目
    feature_type为0-1数组，表示特征的数据类型，0表示离散型，1表示连续型
    '''
    def predict(self,features,featuretype):
        '''
        需要你实现的部分
        '''       
        pred=[]
        for num in range(features.shape[0]):#对每个测试数据进行预测
            probs=[]
            for label in range(1,4):
                prob=np.log(self.Pc[label])
                for i,type_attr in enumerate(featuretype):
                    if type_attr==0:#离散型
                        prob += np.log(self.Pxc[i][label][features[num][i]])
                    elif type_attr==1:#连续型
                        mu=self.Pxc[i][label][0]#均值
                        std=self.Pxc[i][label][1]#标准差
                        x_i=features[num][i]
                        prob += np.log( 1/(np.sqrt(2*np.pi)*std)*np.exp(-(x_i-mu)*(x_i-mu)/(2*std*std) ) )
                probs.append(prob)
            pred.append(probs.index(max(probs))+1)
        
        return np.array(pred).astype(int).reshape(-1,1)



def main():
    # 加载训练集和测试集
    train_data,train_label,test_data,test_label=load_and_process_data()
    feature_type=[0,1,1,1,1,1,1,1] #表示特征的数据类型，0表示离散型，1表示连续型

    Nayes=NaiveBayes()
    Nayes.fit(train_data,train_label,feature_type) # 在训练集上计算先验概率和条件概率

    pred=Nayes.predict(test_data,feature_type)  # 得到测试集上的预测结果

    # 计算准确率Acc及多分类的F1-score
    print("Acc: "+str(get_acc(test_label,pred)))
    print("macro-F1: "+str(get_macro_F1(test_label,pred)))
    print("micro-F1: "+str(get_micro_F1(test_label,pred)))

main()