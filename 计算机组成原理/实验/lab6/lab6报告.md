# 中国科学技术大学计算机学院**

## **《计算机组成与原理》报告**

##### 实验题目：综合设计

##### 学生姓名：雷雨轩

##### 学生学号：PB18111791

##### 完成日期：2020-7-9

***

### 【实验目的】

* 理解计算机系统的组成结构和工作原理；
* 理解计算机总线和接口的结构和功能；
* 掌握软硬件综合系统的设计和调试方法。

***

### 【逻辑设计分析与代码】

##### 总线及IO接口设计

* 思路：采用单周期CPU，把dataMem从CPU中隔离开，把数据输入和输出以及地址，控制当成接口输出到外面，并由总线控制器来判断是Mem还是IO，对IO的读写仍用lw，sw指令

  高位片选：

  * 高位为0，选择Data memory，接口就同单周期CPU的Data mem接口

  * 高位为1，选择IO接口，目前仅设置输入和输出

    lw指令：大体就是若是对IO进行lw指令，则相当于要求用户输入，那么cpu会原地等待，当用户通过开关选好数据，并通过按钮按下确认后，数据会存入lw指令所要求的寄存器中。

    具体实现细节：信号设置，传到总线控制器的组合逻辑进行判断，如果是IO，那么反馈回来一个pc_en信号，这样在周期结束时，pc不会更新；当前周期结束后，cpu进入WAIT_IO状态，所有控制信号全为0，即什么也不干。另一方面，该周期结束后，CR寄存器被置1（用button+rd做片选信号)，然后等待一个button信号来临时，表明有数据输入了，此时button信号持续的周期结束时，CR被置0，并把数据装载进入DIR(其写使能由button控制)；此时，进入下一周期后，判断到CR为0了，处于WAIT_IO状态的就会在下一周期转回正常，并在当前周期把lw指令的信号拿出来，在该周期结束时把数据写入寄存器；同时pc_en信号置为1，在该周期结束时同步开始更新pc

    

    sw指令：考虑对Data mem和DOR的写使能做操作，address[8]即为选择信号(当然前提均需要wr有效)

    如果是存入存储器（address[8]），则会在周期结束时写回存储器；

    若是IO，则寄存器中数据直接在周期结束时放入DOR寄存器中，然后对接到LED或者数码管之类的

* 与外设连接：

  当address高位选到IO接口时，规定根据低位地址选择所用到的IO设备：

  ​	0：选择开关输入或led输出

  ​	1：选择键盘输入或数码管输出

  ​	2:  选择Vga输出

  不过此处只用到了开关输入和LED输出

  这样的约定也使得IO访存指令有了约束：如选择0: lw $t0, 1024($0),sw同理   （1024即二进制0000 0100 0000 0000）

  

##### 汇编程序设计：

* 有了总线及接口设计处的定义，那么程序设计就迎刃而解了

* 冒泡排序程序设计：

  * 阅读相关文档(MARS官方文档)以及熟悉常用MIPS指令后，在C语言的冒泡排序基础上完成了MIPS的代码

  * 需注意的几个点:

    * MIPS汇编格式:`.data`和`.text`

    * C语言循环与汇编的对应:

      ```c
      for(i=0;i<n-1;i++){//比较次数为n-1
          for(j=0;j<n-1-i;j++){
              if(a[j]>a[j+1]){
                  temp=a[j];
                  a[j]=a[j+1];
                  a[j+1]=temp;
              }
          }
      }
      ```

      则为MIPS寄存器的数据存放做如下规定：

      ```
      t0:i; t1:j;	t2:n-1(n为排序总数); t3:a[j]; t4:a[j+1]; t6:n-1-i; t7:当前参加比较的两数据的起始地址; t9:数据基地址(默认以0x00000000开始的内存地址中，一个数据占一个字)
      ```

* 最终代码：

  ```asm
  .data
  #no data
  .text
  ####enter n and put n-1 into $t2
  _main: 
          lw $t2,1024($0)                      #IO读
          addi $t2,$t2,-1
  ####设置i，temp,地址，调用syscall输出当前时间
  li $t0,-1
  li $t9,0x00000000
  
  ####外循环
  _loop1:
          addi $t0,$t0,1
          beq $t0,$t2,_done
          li $t1,0
          li $t7,0xfffffffc
          sub $t6,$t2,$t0
  ####内循环
  _loop2: 
          beq $t1,$t6,_loop1
          addi $t7,$t7,4
          lw $t3,0($t7)
          lw $t4,4($t7)
          ble	$t3,$t4,_target #<=才跳转
          sw $t4, 0($t7)
          sw $t3, 4($t7)
  _target: 
          addi $t1,$t1,1
          j _loop2
  
  ####end
  _done: 
          lw $t0,0($0)    #将排序后的最小值存入$t0
          sw $t0,1024($0) #输出排序后的最小值
          j _done
  ```

  通过Mars汇编后得到需要在原来cpu基础上新加的指令：**这些指令均在项目里实现了**

  * ori：将16位立即数immediate进行无符号扩展至32位，与rs寄存器里的值进行“或”运算，结果放入rt寄存器中
  * addiu:add立即数，不过是无符号扩展
  * addu:add两寄存器的数，不过均为无符号数
  * sub
  * sll
  * slt

  | ori   | 001101 | rs   | rt   | immediate |                 | $1=$2 \| 10 | rt <- rs \| (zero-extend)immediate ；其中rt=$1,rs=$2 |
  | ----- | ------ | ---- | ---- | --------- | --------------- | ----------- | ---------------------------------------------------- |
  | addiu | 001001 | rs   | rt   | immediate | addiu $1,$2,100 | $1=$2+100   | rt <- rs + (zero-extend)immediate ；其中rt=$1,rs=$2  |

  | addu | 000000 | rs   | rt   | rd   | 00000 | 100001 | addu $1,$2,$3 | $1=$2+$3 | rd <- rs + rt  ；其中rs＝$2，rt=$3, rd=$1,无符号数 |
  | ---- | ------ | ---- | ---- | ---- | ----- | ------ | ------------- | -------- | -------------------------------------------------- |
  | sub  | 000000 | rs   | rt   | rd   | 00000 | 100010 | sub $1,$2,$3  | $1=$2-$3 | rd <- rs - rt  ；其中rs＝$2，rt=$3, rd=$1          |

  |      |        |      |      |      |       |        |              |                              |                                                        |
  | ---- | ------ | ---- | ---- | ---- | ----- | ------ | ------------ | ---------------------------- | ------------------------------------------------------ |
  | slt  | 000000 | rs   | rt   | rd   | 00000 | 101010 | slt $1,$2,$3 | if($2<$3)   $1=1 else   $1=0 | if (rs < rt) rd=1 else rd=0 ；其中rs＝$2，rt=$3, rd=$1 |

##### 最终成果

除了以上特别的设计外，对有无符号扩展做了模块化完善；此外还有对button信号的处理，即去抖动和取边沿信号两个模块；

最后只需把所附文件里的coe代码放入vivado工程的Instruction mem中，即可运行整个项目：

先是通过16个开关和板子上一个按钮来输入数据(即参与排序数据的个数，排序的数据需实现把相应coe文件加载到Data memory中)；然后程序运行到结尾，把排序后最小的数的值显示到led

##### 数据通路:

主要就是在单周期CPU的基础上，把Data memory 换成了总线控制器，然后再与IO接口相接

![](D:\科大\大二下\计组实验\lab6\图片\数据通路.JPG)

![](D:\科大\大二下\计组实验\lab6\图片\1.JPG)

不含上图的SR寄存器

![](D:\科大\大二下\计组实验\lab6\图片\2.JPG)

控制we改为rd和wr（读和写信号）

##### 代码及注释

cpu代码：

```Verilog
module cpu_one_cycle    //单周期CPU
    (input clk,         //时钟(上升沿有效)
    input rst           //异步复位，高电平有效
    input [31:0] din,
    input pc_en,
    input cr,
    output [8:0] address,
    output [31:0] dout,
    output reg rd,
    output reg wr
    );

reg RegDst,Jump,Branch,MemtoReg,ALUSrc,RegWrite,ext_sig;//ext_sig是符号扩展的信号，为1时有符号扩展，否则无符号扩展
reg [1:0]ALUOp;
//绝大多数命名与数据通路上标注一致
wire [31:0] pc_out,pc_in;//pc寄存器的输入和输出
wire [31:0] instr; //当前周期的指令
wire [31:0] ALU_result;
reg [2:0] ALU_control;
wire m3,Zero;
//wire [31:0] mem_read_data;
wire [31:0] w1,w2,w4,w5,Jump_address,extend_result;
//寄存器堆参数
wire [4:0] write_reg;
wire [31:0] reg_write_data,reg_read_data1,reg_read_data2;

wire en_pc;//pc寄存器写使能
parameter EXE=1'b0,WAIT_IO=1'b1
reg cur_state,next_state;


assign address = ALU_result[10:2];
assign dout = reg_read_data2;


Instruction_memory mem1
    (.a(pc_out[9:2]),
    .spo(instr));


register #(.WIDTH(32)) PC
    (.clk(clk),
    .rst(rst),
    .en(pc_en),
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
    .b(extend_result),
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
    .b(din),
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

extend ext
    (.sign_ext(ext_sig)
    .immediate(instr[15:0])
    .result(extend_result)
    )

assign w1=pc_out+4;
assign Jump_address={w1[31:28],instr[25:0],2'b00};
//assign sign_extend=instr[15]? {16'b1111_1111_1111_1111,instr[15:0]}:{16'b0000_0000_0000_0000,instr[15:0]};
//assign zero_extend={16'b0000_0000_0000_0000,instr[15:0]};
assign w4={extend_result,2'b00} + w1;
assign m3=Branch & Zero;


always@(posedge clk,posedge rst)
begin
    if(rst) cur_state<=EXE;
    else cur_state<=next_state;
end

//next_state logic
always@(*)
begin
    case(state)
        EXE:begin
            if(pc_en) next_state=WAIT_IO;
            else next_state=EXE;
        end
        WAIT_IO:begin
            if(!cr) next_state=EXE;
            else next_state=WAIT_IO;
        end
    endcase 
end


//control_unit,output logic
always@(*)
begin
    ext_sig=1;
    RegDst=0;
    ALUSrc=0;
    MemtoReg=0;
    RegWrite=0;
    rd=0;
    wr=0;
    Branch=0;
    //ALUOp=2'b00;
    Jump=0;
    ALU_control=3'b000;
    case(cur_state)
        EXE:begin
            if(rst)begin
                RegDst=0;
            end
            else 
            begin
                case (instr[31:26])
                    6'b001101:begin//ori
                        ext_sig=0;
                        //RegDst=0;
                        ALUSrc=1;
                        //MemtoReg=0;
                        RegWrite=1;
                        //rd=0;
                        //wr=0;
                        //Branch=0;
                        //ALUOp=2'b10;
                        //Jump=0;
                        ALU_control=3'b011;
                    end
                    6'b001001:begin//addiu
                        ext_sig=0;
                        //RegDst=0;
                        ALUSrc=1;
                        //MemtoReg=0;
                        RegWrite=1;
                        //rd=0;
                        //wr=0;
                        //Branch=0;
                        //ALUOp=2'b10;
                        //Jump=0;
                        //ALU_control=3'b000;
                    end
                    6'b000000:begin
                        case(instr[5:0])
                            6'b100000:begin//add
                                RegDst=1;
                                //ALUSrc=0;
                                //MemtoReg=0;
                                RegWrite=1;
                                //rd=0;
                                //wr=0;
                                //Branch=0;
                                //ALUOp=2'b10;
                                //Jump=0;
                            end
                            6'b100001:begin//addu
                                RegDst=1;
                                RegWrite=1;
                            end
                            6'b100010:begin//sub
                                RegDst=1;
                                RegWrite=1;
                                ALU_control=3'b001;
                            end
                            6'b101010:begin//slt
                                RegDst=1;
                                RegWrite=1;
                                ALU_control=3'b101;
                            end
                        endcase
                    end
                        
                    6'b001000:begin//addi
                        if(instr[20:16]!=0)begin
                            //RegDst=0;
                            ALUSrc=1;
                            //MemtoReg=0;
                            RegWrite=1;
                            //rd=0;
                            //wr=0;
                            //Branch=0;
                            //ALUOp=2'b10;
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
                            rd=1;
                            //wr=0;
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
                        //rd=0;
                        wr=1;
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
                        //rd=0;
                        //wr=0;
                        Branch=1;
                        //ALUOp=2'b01;
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

        WAIT_IO:begin
            if(cr)begin
                RegDst=0;
            end
            else begin
                ALUSrc=1;
                MemtoReg=1;
                RegWrite=1;
                //rd=1;
            end
            
        end 
end
endmodule
```

总线及IO接口:

```Verilog
//总线控制器
module BUS_ctrl    //输入输出定义分别是来自CPU的信号和要向CPU输出的信号
    (input clk,         //时钟(上升沿有效)
    input rst           //异步复位，高电平有效
    input [15:0] sw,//开关
    input button,//按钮
    output [31:0] din,//输入到CPU的数据
    output [15:0] led,//输出到led的数据
    output req//请求外设输入数据的信号
    );

wire button_clean,button_redge;//去抖动，取边沿信号

//写：管写信号即可； 读：管输出数据即可
wire [8:0] address;
wire [31:0] dout;
wire rd,wr;
wire  [31:0] din;
wire [31:0] mem_read_data;
wire pc_en;
wire cr;
wire [15:0] dir;



deburring debur1//去抖动例化
    (.clk (clk),
    .button (button),
    .button_clean (button_clean));

signal_edge sig1        //取边沿例化
    (.clk (clk),
    .button(button_clean),
    .button_redge(button_redge));   

cpu_one_cycle cpu
    (.clk(clk)         //时钟(上升沿有效)
    .rst(rst)           //异步复位，高电平有效
    .din(din),
    .pc_en(pc_en),
    .cr(cr),
    .address(address),//out
    .dout(dout),
    .rd(rd),
    .wr(wr));

Data_memory mem2
    (.a(address[7:0]),
    .d(dout),
    .clk(clk),
    .we(wr&&(~address[8])),
    .spo(mem_read_data));

register #(.WIDTH(16)) DIR
    (.clk(clk),
    .rst(rst),
    .en(button_redge),
    .d(sw),
    .q(dir));

register #(.WIDTH(16)) DOR
    (.clk(clk),
    .rst(rst),
    .en(wr&&address[8]),
    .d(dout[15:0]),
    .q(led));

register #(.WIDTH(1)) CR
    (.clk(clk),
    .rst(rst),
    .en(1),
    .d(cr_in),
    .q(cr));

mux2 #(.WIDTH(1)) N1
    (.y(cr_in),
    .a(0),
    .b(1),
    .s(rd&&(~button_redge)));

assign din = address[8]? {16'b0,dir} : mem_read_data;
//组合逻辑
always@(*)
begin
    if(rst) pc_en=1;
    //pc_en=1;
    if(address[8]&&cr==0)
        pc_en=~pc_en;
    end
end


endmodule
```

其余代码等放在附件里

***

### 【仿真结果及分析】

考虑到没有FPGA板子，所以本次实验没有仿真和下载，但是附上对汇编代码的验证：

运行前：（重点关注下方0x10010100行的数据）

![](D:\科大\大二下\计组实验\lab6\图片\Mars测试1.JPG)

运行后：该行5个数据被成功排序

![](D:\科大\大二下\计组实验\lab6\图片\Mars测试2.JPG)



### 【实验总结】

通过本次实验，对于计算机总线和接口的结构和功能有了深入了解，对掌握软硬件综合系统的设计和调试方法有了更浓厚的兴趣，想要更深入的学习。

***

### 【意见建议】

对于本次实验整体较为满意，实验完成后自我感觉所达到的效果与实验目的基本一致。