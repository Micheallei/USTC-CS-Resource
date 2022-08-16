import torch
import torch.nn as nn
from torchvision import transforms
from torchvision.datasets import MNIST
#禁止import除了torch以外的其他包，依赖这几个包已经可以完成实验了

device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")

class Mixer_Layer(nn.Module):
    def __init__(self, patch_size, hidden_dim):
        super(Mixer_Layer, self).__init__()
        ########################################################################
        #这里需要写Mixer_Layer（layernorm，mlp1，mlp2，skip_connection）
        self.patches=int(28/patch_size)**2
        self.hidden_dim=hidden_dim
        self.layernorm = nn.LayerNorm(hidden_dim)
        self.tokens_mlp_block=nn.Sequential(
            nn.Linear(self.patches,hidden_dim),
            nn.GELU(),
            nn.Linear(hidden_dim,self.patches),
        )
        self.channels_mlp_block=nn.Sequential(
            nn.Linear(hidden_dim,self.patches),
            nn.GELU(),
            nn.Linear(self.patches,hidden_dim),
        )
        ########################################################################

    def forward(self, x):
        ########################################################################
        #x : (batch_size,patches,hidden_dim)
        y=self.layernorm(x)#对每一行进行归一化，即hidden_dim维的归一化  ,(batch_size,patches,hidden_dim)
        y=y.transpose(1,2)#(batch_size,hidden_dim,patches)
        y=self.tokens_mlp_block(y) #(batch_size,hidden_dim,patches)
        y=y.transpose(1,2)#(batch_size,patches,hidden_dim)
        y=x+y #skip connection
        out=self.layernorm(y)#(batch_size,patches,hidden_dim)
        out=self.channels_mlp_block(out)#(batch_size,patches,hidden_dim)
        out=y+out #skip connection
        return out
        ########################################################################


class MLPMixer(nn.Module):
    def __init__(self, patch_size, hidden_dim, depth):
        super(MLPMixer, self).__init__()
        assert 28 % patch_size == 0, 'image_size must be divisible by patch_size'
        assert depth > 1, 'depth must be larger than 1'
        ########################################################################
        #这里写Pre-patch Fully-connected, Global average pooling, fully connected
        self.patch_size=patch_size
        self.hidden_dim=hidden_dim
        self.depth=depth
        self.per_patch = nn.Conv2d(1,hidden_dim,kernel_size=patch_size,stride=patch_size)#在forward阶段还需要把patch铺平
        self.mlp_blocks=nn.Sequential()
        for i in range(depth):
            self.mlp_blocks.add_module('{0}th Mixer_Layer'.format(i),Mixer_Layer(self.patch_size,self.hidden_dim))
        self.fc=nn.Linear(hidden_dim,10)
        ########################################################################
        

    def forward(self, data):
        ########################################################################
        #注意维度的变化
        #data: (batch_size,1, 28,28)  , 28/patch_size = C , patches=C^2
        y=self.per_patch(data) #(batch_size,hidden_dim,28/patch_size,28/patch_size)
        bs,c,h,w=y.shape
        y=y.view(bs,c,-1).transpose(1,2) #(batch_size,patches,hidden_dim)
        y=self.mlp_blocks(y)
        y=torch.mean(y,dim=1,keepdim=False) #(batch_size,hidden_dim)
        preds=self.fc(y)#(batch_size,10)
        return preds
        ########################################################################


def train(model, train_loader, optimizer, n_epochs, criterion):
    model.train()
    for epoch in range(n_epochs):
        for batch_idx, (data, target) in enumerate(train_loader):
            data, target = data.to(device), target.to(device)
            ########################################################################
            #计算loss并进行优化
            #print("data.shape：",data.shape)#torch.Size([128, 1, 28, 28])
            #print("target.shape: ",target.shape)#torch.Size([128])
            y_pred=model(data) #(batch_size,10)
            loss = criterion(y_pred, target)
            #loss_full.append(loss.item())#记录每次的损失
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()
            ########################################################################
            if batch_idx % 100 == 0:
                print('Train Epoch: {}/{} [{}/{}]\tLoss: {:.6f}'.format(
                    epoch, n_epochs, batch_idx * len(data), len(train_loader.dataset), loss.item()))


def test(model, test_loader, criterion):
    model.eval()
    test_loss = 0.
    num_correct = 0 #correct的个数
    with torch.no_grad():
        for data, target in test_loader:
            data, target = data.to(device), target.to(device)
        ########################################################################
        #需要计算测试集的loss和accuracy
            y_pred=model(data)#(batch_size,10)
            loss=criterion(y_pred, target)
            test_loss+=loss
            index=y_pred.argmax(dim=1)#获得每一个数据的预测结果
            true_false=torch.eq(index,target)
            num_correct += len(true_false[true_false==True])
        accuracy=num_correct/(len(test_loader)*128)
        test_loss = test_loss/len(test_loader)     #平均每个batch的loss
        ########################################################################
        print("Test set: Average loss: {:.4f}\t Acc {:.2f}".format(test_loss.item(), accuracy))




if __name__ == '__main__':
    n_epochs = 5
    batch_size = 128
    learning_rate = 1e-3

    transform = transforms.Compose(
        [transforms.ToTensor(),
        transforms.Normalize((0.1307,), (0.3081,))])

    trainset = MNIST(root = './data', train=True, download=True, transform=transform)
    train_loader = torch.utils.data.DataLoader(trainset, batch_size=batch_size, shuffle=True, num_workers=2, pin_memory=True)

    testset = MNIST(root = './data', train=False, download=True, transform=transform)
    test_loader = torch.utils.data.DataLoader(testset, batch_size=batch_size, shuffle=False, num_workers=2, pin_memory=True)

    
    ########################################################################
    model = MLPMixer(patch_size = 7, hidden_dim = 30, depth = 10).to(device) # 参数自己设定，其中depth必须大于1
    # 这里需要调用optimizer，criterion(交叉熵)
    criterion = nn.CrossEntropyLoss()
    optimizer = torch.optim.Adam(model.parameters(),lr=learning_rate)
    ########################################################################
    
    train(model, train_loader, optimizer, n_epochs, criterion)
    test(model, test_loader, criterion)