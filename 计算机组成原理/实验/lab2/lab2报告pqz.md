# 《计算机组成原理》lab2-寄存器堆与队列实验报告

##### 姓名：裴启智

##### 学号：PB18111793

##### 日期：2020.5.4

## 【实验目标】

* 掌握寄存器堆（Register File）和存储器（Memory）的功能、时序及其应用
* 熟练掌握数据通路和控制器的设计和描述方法

## 【逻辑分析与代码】

### 寄存器堆

* 通过使用`reg [WIDTH - 1 : 0] mem [0 : 31]`创建寄存器堆，并使用`readmemh`系统任务进行初始化

* 使用`assign`实现异步的读取，使用`always @(posedge clk)`块处理同步的写入

* RTL分析如下

  ![image-20200514103644732](C:\Users\dell\AppData\Roaming\Typora\typora-user-images\image-20200514103644732.png)

* 设计代码：

  ```verilog
  module register_file
      #(parameter WIDTH = 32)
      (input clk,
      input [4 : 0] ra0,
      output [WIDTH - 1 : 0] rd0,
      input [4 : 0] ra1,
      output [WIDTH - 1 : 0] rd1,
      input [4 : 0] wa,
      input we,
      input [WIDTH - 1 : 0] wd);
  
  //异步读取，同步写入
  
  reg [WIDTH - 1 : 0] mem [0 : 31]; 
  
  initial
     $readmemh("G:/vivadoprogram/lab2_regfile/init.vec",mem);
  
  assign rd0 = mem[ra0];
  assign rd1 = mem[ra1];
  
  always @(posedge clk)
  begin
      if(we)
      begin
          mem[wa] <= wd;
      end
  end
  
  endmodule
  ```

### 存储器

* 通过例化IP核实现，并使用coe文件对IP核进行初始化

* **对比**分布式和块式的RAM可知

  * 分布式没有使能端口，只要输入地址就会读出对应的数据，而块式的端口更多，且具有使能端口

  * 分布式RAM为异步读取，同步写入，块式RAM为同步读写，故分布式的延迟要明显低于块式

* 功能仿真见仿真部分

### FIFO队列

* 利用结构化方式描述数据通路，两段式FSM描述控制器，设有初始化状态（FIRST）、输入状态（IN）、输出状态（OUT），还有两个状态（IN_MT,OUT_MT)用于处理收到读写信号时的情况，采用先写后读策略，保证队列为空的时候也有数据读出

* 数据通路

  ![image-20200514104932313](C:\Users\dell\AppData\Roaming\Typora\typora-user-images\image-20200514104932313.png)

* 控制器

  ![image-20200508151718100](C:\Users\dell\AppData\Roaming\Typora\typora-user-images\image-20200508151718100.png)

* 设计代码（包含取边沿电路）

  ```Verilog
  module Take_Edge
      (input clk,
      input rst,
      input y,
      output p
      );
  
  reg [1 : 0] state;
  reg [1 : 0] next_state;
  
  parameter S0 = 2'b00, S1 = 2'b01, S2 = 2'b10;
  
  assign p = (state == S1);
  
  always @(posedge clk, posedge rst)
  begin
      if(rst) state <= S0;
      else    state <= next_state;
  end
  
  always @(*)
  begin
      next_state = state;
      case (state)
          S0 : 
          begin
              if(y)   next_state = S1;
              else    next_state = S0;
          end
          S1 :
          begin
              if(y)   next_state = S2;
              else    next_state = S0;
          end
          S2 : 
          begin
              if(y)   next_state = S2;
              else    next_state = S0;
          end
          default :   next_state = S0;
      endcase
  end
  endmodule
  
  
  module fifo
      (input clk, rst,        
      input [7 : 0] din,      //入队数据
      input en_in,            //写信号，入队使能，高电平有效
      input en_out,           //读信号，出队使能，高电平有效
      output [7 : 0] dout,    //出队列数据
      output [4 : 0] count    //当前队列数据个数
      );
  
  parameter FIRST = 3'b000, IN = 3'b001 , OUT = 3'b010, IN_MT = 3'b011, OUT_MT = 3'b100;
  wire en_in1, en_out1;
  wire empty, full;
  reg we;
  reg [4 : 0] cnt;
  reg [3 : 0] addr;//作为M3的输入
  reg [3 : 0] w_p, r_p;//入队、出队指针
  reg [2 : 0] curr_state, next_state;
  Take_Edge T1(.clk(clk), .rst(rst), .y(en_in), .p(en_in1));
  Take_Edge T2(.clk(clk), .rst(rst), .y(en_out), .p(en_out1));
  dist_mem_gen_1 M3(.clk(clk), .we(we), .a(addr), .d(din), .spo(dout));//将wire型dout直接输出
  
  assign empty = (cnt == 0) ? 1'b1 : 1'b0;
  assign full = (cnt == 5'b10000) ? 1'b1 : 1'b0;
  
  always @(posedge clk, posedge rst)
  begin
      if(rst) curr_state <= FIRST;
      else    curr_state <= next_state;
  end
  
  always @(*)
  begin
      next_state = FIRST;
      case(curr_state)
          FIRST:
          begin
              if(en_in1 && en_out1)  next_state = IN_MT;//同时读写时，采用先写后读的处理，这样在队列为空的时候也可以读出数据
              else if(en_in1)  next_state = IN;
              else if(en_out1)    next_state = OUT;
              
          end
          IN:
          begin
              if(en_out1) next_state = OUT;
          end
          OUT:
          begin
              if(en_in1)  next_state = IN;
          end
          IN_MT:
          begin
              next_state = OUT_MT;
          end
          OUT_MT:
          begin
              if(en_in1 && en_out1)  next_state = IN_MT;
              else if(en_in1)  next_state = IN;
              else if(en_out1)    next_state = OUT;
          end
          default:    next_state = FIRST;
      endcase
  end
  
  always @(*)
  begin
      if(rst)
      begin
          w_p = 0;
          r_p = 0;
          cnt = 0;
      end
      else
      begin
          we = 0;
          if((curr_state == IN) || (curr_state == IN_MT))
          begin
              if(!full)
              begin               
                  addr = w_p;
                  we = 1;
                  w_p = (w_p + 1) % 16;
                  cnt = cnt + 1;
              end
          end
          else if((curr_state == OUT) || (curr_state == OUT_MT))
          begin
              if(cnt)
              begin
                  addr = r_p;
                  r_p = (r_p + 1) % 16;
                  cnt = cnt - 1;
              end
          end
      end
  end
  
  assign count = cnt;
  endmodule
  ```

## 【仿真波形与结果分析】

### 寄存器堆仿真

* 仿真代码如下

  ```Verilog
  module reg_file_tb;
      reg clk;
      reg [4 : 0] ra0, ra1;
      reg [4 : 0] wa;
      reg we;
      reg [31 : 0] wd;
      wire [31 : 0] rd0, rd1;
  
  parameter PERIOD = 10, CYCLE = 20;
  parameter ASY_TIME = 3;
  
  
      register_file #(32) REG_FILE(.clk(clk), .ra0(ra0), .rd0(rd0), .ra1(ra1), .rd1(rd1), .wa(wa), .we(we), .wd(wd));
  
      initial
      begin
          clk = 0;
          repeat(2 * CYCLE)
              #(PERIOD/2) clk = ~clk;
          $finish;
      end
  
      initial 
      begin
          ra0 = 5'b00000;//rd0 = 00000000
          ra1 = 5'b11111;//rd1 = aaaaaaaa
      #(ASY_TIME * 4)//异步读
          ra1 = 5'b00110;//rd1 = 00001234
          #(PERIOD * 2)//在非时钟边沿写，发现写是同步的,并在后续读取出来，检验结果
          we = 1'b1;
          wd = 32'habcd_1234;
          wa = 5'b1_1111;
      #(ASY_TIME)
          ra0 = 5'b11111;//ra1 = abcd1234
      end
  endmodule
  ```

* init.vec文件如下

  ```verilog
  00000000
  00000001
  00000000
  00000001
  00000000
  00000001
  00001234
  00000001
  00000000
  00000001
  00000000
  00000001
  00000000
  00000001
  00000000
  00000001
  00000000
  00000001
  00000000
  00000001
  00000000
  00000001
  00000000
  00000001
  00000000
  00000001
  00000000
  00000001
  00000000
  00000001
  00000000
  aaaaaaaa
  ```

* 仿真结果如下

  ![image-20200508152030020](C:\Users\dell\AppData\Roaming\Typora\typora-user-images\image-20200508152030020.png)
  
  可见输出结果和寄存器堆中的内容一致

### 存储器仿真

#### 分布式单端口RAM

* 仿真代码

```Verilog
module ram_tb;
    reg clk;
    reg [3 : 0] a;
    reg we;
    reg [7 : 0] d;
    wire [7 : 0] spo;

parameter PERIOD = 10, CYCLE = 20;
parameter ASY_TIME = 3;

    dist_mem_gen_0 M0(.clk(clk), .we(we), .a(a), .d(d), .spo(spo));

    initial
    begin
        clk = 0;
        repeat(2 * CYCLE)
            #(PERIOD/2) clk = ~clk;
        $finish;
    end

    initial 
    begin
        a = 4'b0010;
    #(ASY_TIME * 4)//异步读,12ns
        a = 4'b1111;
    #(PERIOD * 2)//在非时钟边沿写，发现写是同步的,32ns
        d = 8'h9f;
        a = 4'b0001;
        we = 1'b1;
    #(PERIOD)//42ns
        we = 1'b0;
    #(PERIOD * 2)
        d = 8'ha0;
        a = 4'b0010;
        we = 1'b1;
    #(PERIOD * 2 + ASY_TIME)
        we = 1'b0;
        a = 4'b0001;
    end
endmodule
```

* coe文件如下

  ```verilog
  memory_initialization_radix = 16;
  memory_initialization_vector =
  a1,b2,c3,d4,e5,f6,11,22,
  33,44,55,66,15,68,7a,66;
  ```

* 仿真结果

  ![image-20200508154800115](C:\Users\dell\AppData\Roaming\Typora\typora-user-images\image-20200508154800115.png)
  
  可见输出结果与RAM中的内容一致

#### 块式单端口RAM

* 仿真代码

  ```Verilog
  module ram_block_tb;
      reg clka;
      reg [3 : 0] addra;
      reg ena,wea;
      reg [7 : 0] dina;
      wire [7 : 0] douta;
  
  parameter PERIOD = 10, CYCLE = 20;
  parameter ASY_TIME = 3;
  
      blk_mem_gen_0 M1(.clka(clka), .addra(addra), .dina(dina), .ena(ena), .wea(wea), .douta(douta));
      initial
      begin
          clka = 0;
          repeat(2 * CYCLE)
              #(PERIOD/2) clka = ~clka;
          $finish;
      end
  
      initial 
      begin
          ena = 1'b0;
      #(PERIOD)//10ns
          ena = 1'b1;
          addra = 4'b0010;
          #(ASY_TIME * 4)//同步读,22ns
          addra = 4'b1111;
      #(PERIOD * 2)//在非时钟边沿写，发现写是同步的,42ns(但由于RAM本身的延迟，使得到下下次上升沿才读出数据）
          dina = 8'h9f;
          addra = 4'b0001;
          wea  = 1'b1;
      #(PERIOD)//52ns
          wea = 1'b1;
      #(PERIOD * 2)//72ns
          dina = 8'ha0;
          addra = 4'b0010;
      #(PERIOD * 2)
          wea = 1'b0;
      #(PERIOD * 2 + ASY_TIME)//95ns
          addra = 4'b0100;
      #(PERIOD * 3)
          addra = 4'b1111;
      end
  endmodule
  ```

* coe文件如下

  ```Verilog
  memory_initialization_radix = 16;
  memory_initialization_vector =
  a1,b2,c3,d4,e5,f6,11,22,
  33,44,55,66,15,68,7a,66;
  ```

* 仿真结果

  ![image-20200508155056348](C:\Users\dell\AppData\Roaming\Typora\typora-user-images\image-20200508155056348.png)
  
  可见输出结果与RAM中的内容一致

### FIFO队列仿真

* 仿真代码

  ```Verilog
  module fifo_tb;
      reg clk, rst;
      reg en_in, en_out;
      reg [7 : 0] din;
      wire [7 : 0] dout;
      wire [4 : 0] count;
  
  parameter PERIOD = 4, CYCLE = 20;
  parameter ASY_TIME = 3;
  
  
      fifo FIFO(.clk(clk), .rst(rst), .din(din), .en_in(en_in), .en_out(en_out), .dout(dout), .count(count));
  
      initial
      begin
          clk = 0;
          repeat(4 * CYCLE)
              #(PERIOD/2) clk = ~clk;
          $finish;
      end
  
      initial 
      begin
          rst = 1'b1;
      #(PERIOD * 2)
          rst = 1'b0;
      #(PERIOD * 2)
          en_in = 1'b1;
      #(PERIOD * 2)//a1入队
          din = 8'ha1;
      #(PERIOD * 2)
          en_in = 1'b0;
      #(PERIOD * 2)
          en_in = 1'b1;
      #(PERIOD * 2)//b2入队
          din = 8'hb2;
      #(PERIOD * 2)
          en_in = 1'b0; 
      #(PERIOD * 2)
          en_in = 1'b1;
      #(PERIOD * 2)//c3入队
          din = 8'hc3;        
      #(PERIOD * 2)
          en_in  = 1'b0;
      #(PERIOD * 2)//a1出队
          en_out = 1'b1;
      #(PERIOD * 2)
          en_out = 1'b0;
      #(PERIOD * 2)//b2出队
          en_out = 1'b1;
      #(PERIOD * 2)
          en_out = 1'b0;
      #(PERIOD * 2)//c3出队
          en_out = 1'b1;
      #(PERIOD * 2)
          en_out = 1'b0;  
      #(PERIOD * 2)//55入队，并且使入队和出队信号同时有效
          en_out = 1'b1;
          en_in = 1'b1;
          din = 8'h55;   
      end
  endmodule
  ```

  

* 仿真结果

![image-20200508153230892](C:\Users\dell\AppData\Roaming\Typora\typora-user-images\image-20200508153230892.png)

可见结果符合预期，在出队信号有效后原来入队的元素依入队顺序出队。对于同时出入队列的情况，也可以读出刚入队的数据（先写后读策略）

## 【下载结果及分析】

## 【思考题】

* 将所要建立的寄存器的数量作为参数传入模块，如

  ```verilog
  module register_file
      #(parameter WIDTH = 32
        parameter NUM = ...)
  ```

  这样就可以生成可变数量的寄存器堆，有了数量（假设为n）作为参数之后，就可以进行排序了。

  设置一个计数变量和标记数组（一个多位寄存器），采用**选择排序法**（假设由大到小排列），每当时钟边沿到来时，就找出当前未被标记的所有寄存器堆中最大的数，并将其放在结果寄存器堆的1号位置，并将其对应的标记位打上标记。重复上述操作知道所有标记位都被标记（可用缩位与运算来判断），则生成done信号

## 【实验总结】

* 通过这次实验，自己对寄存器堆的操作更加熟悉，了解了寄存器堆与存储器RAM的区别和联系，并了解了如何在设计文件中通过`readmemh`等系统任务来完成寄存器组的初始化
* 对IP核的例化以及coe文件的使用更为熟悉，同时通过仿真认识到分布式存储和块式存储的相同点和不同点
* 在第三部分中自己遇到了较大的困难，刚开始没有使用状态机来实现，但这样会因为信号延迟产生一系列问题，且无法较好的处理同时读写的情况。后来在询问同学并思考后采用了状态机来实现，这样可以有效地解决了上述问题

## 【意见与建议】

* 建议放宽FIFO部分必须使用例化IP核的要求，这样会使得实现更加方便和个性化，而且各种延迟等问题也可以通过更改自己写的RAM代码来解决




