`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: USTC ESLAB 
// Engineer: Wu Yuzhang
// 
// Design Name: RISCV-Pipline CPU
// Module Name: ALU
// Target Devices: Nexys4
// Tool Versions: Vivado 2017.4.1
// Description: ALU unit of RISCV CPU
//////////////////////////////////////////////////////////////////////////////////

//功能和接口说明
	//ALU接受两个操作数，根据AluContrl的不同，进行不同的计算操作，将计算结果输出到AluOut
	//AluContrl的类型定义在Parameters.v中
//推荐格式：
    //case()
    //    `ADD:        AluOut<=Operand1 + Operand2; 
    //   	.......
    //    default:    AluOut <= 32'hxxxxxxxx;                          
    //endcase
//实验要求  
    //补全模块

`include "Parameters.v"   
module ALU(
    input wire [31:0] Operand1,
    input wire [31:0] Operand2,
    input wire [3:0] AluContrl,
    output reg [31:0] AluOut
    );    
    
    wire signed [31:0] Operand1S = $signed(Operand1);
    wire signed [31:0] Operand2S = $signed(Operand2);
    always@(*)
    case(AluContrl)
        `ADD:        AluOut<=Operand1 + Operand2; 
        `SUB:        AluOut<=Operand1 - Operand2;
        `XOR:        AluOut<=Operand1 ^ Operand2;
        `OR:         AluOut<=Operand1 | Operand2;   
        `AND:        AluOut<=Operand1 & Operand2;
        `SLL:        AluOut<=Operand1<<(Operand2[4:0]); //SLL SRL SRA移位操作和"寄存器-立即数"指令一致，将r2的低5位作为立即数即可
        `SRL:        AluOut<=Operand1>>(Operand2[4:0]);
        `SRA:        AluOut<=Operand1S >>> (Operand2[4:0]); // >>> 是算术右移
        `SLT:        AluOut<=Operand1S < Operand2S ? 32'd1:32'd0;  //SLT/SLTU: 如果rs1<rs2，rd写1; 否则rd为0
        `SLTU:       AluOut<=Operand1 < Operand2 ? 32'd1:32'd0;
        `LUI:        AluOut<={ Operand2[31:12],12'b0 };//将20位的U立即数放到目标寄存器rd的31-12位，rd的低12位填0
        default:    AluOut <= 32'hxxxxxxxx;                          
    endcase

endmodule

