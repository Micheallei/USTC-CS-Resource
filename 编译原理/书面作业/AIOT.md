#### 思考

* java跨平台
* 联系方舟编译器
* maple engine编译执行



#### 趋势与挑战

* 微处理器的发展趋势![image-20201129210222727](D:\Typora\photos\image-20201129210222727.png)
  * 晶体管数量--摩尔定律（黄线）
  * 单核趋势（蓝线），近年开始发展缓慢
  * 频率：更早就走缓，受工艺限制
  * 能耗(黑线)：芯片能耗散的功耗已经达到了极限
* 通用计算的时代结束，进入异构计算时代
  * 处理器越来越专用、定制化
  * 对计算的并行、访存特征这个模式来定制硬件：eg：AI处理器

* 指令集发展
  * 通用CPU指令集架构的多样化和融合统一趋势
    * CISC指令集有RISC指令集互相的借鉴
    * 指令组合概念的引入
  * 指令集例子：CISC，RISC，VLIW（超长指令集）
  * 这些年发展下来，指令集架构种类的构想很多，但是目前现存商用的指令集数目少，趋于稳定
* 编译器与各种指令集架构之间发展的演进路径![image-20201129211253597](D:\Typora\photos\image-20201129211253597.png)
  * 单核优化
  * 多核优化
  * 异构优化:多核CPU已经不再是原来的通用CPU了，更转向了GPU、专用AI处理器、DSP、FPGA等，不同架构的核如何协同工作的问题。
* 后摩尔时代![image-20201129211704044](D:\Typora\photos\image-20201129211704044.png)
  * 既懂上层应用算法，又懂底层硬件模型，才能写出高质量代码，充分利用计算资源。
  * TPU：谷歌推出，针对领域定制的体系结构
  * XPU概念
  * 兼容性：
    * 二进制兼容性(太难)：直接可执行二进制文件在各个平台运行
    * 源代码兼容性(有挑战)：如英伟达GPU可通过opencl实现在不同GPU上开发。但是性能兼容性没法保证，而且软件移植成本高。
  * opencl
* 如何进行超异构时代的软件开发![image-20201129212331813](D:\Typora\photos\image-20201129212331813.png)
  * 声明式开发：所见即所得的交互式编程(实时预览等)
  * 协同式开发：多人协作/端云协同编程
    * 考虑同时开发的效率以及可能的冲突
  * 智慧化开发：AI辅助编程/AI可视化编程(比单纯代码提示更进一步)
    * 代码补全
    * 代码性能评估：如auto-tuning编译技术

#### 产业案例

* ![image-20201129212930497](D:\Typora\photos\image-20201129212930497.png)
  * super kernel优化技术：二进制层面实现算子Kernel融合调度，减少TS调度kernel的开销
  * 并行编译、ccache优化：编译构建软件的时间降低，即提高开发效率(比如一个底层小改动，可能使得全部工程需要重新编译)
  * 算子编译优化和代码生成技术
  * 总结：编译解决的问题：性能问题、开发时间(效率)问题
* ![image-20201129213255224](D:\Typora\photos\image-20201129213255224.png)
  * 关键：通过编译技术提升软件性能

#### 技术案例互动分享

* ![image-20201129213533263](D:\Typora\photos\image-20201129213533263.png)

  * AI领域主要使用python而非c：python的表达能力更好，易描述计算、业务逻辑
  * 编译器实现的功能：让芯片看懂高层语言并执行

* ![image-20201129213821238](D:\Typora\photos\image-20201129213821238.png)

  * 编译基本过程见上图，目前课内已经讲到代码生成一块

  * 解释执行、编译执行及字节码虚拟机

    * 动态类型：在python中我们是不会声明所使用对象的确切类型的。所谓的python动态类型，就是在程序运行的过程中自动决定对象的类型。

    * 编译执行：生成可执行文件(二进制格式)，性能好，速度最快

    * 解释执行：性能差，效率不高

    * 字节码：java源码-》自定义字节码-》解释执行，在执行过程中用JIT即时编译方式，把热点代码编译成机器码在硬件上直接执行，效率更高

      java虚拟机：运行字节码的机器，而非模拟系统硬件资源的PC机

* ![image-20201129215150171](D:\Typora\photos\image-20201129215150171.png)

  * 特性

    * 跨平台：独立于架构、设备、OS
    * runtime engine
    * 编程语言支持：java和c，实现了共同IR

  * ![image-20201129215222068](D:\Typora\photos\image-20201129215222068.png)

    * Maple engine使得java和c有了统一中间表示，可以统一优化，消除了跨语言通信的计算开销

      同时使得静态编译直接生成二进制可执行文件成为可能，不再依靠java虚拟机

* ![image-20201129215748293](D:\Typora\photos\image-20201129215748293.png)

  * eg：循环展开(因为跳转对处理器有额外开销)，所以考虑线性复制代码

  * 静态编译策略往往固定且单一，无法很好适应不同平台以及用户使用习惯。

  * ![image-20201129220105751](D:\Typora\photos\image-20201129220105751.png)

    一次编译肯定不可能达到最优，所以是反复迭代优化

    * 把内部编译策略、需调整的参数传给Search driver
    * profiler通过运行信息、性能指标、执行效率、访存时延等信息综合评估当前得到的编译解的性能是否高效
    * 流程：source-》excutable(可执行文件)-》在平台上执行(红色)，得到当前性能 -》profiler评估当前性能是否高效 -》综合这些信息交给search driver，在参数空间里搜索进一步的优化-》一轮结束，开始下一轮

    * NP完全问题：只能找到相对优的解

* ![image-20201129220709495](D:\Typora\photos\image-20201129220709495.png)

  * 核心问题：**如何使用特定指令来对软件应用进行优化的案例**；**反之，通过具体场景的问题来驱动指令集的设计**
  * 数据库应用的例子
    * 自旋锁使用
    * 考虑引入双寄存器原子指令，某个线程的挂起不会引起其他线程的执行

* ![image-20201129221452222](D:\Typora\photos\image-20201129221452222.png)

  * 例子：无锁共享数据结构

#### 挑战课题

* ![image-20201129221933830](D:\Typora\photos\image-20201129221933830.png)
* ![image-20201129222210710](D:\Typora\photos\image-20201129222210710.png)
* ![image-20201129222519817](D:\Typora\photos\image-20201129222519817.png)
  * 研究出一个通用的统一异构编程框架及一系列技术支持，以充分挖掘硬件潜力。面向应用统一IR能提高开发效率，提高可移植性，更易复用。