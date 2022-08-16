import torch
import torch.autograd as autograd
import matplotlib.pyplot as plt

#超参数设置
lr=0.01
samples=100#样本个数
epochs=20

#数据和标签生成
inputs=torch.rand(100,5,requires_grad=True) #(100,5)
label=[0]*40+[1]*30+[2]*30
labels=torch.LongTensor(label).view(-1,1) #(100,1)
idx = torch.randperm(labels.nelement())
#print(idx)
labels = labels.view(-1)[idx].view(labels.size())
#print(labels)

#参数初始化
W1=torch.rand(4,5,requires_grad=True)
W2=torch.rand(4,4,requires_grad=True)
W3=torch.rand(3,4,requires_grad=True)

def sigmoid(x):
    return 1.0/(1.0+torch.exp(-x))

def softmax(x):
    c=1.0/torch.sum(torch.exp(x),dim=0)
    return  c.view(-1,1) * torch.exp(x)

def CrossEntropy(label,pred):#lable: [1], pred:[3,1]
    #return torch.sum(-torch.log(torch.gather(pred,1,label).squeeze(1)))/100
    return -torch.log(pred[label[0]])

losses=[]
for epoch in range(epochs):#每个epoch对数据集进行一次迭代
    print("epoch: ",epoch)
    loss_add=0.0
    for i in range(samples):
        #forward
        x=inputs[i].view(-1,1) #(5,1)
        h1=sigmoid(torch.mm(W1,x))#(4,1)
        h2=sigmoid(torch.mm(W2,h1))#(4,1)
        y=softmax(torch.mm(W3,h2))#(3,1)
        loss=CrossEntropy(labels[i],y)#[1]
        
        #自动梯度求导
        W3_grad = autograd.grad(loss, W3, retain_graph=True)
        W2_grad = autograd.grad(loss, W2, retain_graph=True)
        W1_grad = autograd.grad(loss, W1, retain_graph=True)
        
        #手动BP
        l_s3=y.clone() #(3,1)
        for j in range(3):
            if j==labels[i][0]:
                l_s3[j][0]=l_s3[j][0]-1
            
        L_W3=torch.mm(l_s3,h2.T)  #(3,4)

        W3_l_s3_s2 = torch.mm(W3.T,l_s3) * h2 * (1-h2) #(4,1)

        L_W2 = torch.mm(W3_l_s3_s2,h1.T) #(4,4)

        W2_W3_l_s3_s2_s1 = torch.mm(W2.T,W3_l_s3_s2) * h1 * (1-h1)#(4,1)

        L_W1 = torch.mm(W2_W3_l_s3_s2_s1, x.T)#(4,5)

        #梯度下降
        W1=W1-lr*L_W1
        W2=W2-lr*L_W2
        W3=W3-lr*L_W3    
        #W1=W1-lr*W1_grad[0]
        #W2=W2-lr*W2_grad[0]
        #W3=W3-lr*W3_grad[0]
        loss_add+=loss[0].float()
        #losses.append(loss[0].detach().numpy())
    losses.append(loss_add/100)

#打印梯度以验证正确性
print("W1_grad: ",W1_grad)
print("W2_grad: ",W2_grad)
print("W3_grad: ",W3_grad)
print("L_W1: ",L_W1)
print("L_W2: ",L_W2)
print("L_W3: ",L_W3)

#绘图
x=[i for i in range(epochs)]
fig = plt.figure(figsize = (7,5))
p = plt.plot(x,losses, 'b-', label = u'loss')
plt.legend()
plt.xlabel(u'iters')
plt.ylabel(u'loss')
plt.title('loss curve')
plt.show()





