# **中国科学技术大学计算机学院**

## **《计算机组成与原理》报告**

##### 实验题目：单周期CPU

##### 学生姓名：雷雨轩

##### 学生学号：PB18111791

##### 完成日期：2020-5-15

***

### 【实验目的】

* 理解计算机硬件的基本组成、结构和工作原理；
* 掌握数字系统的设计和调试方法；
* 熟练掌握数据通路和控制器的设计和描述方法。

***

### 【逻辑设计分析与代码】

##### CPU的设计: 

* 数据通路:（在ppt基础上加了一些数据线的名字)

  ![](D:\科大\大二下\计组实验\lab3\图片\cpu数据通路.png)

* 老师已经给出数据通路，只需要对数据线命名，再用之前lab1，lab2里已经实现过的register,Register_File,alu,mux2，以及IP核Instruction_memory，Data_memory来例化即可，例化时需要注意的就是参数不要带错了。

  控制单元的设计，考虑到单周期完成一条指令，所以控制单元直接用组合逻辑，根据instr[31:26]的内容直接为9个信号赋值即可。

  最后cpu工作机制大致是，在时钟上升沿到来时刷新pc(pc_in->pc_out)，然后异步读出instr，同时由组合逻辑产生control unit的所有信号，再赋值到各个部件，实现相应运算，最后在下一次时钟上升沿到来之前，将pc_in准备好，写回寄存器堆或data_memory的数据准备好，在下一次时钟上升沿来时，更新pc_out，也写入要写入的数据

* 代码及注释

  ```Verilog
  module cpu_one_cycle    //单周期CPU
      (input clk,         //时钟(上升沿有效)
      input rst           //异步复位，高电平有效
      );
  
  reg RegDst,Jump,Branch,MemRead,MemtoReg,MemWrite,ALUSrc,RegWrite;
  reg [1:0]ALUOp;
  //绝大多数命名与数据通路上标注一致
  wire [31:0] pc_out,pc_in;//pc寄存器的输入和输出
  wire [31:0] instr; //当前周期的指令
  wire [31:0] ALU_result;
  reg [2:0] ALU_control;
  wire m3,Zero;
  wire [31:0] mem_read_data;
  wire [31:0] w1,w2,w4,w5,Jump_address,sign_extend;
  //寄存器堆参数
  wire [4:0] write_reg;
  wire [31:0] reg_write_data,reg_read_data1,reg_read_data2;
  
  Instruction_memory mem1
      (.a(pc_out[9:2]),
      .spo(instr));
  
  Data_memory mem2
      (.a(ALU_result[9:2]),
      .d(reg_read_data2),
      .clk(clk),
      .we(MemWrite),
      .spo(mem_read_data));
  
  register #(.WIDTH(32)) PC
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(pc_in),
      .q(pc_out));
  
  Register_File #(.WIDTH(32)) Registers
      (.clk(clk),
      .ra0(instr[25:21]),
      .rd0(reg_read_data1),
      .ra1(instr[20:16]),
      .rd1(reg_read_data2),
      .wa(write_reg),
      .we(RegWrite),
      .wd(reg_write_data));
  
  mux2 #(.WIDTH(5)) M1
      (.y(write_reg),
      .a(instr[20:16]),
      .b(instr[15:11]),
      .s(RegDst));
  mux2 #(.WIDTH(32)) M2
      (.y(w2),
      .a(reg_read_data2),
      .b(sign_extend),
      .s(ALUSrc));
  mux2 #(.WIDTH(32)) M3
      (.y(w5),
      .a(w1),
      .b(w4),
      .s(m3));
  mux2 #(.WIDTH(32)) M4
      (.y(pc_in),
      .a(w5),
      .b(Jump_address),
      .s(Jump));
  mux2 #(.WIDTH(32)) M5
      (.y(reg_write_data),
      .a(ALU_result),
      .b(mem_read_data),
      .s(MemtoReg));
  
  alu ALU
      (.y(ALU_result),
      .zf(Zero),
      .cf(),
      .of(),
      .sf(),
      .a(reg_read_data1),
      .b(w2),
      .m(ALU_control));
  
  assign w1=pc_out+4;
  assign Jump_address={w1[31:28],instr[25:0],2'b00};
  assign sign_extend=instr[15]? {16'b1111_1111_1111_1111,instr[15:0]}:{16'b0000_0000_0000_0000,instr[15:0]};
  assign w4={sign_extend,2'b00} + w1;
  assign m3=Branch & Zero;
  
  //control_unit
  always@(*)
  begin
      RegDst=0;
      ALUSrc=0;
      MemtoReg=0;
      RegWrite=0;
      MemRead=0;
      MemWrite=0;
      Branch=0;
      ALUOp=2'b00;
      Jump=0;
      ALU_control=3'b000;
      if(rst)begin
          RegDst=0;
      end
      else 
      begin
          case (instr[31:26])
              6'b000000:begin//add
                  if(instr[15:11]!=0)begin//不能对$0操作
                      RegDst=1;
                      //ALUSrc=0;
                      //MemtoReg=0;
                      RegWrite=1;
                      //MemRead=0;
                      //MemWrite=0;
                      //Branch=0;
                      ALUOp=2'b10;
                      //Jump=0;
                      if(instr[5:0]==6'b100000)begin
                          ALU_control=3'b000;
                      end
                  end
                  
              end
              6'b001000:begin//addi
                  if(instr[20:16]!=0)begin
                      //RegDst=0;
                      ALUSrc=1;
                      //MemtoReg=0;
                      RegWrite=1;
                      //MemRead=0;
                      //MemWrite=0;
                      //Branch=0;
                      ALUOp=2'b10;
                      //Jump=0;
                      //ALU_control=3'b000;
                  end
                  
              end
              6'b100011:begin//lw
                  if(instr[20:16]!=0)begin
                      //RegDst=0;
                      ALUSrc=1;
                      MemtoReg=1;
                      RegWrite=1;
                      MemRead=1;
                      //MemWrite=0;
                      //Branch=0;
                      //ALUOp=2'b00;
                      //Jump=0;
                      //ALU_control=3'b000;
                  end
              end
              6'b101011:begin//sw
                  //RegDst=0;
                  ALUSrc=1;
                  //MemtoReg=0;
                  //RegWrite=0;
                  //MemRead=0;
                  MemWrite=1;
                  //Branch=0;
                  //ALUOp=2'b00;
                  //Jump=0;
                  //ALU_control=3'b000;
              end
              6'b000100:begin//beq
                  //RegDst=0;
                  //ALUSrc=0;
                  //MemtoReg=0;
                  //RegWrite=0;
                  //MemRead=0;
                  //MemWrite=0;
                  Branch=1;
                  ALUOp=2'b01;
                  //Jump=0;
                  ALU_control=3'b001;
              end
              6'b000010:begin//j
                  Jump=1;//仅此信号有用
              end
              default:RegDst=0;
          endcase
      end  
  end
  endmodule
  ```



##### DBU的设计

* 本次实验自己觉得找到了自己的几个盲区，一个是对自增1的信号inc,或者自减1的信号dec的实现(应该采用时序加组合的两段式完成，而不能仅仅在组合逻辑里addr=addr+1这种操作)。

  另外就是run信号的使用，考虑用assign+succ来选择clk或者step的去抖动加取边沿信号

  在板子上使用的话，一定要对button去抖动并且取边沿。而由于本次实验只是仿真，所以个人将去抖动模块注释掉了，需要的时候加上即可。

  数码管的实现参照了上一学期的实验内容，具体思想就是分频(生成一个低频时钟，这样才能肉眼观察到数码管的亮度变化)，然后时分复用的方式，结合位选和段选信号输出要求的数据，比如pc_in。并通过译码模块将数据转化为显示该数据的段选信号。

  led的话则直接对其赋值即可，在下载到板子上时只需要在约束文件里连接好对应管脚。

* dbu的具体实现则考虑按ppt的逻辑图来把输入输出确定，然后改编一下前一节实现的cpu，Register_File，使其多一些输入输出便于与dbu模块交互。另外data_memory也改为了双端口以实现调试时读数据存储器内容。

* 代码及注释

  ```Verilog
  module dbu
      (input clk,         //时钟(上升沿有效)
      input rst,          //异步复位，高电平有效
      input succ,
      input step,
      input [2:0] sel,
      input m_rf,
      input inc,
      input dec,
      output reg [7:0] an,
      output [7:0] seg,
      output reg [15:0] led
      );
  
  wire [31:0] m_data,rf_data;//从RF/MEM读取的数据字
  wire [11:0] status;         //{Jump,Branch,RegDst,RegWrite,MemRead,MemtoReg,MemWrite,ALU_control,ALUSrc,ALUZero,
  wire [31:0] pc_in,pc_out,instr;
  wire [31:0] rf_rd1,rf_rd2;//寄存器堆读口1,2的输出数据
  wire [31:0] alu_y,m_rd;//ALU计算结果及data_memery的输出数据
  wire run;
  reg [7:0] m_rf_addr;//MEM/RF的调试读口字地址
  
  wire step_clean,step_redge,inc_clean,inc_redge,dec_clean,dec_redge;//去抖动，取边沿信号
  
  //parameter STATUS=2'b00,RESULT=2'b01,INC=2'b10,DEC=2'b11;
  //reg [1:0] state,next_state;
  
  //数码管
  reg [19:0] cnt;
  reg [3:0] scan_cnt;
  reg [3:0] data;
  reg [31:0] data1;
  wire pulse;
  
  cpu_one_cycle CPU       //CPU例化
      (.clk(run),
      .rst(rst),
      .m_rf_addr(m_rf_addr),
      .status(status),
      .m_data(m_data),
      .rf_data(rf_data),
      .pc_in(pc_in),
      .pc_out(pc_out),
      .instr(instr),
      .reg_read_data1(rf_rd1),
      .reg_read_data2(rf_rd2),
      .alu_y(alu_y),
      .mem_read_data(m_rd));
  
  /*deburring debur1
      (.clk (clk),
      .button (step),
      .button_clean (step_clean));
  deburring debur2
      (.clk (clk),
      .button (inc),
      .button_clean (inc_clean));
  deburring debur3
      (.clk (clk),
      .button (dec),
      .button_clean (dec_clean));
  */
  signal_edge sig1        //取边沿例化
      (.clk (clk),
      .button(step),
      .button_redge(step_redge));   
  
  signal_edge sig2
      (.clk (clk),
      .button(inc),
      .button_redge(inc_redge)); 
  
  signal_edge sig3
      (.clk (clk),
      .button(dec),
      .button_redge(dec_redge));   
  
  decode Decode//调用译码模块
      (.data  (data),
      .seg   (seg));
  
  assign run = (succ==1)?clk:step_redge;  //run信号用作cpu的clk
  
  //根据sel信号及inc,dec来显示相应内容
  reg [7:0] new_m_rf_addr;
  always@(posedge clk or posedge rst)
  begin
      if(rst) m_rf_addr<=0;
      else m_rf_addr <= new_m_rf_addr;
  end
  
  always@(*)
  begin
      if(rst)begin
          led=16'h0000;
          an=8'b1111_1111;
          data1=0;
          m_rf_addr=0;
      end
      else 
      begin
          led={4'b0000,status};
          case (sel)
              0:begin
                  if(m_rf==1) begin data1=m_data; end
                  else begin data1 = rf_data; end
                  led={8'h00,m_rf_addr}; 
                  if(inc_redge&&~dec_redge) new_m_rf_addr=m_rf_addr+1;
                  else if (~inc_redge && dec_redge && m_rf_addr!=0) new_m_rf_addr=m_rf_addr-1;
                  else new_m_rf_addr=m_rf_addr;
              end
              1:data1=pc_in;
              2:data1=pc_out;
              3:data1=instr;
              4:data1=rf_rd1;
              5:data1=rf_rd2;
              6:data1=alu_y;
              7:data1=m_rd;
              default:data1=0;
          endcase
      end
  end
  
  
  
  
  //数码管
  always@(posedge clk)
  begin
      if(rst) cnt<=20'b0;
      else if(cnt==20'h7fff) cnt <= 0;
      else cnt <= cnt + 20'b1;
  end
  assign pulse = (cnt==20'h0)?1'b1:1'b0;
  always@(posedge clk)//pulse作为计数使能信号
  begin
      if(rst) scan_cnt <= 4'b0;
      else if(pulse) 
          if(scan_cnt == 4'h7)
              scan_cnt <= 4'b0;
          else
              scan_cnt <= scan_cnt + 4'b1;
  end
  always@(posedge clk)    //分时复用
  begin
      case(scan_cnt)
          4'h0: an<=8'b1111_1110;
          4'h1: an<=8'b1111_1101;
          4'h2: an<=8'b1111_1011;
          4'h3: an<=8'b1111_0111;
          4'h4: an<=8'b1110_1111;
          4'h5: an<=8'b1101_1111;
          4'h6: an<=8'b1011_1111;
          4'h7: an<=8'b0111_1111;
      endcase
  end
  
  always@(posedge clk)
  begin
      case(scan_cnt)
          4'h0: data <= data1[3:0];
          4'h1: data <= data1[7:4];
          4'h2: data <= data1[11:8];
          4'h3: data <= data1[15:12];
          4'h4: data <= data1[19:16];
          4'h5: data <= data1[23:20];
          4'h6: data <= data1[27:24];
          4'h7: data <= data1[31:28];
      endcase
  end
  endmodule
  ```

其余如cpu的代码等放在附件里

***

### 【仿真结果及分析】

##### CPU功能仿真

* 仿真代码

  ```Verilog
  module cpu_tb;
      reg clk, rst;
  
  parameter PERIOD = 20, 	//时钟周期长度
  CYCLE = 1000;		//时钟个数
  
  cpu_one_cycle CPU(clk,rst);
  
  initial
  begin
      clk = 0;
      repeat (2 * CYCLE)
          #(PERIOD/2) clk = ~clk;
      $finish;
  end
  
  initial
  begin
      rst = 1;
      #PERIOD rst = 0;
  end
  endmodule
  ```
  
* 仿真结果及分析:

  ![](D:\科大\大二下\计组实验\lab3\图片\cpu仿真1.JPG)
  
  看黄线处，由sim.asm可知此条指令为`add $s0,$t1,$t0    #s0=t1+t0=8`，而观察下方reg_read_data1,2，其值分别为加数3和5；以及reg_write_data=8,说明计算结果正确，再由write_reg=0x10,知写入的是16号寄存器，正是$s0;
  
  同理，在下一周期，为指令`lw  $s1,12($0)     #s1=8`，由图像知write_data=8，mem_read_data=8,write_reg=0x11,说明是把8从data memory里读出并写入了$s1.
  
  再下一周期，为指令`beq $s1,$s0,_next1   #正确跳到_next` ，图像显示pc_out在beq时为18，在下一周期直接变为了20,是正确跳转到了_next1
  
  此后几条指令与前面指令类型相同，不再重复分析
  
  ![](D:\科大\大二下\计组实验\lab3\图片\cpu仿真2.JPG)
  
  再看到黄线处，这里前一个周期是beq指令，当前周期为指令`add $0, $0, $t2      #$0应该一直为0`，实现代码时，在control unit里做了判断，如果是R类的rd或者addi，lw的rt为寄存器$0,则当前中期啥都不做(以保持寄存器$s0始终为0)，所有信号赋值为0；
  
  再后面是beq，再后面则是跳转到倒数第二条指令`sw  $t2,8($0)  #全部测试通过，存储器地址0x08里值为1`,由图像，reg_read_data2=1,ALU_result=0x08,说明确实是要把1写入data_memory的第三个地址(按字寻址)
  
  此后pc_out一直在0x48和0x4c间跳转，与sim.asm里j指令效果一致

##### DBU功能仿真

* 仿真代码

  ```Verilog
  `timescale 1ns / 100ps
  
  module dbu_tb;
      reg clk, rst,succ,step,m_rf,inc,dec;
      reg [2:0] sel;
      wire [7:0] an,seg;
      wire [15:0] led;
       
      parameter PERIOD = 20;//时钟周期长度
      parameter CYCLE = 10000;		//时钟个数
      dbu DBU(.clk(clk),.rst(rst),.succ(succ),.step(step),.sel(sel),
              .m_rf(m_rf),.inc(inc),.dec(dec),.an(an),.seg(seg),.led(led));
      
      initial
      begin
          clk = 0;
          repeat (2 * CYCLE)
              #(PERIOD/2) clk = ~clk;
          $finish;
      end
      
      initial
      begin
      rst = 0;
      #(PERIOD);rst=1;
      #(PERIOD); rst = 0;
      /*
      #(PERIOD*14) rst = 1;
      #PERIOD rst = 0;
      
      #(PERIOD*14) rst = 1;
      #PERIOD rst = 0;  
      */  
      end
      
      initial 
      begin
      succ=0;step=0;sel=0;m_rf=0;inc=0;dec=0;
      #(PERIOD);
      #(PERIOD); step=1;inc=1;//执行一条指令
      //#(PERIOD*8); 
      #(PERIOD); step=0;inc=0;
      #(PERIOD); step=1;inc=1;//执行一条指令
      //#(PERIOD*8); 
      #(PERIOD); step=0;inc=0;
      #(PERIOD); step=1;inc=1;//执行一条指令
      //#(PERIOD*8); 
      #(PERIOD); step=0;inc=0;
      #(PERIOD); step=1;inc=1;//执行一条指令
      //#(PERIOD*8); 
      #(PERIOD); step=0;inc=0;//m_rd_addr=5
      #(PERIOD); inc=1;
      //#(PERIOD*8); 
      #(PERIOD); inc=0;//m_rd_addr=5
      //add 执行完还未写回(下一时钟上升沿写回)
      #(PERIOD);m_rf=1;//看data_memory,80000100
  
      #(PERIOD); inc=1;
      //#(PERIOD*8); 
      #(PERIOD); inc=0;
      #(PERIOD); inc=1;
      //#(PERIOD*8); 
      #(PERIOD); inc=0;
      #(PERIOD); inc=1;
      //#(PERIOD*8); 
      #(PERIOD); inc=0; //m_rd_addr=8,即$t0
      
      #(PERIOD); m_rf=0;//看$t0的值
      #(PERIOD); sel=1;
      #(PERIOD); sel=2;
      #(PERIOD); sel=3;
      #(PERIOD); sel=4;
      #(PERIOD); sel=5;
      #(PERIOD); sel=6;
      #(PERIOD); sel=7;
      #(PERIOD); step=1;sel=0;//执行一条指令
      //#(PERIOD*8); 
      #(PERIOD); step=0;
      //lw 执行完还未写回(下一时钟上升沿写回)
      #(PERIOD); inc=1;
      //#(PERIOD*8); 
      #(PERIOD); inc=0;//m_rd_addr=9,即$t1
  
      #(PERIOD); sel=0;//看$t1
      #(PERIOD); sel=1;
      #(PERIOD); sel=2;
      #(PERIOD); sel=3;
      #(PERIOD); sel=4;
      #(PERIOD); sel=5;
      #(PERIOD); sel=6;
      #(PERIOD); sel=7;
  
      #(PERIOD); step=1;sel=0;inc=1;//执行一条指令beq
      //#(PERIOD*8); 
      #(PERIOD); step=0;inc=0;
      #(PERIOD); inc=1;
      //#(PERIOD*8); 
      #(PERIOD); inc=0;
      #(PERIOD); inc=1;
      //#(PERIOD*8); 
      #(PERIOD); inc=0; 
      #(PERIOD); inc=1;
      //#(PERIOD*8); 
      #(PERIOD); inc=0;
      #(PERIOD); inc=1;
      //#(PERIOD*8); 
      #(PERIOD); inc=0;
      #(PERIOD); inc=1;
      //#(PERIOD*8); 
      #(PERIOD); inc=0;
      #(PERIOD); inc=1;
      //#(PERIOD*8); 
      #(PERIOD); inc=0;//m_rd_addr=16,即$s1
      #(PERIOD); sel=0;//看$s1
      #(PERIOD); sel=1;
      #(PERIOD); sel=2;
      #(PERIOD); sel=3;
      #(PERIOD); sel=4;
      #(PERIOD); sel=5;
      #(PERIOD); sel=6;
      #(PERIOD); sel=7;
  
      #(PERIOD); succ=1;
      #(PERIOD*8); succ=0;sel=0;//执行完了最后的sw指令，但还未写回
      #(PERIOD); dec=1;
      //#(PERIOD*8); 
      #(PERIOD); dec=0;
      #(PERIOD); dec=1;
      //#(PERIOD*8); 
      #(PERIOD); dec=0;
      #(PERIOD); dec=1;
      //#(PERIOD*8); 
      #(PERIOD); dec=0; 
      #(PERIOD); dec=1;
      //#(PERIOD*8); 
      #(PERIOD); dec=0;
      #(PERIOD); dec=1;
      //#(PERIOD*8); 
      #(PERIOD); dec=0;
      #(PERIOD); dec=1;
      //#(PERIOD*8); 
      #(PERIOD); dec=0; //m_rd_addr=10,即$t2
  
      #(PERIOD); sel=0;//看$t2
      #(PERIOD); sel=1;
      #(PERIOD); sel=2;
      #(PERIOD); sel=3;
      #(PERIOD); sel=4;
      #(PERIOD); sel=5;
      #(PERIOD); sel=6;
      #(PERIOD); sel=7;
      #(PERIOD); sel=0;
      #(PERIOD); dec=1;
      //#(PERIOD*8); 
      #(PERIOD); dec=0;
      #(PERIOD); dec=1;
      //#(PERIOD*8); 
      #(PERIOD); dec=0;
      #(PERIOD); dec=1;
      //#(PERIOD*8); 
      #(PERIOD); dec=0;
      #(PERIOD); dec=1;
      //#(PERIOD*8); 
      #(PERIOD); dec=0;
      #(PERIOD); dec=1;
      //#(PERIOD*8); 
      #(PERIOD); dec=0;
      #(PERIOD); dec=1;
      //#(PERIOD*8); 
      #(PERIOD); dec=0;
      #(PERIOD); dec=1;
      //#(PERIOD*8); 
      #(PERIOD); dec=0;
      #(PERIOD); dec=1;
      //#(PERIOD*8); 
      #(PERIOD); dec=0;//m_rd_addr=2,看data_memory
  
      #(PERIOD); step=1;//执行一条指令
      //#(PERIOD*8); 
      #(PERIOD); step=0;
      //j 执行完
  
      #(PERIOD); sel=0;m_rf=1;//看data_memory
      #(PERIOD); sel=1;
      #(PERIOD); sel=2;
      #(PERIOD); sel=3;
      #(PERIOD); sel=4;
      #(PERIOD); sel=5;
      #(PERIOD); sel=6;
      #(PERIOD); sel=7;
      end 
  endmodule
  ```

* 仿真结果及分析

  ![](D:\科大\大二下\计组实验\lab3\图片\dbu仿真1.JPG)

  如图黄线处，此时按照仿真文件可知，应该是读存储器的第六号位置的数据0x80000100。而看图，data1(要赋值给数码管的数据)=m_data=0x80000100,且m_rf_addr=0x05,说明是对的。

  ![](D:\科大\大二下\计组实验\lab3\图片\dbu仿真2.JPG)
  
  又看黄线处，从此处开始的七个周期，sel依次置为1-7。观察data1可知其值与pc_in,pc_out,instr,等要求完全一致。而相应的led值为0x0300，说明其RegDst和RegWrite为1，其余全为0，(因为当前add指令周期刚结束，所以信号与add指令时一致，alu的操作信号为3'b000，即+信号)。



​	![](D:\科大\大二下\计组实验\lab3\图片\dbu仿真3.JPG)

再看到仿真文件第三次观察时，sel又从0-7变化，此时是观察的beq指令刚运行完的时候。观察图像可看到m_rd_addr=16,即$s1，其值data1为8，与sim.asm中所述一致，而且之后pc_in,pc_out也与data1一致，sel=3时，data1=0x12300001,是beq指令的二进制数据，也正确。

此后succ=1，连续运行八个周期后，程序来到倒数第二条指令sw运行结束的位置。

![](D:\科大\大二下\计组实验\lab3\图片\dbu仿真4.JPG)

此时Instr=0xac0a0008，说明确实是sw指令；根据sel的值从0-7变化，m_rf_addr为0x0a,即$t2。可看到data1依次为1，pc_in,pc_out,instr等的值。此时alu_y结果为0x8,与`sw  $t3,8($0)`结果一致。status=0x022,正是MemWrite和ALUSrc为1，与实际相符。

![](D:\科大\大二下\计组实验\lab3\图片\dbu仿真5.JPG)

最后m_rf_addr调到0x2,读出其数据至data1，黄线部分可见值为0x1，与sw指令存入该data_memory的值一致

综上，仿真结果正确。

***

### 【下载结果及分析】

暂无

***

### 【思考题】

修改数据通路和控制器，增加支持如下指令：

accm: rd <- M(rs) + rt; 	op = 000000, funct = 101000

为了和add指令作区分，在代码里还得对funct做判断，不过代码里直接在case语句里判断即可，但是如果涉及更多的指令，要体现在数据通路的图中，可能得考虑把funct指令也传递给Control。

除此之外，accm指令是先访存再计算，所以考虑利用双端口ram的另一个端口dpra来读取rs对应地址的内容，再把数据返回到ALU计算，最后再写回Register_File。

数据通路如下,相应信号的设置照图片能很容易得到，就不再列出。

![](D:\科大\大二下\计组实验\lab3\图片\思考题.png)

***

### 【实验总结】

通过本次实验，第一次设计出了cpu，因为有理论课和实验课上老师的讲解，以及现成的数据通路，cpu的设计反而比想象中的简单，基本上是按部就班的完成，但这也让我更加好奇多周期和流水线的cpu设计，肯定会更加有趣。不过dbu这一块算是很复杂的地方，一方面是因为参数太多，例化时来回看容易弄错，另一方面就是dbu的仿真代码的设计，因为本来dbu就是为了在板子上看到cpu运行效果的，现在写个仿真文件还是挺麻烦，需要认真分析助教所给的sim_asm文件功能，对step，succ等信号做控制来观察想要观察的数据。

本次实验还是有很大收获，对于Verilog组合逻辑和时序逻辑的一些细节有了更深刻的理解，也解决了一些盲区。

***

### 【意见建议】

对于本次实验整体较为满意，实验完成后自我感觉所达到的效果与实验目的基本一致。本次实验教程也给的不错，对于dbu一些细节的设计留给我们自己思考还是很有意义的。