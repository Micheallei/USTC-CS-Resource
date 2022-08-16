`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: USTC ESLAB 
// Engineer: Wu Yuzhang
// 
// Design Name: RISCV-Pipline CPU
// Module Name: ControlUnit
// Target Devices: Nexys4
// Tool Versions: Vivado 2017.4.1
// Description: RISC-V Instruction Decoder
//////////////////////////////////////////////////////////////////////////////////
//功能和接口说明
    //ControlUnit       是本CPU的指令译码器，组合逻辑电路
//输入
    // Op               是指令的操作码部分
    // Fn3              是指令的func3部分
    // Fn7              是指令的func7部分
//输出
    // JalD==1          表示Jal指令到达ID译码阶段
    // JalrD==1         表示Jalr指令到达ID译码阶段
    // RegWriteD        表示ID阶段的指令对应的寄存器写入模式
    // MemToRegD==1     表示ID阶段的指令需要将data memory读取的值写入寄存器,
    // MemWriteD        共4bit，为1的部分表示有效，对于data memory的32bit字按byte进行写入,MemWriteD=0001表示只写入最低1个byte，和xilinx bram的接口类似
    // LoadNpcD==1      表示将NextPC输出到ResultM
    // RegReadD         表示A1和A2对应的寄存器值是否被使用到了，用于forward的处理
    // BranchTypeD      表示不同的分支类型，所有类型定义在Parameters.v中
    // AluContrlD       表示不同的ALU计算功能，所有类型定义在Parameters.v中
    // AluSrc2D         表示Alu输入源2的选择
    // AluSrc1D         表示Alu输入源1的选择
    // ImmType          表示指令的立即数格式
//实验要求  
    //补全模块  

`include "Parameters.v"   
module ControlUnit(
    input wire [6:0] Op,
    input wire [2:0] Fn3,
    input wire [6:0] Fn7,
    output wire JalD,//
    output wire JalrD,//
    output reg [2:0] RegWriteD,//
    output wire MemToRegD,//
    output reg [3:0] MemWriteD,//
    output wire LoadNpcD,//
    output reg [1:0] RegReadD,//
    output reg [2:0] BranchTypeD,//
    output reg [3:0] AluContrlD,//
    output wire [1:0] AluSrc2D,//
    output wire AluSrc1D,//
    output reg [2:0] ImmType   //     
    ); 
    
    //首先把能快速确定的信号先直接确定下来
    assign LoadNpcD = JalD | JalrD;
    assign JalrD = (Op==7'b1100111)?1'b1:1'b0;
    assign JalD = (Op==7'b1101111)?1'b1:1'b0;
    assign AluSrc1D = (Op==7'b0010111)?1'b1:1'b0; //只有AUIPC指令才会为1
    assign AluSrc2D = ( (Op==7'b0010011)&&(Fn3[1:0]==2'b01) )?(2'b01):(((Op==7'b0110011)||(Op==7'b1100011))?2'b00:2'b10);
    //AluSrc2D: 取01时，对应图里的Rs1E, 即SRAI的这条算术右移指令； 正常R类指令或者B类型指令则需要reg2参与算术运算，所以信号为00； 否则为10，对应ImmE
    assign MemToRegD = (Op==7'b0000011)?1'b1:1'b0; //load指令的Op都一样，而且只有load指令会有mem to reg操作


    always @(*) 
    begin
        case(ImmType) //ImmType用于判断用到的是哪种立即数扩展方式，其实也就是表明当前是什么类型的指令，然后据此设置A1和A2寄存器的使用情况，当然Immtype的值还需根据指令类型来设置后才能在这里用到
            `RTYPE: RegReadD = 2'b11;
            `ITYPE: RegReadD = 2'b10;
            `STYPE: RegReadD = 2'b11;
            `BTYPE: RegReadD = 2'b11;
            `UTYPE: RegReadD = 2'b00;
            `JTYPE: RegReadD = 2'b00;
            default:RegReadD = 2'b00;                                      
        endcase
    end
    
    always@(*)
    case(Op)
        7'b0110011:begin    //R类型指令
            RegWriteD<=`LW;
            MemWriteD<=4'b0000;
            ImmType<=`RTYPE;
            BranchTypeD <= `NOBRANCH;
            case(Fn3)
                3'b000:begin
                    if(Fn7[5]==1)
                        AluContrlD<=`SUB;   //SUB
                    else
                        AluContrlD<=`ADD;   //ADD
                end
                3'b001:AluContrlD<=`SLL;    //SLL
                3'b010:AluContrlD<=`SLT;    //SLT
                3'b011:AluContrlD<=`SLTU;    //SLTU
                3'b100:AluContrlD<=`XOR;    //XOR
                3'b101:begin
                    if(Fn7[5]==1)
                        AluContrlD<=`SRA;   //SRA
                    else
                        AluContrlD<=`SRL;   //SRL
                end  
                3'b110:AluContrlD<=`OR;    //OR
                default:AluContrlD<=`AND;    //AND   3'b111                                       
            endcase
        end
        7'b0010011:begin    //除去load之外的I类指令
            RegWriteD<=`LW; //I类指令都需要32bit写回reg
            MemWriteD<=4'b0000;
            ImmType<=`ITYPE;
            BranchTypeD <= `NOBRANCH;
            case(Fn3)
                3'b000:AluContrlD<=`ADD;  //ADDI
                3'b001:AluContrlD<=`SLL;  //SLLI
                3'b010:AluContrlD<=`SLT;  //SLTI
                3'b011:AluContrlD<=`SLTU;   //SLTIU
                3'b100:AluContrlD<=`XOR;    //XORI
                3'b101:
                    if(Fn7[5]==1)
                        AluContrlD<=`SRA;   //SRAI
                    else
                        AluContrlD<=`SRL;   //SRLI
                3'b110:AluContrlD<=`OR;   //ORI
                default:AluContrlD<=`AND;    //ANDI     3'b111                                                    
            endcase
        end
        7'b0000011:begin    //load指令，将寄存器rs1与符号扩展的12位偏移量下相加得到有效地址，将存储器的一个值复制到寄存器rd中
            MemWriteD<=4'b0000;
            AluContrlD<=`ADD;
            ImmType<=`ITYPE;
            BranchTypeD <= `NOBRANCH;
            case(Fn3)//根据func3确定写入位数，如LB为读取一个8位数值，符号扩展到32位
                3'b000:RegWriteD<=`LB;    //LB
                3'b001:RegWriteD<=`LH;    //LH
                3'b010:RegWriteD<=`LW;    //LW
                3'b100:RegWriteD<=`LBU;    //LBU
                default:RegWriteD<=`LHU;    //LHU  3'b101                                                            
            endcase
        end
        7'b0100011:begin    //S型装载指令，将寄存器rs1与符号扩展的12位偏移量下相加得到有效地址，将寄存器rs2的值复制到存储器中
            RegWriteD<=`NOREGWRITE;
            AluContrlD<=`ADD;
            ImmType<=`STYPE; 
            BranchTypeD <= `NOBRANCH;
            case(Fn3)
                3'b000:MemWriteD<=4'b0001;    //SB
                3'b001:MemWriteD<=4'b0011;    //SH
                default:MemWriteD<=4'b1111;   //SW    3'b010                                                       
            endcase
        end
        7'b1100011:begin    //BRANCH
            RegWriteD<=`NOREGWRITE;
            MemWriteD<=4'b0000;
            ImmType<=`BTYPE;
            AluContrlD<=`ADD;
            case(Fn3) //根据func3判断是哪种B类指令
                3'b000:BranchTypeD<=`BEQ;    //BEQ
                3'b001:BranchTypeD<=`BNE;    //BNE
                3'b100:BranchTypeD<=`BLT;    //BLT
                3'b101:BranchTypeD<=`BGE;    //BGE 
                3'b110:BranchTypeD<=`BLTU;    //BLTU
                default:BranchTypeD<=`BGEU;    //BGEU 3'b111                                                        
            endcase 
        end
        7'b0110111:begin    //LUI
            RegWriteD<=`LW;
            MemWriteD<=4'b0000;
            AluContrlD<=`LUI;
            ImmType<=`UTYPE;
            BranchTypeD <= `NOBRANCH;  
        end 
        7'b0010111:begin    //AUIPC
            RegWriteD<=`LW;
            MemWriteD<=4'b0000;
            AluContrlD<=`ADD;
            ImmType<=`UTYPE;
            BranchTypeD <= `NOBRANCH;
        end
        7'b1101111:begin    //JAL
            RegWriteD<=`LW;
            MemWriteD<=4'b0000;
            AluContrlD<=`ADD;
            ImmType<=`JTYPE;
            BranchTypeD <= `NOBRANCH;    
        end
        7'b1100111:begin    //JALR
            RegWriteD<=`LW;
            MemWriteD<=4'b0000;
            AluContrlD<=`ADD;
            ImmType<=`ITYPE;
            BranchTypeD <= `NOBRANCH;      
        end
        default:begin       //默认情况即不写回寄存器且不写回内存
            RegWriteD<=`NOREGWRITE;
            MemWriteD<=4'b0000;
            AluContrlD<=`ADD;
            ImmType<=`ITYPE;
            BranchTypeD <= `NOBRANCH;
        end
    endcase
endmodule

