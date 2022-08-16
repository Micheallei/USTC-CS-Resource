### Python

* eval和ast.literal_eval区别：https://blog.csdn.net/Jerry_1126/article/details/68831254

* 正负无穷：float("inf"), float("-inf")

* 格式化输出：%和format用法   https://www.cnblogs.com/fat39/p/7159881.html

* enumerate()函数 :https://www.runoob.com/python/python-func-enumerate.html

* **getattr()** 函数用于返回一个对象属性值

  ```python
  getattr(object, name[, default])
  ```

  - object -- 对象。
  - name -- 字符串，对象属性。
  - default -- 默认返回值，如果不提供该参数，在没有对应属性时，将触发 AttributeError。

* importlib.import_module()

  动态导入对应的配置文件运行

### Numpy

* np.transpose：简单来说即矩阵转置

* np.dot:

  * 向量内积：对应位置数相乘再求和
  * 矩阵乘法运算

* np.mean：对所有元素求均值

* numpy.linspace(start, stop, num=50, endpoint=True, retstep=False, dtype=None)

  在指定的间隔内返回均匀间隔的数字。

* np.reshape(m,n),返回一个m行n列的数组 

  np.reshape(-1),返回一个一维数组 

  np.reshape(-1,n),返回一个n列的数组，行数自动给定

* np.meshgrid:生成网格坐标矩阵

  https://blog.csdn.net/lllxxq141592654/article/details/81532855

* np.vstack(tup)使用：沿着竖直方向将矩阵堆叠起来

  np.hstack(tup)：沿着水平方向将数组堆叠起来。

  实例https://blog.csdn.net/u012609509/article/details/70319293

* np.ravel()和np.flatten()：多维数组降为一维

* np.sign: x>0,返回1； x=0返回0；x<0返回-1

* np.asarray: 对列表等转换为numpy数组https://www.jianshu.com/p/2303467ea896

* b=np.argmax(a)：取出a中元素最大值所对应的索引

* np.concatenate((a,b))：两个数组拼接

* np.multiply():数组和矩阵对应位置相乘，输出与相乘数组/矩阵的大小一致

* np.astype()：转换numpy数组的数据类型

* np.square():计算每个元素的平方

* np.c_是按行连接两个矩阵，就是把两矩阵左右相加，要求行数相等，类似于pandas中的merge()

  np.r_是按列连接两个矩阵，就是把两矩阵上下相加，要求列数相等，类似于pandas中的concat()

* numpy.lexsort() 用于对多个序列进行排序：https://blog.csdn.net/u013066730/article/details/96485837

  对二维数组按某行或列排序：https://www.jb51.net/article/124348.htm

* 数组索引 None：https://www.cnblogs.com/sgdd123/p/7603004.html

* **np.searchsorted(a, v, side='left', sorter=None)**

  在数组a中插入数组v（并不执行插入操作），返回一个下标列表，这个列表指明了v中对应元素应该插入在a中那个位置上
  
* numpy.argmin(a, axis=None, out=None) 给出axis方向最小值的下标

  https://blog.csdn.net/weixin_41770169/article/details/80714461

* np.where: 根据条件筛选np.array中的项

  y= np.where(x > 3)

  print(x[y])

* numpy.random.multinomial：从多项式分布中提取样本。https://blog.csdn.net/qq_15192373/article/details/90265450

* [np.argsort()](https://www.cnblogs.com/lucas-zhao/p/11697203.html):将数组排序，得到从小到大排序结果的下标序列

* np.identity() :生成单位阵

### matplotlib

* plt.contour：绘制等高线轮廓

  https://blog.csdn.net/lanchunhui/article/details/70495353 参数解释

  https://blog.csdn.net/qq_42505705/article/details/88771942代码及图示
  
* `pyplot`是一个模块，它收集了一些允许matplotlib以功能方式使用的函数。 我在这里假设`pyplot`已被导入为“`import matplotlib.pyplot as plt”`。 在这种情况下，有三个不同的命令可以删除内容：

  [`plt.cla()`清除轴](http://matplotlib.org/api/pyplot_api.html#matplotlib.pyplot.cla)，当前活动轴在当前图中。 它保持其他轴不变。

  [`plt.clf()`清除整个当前数字。](http://matplotlib.org/api/pyplot_api.html#matplotlib.pyplot.clf)与所有的轴，但离开窗口打开，这样它就可以再用在其他的 plots上了。

  [`plt.close()`关上窗户](http://matplotlib.org/api/pyplot_api.html#matplotlib.pyplot.close)，如果未另指定，则该窗口将是当前窗口。

  https://cloud.tencent.com/developer/ask/37449

### sklearn 

##### **import** svm

* `coef_`和`intercept_`都是模型参数，即为w

  coef_为w1到w4

  intercept_为w0

  https://www.jianshu.com/p/6a818b53a37e

##### Sklearn.metrics

* 无论利用机器学习算法进行回归、分类或者聚类时，**评价指标**，即检验机器学习模型效果的定量指标，都是一个不可避免且十分重要的问题

* 计算均方误差、回归的决定系数R2。
* accuracy_score(y_true,y_pre) : 精度
* https://blog.csdn.net/Yqq19950707/article/details/90169913



### Graphviz

* 绘制树
* 官方API文档 https://graphviz.readthedocs.io/en/stable/api.html#other
* 简单接口讲解 https://www.cnblogs.com/Zzbj/p/11431015.html#autoid-2-0-0



### Pandas

* pandas.iloc: 提取pandas里指定行、列的数据

  https://blog.csdn.net/W_weiying/article/details/81411257

* .as_matrix()：将表格转化为只含数据的矩阵

  现在不能用了，应该为.values
  
* pd.read_table, pd.read_csv

* 遍历dataframe方式

  * iterrows():将DataFrame迭代为(index, Series)对。即每行为一次迭代

  * **itertuples():** 将DataFrame迭代为元组。也算按行

  * **iteritems():** 将DataFrame迭代为(列名, Series)对

* dataframe.explode：将 dataframe 按照某一指定列进行展开，使得原来的每一行展开成一行或多行。( 注：该列可迭代， 例如list, tuple, set)

* read_table时参数：quoting**=**csv.QUOTE_NONE，指定quote方式为不引用。这意味着读的时候都认为内容是不被默认引用符(")包围的

### Pytorch

官网教程

pytorch入门

* 张量
* Autograd自动求导
* 神经网络
* 图像分类器
* 数据并行处理

pytorch之入门强化教程

* 数据加载和处理
* pytorch小试牛刀

pytorch之文本篇

* pytorch简介
* 使用pytorch进行深度学习
* 词向量Word_Embedding
* 序列模型和长短期记忆(LSTM)模型
* 制定动态决策和Bi-LSTM CRF
* 聊天机器人
  * 数据清洗策略
    * 删除无用标点、非字母字符、转为小写
    * 对话sentence长度限制
    * 删除频次少的单词



用法

* `nn.Linear（）`是用于设置网络中的**全连接层的**，需要注意的是全连接层的输入与输出都是二维张量

  https://blog.csdn.net/qq_42079689/article/details/102873766

* tensor.transpose :只能选择tensor中**两个维度**进行转置

  以及 tensor.permute:可以让tensor按照**指定维度顺序**（维度的个数就是该tensor的维度数）进行转置

  https://blog.csdn.net/qq_41740004/article/details/104712173

* 对tensor的view操作，类似于numpy的resize， https://blog.csdn.net/york1996/article/details/81949843

* torch.linspace：函数的作用是，返回一个一维的tensor（张量），这个张量包含了从start到end，分成steps个线段得到的向量。常用的几个变量

  > start：开始值
  >
  > end：结束值
  >
  > steps：分割的点数，默认是100
  >
  > dtype：返回值（张量）的数据类型

  https://blog.csdn.net/york1996/article/details/81671128

* ```
  torch.unsqueeze(input, dim, out=None)
  ```

  - **作用**：扩展维度
  - https://zhuanlan.zhihu.com/p/86763381
  
* torch.nn.Conv2d()

  二维卷积操作

  https://blog.csdn.net/qq_34243930/article/details/107231539
  
* torch.utils.data：https://pytorch.apachecn.org/docs/1.0/data.html

  * ```py
    class torch.utils.data.Dataset
    ```

    表示数据集的抽象类。

    所有用到的数据集都必须是其子类。这些子类都必须重写以下方法：`__len__`：定义了数据集的规模；`__getitem__`：支持0到len(self)范围内的整数索引。

  * `class torch.utils.data.DataLoader`

    数据加载器。组合数据集和采样器，并在数据集上提供单进程或多进程迭代器。

* torch.mm 或 torch.matmul：矩阵乘

  https://blog.csdn.net/u013230189/article/details/82627077
  
  eg：
  
  ```
  a=torch.randn(3,2,4)
  b=torch.randn(4,6)
  #print(torch.mm(a,b))
  print(torch.matmul(a,b).size()) [3,2,6]
  ```
  
  torch.bmm: 严格规定相乘的维度和大小：
  
  https://blog.csdn.net/foneone/article/details/103876519
  
* torch.manual_seed(number): 为CPU设置种子，生成随机数

  torch.cuda.manual_seed(number): 为GPU设置种子

  https://zhuanlan.zhihu.com/p/161575780

* nn.NLLLoss的原理：https://blog.csdn.net/Jeremy_lf/article/details/102725285

* tensor.argmax(dim**=**1)

  求tensor里最大值对应的index，dim=1求每行最大；dim=0求每一列最大；  无dim参数则求全局最大

* torch.max: 在分类问题中，通常需要使用`max()`函数对`softmax`函数的输出值进行操作，求出预测值索引，然后与标签进行比对，计算准确率

  https://www.jianshu.com/p/3ed11362b54f

* tensor.item(): 得到对应的python数值

* torch.cat: 连接操作，根据dim来按维度拼接（默认dim=0,按行拼）

  https://blog.csdn.net/xinjieyuan/article/details/105208352

* nn.Embedding(vocab_size, embedding_dim)

  ```python
  self.word_embeddings = nn.Embedding(vocab_size, embedding_dim) #定义
  
  embeds = self.word_embeddings(sentence) #前向传播里的使用
  ```

  输入sentence是句子里每个词对应在词库里的索引，且是一个Tensor类型。

  输出：每个词一个embedding_dim的向量，所以输出是N*embedding_dim维（N为句子所含词数)
  
  注意，Embedding里也是有参数，是可以前向传播后训练的
  
  * padding_idx=0: https://blog.csdn.net/weixin_40426830/article/details/108870956
  
* nn.LSTM参数详解: https://blog.csdn.net/foneone/article/details/104002372

  - dropout：除最后一层，每一层的输出都进行dropout，默认为: 0； 是一种正则化方法

    对于每个训练样本，重新调整网络并丢弃一组新的神经元

    https://lonepatient.top/2018/09/24/a-review-of-dropout-as-applied-to-rnns.html

* torch.nn.Parameter : 是继承自torch.Tensor的子类，其主要作用是作为nn.Module中的可训练参数使用。它与torch.Tensor的区别就是nn.Parameter会自动被认为是module的可训练参数，即加入到parameter()这个迭代器中去；而module中非nn.Parameter()的普通tensor是不在parameter中的。
  注意到，nn.Parameter的对象的requires_grad属性的默认值是True，即是可被训练的，这与torth.Tensor对象的默认值相反。
  在nn.Module类中，pytorch也是使用nn.Parameter来对每一个module的参数进行初始化的。

* optim.SGD(model.parameters(), lr**=**0.01, weight_decay**=**1e-4) 

  weight_decay：L2正则化

* ```python
  torch.full(size, fill_value, *, out=None, dtype=None, layout=torch.strided, device=None, requires_grad=False) → Tensor
  
  
  torch.full_like(input, fill_value, *, out=None, dtype=None, layout=torch.strided, device=None, requires_grad=False, memory_format=torch.preserve_format) → Tensor
  ```

  两个方法的作用几乎相同，就是给定一个值fill_value和一个size，创建一个矩阵元素全为fill_value的大小为size的tensor。

* seq2seq：里的torch.nn.utils.rnn.pack_padded_sequence和torch.nn.utils.rnn.pad_packed_sequence

  https://www.cnblogs.com/luckyplj/p/13376818.html

* tensor.expand函数：https://blog.csdn.net/qq_40178291/article/details/100183457

  扩展成指定的size

* nn.Module.train 和 nn.Module.eval的区别

  https://blog.csdn.net/qq_37959202/article/details/105148135

* torch.squeeze和torch.unsqueeze

  https://www.cnblogs.com/sbj123456789/p/9231571.html

* torch.gather:https://blog.csdn.net/cpluss/article/details/90260550

  收集输入的特定维度指定位置的数值，比如按索引取每一行的特定位置的值

* torch.full : 

  ```
  >>> torch.full((2, 3), 3.141592)
  tensor([[ 3.1416,  3.1416,  3.1416],
          [ 3.1416,  3.1416,  3.1416]])
  ```

* torch.utils.data里的Dataset和DataLoader： https://pytorch-cn.readthedocs.io/zh/latest/package_references/data/

* model.eval()，pytorch会自动把BN和DropOut固定住，不会取平均，而是用训练好的值。不然的话，一旦test的batch_size过小，很容易就会被BN层导致生成图片颜色失真极大；在模型测试阶段使用

  model.train() 让model变成训练模式，此时 dropout和batch normalization的操作在训练q起到防止网络过拟合的问题

  总结： model.train() 和 model.eval() 一般在模型训练和评价的时候会加上这两句，主要是针对由于model 在训练时和评价时 Batch Normalization 和 Dropout 方法模式不同；**因此，在使用PyTorch进行训练和测试时一定注意要把实例化的model指定train/eval；**

* tensor.uniform_()： tensor从均匀分布中抽样数值进行填充。

* 关于nn.ModuleDict():https://blog.csdn.net/qq_23981335/article/details/103608504

* torch.stack()  : 如图有a, b, c 三个 3x3的Tensor,如果我想把这三个tensor的最后一个维度的元素相叠加，形成一个新的tensor,输入 d=torch.stack( (a,b,c) ,dim = 2)

* state_dict():https://blog.csdn.net/bigFatCat_Tom/article/details/90722261

* torch.norm() :求范数

* torch.full() : 按指定size把一个值填充成tensor：https://blog.csdn.net/cpluss/article/details/90266827

* nn.LayerNorm() :https://blog.csdn.net/weixin_39228381/article/details/107939602
