# **中国科学技术大学计算机学院**

## **《计算机组成与原理》报告**

##### 实验题目：流水线CPU

##### 学生姓名：雷雨轩

##### 学生学号：PB18111791

##### 完成日期：2020-6-16

***

### 【实验目的】

* 理解计算机硬件的基本组成、结构和工作原理；
* 掌握数字系统的设计和调试方法；
* 熟练掌握数据通路和控制器的设计和描述方法。

***

### 【逻辑设计分析与代码】

##### 流水线CPU的设计: 

* 完成思路:先综合实验课上所给图把数据通路搭起来：自己把图绘完整，再代码搭建，最后考虑细化，对控制器及变量名做一定处理，最后调试。

  * 有三个控制器: Hazard detection unit , Forwarding unit , control unit ,均为组合逻辑,时序只体现在寄存器，寄存器堆和存储器（否则会因为延迟导致数据出现错误）

    控制信号的产生:每个指令的控制信号由段间寄存器存储，所以每个时钟周期内每个段都操作该段的东西，信号也在对应的段间寄存器中读取

    每条指令的Control unit控制信号均在ID段产生，每个段需要用到的信号有

    **Control unit:**

    ​	opcode：Forwarding unit信号的产生需要综合考虑处于MEM，WB段的指令，还需考虑处于EX段的指令类型，这样才能对不同形式的数据相关做出全面的处理；所以考虑在Control unit把指令的编码也一并作为段间信号传递

    ​	ID段:Jump

    ​	EX段:RegDst, ALUm(3位)，Branch(与Zero合并为PCSrc),Zero

    ​	MEM段:MemRead,MemWrite

    ​	WB段:RegWrite,MemtoReg

    * 注意段间寄存器所存储的信号: 为简化实现，把所有信号按顺序拼接后放在一个寄存器里实现

      类似于`register #(.WIDTH(12)) ID_EX_SIG` ，其同时包含了以下信号：

      EX:RegDst+ALUm+Branch+ MEM:MemRead+MemWrite+ WB:RegWrite,MemtoReg +opcode

    **Forwarding unit**(EX段产生信号并使用)

    ​	主要是m1,m2,m4(即相应mux的片选信号)

    **Hazard detedction unit**(ID段产生信号并使用):

    ​	需要同时处理特殊的数据相关: eg: lw $1,45($2)  ;  add $5,$7,$1这种

    ​	另外还需要对j和beq指令作stall处理，避免因为跳转使得不需要运行的指令也读入了流水线并执行

    ​	信号:m9,PCWrite,IF_IDWrite,IF_Flush;(PCWrite,IF_IDWrite为对应的写使能信号，IF_Flush是为了实现而在寄存器模块里加入的同步清零信号)

  * j类指令: 需对数据通路做一定改动：老师给的图中没有画出，可以参考单周期cpu的形式，这里为了减少控制相关的延迟，考虑在ID段结束时就把新的跳转地址写入PC

  * beq:同样为了减少控制相关延迟并且实现简单，考虑在EX段结束时将更新的地址写入PCWrite

  * 寄存器堆的操作：因为教材上要求是同一周期先写后读(当对同一寄存器操作时)；0地址不能变，所以在Register_File模块添加了两个选择器实例，用于对读端口读出的数据做处理(相当于同一周期读写同一端口时会在寄存器堆内部产生Forwarding)

  * 控制相关通过stall解决：即PC，IF/ID.NPC,IF/ID.IR 不更新，并将IF/ID.NPC,IF/ID.IR清零，以及将ID段产生的control unit信号全部置为0（opcode信号置为6）

  * 数据相关的解决:关键还是理清楚有哪些可能的数据相关

    * 产生源:(即后写的指令)add,addi,lw,其中add，addi为一个类型，计算的值在其MEM段开始时即可得到；lw的话其要写回寄存器的值应该在WB段才能得到

      不过因为实验里data memory是异步读的，所以实际上lw要写回寄存器堆的值也能在MEM段通过Data memory的读端口数据得到，这样所有数据相关都可以用forwarding解决
      不过这样forwarding unit逻辑更复杂，可能实现起来比直接stall麻烦一点点。所以本次实验还是把特殊的lw形式用stall处理

    * 应答源(写前读的指令):

      * 每个指令(j除外)均需要读取rs
      * add，beq需要读取rt
      * sw有其独特的数据通路(EX段)（可参见下面所给图)
      * 此外，根据实验课讲义，lw后紧跟的指令,如lw $1,45($2)  ;  add $5,$7,$1这种，需要对add做一个周期的停顿，变成lw  空 add三条指令的模式，这样可以转化为Forwarding来最终实现

* 数据通路:综合ppt所给图像以及自己对于相关所做的处理，得到最后的数据通路如下(省略了j指令的计算通路，不过可参考单周期cpu图，比较简单)

  ![](D:\科大\大二下\计组实验\lab5\图片\数据通路1.PNG)

  ![](D:\科大\大二下\计组实验\lab5\图片\数据通路2.PNG)

* RTL图

  ![](D:\科大\大二下\计组实验\lab5\图片\RTL.JPG)
  
* 代码及注释：主模块(cpu_aline) 主要为四部分: 信号声明；模块调用；assign组合逻辑；三个控制器逻辑

  ```Verilog
  module cpu_aline    //有相关的流水线CPU
      (input clk,         //时钟(上升沿有效)
      input rst           //异步复位，高电平有效
      );
  
  reg Jump;//j指令的信号
  wire PCSrc;//选择pc+4和beq的信号
  //Control unit信号
  reg [2:0] opcode;//0:add, 1:addi, 2:lw, 3:sw, 4:beq, 5:j
  reg RegDst,Branch,MemRead,MemWrite,RegWrite,MemtoReg;//ALUSrc用于选择rt还是rd作为ALU的第二个输入
  reg [2:0] ALUm;
  wire Zero;
  wire [11:0] control_sig;
  wire [11:0] id_ex_sig;
  wire [6:0] ex_mem_sig;
  wire [4:0] mem_wb_sig;
  
  //Forwarding unit 信号
  reg [1:0] m1,m2,m4;//MUX1,2,4的选择信号
  
  
  //Hazard detection unit 信号
  reg m9,PCWrite,IF_IDWrite,IF_Flush;
  
  
  wire [31:0] pc_out,pc_in;//pc寄存器的输入和输出
  wire [31:0] instr;//instr memory里读出的数据
  wire [31:0] reg_read_data1,reg_read_data2;//寄存器堆读出的数据
  wire [31:0] sign_extend,sign_extend_shift,Jump_address;
  wire [31:0] mem_read_data;//Data memory里读出的数据
  wire [31:0] reg_wdata;//寄存器堆写入端口数据
  wire [31:0] ALU_result;//ALU的计算结果
  wire [31:0] pc_beq;
  
  wire [31:0] m1_out,m2_out,m4_out,m7_out;//MUX1,2,4,7的输出
  wire [4:0] m3_out;
  wire [11:0]m9_out;
  //段间信号寄存器
  //EX:RegDst+ALUm+Branch+ MEM:MemRead+MemWrite+ WB:RegWrite,MemtoReg  +opcode
  register #(.WIDTH(12)) ID_EX_SIG 
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(m9_out),
      .q(id_ex_sig));
  //MEM:MemRead+MemWrite+ WB:RegWrite,MemtoReg  +opcode
  register #(.WIDTH(7)) EX_MEM_SIG 
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(id_ex_sig[6:0]),
      .q(ex_mem_sig));
  //WB:RegWrite,MemtoReg  +opcode
  register #(.WIDTH(5)) MEM_WB_SIG 
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(ex_mem_sig[4:0]),
      .q(mem_wb_sig));
  //段间寄存器
  wire [31:0] if_id_npc,if_id_ir,id_ex_npc,id_ex_a,id_ex_b,id_ex_imm,ex_mem_y,ex_mem_b,mem_wb_mdr,mem_wb_y;
  wire [4:0] id_ex_rs,id_ex_rt,id_ex_rd,ex_mem_wa,mem_wb_wa;
  register1 #(.WIDTH(32)) IF_ID_NPC
      (.clk(clk),
      .rst(rst),
      .en(IF_IDWrite),
      .flush(IF_Flush),
      .d(pc_out+4),
      .q(if_id_npc));
  
  register1 #(.WIDTH(32)) IF_ID_IR
      (.clk(clk),
      .rst(rst),
      .en(IF_IDWrite),
      .flush(IF_Flush),
      .d(instr),
      .q(if_id_ir));
  
  register #(.WIDTH(32)) ID_EX_NPC
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(if_id_npc),
      .q(id_ex_npc));
  
  register #(.WIDTH(32)) ID_EX_A
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(reg_read_data1),
      .q(id_ex_a));
  
  register #(.WIDTH(32)) ID_EX_B
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(reg_read_data2),
      .q(id_ex_b));
  
  register #(.WIDTH(32)) ID_EX_IMM
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(sign_extend),
      .q(id_ex_imm));
  
  register #(.WIDTH(5)) ID_EX_Rs
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(if_id_ir[25:21]),
      .q(id_ex_rs));
  
  register #(.WIDTH(5)) ID_EX_Rt
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(if_id_ir[20:16]),
      .q(id_ex_rt));
  
  register #(.WIDTH(5)) ID_EX_Rd
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(if_id_ir[15:11]),
      .q(id_ex_rd));
  
  register #(.WIDTH(32)) EX_MEM_Y
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(ALU_result),
      .q(ex_mem_y));
  
  register #(.WIDTH(32)) EX_MEM_B
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(m4_out),
      .q(ex_mem_b));
  
  register #(.WIDTH(5)) EX_MEM_WA
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(m3_out),
      .q(ex_mem_wa));
  
  register #(.WIDTH(32)) MEM_WB_MDR
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(mem_read_data),
      .q(mem_wb_mdr));   
  
  register #(.WIDTH(32)) MEM_WB_Y
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(ex_mem_y),
      .q(mem_wb_y));
  
  register #(.WIDTH(5)) MEM_WB_WA
      (.clk(clk),
      .rst(rst),
      .en(1),
      .d(ex_mem_wa),
      .q(mem_wb_wa));
  //数据通路
  register #(.WIDTH(32)) PC
      (.clk(clk),
      .rst(rst),
      .en(PCWrite),
      .d(pc_in),
      .q(pc_out));
  
  Instruction_memory mem1
      (.a(pc_out[9:2]),
      .spo(instr));
  
  Data_memory mem2
      (.a(ex_mem_y[9:2]),
      .d(ex_mem_b),
      .clk(clk),
      .we(ex_mem_sig[5]),
      .spo(mem_read_data));
  
  Register_File #(.WIDTH(32)) Registers
      (.clk(clk),
      .ra0(if_id_ir[25:21]),
      .rd0(reg_read_data1),
      .ra1(if_id_ir[20:16]),
      .rd1(reg_read_data2),
      .wa(mem_wb_wa),//写端口地址
      .we(mem_wb_sig[4]),
      .wd(reg_wdata));
  
  alu ALU
      (.y(ALU_result),
      .zf(Zero),
      .cf(),
      .of(),
      .sf(),
      .a(m1_out),
      .b(m2_out),
      .m(id_ex_sig[10:8]));
  alu ADD
      (.y(pc_beq),
      .zf(),
      .cf(),
      .of(),
      .sf(),
      .a(id_ex_npc),
      .b(sign_extend_shift),
      .m(3'b000));
  
  mux4 #(.WIDTH(32)) M1
      (.y(m1_out),
      .a(id_ex_a),
      .b(reg_wdata),
      .c(ex_mem_y),
      .d(0),
      .s(m1));
  mux4 #(.WIDTH(32)) M2
      (.y(m2_out),
      .a(id_ex_b),
      .b(id_ex_imm),
      .c(reg_wdata),
      .d(ex_mem_y),
      .s(m2));
  mux2 #(.WIDTH(5)) M3
      (.y(m3_out),
      .a(id_ex_rt),
      .b(id_ex_rd),
      .s(id_ex_sig[11]));
  mux4 #(.WIDTH(32)) M4
      (.y(m4_out),
      .a(id_ex_b),
      .b(ex_mem_y),
      .c(reg_wdata),
      .d(mem_read_data),
      .s(m4));
  
  mux2 #(.WIDTH(32)) M6
      (.y(reg_wdata),
      .a(mem_wb_mdr),
      .b(mem_wb_y),
      .s(mem_wb_sig[3]));
  mux2 #(.WIDTH(32)) M7
      (.y(m7_out),
      .a(pc_out+4),
      .b(pc_beq),
      .s(PCSrc));
  mux2 #(.WIDTH(32)) M8
      (.y(pc_in),
      .a(m7_out),
      .b(Jump_address),
      .s(Jump));
  mux2 #(.WIDTH(12)) M9
      (.y(m9_out),
      .a(control_sig),
      .b(12'b000000000110),
      .s(m9));
  
  assign sign_extend=if_id_ir[15]? {16'b1111_1111_1111_1111,if_id_ir[15:0]}:{16'b0000_0000_0000_0000,if_id_ir[15:0]};
  assign sign_extend_shift={id_ex_imm[29:0],2'b00};
  assign Jump_address={if_id_npc[31:28],if_id_ir[25:0],2'b00};
  assign control_sig = {RegDst,ALUm,Branch,MemRead,MemWrite,RegWrite,MemtoReg,opcode};
  assign PCSrc=(Zero)&&id_ex_sig[7];
  
  //信号产生逻辑
  //Control unit
  always@(*)
  begin
      if(rst) begin Jump=0;RegDst=0;ALUm=3'b000;Branch=0;MemRead=0;MemWrite=0;RegWrite=0;MemtoReg=0;opcode=6;end
      else 
      begin
          Jump=0;
          case(if_id_ir[31:26])
              6'b000000:begin//add
                  RegDst=1;ALUm=3'b000;Branch=0;MemRead=0;MemWrite=0;RegWrite=1;MemtoReg=1;opcode=0;
              end
              6'b001000:begin//addi
                  RegDst=0;ALUm=3'b000;Branch=0;MemRead=0;MemWrite=0;RegWrite=1;MemtoReg=1;opcode=1;
              end
              6'b100011:begin//lw
                  RegDst=0;ALUm=3'b000;Branch=0;MemRead=1;MemWrite=0;RegWrite=1;MemtoReg=0;opcode=2;
              end
              6'b101011:begin//sw
                  RegDst=0;ALUm=3'b000;Branch=0;MemRead=0;MemWrite=1;RegWrite=0;MemtoReg=0;opcode=3;
              end
              6'b000100:begin//beq
                  RegDst=0;ALUm=3'b001;Branch=1;MemRead=0;MemWrite=0;RegWrite=0;MemtoReg=0;opcode=4;
              end
              6'b000010:begin//j
                  RegDst=0;ALUm=3'b000;Branch=0;MemRead=0;MemWrite=0;RegWrite=0;MemtoReg=0;opcode=5;Jump=1;
              end
              default:begin
                  RegDst=0;ALUm=3'b000;Branch=0;MemRead=0;MemWrite=0;RegWrite=0;MemtoReg=0;opcode=6;
              end
          endcase
      end
  end
  
  //Forwarding unit
  //输入:ex_mem_sig[4],ex_mem_sig[2:0]; mem_wb_sig[4],mem_wb_sig[2:0],ex_mem_wa;mem_wb_wa;id_ex_rs;id_ex_rt;
  //id_ex_sig[2:0]
  //输出：m1,m2,m4,m5
  always@(*)
  begin
      if(rst) begin m4=0;m1=0;m2=0; end
      else
      begin
          //无相关发生时的值
          m4=0;m1=0;m2=1;
          if(id_ex_sig[2:0]==0||id_ex_sig[2:0]==4) m2=0;
  
  //需注意若lw或add或addi要写回的寄存器为$0，则相当于无相关
          if(mem_wb_sig[4]&&mem_wb_wa!=5'b00000)begin//先看WB段的数据相关；即此时WB段为lw,add,或addi
              //再根据EX段指令类型来作相应处理
              if(mem_wb_wa==id_ex_rs) m1=1;//所有指令均需要考虑rs是否读后写(当然j指令与此无关，故算在里面)
              if(mem_wb_wa==id_ex_rt&&(id_ex_sig[2:0]==0||id_ex_sig[2:0]==4))m2=2;//add和beq要考虑rt是否读后写
              if(mem_wb_wa==id_ex_rt&&id_ex_sig[2:0]==3) m4=2;//sw要考虑特殊情况 
          end
  
          if(ex_mem_sig[4]&&ex_mem_wa!=5'b00000)begin//再看MEM段的数据相关；即此时MEM段为lw,add,或addi
              //再根据EX段指令类型来作相应处理
              if(ex_mem_sig[2:0]==2)begin//lw
                  if(ex_mem_wa==id_ex_rt&&id_ex_sig[2:0]==3) m4=3;//sw要考虑特殊情况 
                  //其余的读后写则没法解决，只能通过stall
              end
              else begin//add或addi
                  if(ex_mem_wa==id_ex_rs) m1=2;//所有指令均需要考虑rs是否读后写(当然j指令与此无关，故算在里面)
                  if(ex_mem_wa==id_ex_rt&&(id_ex_sig[2:0]==0||id_ex_sig[2:0]==4))m2=3;//add和beq要考虑rt是否读后写
                  if(ex_mem_wa==id_ex_rt&&id_ex_sig[2:0]==3) m4=1;//sw则要考虑特殊情况 
              end
          end
      end
  end
  
  //Hazard detection unit
  always@(*)
  begin
      if(rst) begin PCWrite=1;IF_IDWrite=1;m9=1;IF_Flush=0;end
      else
      begin
          PCWrite=1;IF_IDWrite=1;m9=0;IF_Flush=0;
          //控制相关的stall
          if(if_id_ir[31:26]==6'b000010)begin//ID段为j
              m9=1;IF_Flush=1;
          end
          else if(if_id_ir[31:26]==6'b000100)begin//ID段为beq
              PCWrite=0;IF_IDWrite=0;IF_Flush=1;
          end
          if(id_ex_sig[2:0]==4)begin//EX段为beq
              IF_IDWrite=0;m9=1;IF_Flush=1;
          end
          //数据相关的stall:
          if(id_ex_sig[6]==1&& if_id_ir[31:26]!=6'b000010 &&id_ex_rt!=5'b00000 )begin//EX段有lw指令且ID段不为j类指令
              if(id_ex_rt==if_id_ir[25:21]) begin PCWrite=0;IF_IDWrite=0;m9=1;IF_Flush=0; end//所有5条指令都要在EX段用到rs，所以此处需stall
              else if(id_ex_rt==if_id_ir[20:16] &&(if_id_ir[31:26]==6'b000000||if_id_ir[31:26]==6'b000100)) begin PCWrite=0;IF_IDWrite=0;m9=1;IF_Flush=0;end//beq和add指令还需考虑rt
          end
      end  
  end
  
  endmodule
  ```

  ```Verilog
  module Register_File   //32x  WIDTH寄存器堆
      #(parameter WIDTH = 32)  //数据宽度
      (input clk,                   //时钟(上升沿有效)
      input [4:0] ra0,        //读端口0地址)
      output  [WIDTH-1:0] rd0, //读端口0 数据
      input [4:0] ra1,        //读端口1 地址
      output  [WIDTH-1:0] rd1, //读端口1 数据
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
  wire m1,m2;
  mux2 #(.WIDTH(32)) N1
      (.y(rd0),
      .a(registers[ra0]),
      .b(wd),
      .s(m1));
  mux2 #(.WIDTH(32)) N2
      (.y(rd1),
      .a(registers[ra1]),
      .b(wd),
      .s(m2));
  //在写使能有效时，若此周期内要写入和读取的寄存器是同一寄存器，则先写后读
  assign m1= wa==ra0 && we && wa!=5'b00000;
  assign m2 = wa==ra1 && we && wa!=5'b00000;
  
  
  always@(posedge clk)
  begin
      if(we)
          if(wa!=5'b00000)//不能对0号寄存器操作
              registers[wa] <= wd;//同步写
  end
  
  endmodule
  ```

  ```verilog
  module register1//相较于原来的register多了一个同步清零信号
      #(parameter WIDTH = 32,RST_VALUE = 0)
      (input clk,rst,en,flush,          //时钟(上升沿有效)，复位(高电平有效)，使能(高电平有效),同步清0信号
      input [WIDTH-1:0] d,        //输入数据
      output reg [WIDTH-1:0] q);  //输出数据
  
  always@(posedge clk,posedge rst)
      if(rst) q<=RST_VALUE;
      else if(flush) q<=0;
      else if(en)
          q<=d;
          
  endmodule
  ```

##### DBU的设计

* dbu的设计相对简单，和单周期、多周期中结构基本一样，只是说信号类型等有了一定变化，所以本次实验考虑不实现dbu，这也不是本次实验的重要考察点


***

### 【仿真结果及分析】

##### CPU功能仿真

仿真代码

```Verilog
module cpu_tb;
    reg clk, rst;

parameter PERIOD = 20, 	//时钟周期长度
CYCLE = 10000;		//时钟个数

cpu_aline CPU(clk,rst);

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

仿真结果及分析:（本次实验给了两份测试文件，在此处分别分析）

* test1:

  ```asm
  # 本文档存储器以字节编址
  # 本文档存储器以字节编址
  # 本文档存储器以字节编址
  # 初始PC = 0x00000000
  
  .data
      .word 0,6,0,16,0x80000000,0x80000100,0x100,5,0   #编译成机器码时，编译器会在前面多加个0，所以后面lw指令地址会多加4
  
  _start:    
  		addi $t0,$0,3       	#t0=3   0
          addi $t1,$t0,2   		#t1=5	4
  		addi $t2,$0,1       	#t2=1	8
  		addi $t3,$0,0			#t3=0	12
  
          add  $s0,$t1,$t0  		#s0=t1+t0=8  测试add指令	16
          add  $s0,$s0,$s0		#s0=s0+s0=16				20
  		lw   $s1,12($0)  		#s1=16							24
          beq  $s1,$s0,_next1		#正确跳到_next 				28
  		
  		j _fail					
  
  _next1:	
  		lw $t0, 16($0)			#t0 = 0x80000000	36
  		lw $t1, 20($0)			#t1 = 0x80000100	40
  		
  		add  $s0,$t1,$t0		#s0 = 0x00000100 = 256	44
  		lw $s1, 24($0)			#						48
          beq  $s1,$s0,_next2		#正确跳到_success		52
  		
  		j _fail
  
  _next2:
  		add $0, $0, $t2			#$0应该一直为0			60
  		beq $0,$t3,_success		#						64
  		
  		
  _fail:  
  		sw   $t3,8($0) #失败通过看存储器地址0x08里值，若为0则测试不通过，最初地址0x08里值为0   64
          j    _fail																				68
  
  _success: 
  		sw   $t2,8($0)    #全部测试通过，存储器地址0x08里值为1												72
  		j   _success       																				76
  
  					  #判断测试通过的条件是最后存储器地址0x08里值为1，说明全部通过测试
  ```

  ![图1](D:\科大\大二下\计组实验\lab5\图片\test1-1.JPG)

  如上图(图1)黄线位置的if_id_ir为20080003，为第一条addi指令的ID段，那么在往后两列的ex_mem_y里可以看到其值为3,ex_mem_wa=0x08(即$t0),在往后三列的mem_wb_wa也为0x08,mem_wb_y=3,且mem_wb_sig[4]（即addi的RegWrite信号为1，见图2黄线部分）

  ![图2](D:\科大\大二下\计组实验\lab5\图片\test1-2.JPG)

  再回到图1，接着连续5条指令均为addi和add指令(一直到140ns位置，此时最后一条add $s0,$s0,$s0指令处于ID段)。做同样的分析，每条if_id_ir值对应指令的往后数第二列的ex_mem_y,ex_mem_wa均正确，往后数第三列的mem_wb_y,mem_wb_wa,mem_wb_sig[4]也正确，说明均执行成功，数据相关被成功解决。这里以140ns处对应的if_id_ir值即指令add $s0,$s0,$s0做信号分析:此处数据相关主要是上一条指令写入的寄存器也为$s0。看图2的160ns处，m1=2,m2=3,对应数据通路中MUX1，MUX2的选择信号均选择了ex_mem_y的值，即上一条add指令将写回$s0的值，所以正确。

  ![](D:\科大\大二下\计组实验\lab5\图片\test1-3.JPG)

  再看到黄线处进入ID段的beq $s1,$s0,_next1 指令，其前一条指令为lw  $s1,12($0)，此处发生数据冲突，而且需通过stall解决，于是看到if_id_ir的值在接下来的三个cycle中分别为beq(12300001)，0 , 0。分析原因: 因为lw要写回的寄存器为$1，与beq要取的值冲突，所以产生第一次stall，此时PC，IF/ID.NPC，IF/ID..IR保持不变，并且传递空信号给ID_EX_SIG；之后可以正常执行beq指令了，但还有控制相关，此时会stall两个周期，即把IF/ID.NPC，IF/ID..IR置为0，并仅在后一个stall开始时根据beq是否成功把新的pc更新（前一个stall开始时pc保持不变，PCWrite=1），此处分析可以在仿真图中一一对应找到。并且此后if_id_ir变为add指令，说明确实跳过了j _fail的执行。

  从后代码里的分析大同小异，主要从if_id_ir入手，并对应分析该指令接下来几个cycle的表现。不过需要注意的一点是若lw，add，addi要写入的寄存器是$0,因为该寄存器不能被更改，所以在实现上相当于是没有数据相关的。

  ![](D:\科大\大二下\计组实验\lab5\图片\test1-4.JPG)

  最后是上图4(520ns时)的sw指令即之后的j指令，往后数第二列可以看到，ex_mem_y为0x08，ex_mem_b为1,说明是要把值1写入Data memory的地址MEM[2],（此时ex_mem_sig[5]（即sw指令的MemWrite信号也为1）），所以成功执行。

  j指令的执行则也很好分析，继续看if_id_ir的值，之后是循环出现08000013，00000000，ac0a0008(分别即j，空，sw指令)。理解: 空是因为j类指令的控制相关，由于j指令在ID段完成并在ID段结束时把新的地址写入PC，所以需要在j的ID段结束时将IF/ID.NPC，IF/ID.IR清零，这样j指令紧接着的那条指令就不会进入流水线，与stall的思想一致。

* test2

  ```asm
  # Test cases for MIPS 5-Stage pipeline
  
  .data
      .word 0,1,2,3,0x80000000,0x80000100,0x100,5,0
  
  _start:
      add $t1, $0, $0     # $t1 = 0
      j _test0
  
  _test0:
      addi $t2, $0, 1      # $t2 = 1
      addi $t2, $t2, 1      # $t2 = 2
      add $t2, $t2, $t2      # $t2 = 4
      addi $t2, $t2, -4     # $t2 = 0
      beq $t2, $0, _next0   # if $t2 == $0: $t1++, go next testcase, else: go fail
      j _fail
  _next0:
      addi $t1, $t1, 1      # $t1++,t1=1
      j _test1
  
  _test1:
      addi $0, $0, 4      # $0 += 4
      lw $t2, 4($0)       # $t2 = MEM[1]=00000001
      lw $t3, 8($0)       # $t3 = MEM[2]=00000002
      add $t4, $t2, $t3   # $t4=00000003  
      sw $t4, 0($0)       # MEM[0] = $t4=00000003
      lw $t5, 0($0)       # $t5 = MEM[0]=00000003
      lw $t6, 12($0)      # $t6 = MEM[3]=00000003
      beq $t5, $t6, _next1
      j _fail
  	
  _next1:
      addi $t1, $t1, 1    # $t1=2
      j _success
  
  _fail:
      j _fail
  
  _success: 
      j _success   # if success: $t1 == 2
  ```

  ![](D:\科大\大二下\计组实验\lab5\图片\test2-1.JPG)

  实际上第二个测试文件与第一个大同小异，所以重点分析一些测试1没检查到的地方。如上图1，黄线处为第一条指令add $t1, $0, $0 的ID段，其正确性检查同测试1的分析。紧跟着if_id_ir依次变为j指令以及00000000,为前面所分析的j指令机制，且此后if_id_ir变为200a0001，跳转正确（实际从代码来看，这次j指令等于没跳转）

  此后再看到180ns处的处于ID段的beq指令（beq $t2, $0, _next0），因为其前面有add和addi指令（add $t2, $t2  ；addi $t2, $t2, -4 ），发生了数据相关，而根据理解，应该取addi写回$t2的值(即0)，此处的实现时是先对WB段的指令分析数据相关，再对MEM段的指令分析数据相关，这样同时出现数据相关时就可以把WB段的重叠的影响消去。并且在200ns段，PCSrc为1，说明该cycle结束时beq计算的地址会写入PC，与预期一致。

  ![](D:\科大\大二下\计组实验\lab5\图片\test2-2.JPG)

  此时再看到图2黄线处对应的处于ID段的指令beq $t5, $t6, _next1，其前面两条指令分别为lw $t5, 0($0)  和

    lw $t6, 12($0)，均产生了数据相关。不过按照处理规则，beq会先stall一个周期以处理EX段的lw $t6, 12($0)带来的数据冲突，那么此后当beq进入EX前，lw $t5, 0($0) 已经完成了写回(这里用到了同一周期对寄存器堆同一位置先写后读的逻辑)，所以此时已经没有这个数据相关了，只需处理lw $t6, 12($0)的Forwarding，对应查看信号亦可知其正确性。

  而在最后，需特别注意的是最后一个j _success是在反复运行，那么由图2，if_id_ir在08000016和00000000间循环，pc_out在00000058和0000005c间循环，这里主要是因为j指令出现时pc写使能仍为高电平(也可以设置为低电平，使得pc_out在pc_out在00000058和00000000间循环)，不过这个都可以，因为if_id_ir是被清零了的，所以整个测试样例完全正确

##### DBU功能仿真

与单周期、多周期中仿真类似，本次实验未做要求

***

### 【下载结果及分析】

暂无

***

### 【思考题】

支持分支预测的流水线CPU的设计，并进行功能仿真和下载测试。

思路：

分支预测技术分为

* 静态预测:按讲义所述，可考虑选定预测分支是否成功，或者对于循环比较多的代码，考虑对于向后转移预测会跳转，对于向前的转移预测不会跳转。

  那么具体到代码上，则考虑对于beq指令中立即数正负在ID段做出判断，若为正，说明是向后跳转，认为其会发生，直接在ID段结束时把跳转的PC对应的指令放入IF/ID.IR（需要对应的数据通路做支持);若为负，说明是向前跳转，认为跳转不会发生，则对CPU不做特殊处理，直接PC+4的值在beq的ID段结束时放入IF/ID.IR中

* 动态预测：于静态预测相比多了一个转移预测缓冲器：

  * 1位转移预测缓冲器: 考虑在ID段查看其值来判断是否预测跳转成功，若认为成功，同理在ID段结束时把跳转的PC对应的指令放入IF/ID.IR；若认为不成功，则PC+4的值在beq的ID段结束时放入IF/ID.IR中；此外，在beq的EX段，对于实际跳转是否发生，再往缓冲器中填入相应值即可
  * 2位转移预测缓冲器：与1位的形式相同，不过对于某指令预测结果的改变，需要两位状态位作裁定，即连续两次预测与实际结果不符时才会修改该指令的转移预测方向

***

### 【实验总结】

通过本次实验，设计出了流水线cpu，相比于多周期和单周期cpu,流水线cpu的复杂程度高很多，不仅仅体现在数据通路的复杂程度上，也体现在其信号产生的逻辑分析上。通过对相关的分析，到最终数据通路作图，再到verilog搭建数据通路，到仿代码真与调试，我实现了有相关的流水线cpu，对cpu集大成的思想有了更深刻的理解，对理论课上所学的cpu知识有了全面的回顾与理解的翻新。不过自己的cpu仍有改进空间，比如分支预测，以及添加一些新的指令，以及降低控制相关的延迟等等，这些内容都是值得我课下多多思考的。

***

### 【意见建议】

对于本次实验整体较为满意，实验完成后自我感觉所达到的效果与实验目的基本一致。不过可能对于数据通路给的很零散，导致实验难度有一定增加(也算一种锻炼吧)