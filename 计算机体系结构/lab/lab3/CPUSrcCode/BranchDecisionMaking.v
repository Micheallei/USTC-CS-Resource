`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: USTC ESLAB 
// Engineer: Wu Yuzhang
// 
// Design Name: RISCV-Pipline CPU
// Module Name: BranchDecisionMaking
// Target Devices: Nexys4
// Tool Versions: Vivado 2017.4.1
// Description: Decide whether to branch 
//////////////////////////////////////////////////////////////////////////////////
//功能和接口说明
    //BranchDecisionMaking接受两个操作数，根据BranchTypeE的不同，进行不同的判断，当分支应该taken时，令BranchE=1'b1
    //BranchTypeE的类型定义在Parameters.v中
//推荐格式：
    //case()
    //    `BEQ: ???
    //      .......
    //    default:                            BranchE<=1'b0;  //NOBRANCH
    //endcase
//实验要求  
    //补全模块
 
`include "Parameters.v"   
module BranchDecisionMaking(
    input wire [2:0] BranchTypeE,
    input wire [31:0] Operand1,Operand2,
    output reg BranchE
    );
    
    // 请补全此处代码
    //注意wire默认是无符号类型，所以在进行有符号的大小比较时需作转换，而=和!=有符号无符号比较都一样
    wire signed [31:0] Operand1S = $signed(Operand1);
    wire signed [31:0] Operand2S = $signed(Operand2);
    always@(*)
    case(BranchTypeE)
        `BEQ: if(Operand1==Operand2) BranchE<=1'b1;  //BEQ
              else                   BranchE<=1'b0;
        `BNE: if(Operand1!=Operand2) BranchE<=1'b1;  //BNE
              else                   BranchE<=1'b0;   
        `BLT: if(Operand1S<Operand2S)BranchE<=1'b1;  //BLT
              else                   BranchE<=1'b0;
        `BGE: if(Operand1S>=Operand2S)BranchE<=1'b1;  //BGE
              else                   BranchE<=1'b0;
        `BLTU:if(Operand1<Operand2)  BranchE<=1'b1;  //BLTU
              else                   BranchE<=1'b0;
        `BGEU:if(Operand1>=Operand2) BranchE<=1'b1;  //BGEU
              else                   BranchE<=1'b0;
        default:                     BranchE<=1'b0;  //NOBRANCH                            
    endcase

endmodule

