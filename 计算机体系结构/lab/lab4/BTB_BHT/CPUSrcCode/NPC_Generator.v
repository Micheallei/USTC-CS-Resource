`timescale 1ns / 1ps



module NPC_Generator(
    input wire [31:0] PCF,JalrTarget, BranchTarget, JalTarget,
    input wire BranchE,JalD,JalrE,
    output reg [31:0] PC_In,
    // BTB
    input wire PredictedF, PredictedE,
    input wire [31:0] PredictedTargetF, PCE,
    // BHT
    input PredictedTakenF, PredictedTakenE
    );
    
    // 请补全此处代码
    always@(*)
    begin
        //注意这里判断的顺序体现了优先级，EX阶段的优先级高于ID段,具体原因可参考lab1报告的分析
        if (JalrE)
            PC_In <= JalrTarget;
        else if((~PredictedE || PredictedE && ~PredictedTakenE) && BranchE) // 没预测或预测不跳转，但实际跳转了
            PC_In <= BranchTarget;
        else if(~BranchE && PredictedE && PredictedTakenE) // 预测跳转，实际不跳转
            PC_In <= PCE + 4;
        else if(JalD) 
            PC_In <= JalTarget;
        else if(PredictedF && PredictedTakenF) // 预测且预测跳转
            PC_In <= PredictedTargetF;
        else
            PC_In <= PCF + 4;
    end
    
endmodule
