# **中国科学技术大学计算机学院**

## **《计算机组成与原理》报告**

##### 实验题目：寄存器堆与队列

##### 学生姓名：雷雨轩

##### 学生学号：PB18111791

##### 完成日期：2020-5-8

***

### 【实验目的】

* 掌握寄存器堆（Register File）和存储器（Memory）的功能、时序及其应用；
* 熟练掌握数据通路和控制器的设计和描述方法。

***

### 【逻辑设计分析与代码】

##### 寄存器堆的设计: 

* 地址位数为5，所以有32个数据；数据位数为32；

  初始将寄存器堆所有地址存储内容全置为0；

  同步写，异步读

* 代码及注释

```Verilog
module Register_File   //32x  WIDTH寄存器堆
    #(parameter WIDTH = 32)  //数据宽度
    (input clk,                   //时钟(上升沿有效)
    input [4:0] ra0,        //读端口0地址)
    output [WIDTH-1:0] rd0, //读端口0 数据
    input [4:0] ra1,        //读端口1 地址
    output [WIDTH-1:0] rd1, //读端口1 数据
    input [4:0] wa,         //写端口地址
    input we,               //写使能，高电平有效
    input [WIDTH-1:0] wd    //写端口数据
    );

reg [WIDTH-1:0] registers[0:31];//32个数据，每个数据宽度为32位
integer i;

initial//初始化
begin
    for(i=0;i<32;i=i+1) registers[i] <= 0;
end

assign rd0 = registers[ra0];    //异步读
assign rd1 = registers[ra1];
always@(posedge clk)
begin
    if(we)
        registers[wa] <= wd;//同步写
end

endmodule
```

##### 存储器的设计

| RAM类型  | 官方解释                                                     | 数据读写接口数量 | 地址接口数量 |
| -------- | ------------------------------------------------------------ | ---------------- | ------------ |
| 单端口   | 允许通过一个端口对存储进行读写访问                           | 1                | 1            |
| 伪双端口 | 提供了两个端口A和B，通过端口A进行写访问，通过端口B进行读访问。 | 1                | 2            |
| 双端口   | 提供两个端口A和B，这两个端口都可以对存储进行读写访问         | 2                | 2            |

利用IP核例化生成16x8位分布式单端口ram和块式单端口ram，具体过程与讲义一致，不再赘述

##### FIFO队列的设计

* 考虑设计循环队列，设置头指针fifo_front,尾指针fifo_end；具体情况如图

  ![](D:\科大\大二下\计组实验\lab2\图片\循环队列.png)

* 数据通路

  用16*8位分布式单端口ram以及两个取边沿模块实现了数据通路，但由于数据通路东西很少，很多是逻辑上的信号控制，所以只做了简单的示意

  ![](D:\科大\大二下\计组实验\lab2\图片\数据通路.png)

* 控制器状态图及特殊情况考虑

  考虑到取边沿信号的延后，直接用组合逻辑实现入队出队操作是不现实的，有延迟。所以采用状态机，分几种状态: 	init：初态及无出队入队信号态；fifo_out:仅出队信号有效态；fifo_in：仅入队信号有效态；fifo_out1：入队出队信号同时有效态(此时优先处理出队操作); fifo_in1：fifo_out1的下一个状态，用于处理入队操作

  p1和p2分别是入队和出队的取边沿信号,状态机如下

  ![](D:\科大\大二下\计组实验\lab2\图片\状态机.png)

  由于同一个p信号有效一次后至少隔一个周期后才会第二次有效，所以考虑几种裴p1,p2有效的特殊情况如上图，并据此写出代码并仿真

  注意到具体操作应该是: 当p信号变化时，根据state和p信号设置相应next_state，在下一个时钟上升沿state变为刚才设置的next_state,此时根据state的值进行入队或出队参数设置(addr,fifo_front,fifo_end等)，由于分布式ram是异步读，同步写，所以在当前周期出队操作即可完成，即可以从dout读出数据。而入队操作则要在下一时钟上升沿时写入。

* 代码及注释

  ```Verilog
  module fifo
      (input clk,rst, //时钟(上升沿有效),异步复位（高电平有效）
      input [7:0] din, //入队列数据
      input en_in,    //入队列使能，高电平有效
      input en_out,   //出队列使能，高电平有效
      output [7:0] dout, //出队列数据
      output [4:0] count      //队列数据计数
      );
  //状态机5个状态
  parameter INIT=3'b000,FIFO_OUT=3'b001;
  parameter FIFO_IN=3'b010,FIFO_OUT1=3'b011,FIFO_IN1=3'b100;
  reg [3:0] fifo_front,fifo_end;//循环fifo的队头，队尾指针
  reg [2:0] state,next_state;
  
  //模块参数
  wire p1,p2;
  reg [3:0] addr;
  reg we;
  reg [4:0] count1;
  
  
  distributed_single_ram  ram1
      (.clk(clk),
      .we(we),
      .a(addr),
      .d(din),
      .spo(dout));
  take_edge c1        //入队使能信号的取边沿
      (.clk(clk),
      .rst(rst),
      .y(en_in),
      .p(p1));
  take_edge c2        //出队使能信号的取边沿
      (.clk(clk),
      .rst(rst),
      .y(en_out),
      .p(p2));
  
  assign count=count1;
  
  always@(posedge clk,posedge rst)
  begin
      if(rst) state<=INIT;
      else state<=next_state;
  end
  
  //next_state logic
  always@(*)
  begin
      next_state=INIT;
      case(state)
          INIT:begin
              if(p1&&p2) next_state=FIFO_OUT1;
              else if(p1) next_state=FIFO_IN;
              else if(p2) next_state=FIFO_OUT;
          end
          FIFO_OUT:begin
              if(p1) next_state=FIFO_IN;
          end
          FIFO_IN:begin
              if(p2) next_state=FIFO_OUT;
          end
          FIFO_OUT1:next_state=FIFO_IN1;
          FIFO_IN1:begin
              if(p1&&p2) next_state=FIFO_OUT1;
              else if(p1) next_state=FIFO_IN;
              else if(p2) next_state=FIFO_OUT;
          end
          default:next_state=INIT;
      endcase
  end
  
  //output logic
  always@(*)
  begin
      if(rst)//复位信号有效时，队列清空
      begin
          count1<=0;
          fifo_end<=0;
          fifo_front<=0;
      end
      else 
      begin
          we=0;
          if((state==FIFO_OUT)||(state==FIFO_OUT1)) //出队信号有效时，若队列不为空，则设置addr和队列头指针及计数count
          begin
              //flag<=1;
              if(count1)
              begin
                  //we=0;
                  addr=fifo_front;
                  fifo_front=(fifo_front+1)%16;
                  count1=count1-1;
              end
          end
          else if((state==FIFO_IN)||(state==FIFO_IN1))//入队信号有效时，若队列不满，则设置addr和队列尾指针及计数count
          begin
              if(count1!=16)
              begin
                  addr=fifo_end;
                  we=1;
                  fifo_end=(fifo_end+1)%16;
                  count1=count1+1;
              end
          end
      end
  end
  endmodule
  ```

  取边沿电路:状态图参见lab2ppt

  ```Verilog
  module take_edge
      (input clk,rst,     //时钟(上升沿有效),异步复位（高电平有效）
      input y,            //入队使能或者出队使能信号
      output p           //输出的一周期信号
      );
  
  //状态机的三个状态
  parameter S0=2'b00,S1=2'b01,S2=2'b10;
  reg [1:0] state,next_state;
  //output logic
  assign p=(state==S1);
  
  //state logic
  always@(posedge clk , posedge rst)
  begin
      if(rst) state<=S0;
      else state<=next_state;
  end
  
  //next_state logic
  always@(*)
  begin
      next_state=state;//默认赋值
      case(state)
          S0:if(y) next_state=S1;
          S1:begin
              if(y) next_state=S2;
              else next_state=S0;
          end
          S2:if(!y) next_state=S0;
          default:next_state=S0;
      endcase
  end
  endmodule
  ```

  

***

### 【仿真结果及分析】

#### 寄存器堆功能仿真

* 仿真代码

  ```Verilog
  `timescale 1ns / 100ps
  
  module Register_File_tb;
      reg [4:0] ra0,ra1,wa;//读端口0,1地址,写端口地址
      wire [31:0] rd0,rd1;//读端口0,1数据
      reg [31:0] wd;      //写端口数据
      reg we;//写使能，高电平有效
      reg clk;
  parameter PERIOD = 4;//时钟周期长度
  parameter CYCLE = 200;		//时钟个数
      Register_File #() RISFILE(.ra0(ra0), .rd0(rd0), .ra1(ra1), .rd1(rd1), .wa(wa), .we(we), .wd(wd), .clk(clk));
      
  initial
  begin
      clk = 0;
      ra0=0;ra1=0;we=0;wa=0;wd=0;
      repeat (2 * CYCLE)
          #(PERIOD/2) clk = ~clk;
      $finish;
  end
   
  initial 
  begin
  #(PERIOD); we=1;
  #(PERIOD*3); wa=5'b00000; wd=32'h00001111;ra0=5'b00000; ra1=4'b00000;
  #(PERIOD*3); wa=5'b00001; wd=32'h00111101;ra0=5'b00001; ra1=5'b00010;
  #(PERIOD*3); wa=5'b00010; wd=32'h00101101;ra0=5'b00010;
  #(PERIOD*3); wa=5'b00011; wd=32'h00111100;ra0=5'b00000; ra1=5'b00100;
  #(PERIOD*3); wa=5'b00100; wd=32'h00000001;ra0=5'b00011; ra1=5'b00010;
  #(PERIOD*3); wa=5'b11101; wd=32'h10000000;ra0=5'b00100; ra1=5'b11101;
  
  #(PERIOD*3); wa=5'b00011; wd=32'h00111100;ra0=5'b00011; ra1=5'b00100;
  #(PERIOD*3); wa=5'b00100; wd=32'h00000001;ra0=5'b00011; ra1=5'b00010;
  #(PERIOD*3); wa=5'b11101; wd=32'h10000000;ra0=5'b00100; ra1=5'b11101;
  #(PERIOD/2);we=0;
  #(PERIOD*3); wa=5'b00000; wd=32'h10000000;ra0=5'b00000; ra1=5'b11101;
  #(PERIOD*3); wa=5'b00010; wd=32'h11110000;ra0=5'b00000; ra1=5'b00010;
  end 
  endmodule
  ```

* 仿真结果及分析:

  对比代码和图分析，在16ns时，wa，wd，we已就位，当时钟上升沿来临时(18ns),5‘b00000位置被写入值32'h00001111,与此同时，两个读地址ra0,ra1均为5'00000,所以其读数据rd0,rd1也在此刻变为32'h00001111

  此后，再看28ns时刻，此时ra0变为5'b00001,ra1变为5'b00010,wa为5’b00001,由于未到时钟上升沿，所以wd并未写入寄存器堆，所以rd0仍未初始化的值0；当30ns时钟上升沿到来时，wd被写入，所以rd0变为32'h00111101

![](D:\科大\大二下\计组实验\lab2\图片\寄存器堆仿真1.JPG)

​	又由下图，当we信号变为0时，此时不能写入；由图像，wa=5'b00000,wd为32‘h10000000,在134ns时钟上升	沿到来时，可以看到ra0地址5'b00000，但数据rd0仍为32'h00001111,说明此时写无效。

![](D:\科大\大二下\计组实验\lab2\图片\寄存器堆仿真2.JPG)

#### 存储器功能仿真

##### 分布式单端口RAM

* 仿真代码

  ```Verilog
  `timescale 1ns / 100ps
  
  module distributed_ram_tb;
      reg [3:0] a;//读写端口地址
      reg [7:0] d;//写端口数据
      wire [7:0] spo;      //读端口数据
      reg we;//写使能，高电平有效
      reg clk;
  parameter PERIOD = 4;//时钟周期长度
  parameter CYCLE = 200;		//时钟个数
      distributed_single_ram  ram1(.a(a), .d(d), .clk(clk), .we(we), .spo(spo));
      
  initial
  begin
      clk = 0;
      a=0;d=0;we=0;
      repeat (2 * CYCLE)
          #(PERIOD/2) clk = ~clk;
      $finish;
  end
   
  initial 
  begin
  #(PERIOD); we=1;
  #(PERIOD*3); a=4'b0000; d=8'b00001111;
  #(PERIOD*3); a=4'b0001; d=8'b00111101;
  #(PERIOD); we=0;
  #(PERIOD*3); a=4'b1111; d=8'b00101101;
  #(PERIOD*3); a=4'b0101; 
  #(PERIOD*3); a=4'b0000;
  #(PERIOD); we=1;
  #(PERIOD*3); a=4'b1111; d=8'b10000000;
  #(PERIOD*3); a=4'b0011; d=8'b00111100;
  end 
  endmodule
  ```

* 仿真结果及分析

  coe文件

  ```
  memory_initialization_radix = 16;
  memory_initialization_vector=
  00,01,02,03,04,15,1a,1f,20,32,43,54,69,7B,AD,FF
  ```

  ![](D:\科大\大二下\计组实验\lab2\图片\分布式ram.JPG)

  由仿真结果可知，在18ns时，时钟上升沿到来，此时we有效，所以a所指的地址4'b0000,d数据为8'h0f,所以spo输出为8'h0f(说明分布式ram是write first)；在30ns时钟上升沿到来前，spo数据为8'h01(图中那个很短的一段，无数据显示)，这是由coe文件初始化到地址4'b0001的数据。30ns时钟上升沿到来，spo改变为d的数据，即8'h3d; 当we为0时，可以看到时钟上升沿到来时(如46ns)，此时a=4'b1111,d=8'h2d,spo=8'hff,说明d数据并未被写入，spo仍是读取的ram内存被初始化的置8'hff. 

  且可以发现，分布式ram是异步读，也就是说spo只取决于a地址值，而与clk无关；同步写

##### 块式单端口ram（Write first）

coe文件同分布式ram

![](D:\科大\大二下\计组实验\lab2\图片\块式ram1.JPG)

由图像可知，由于勾选了primitives output register,所以ena=1，wea=0时，当addra变为4'hc时(16ns),要在此刻开始遇到的第二次时钟上升沿才会把数据8'h69读到dout。当wea=1，且ena=1时，可见在34ns到38ns期间，dout仍为8’had, 而实际此时地址4‘he应该已经被写入dina数据8'h3d(每次时钟上升沿写入)，但dout变为8'h3d仍然是在遇到两个时钟上升沿之后。

![](D:\科大\大二下\计组实验\lab2\图片\块式ram2.JPG)

而当块式ram不勾选primitives output register时，此时可发现，addra变化后遇到的第一个时钟上升沿即可把数据送到douta

所以块式ram是同步读，同步写

#### FIFO队列功能仿真

* 仿真代码

  ```Verilog
  `timescale 1ns / 100ps
  
  module fifo_tb;
      reg clk, rst;
      reg [7:0] din;
      reg en_in,en_out;
      wire [7:0] dout;
      wire [4:0] count;
  
       
      parameter PERIOD = 4;//时钟周期长度
      parameter CYCLE = 200;		//时钟个数
      fifo  fifo1(.clk(clk), .rst(rst), 
              .din(din), .en_in(en_in), .en_out(en_out),
              .dout(dout),.count(count));
      
      initial
      begin
          clk = 0;en_in=0;en_out=0;din=0;
          repeat (2 * CYCLE)
          	#(PERIOD/2) clk = ~clk;
          $finish;
      end
      
      initial
      begin
      rst = 1;
      #(PERIOD*1/2) rst = 0;
      /*
      #(PERIOD*14) rst = 1;
      #PERIOD rst = 0;
      
      #(PERIOD*14) rst = 1;
      #PERIOD rst = 0;  
      */  
      end
      
      initial 
      begin
      #(PERIOD); en_out=1;
      #(PERIOD); en_in=1;din=8'hff;
      #(PERIOD*2); en_in=0; 
      #(PERIOD); en_in=1;din=8'hdd;
      #(PERIOD*3); en_in=0;en_out=1;
      #(PERIOD*2); en_out=0;
      #(PERIOD*2); en_in=1;en_out=1;din=8'h11;
      #(PERIOD); en_in=0;en_out=0;
      #(PERIOD*3); en_in=1;din=8'hee;
      #(PERIOD);  en_out=1;en_in=0;
      #(PERIOD*3);en_in=1;din=8'h01;
      #(PERIOD) en_in=0;
      #(PERIOD*3);en_in=1;din=8'h02;
      #(PERIOD) en_in=0;
      #(PERIOD*3);en_in=1;din=8'h03;
      #(PERIOD) en_in=0;
      #(PERIOD*3);en_in=1;din=8'h04;
      #(PERIOD) en_in=0;
      #(PERIOD*3);en_in=1;din=8'h05;
      #(PERIOD) en_in=0;
      #(PERIOD*3);en_in=1;din=8'h06;
      #(PERIOD) en_in=0;
      #(PERIOD*3);en_in=1;din=8'h07;
      #(PERIOD) en_in=0;
      #(PERIOD*3);en_in=1;din=8'h08;
      #(PERIOD) en_in=0;
      #(PERIOD*3);en_in=1;din=8'h09;
      #(PERIOD) en_in=0;
      #(PERIOD*3);en_in=1;din=8'h0a;
      #(PERIOD) en_in=0;
      #(PERIOD*3);en_in=1;din=8'h0b;
      #(PERIOD) en_in=0;
      #(PERIOD*3);en_in=1;din=8'h0c;
      #(PERIOD) en_in=0;
      #(PERIOD*3);en_in=1;din=8'h0d;
      #(PERIOD) en_in=0;
      #(PERIOD*3);en_in=1;din=8'h0e;
      #(PERIOD) en_in=0;
      #(PERIOD*3);en_in=1;din=8'h0f;
      #(PERIOD) en_in=0;
      #(PERIOD*3);en_in=1;din=8'h10;
      #(PERIOD) en_in=0;
      end 
  endmodule
  ```

* 仿真结果及分析

  ![](D:\科大\大二下\计组实验\lab2\图片\fifo仿真1.JPG)

  如图，根据仿真代码，在一开始队列为空时，en_out=1，此时状态机虽变化，但并无操作，addr仍是x态。在10ns时，p1信号有效，此时可看到next_state被设置为2(FIFO_IN),下一时钟上升沿，state变为FIFO_IN,此时addr置为0(队头)，然后din数据8'hff将在下一次时钟上升沿写入。再看50ns时，p1,p2同时有效，此时根据状态机，先处理出队操作，即下一时钟上升沿

  state变为3(FIFO_OUT1)，此时dout为8'hh，为队头出队数据；下一周期state变为4(FIFO_IN1),在下一次时钟上升沿将写入数据8'h11(由62ns时dout为8'h11知成立)。

  ![](D:\科大\大二下\计组实验\lab2\图片\fifo仿真2.JPG)

  后序仿真代码，将多个数据依次入队，可以看到，当count等于16(5'h10)时，此后入队有效信号p1来时，不再有we写信号有效以及count计数增加了。

### 【下载结果及分析】

暂无

***

### 【思考题】

* 如何利用寄存器堆和适当电路设计实现可变个数的数据排序电路？

  以2<=n<=16为例根据数据个数(n)初始化寄存器堆(从低地址到高地址)，然后设置n为最大值16的数据通路，并在next_state的逻辑里对n值做相应判断后再跳转，即可实现可变个数的数据排序电路

***

### 【实验总结】

通过本次实验，更深入的体会到了数据通路和状态机在verilog代码中的精华之处，对高效而准确地构造状态机更加游刃有余

本次实验难度重点集中在仿真代码的设计以及fifo队列的设计上。

仿真代码设计是一门学问，第一次实验老师给出示例时显得比较容易。但是本次实验完全自己考虑fifo或寄存器堆，ram等会出现的情况而据此在仿真代码中赋相应值及根据仿真图像及时调整代码，整个过程对自己是一种锻炼。

fifo设计上，再次感受到了状态机的美妙之处，一开始想的是用简单时序逻辑搞定，但发现信号有延迟，最后通过设计状态机才得以解决。

***

### 【意见建议】

对于本次实验整体较为满意，实验完成后自我感觉所达到的效果与实验目的基本一致，感觉锻炼挺大但整体也不会感觉压力很大

不过实验文档里可能对于某些题目(比如fifo设计或者ram仿真)描述的不够详细，导致一些细节不是很明确(比如处理入队出队信号同时有效时 要以什么方式处理)