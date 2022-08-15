import numpy as np
import matplotlib.pyplot as plt
import math
import random


class SMO:
    def __init__(self, X, Y, C, e):
        """
        初始化构造函数，初始化训练样本，常量，容错率
        :param X: 样本特征
        :param Y: 样本分类，取 {1,-1}
        :param C: 常量
        :param e: 容错率
        """
        self.X = X
        self.Y = Y
        self.C = C
        self.tol = e
        self.N = 70#训练集样本数
        self.alphas = np.mat(np.zeros((self.N, 1)))#参数alpha对应向量
        self.b = 0#最终划分平面，参数b
        self.eCache = np.mat(np.zeros((self.N, 2)))
        self.K = np.mat(np.zeros((self.N, self.N)))#核函数对应的内积矩阵
        for i in range(self.N):
            self.K[:, i] = np.mat(np.dot(self.X,self.X[i,:].T)).T#线性核

    def calcKernel(self, i, j):
        """
        这里直接返回的之前缓存的核函数值
        :param i:
        :param j:
        :return:
        """
        return self.K[i, j]

    def calcEi(self, i):
        """
        计算 f(x_i) - y_i
        :param i: 索引
        :return: 预测值和真实值的差值
        """
        aiyi = np.multiply(self.alphas, self.Y)#70*1的type
        ki = self.K[:, i]
        #print(aiyi.T * ki.A)
        fxi = float((aiyi.T * ki.A)[0]) + self.b
        ei = fxi - float(self.Y[i])
        return ei

    def selectJ(self, i, ei):
        """
        选择 |Ei - Ej| 取最大值的时候的 j,Ej，要求 Ej 是之前迭代的，否者返回随机选取的 j,Ej
        :param i: 第一个变量
        :param ei: 第一个变量的 Ei
        :return: 第二个变量的 j,Ej
        """
        j = -1
        maxDeltaE = 0
        ej = 0
        self.eCache[i] = [1, ei]
        validEcacheList = np.nonzero(self.eCache[:, 0].A)[0]#.A表示将矩阵matrix转换为array数组
        #np.nonzero得到数组中非零元素的下标
        if len(validEcacheList) > 0:
            for k in validEcacheList:
                if k == i:
                    continue
                ek = self.calcEi(k)
                deltaE = abs(ei - ek)
                if deltaE > maxDeltaE:
                    maxDeltaE = deltaE
                    ej = ek
                    j = k
            return j, ej
        else:
            j = self.selectJRand(i, self.N)
            ej = self.calcEi(j)
            return j, ej

    def selectJRand(self, i, n):
        """
        随机选择第二个变量
        :param i:
        :param n:
        :return:
        """
        j = i
        while j == i:
            j = int(random.uniform(0, n))
        return j

    def updateEi(self, i):
        """
        更新 Ei 的缓存
        :param i:
        :return:
        """
        ei = self.calcEi(i)
        self.eCache[i] = [1, ei]

    def updateAlphaB(self, i, e=0.0001):
        """
        利用 SMO 算法优化一次 alpha_i 和 alpha_j
        :param i:
        :param e:
        :return:
        """
        ei = self.calcEi(i)
        # 选择那些在容错率之外的样本作为第一个变量
        if ((self.Y[i] * ei < -self.tol) and (self.alphas[i] < self.C)) or \
                ((self.Y[i] * ei > self.tol) and (self.alphas[i] > 0)):
            j, ej = self.selectJ(i, ei)
            aiOld = self.alphas[i].copy()
            ajOld = self.alphas[j].copy()
            if self.Y[i] != self.Y[j]:
                L = max(0, ajOld - aiOld)
                H = min(self.C, self.C + ajOld - aiOld)
            else:
                L = max(0, aiOld + ajOld - self.C)
                H = min(self.C, aiOld + ajOld)
            if L == H:
                print("L == H")
                return 0
            eta = self.calcKernel(i, i) + self.calcKernel(j, j) - 2 * self.calcKernel(i, j)
            # eta 类似于二阶导数值，只有当它 大于 0 才能取最小值
            if eta <= 0:
                print("eta <= 0")
                return 0
            # 计算 alpha_j 并截取在[H,L]之内
            self.alphas[j] = ajOld + self.Y[j] * (ei - ej) / eta
            self.alphas[j] = self.truncateAlpha(self.alphas[j], H, L)
            self.updateEi(j)
            if abs(self.alphas[j] - ajOld) < e:
                print("j not moving enough")
                return 0
            self.alphas[i] = aiOld + self.Y[i] * self.Y[j] * (ajOld - self.alphas[j])
            self.updateEi(i)

            # 更新 b 的值
            b1 = -ei - self.Y[i] * self.calcKernel(i, i) * (self.alphas[i] - aiOld) \
                 - self.Y[j] * self.calcKernel(j, i) * (self.alphas[j] - ajOld) + self.b

            b2 = -ej - self.Y[j] * self.calcKernel(i, j) * (self.alphas[i] - aiOld) \
                 - self.Y[j] * self.calcKernel(j, j) * (self.alphas[j] - ajOld) + self.b

            if 0 < self.alphas[j] < self.C:
                self.b = b1
            elif 0 < self.alphas[i] < self.C:
                self.b = b2
            else:
                self.b = (b1 + b2) / 2.0
            return 1
        else:
            return 0

    def truncateAlpha(self, aj, H, L):
        """
        对 Aj 进行截取，只能在 H L 之内
        :param aj:
        :param H:
        :param L:
        :return:
        """
        if aj > H:
            return H
        elif aj < L:
            return L
        else:
            return aj



    def train(self, maxIter):
        """
        训练样本
        :param maxIter: 最大迭代次数
        :return: 训练好的 b,alphas
        """
        iter = 0
        entireSet = True
        alphaPairsChanged = 0#在一轮while循环中做了多少次实际的alpha更新
        # 在所有值和非边界值上面来回切换选取变量
        while (iter < maxIter) and ((alphaPairsChanged > 0) or entireSet):
            alphaPairsChanged = 0
            # 遍历所有值
            if entireSet:
                for i in range(self.N):
                    alphaPairsChanged += self.updateAlphaB(i)
                    print("fullSet, iter: %d i: %d, pairs changed %d" % (iter, i, alphaPairsChanged))
                    iter += 1
            # 遍历非边界值
            else:
                # 在 alphas 中取出大于 0 小于 c 的索引值
                nonBoundIs = np.nonzero((self.alphas.A > 0) * (self.alphas.A < self.C))[0]
                for i in nonBoundIs:
                    alphaPairsChanged += self.updateAlphaB(i)
                    print("non-bound, iter: %d i: %d, pairs changed %d" % (iter, i, alphaPairsChanged))
                iter += 1
            if entireSet:
                entireSet = False
            elif alphaPairsChanged == 0:
                entireSet = True
            print("iteration number: %d" % iter)
        return self.b, self.alphas



#entry: 
if __name__ == '__main__':

    #load data
    train_data = np.load('./data/s-svm/train_data.npy')
    train_target = np.load('./data/s-svm/train_target.npy')
    test_data = np.load('./data/s-svm/test_data.npy')
    test_target = np.load('./data/s-svm/test_target.npy')

    #print("train_target:", train_target.shape,train_target)


    smo = SMO(train_data,train_target, 1, 0.01)
    b, alphas = smo.train(10000)
    # 支持向量的索引
    svIndices = np.nonzero(smo.alphas.A > 0)[0]
    print(svIndices)
    # 支持向量特征
    xSv = train_data[svIndices]
    print("xSv",xSv)

    # 支持向量分类
    ySv = train_target[svIndices]
    print("ySv",ySv)
    train_count=0
    test_count=0
    for i in range(70):
        predict = np.dot(xSv,train_data[i,:].T).T * np.multiply(ySv, smo.alphas[svIndices]) + b
        if (np.sign(predict) == np.sign(train_target[i])).any():
                train_count += 1
    print("训练集精度: ",train_count/70)


    for i in range(30):
        predict = np.dot(xSv,test_data[i,:].T).T * np.multiply(ySv, smo.alphas[svIndices]) + b
        if (np.sign(predict) == np.sign(test_target[i])).any():
                test_count += 1
    print("测试集精度: ",test_count/30)



    #画直线w1*x1+w2*x2+b=0
    w1=np.sum(np.multiply(np.multiply(ySv, smo.alphas[svIndices]),xSv[:,0]))
    w2=np.sum(np.multiply(np.multiply(ySv, smo.alphas[svIndices]),xSv[:,1]))
    print(w1)
    print(w2)
    x11 = -5
    x12=  5
    #print(x1)
    x21 = -(w1/w2)*x11 - b/w2
    x22 = -(w1/w2)*x12 - b/w2
    #print(x2[0])
    plt.plot([x11,x21],[x12,x22])

    #画数据点
    colors = ['green','blue','red']#负样例为绿色，正样例为红色
    plt.scatter(train_data[:,0], train_data[:,1], c=np.array(colors)[train_target+1], s=15, edgecolors='none')
    plt.scatter(test_data[:,0], test_data[:,1], c=np.array(colors)[test_target+1], s=15, edgecolors='none')
    plt.show()

