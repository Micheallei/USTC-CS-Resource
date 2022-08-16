`timescale 1ns / 1ps



    input [31:0] PCE,//EX??PC?
    input [31:0] PredictedPC,//???PC?
    input PredictedBTB,//BTB?????????
    input PredictedBHT,//BHT?????????
    input PredictedE,//
    );
    
    always@(*)
    begin
        if(JalrE)
            PC_In <= JalrTarget;
        else if(BranchE & ~PredictedE) //?EX????????br??????IF??????????????pc??????
            PC_In <= BranchTarget;
        else if(~BranchE & PredictedE) //?EX????????br??????IF??????????????????
            PC_In <= PCE + 4;
        else if(JalD)
            PC_In <= JalTarget;
        else if(PredictedBHT & PredictedBTB)//?ID?EX???????????????????IF???BHT?BTB?????
            PC_In <= PredictedPC;
        else
            PC_In <= PCF+4;
    end
endmodule




module NPC_Generator(
    input wire [31:0] PCF,JalrTarget, BranchTarget, JalTarget,
    input wire BranchE,JalD,JalrE,
    output reg [31:0] PC_In,
    // BTB
    input wire PredictedF, PredictedE,
    input wire [31:0] PredictedTargetF, PCE
    );
    
    // 请补全此处代码
    always@(*)
    begin
        //注意这里判断的顺序体现了优先级，EX阶段的优先级高于ID段,具体原因可参考lab1报告的分析
        if (JalrE)
            PC_In <= JalrTarget;
        else if(BranchE && ~PredictedE) // 预测不跳转，实际跳转 
            PC_In <= BranchTarget;
        else if(~BranchE && PredictedE) // 预测跳转，实际不跳转
            PC_In <= PCE + 4;
        else if(JalD) 
            PC_In <= JalTarget;
        else if(PredictedF) // 预测即跳转
            PC_In <= PredictedTargetF;
        else
            PC_In <= PCF + 4;
    end
    
endmodule
