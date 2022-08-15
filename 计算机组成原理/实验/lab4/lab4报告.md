# **中国科学技术大学计算机学院**

## **《计算机组成与原理》报告**

##### 实验题目：多周期CPU

##### 学生姓名：雷雨轩

##### 学生学号：PB18111791

##### 完成日期：2020-5-23

***

### 【实验目的】

* 理解计算机硬件的基本组成、结构和工作原理；
* 掌握数字系统的设计和调试方法；
* 熟练掌握数据通路和控制器的设计和描述方法。

***

### 【逻辑设计分析与代码】

##### CPU的设计: 

* 数据通路:（在ppt基础上加了一些数据线的名字)，代码里的变量名基本与图中一致，较易识别。

  ![](D:\科大\大二下\计组实验\lab4\图片\cpu数据通路.PNG)

  不过因为原本老师所给图像等并未考虑addi指令，所以自己对状态机以及指令执行内容作了一定补充，如下:

  ![](D:\科大\大二下\计组实验\lab4\图片\cpu指令内容.PNG)

  ![](D:\科大\大二下\计组实验\lab4\图片\cpu控制器状态机.PNG)

* 老师已经给出数据通路，状态机，addi的补充也相对容易，所以只需要对数据线命名，再用之前lab1，lab2里已经实现过的register,Register_File,alu,mux2，以及IP核Memory来例化即可，例化时需要注意的就是参数不要带错了。不过此处还需要实现一个四选一选择器mux4,实现较为简单

  ```Verilog
  module mux4
      #(parameter WIDTH = 32)//数据宽度
      (output reg [WIDTH-1:0] y, //输出数据
      input [WIDTH-1:0] a,b,c,d, //四路输入数据
      input [1:0] s                 //数据选择控制
      );
  
  always@(*)
  begin
      case (s)
          2'b00:y=a;
          2'b01:y=b;
          2'b10:y=c;
          2'b11:y=d;
          default:y=a;
      endcase
  end
  
  endmodule
  ```

* 控制单元的设计，涉及到状态机的实现，还是用最常用有效的两段式，一方面next_state的更新逻辑则需根据状态机的图和instr[31:26]来判断；组合逻辑信号输出则只用考虑对应的状态下的赋值即可(规定无关紧要的信号全部赋值为0)

* 仍需注意的是，写寄存器、Memory、寄存器堆，均是同步写，也就是在一个周期准备好数据后，在下一时钟上升沿时写入相应位置。读的话是异步读。

* 代码及注释

  ```Verilog
  module cpu_multicycle    //多周期CPU
      (input clk,         //时钟(上升沿有效)
      input rst           //异步复位，高电平有效
      );
  
  //信号
  reg PCwe,IRWrite,lorD,MemRead,MemWrite,MemtoReg,ALUSrcA,RegWrite,RegDst;
  reg [1:0] PCSource,ALUSrcB;
  reg [2:0] ALUm;
  
  wire ALUZero;
  wire [31:0] pc_out,pc_in;//pc寄存器的输入和输出
  wire [31:0] instr,MemData;//指令，从Memory里读出来的数据
  wire [31:0] mdr;//MDR输出数据
  wire [31:0] a,b;//a,b寄存器的输出
  wire [31:0] reg_read_data1,reg_read_data2;//寄存器堆读出的数据
  wire [31:0] ALU_result,ALUOut;//ALU计算结果，ALUOut寄存器输出
  wire [31:0] mem_addr;
  wire [31:0] rf_write_data;
  wire [31:0] w1,w2;//ALU的两个输入
  wire [4:0] write_reg;
  //wire [31:0] reg_write_data;
  wire [31:0] sign_extend,sign_extend_shift,Jump_address;
  register #(.WIDTH(32)) PC
      (.clk(clk),
      .rst(rst),
      .en(PCwe),
      .d(pc_in),
      .q(pc_out));
  
  register #(.WIDTH(32)) IR
      (.clk(clk),
      .rst(rst),
      .en(IRWrite),
      .d(MemData),
      .q(instr));
  
  register #(.WIDTH(32)) MDR
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(MemData),
      .q(mdr));
  
  register #(.WIDTH(32)) A
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(reg_read_data1),
      .q(a));
  
  register #(.WIDTH(32)) B
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(reg_read_data2),
      .q(b));
  
  register #(.WIDTH(32)) ALUOut1
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(ALU_result),
      .q(ALUOut));
  
  Memory mem
      (.a(mem_addr[10:2]),
      .d(b),
      .clk(clk),
      .we(MemWrite),
      .spo(MemData));
  
  Register_File #(.WIDTH(32)) Registers
      (.clk(clk),
      .ra0(instr[25:21]),
      .rd0(reg_read_data1),
      .ra1(instr[20:16]),
      .rd1(reg_read_data2),
      .wa(write_reg),//写端口地址
      .we(RegWrite),
      .wd(rf_write_data));
  
  alu ALU
      (.y(ALU_result),
      .zf(ALUZero),
      .cf(),
      .of(),
      .sf(),
      .a(w1),
      .b(w2),
      .m(ALUm));
  
  mux2 #(.WIDTH(32)) M1
      (.y(mem_addr),
      .a(pc_out),
      .b(ALUOut),
      .s(lorD));
  mux2 #(.WIDTH(5)) M2
      (.y(write_reg),
      .a(instr[20:16]),
      .b(instr[15:11]),
      .s(RegDst));
  mux2 #(.WIDTH(32)) M3
      (.y(rf_write_data),
      .a(ALUOut),
      .b(mdr),
      .s(MemtoReg));
  mux2 #(.WIDTH(32)) M4
      (.y(w1),
      .a(pc_out),
      .b(a),
      .s(ALUSrcA));
  
  mux4 #(.WIDTH(32)) M5
      (.y(w2),
      .a(b),
      .b(4),
      .c(sign_extend),
      .d(sign_extend_shift),
      .s(ALUSrcB));
  
  mux4 #(.WIDTH(32)) M6
      (.y(pc_in),
      .a(ALU_result),
      .b(ALUOut),
      .c(Jump_address),
      .d(0),
      .s(PCSource));
  
  //组合逻辑
  assign sign_extend=instr[15]? {16'b1111_1111_1111_1111,instr[15:0]}:{16'b0000_0000_0000_0000,instr[15:0]};
  assign sign_extend_shift={sign_extend[29:0],2'b00};
  assign Jump_address={pc_out[31:28],instr[25:0],2'b00};
  
  parameter Instr_fetch=4'b0000,Instr_decode=4'b0001,Mem_addr_compute=4'b0010,Mem_access=4'b0011,Write_back=4'b0100;
  parameter Mem_access_2=4'b0101,Execution=4'b0110,R_complete=4'b0111,Branch=4'b1000,Jump=4'b1001,AddI=4'b1010;
  reg [3:0] state,next_state;
  
  always@(posedge clk,posedge rst)
  begin
      if(rst) state<=Instr_fetch;
      else state<=next_state;
  end
  
  //next_state logic
  always@(*)
  begin
      case(state)
          Instr_fetch: next_state=Instr_decode;
          Instr_decode:begin
              case(instr[31:26])//根据OP决定下次个状态
                  6'b000000:next_state=Execution;//add
                  6'b001000:next_state=Mem_addr_compute;//addi
                  6'b100011:next_state=Mem_addr_compute;//lw
                  6'b101011:next_state=Mem_addr_compute;//sw
                  6'b000100:next_state=Branch;//beq
                  6'b000010:next_state=Jump;//j
                  default:next_state=Instr_fetch;
              endcase
          end
          Mem_addr_compute:begin
              case(instr[31:26])
                  6'b001000:next_state=AddI;//addi
                  6'b100011:next_state=Mem_access;//lw
                  6'b101011:next_state=Mem_access_2;//sw
                  default:next_state=Instr_fetch;
              endcase
          end
          Mem_access:next_state=Write_back;
          Write_back:next_state=Instr_fetch;
          Mem_access_2:next_state=Instr_fetch;
          Execution:next_state=R_complete;
          R_complete:next_state=Instr_fetch;
          Branch:next_state=Instr_fetch;
          Jump:next_state=Instr_fetch;
          AddI:next_state=Instr_fetch;
          default:next_state=Instr_fetch;
      endcase
  end
  
  //output logic
  always@(*)//根据state输出相应信号，无关信号全部取0
  begin 
      case(state)
          Instr_fetch:begin MemRead=1;lorD=0;IRWrite=1;ALUSrcA=0;ALUSrcB=2'b01;ALUm=3'b000;PCwe=1;PCSource=2'b00;//有用的信号
              MemWrite=0;MemtoReg=0;RegWrite=0;RegDst=0;//无关紧要的信号一律取0
          end
          Instr_decode:begin
              ALUSrcA=0;ALUSrcB=2'b11;ALUm=3'b000;//有用的信号
              MemRead=0;lorD=0;IRWrite=0;MemWrite=0;MemtoReg=0;RegWrite=0;RegDst=0;PCwe=0;PCSource=2'b00;//无关紧要的信号一律取0
          end
          Mem_addr_compute:begin
              ALUSrcA=1;ALUSrcB=2'b10;ALUm=3'b000;//有用的信号
              MemRead=0;lorD=0;IRWrite=0;MemWrite=0;MemtoReg=0;RegWrite=0;RegDst=0;PCwe=0;PCSource=2'b00;//无关紧要的信号一律取0
          end
          Mem_access:begin
              MemRead=1;lorD=1;//有用的信号
              ALUSrcA=0;ALUSrcB=2'b00;ALUm=3'b000;IRWrite=0;MemWrite=0;MemtoReg=0;RegWrite=0;RegDst=0;PCwe=0;PCSource=2'b00;//无关紧要的信号一律取0
          end
          Write_back:begin
              if(instr[20:16]!=0)begin RegDst=0;RegWrite=1;MemtoReg=1; end//有用的信号
              else begin RegDst=0;RegWrite=0;MemtoReg=0; end//不能对0号寄存器做操作
              MemRead=0;lorD=0;ALUSrcA=0;ALUSrcB=2'b00;ALUm=3'b000;IRWrite=0;MemWrite=0;PCwe=0;PCSource=2'b00;//无关紧要的信号一律取0
          end
          Mem_access_2:begin
              MemWrite=1;lorD=1;//有用的信号
              ALUSrcA=0;ALUSrcB=2'b00;ALUm=3'b000;IRWrite=0;MemRead=0;MemtoReg=0;RegWrite=0;RegDst=0;PCwe=0;PCSource=2'b00;//无关紧要的信号一律取0
          end
          Execution:begin
              ALUSrcA=1;ALUSrcB=2'b00;ALUm=3'b000;//有用的信号
              MemRead=0;lorD=0;IRWrite=0;MemWrite=0;MemtoReg=0;RegWrite=0;RegDst=0;PCwe=0;PCSource=2'b00;//无关紧要的信号一律取0
          end
          R_complete:begin
              if(instr[15:11]!=0)begin RegDst=1;RegWrite=1;MemtoReg=0; end//有用的信号
              else begin RegDst=0;RegWrite=0;MemtoReg=0; end//不能对0号寄存器做操作
              MemRead=0;lorD=0;ALUSrcA=0;ALUSrcB=2'b00;ALUm=3'b000;IRWrite=0;MemWrite=0;PCwe=0;PCSource=2'b00;//无关紧要的信号一律取0
          end
          Branch:begin
              ALUSrcA=1;ALUSrcB=2'b00;ALUm=3'b001;PCSource=2'b01;//有用的信号
              if(ALUZero==1) begin PCwe=1; end
              MemRead=0;lorD=0;IRWrite=0;MemWrite=0;MemtoReg=0;RegWrite=0;RegDst=0;//无关紧要的信号一律取0
          end
          Jump:begin
              PCwe=1;PCSource=2'b10;//有用的信号
              RegDst=0;RegWrite=0;MemtoReg=0;MemRead=0;lorD=0;ALUSrcA=0;ALUSrcB=2'b00;ALUm=3'b000;IRWrite=0;MemWrite=0;//无关紧要的信号一律取0
          end
          AddI:begin
              if(instr[20:16]!=0)begin RegDst=0;RegWrite=1;MemtoReg=0; end//有用的信号
              else begin RegDst=0;RegWrite=0;MemtoReg=0; end//不能对0号寄存器做操作
              MemRead=0;lorD=0;ALUSrcA=0;ALUSrcB=2'b00;ALUm=3'b000;IRWrite=0;MemWrite=0;PCwe=0;PCSource=2'b00;//无关紧要的信号一律取0
          end
          default:begin//信号一律取0
              RegDst=0;RegWrite=0;MemtoReg=0;
              MemRead=0;lorD=0;ALUSrcA=0;ALUSrcB=2'b00;ALUm=3'b000;IRWrite=0;MemWrite=0;PCwe=0;PCSource=2'b00;
          end
      endcase
  end
  endmodule
  ```



##### DBU的设计

* dbu的设计相对简单，毕竟和单周期中结构基本一样，除去status里信号的区别，以及sel片选查看的数据有区别外，其他都相同。

* 还是要注意:一个是对自增1的信号inc,或者自减1的信号dec的实现(应该采用时序加组合的两段式完成，而不能仅仅在组合逻辑里addr=addr+1这种操作)。

  另外就是clkd信号的使用，考虑用assign+succ来选择clk或者step的去抖动加取边沿信号

  在板子上使用的话，一定要对button去抖动并且取边沿。本次实验个人将去抖动模块注释掉了，需要的时候加上即可。

  数码管的实现参照了上一学期的实验内容，具体思想就是分频(生成一个低频时钟，这样才能肉眼观察到数码管的亮度变化)，然后时分复用的方式，结合位选和段选信号输出要求的数据，比如pc_out。并通过译码模块将数据转化为显示该数据的段选信号。

  led的话则直接对其赋值即可，在下载到板子上时只需要在约束文件里连接好对应管脚。

* 同样需要按照ppt的逻辑图来把输入输出确定，把多周期cpu，Register_File模块的输入输出相应填充，以与dbu模块交互。另外Memory也改为了双端口以实现调试时读数据存储器内容。

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
  wire [15:0] status;    //PCSource,PCwe,lorD,MemWrite,IRWrite,RegDst,MemtoReg,RegWrite,ALUm,ALUSrcA,ALUSrcB,ALUZero;
  reg [8:0] m_rf_addr;//MEM/RF的调试读口字地址
  wire [31:0] pc_out,instr;
  wire [31:0] rf_rd1,rf_rd2;//寄存器堆读口1,2的输出数据
  wire [31:0] ALUOut,mdr;//ALU运算结果寄存器及MDR的输出数据
  wire clkd;
  wire [31:0] a,b;//A,B寄存器输出
  
  wire step_clean,step_redge,inc_clean,inc_redge,dec_clean,dec_redge;//去抖动，取边沿信号
  
  //parameter STATUS=2'b00,RESULT=2'b01,INC=2'b10,DEC=2'b11;
  //reg [1:0] state,next_state;
  
  //数码管
  reg [19:0] cnt;
  reg [3:0] scan_cnt;
  reg [3:0] data;
  reg [31:0] data1;
  wire pulse;
  
  cpu_multicycle CPU       //CPU例化
      (.clk(clkd),
      .rst(rst),
      .m_rf_addr(m_rf_addr),
      .status(status),
      .m_data(m_data),
      .rf_data(rf_data),
      .pc_out(pc_out),
      .instr(instr),
      //.reg_read_data1(rf_rd1),
      //.reg_read_data2(rf_rd2),
      .ALUOut(ALUOut),
      .mdr(mdr),
      .a(a),
      .b(b));
  
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
  
  assign clkd = (succ==1)?clk:step_redge;  //clkd信号用作cpu的clk
  
  //根据sel信号及inc,dec来显示相应内容
  reg [8:0] new_m_rf_addr;
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
          new_m_rf_addr=0;
      end
      else 
      begin
          led=status;
          case (sel)
              0:begin
                  if(m_rf==1) begin data1=m_data; end
                  else begin data1 = rf_data; end
                  led={7'b000_0000,m_rf_addr}; 
                  if(inc_redge&&~dec_redge) new_m_rf_addr=m_rf_addr+1;
                  else if (~inc_redge && dec_redge && m_rf_addr!=0) new_m_rf_addr=m_rf_addr-1;
                  else new_m_rf_addr=m_rf_addr;
              end
              1:data1=pc_out;
              2:data1=instr;
              3:data1=mdr;
              4:data1=a;
              5:data1=b;
              6:data1=ALUOut;
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

其余如:  cpu等文件代码等放在附件里

***

### 【仿真结果及分析】

##### CPU功能仿真

* 根据助教提供的sim.asm以及test.coe,可知存储器中第一个位置存的是j类指令，之后存了10个数据，再之后全部为指令。

* 仿真代码

  ```Verilog
  module cpu_tb;
      reg clk, rst;
  
  parameter PERIOD = 20, 	//时钟周期长度
  CYCLE = 10000;		//时钟个数
  
  cpu_multicycle CPU(clk,rst);
  
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

  ![](D:\科大\大二下\计组实验\lab4\图片\cpu1.JPG)
  
  一开始，先观察最底部的state变化，0-1-9即j指令，0-1-2-a即addi指令，后面分析同理，得到指令执行顺序与sim.asm里预期完全一致。
  
  再看state=1(黄线右边)时，指令instr=0x0800000b,即指行的j指令，之后state从9变为0时，pc_out更新为0x0000002c,即Memory的44字节处，下一周期state=1时，instr=0x20080003,此即第一条addi指令，与预期一致。接下来state从2变到a时，ALUOut更新为3，即要赋给$t0的值，此时rf_write_data=3，RegWrite=1，write_reg=8(即$t0)，与预期一致，要写入的寄存器是$t0;state从a变为0时，3被写入寄存器$t0,可从reg_readd_data2观察到(因为此时instr[20:16]就是8)。所以addi指令正确。此后addi指令同理。
  
  ![](D:\科大\大二下\计组实验\lab4\图片\cpu2.JPG)
  
  黄线部分时，cpu运行到add指令，state=1时，instr=0x01288020，为`add $s0,$t1,$t0`，state从1变到6时，a,b(即A，B寄存器的输出)分别更新为5和3，正确。state=6变到7时，ALUOut=8，即实现了加法。最后，rf_write_data=8，RegWrite=1，write_reg=0x10(即$s0)，与预期一致，要写入的寄存器是$s0,所以add指令正确。
  
  接下来，state又为1时，instr=0x8c11000c,为`lw  $s1,12($0)` ，state从2变到3时，ALUOut=0xc，说明要读取的内存地址为Memory的第四个数据(以字编址)，state变为4时，mdr变为8，说明读取的Memory的位置正确，state=5时，rf_write_data=8，RegWrite=1，write_reg=0x11(即$s1)，与预期一致，要写入的寄存器是$s1，lw指令正确。
  
  再看接下来的state为0-1-8，即beq指令，观察图像可知，state=8时ALUZero=1,且state从8变为0时，pc_out变为0x0000004c，说明确实跳转成功，跳到了lw指令位置，所以beq指令正确。
  
  此后指令大多重复，观察图像可以很容易得出正确的结论。所以还需重点分析最后的sw指令。
  
  ![](D:\科大\大二下\计组实验\lab4\图片\cpu3.JPG)
  
  在黄线部分，state=8，ALUZero=1，为beq跳转，之后跳转到最后的`sw  $t2,8($0)`，由state=1时instr=0xac0a0008可知确实如此。state从2变为5时，ALUOut变为8,此及要写入的Mem地址，此时b(B寄存器输出)=1，即是要把1写入Memory的第3个数据处(以字编址)。正确。此后，state一直以0-1-9-0-1-2-5循环，即与sim.asm里j和sw指令的循环一致。
  
  综上所述，仿真完全正确。

##### DBU功能仿真

与单周期中仿真类似，本次实验未做要求

***

### 【下载结果及分析】

暂无

***

### 【思考题】

修改数据通路和控制器，扩展对其他MIPS指令的支持，并进行功能仿真和下载测试

***

### 【实验总结】

通过本次实验，设计出了多周期cpu，因为单周期cpu设计的基础，以及现成的数据通路和状态转换图，多周期cpu也基本上是按部就班的完成，比之单周期的最大区别也即状态机的实现。

dbu设计则相对简单，前面也提到，此处dbu与单周期dbu代码基本一致，只是一些数据变量有所变化罢了。

***

### 【意见建议】

对于本次实验整体较为满意，实验完成后自我感觉所达到的效果与实验目的基本一致。