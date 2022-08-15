#include <iostream>
#include <fstream>
#include<stdio.h>
#include <string>
using namespace std;
#define MEMORY_MAX 65536

int memory[MEMORY_MAX];
int breakpoints[MEMORY_MAX]={0}; //断点集合
int bknum=0;//断点个数

enum REG{
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC,
    //R_IR,
    //R_MAR,
    //R_MDR,
    R_COND,//条件码寄存器
    //R_PSR,
    R_COUNT
};

int reg[R_COUNT]; //寄存器


enum OPCODES{//指令顺序与其四位操作码的十进制数值对应
    OP_BR = 0, /*branch ,0000*/
    OP_ADD,    /* add  ,0001*/
    OP_LD,     /* load ,0010*/
    OP_ST,     /* store ,0011*/
    OP_JSR,    /* jump register,0100 */
    OP_AND,    /* bitwise and ,0101*/
    OP_LDR,    /* load register,0110 */
    OP_STR,    /* store register,0111 */
    OP_RTI,    /* unused ,1000*/
    OP_NOT,    /* bitwise not ,1001 */
    OP_LDI,    /* load indirect ,1010*/
    OP_STI,    /* store indirect, 1011 */
    OP_JMP,    /* jump ,1100*/
    OP_RES,    /* reserved (unused), 1101 */
    OP_LEA,    /* load effective address, 1110 */
    OP_TRAP    /* execute trap ,1111*/
};

enum COND{
    C_N,
    C_Z,
    C_P,
};

enum TRAP{
    TRAP_GETC = 0x20,//从键盘读入一个字符，但不会回显屏幕，ASCII码值给R0
    TRAP_OUT = 0x21,//将R0[7:0]的字符输出在屏幕上显示
    TRAP_PUTS = 0x22,//向屏幕写一个字符串，存放在连续内存中，起始地址由R0指定,直到0x0000时结束
    TRAP_IN = 0x23,//打印提示，等待键盘输入一个字符，并回显，装入R0
    TRAP_PUTSP = 0x24,//屏幕写一个字符串，每个内存有两个字符，先bit[7:0],再bit[15:8]，起始地址由R0决定，0x0000时结束
    TRAP_HALT = 0x25
};


int sign_extend(int x, int bit_count){//将bit_count位的x有符号扩展为16位,x为指令
    if((x >> (bit_count - 1)) & 1){
        x = (x | (0xFFFF << bit_count))&0xFFFF;
    }
    return x;
}

void setcc(int r)    //根据相应寄存器的值来判定如何设置条件码
{
    if (reg[r] == 0)
    {
        reg[R_COND] = C_Z;
    }
    else if (reg[r] >> 15) //右移15位还是正数说明第16位为1，则相当于是负数
    {
        reg[R_COND] = C_N;
    }
    else
    {
        reg[R_COND] = C_P;
    }
}

int string_to_int(string s) {
	int i = 0;
	int result = 0;
	while (i < 15) {
		if (s[i] == '1') {
			result = (result + 1) * 2;
		}
		else {
			result = result * 2;
		}
		i++;
	}
	if (s[i] == '1') result += 1;
	return result;
}


int mem_read(int address){//根据地址读取相应内存
    return memory[address];
}

void mem_write(int address, int value){
    memory[address]=value;
}

void load_file(ifstream f){//从文件把数据读入到内存中
    string temp;
    getline(myfile,temp);
    int PC_NOW;
    PC_NOW = string_to_int(temp);
    reg[R_PC] = PC_NOW;
    while(getline(myfile,temp)){
        reg[PC_NOW++] = string_to_int(temp);
    }
    reg[R_COND] = C_Z;
    reg[R_R0] = 0;
    reg[R_R1] = 0;
    reg[R_R2] = 0;
    reg[R_R3] = 0;
    reg[R_R4] = 0;
    reg[R_R5] = 0;
    reg[R_R6] = 0;
    reg[R_R7] = 0;
}

int main(){
    ifstream  myfile("D:\\Microsoft Visual Studio\\MyProjects\\ics\\1.txt");
    if(!myfile.is_open()){
        cout<<"未成功打开文件"<<endl;
    }
    load_file(myfile);


    int running = 1;
    while(running){
        //Fetch instr
        int instr = mem_read(reg[R_PC]++);
        int op = instr >> 12; //取指令前四位，即操作符

        switch (op){
            case OP_ADD:{
                    int DR = (instr>>9)&0x0007;
                    int SR1 = (instr>>6)&0x0007;
                    if((instr>>5)&1){//bit[5]==1,为imm5
                        int imm5 = instr&0x001F;
                        imm5= sign_extend(imm5,5);
                        reg[DR] = (reg[SR1] + imm5)&0xFFFF;
                    }
                    else{
                        int SR2 = instr&0x0007;
                        reg[DR] = (reg[SR1] + reg[SR2])&0xFFFF;
                    }
                    setcc(DR);
                    break;
                }
            case OP_AND:{
                int DR = (instr>>9)&0x0007;
                int SR1 = (instr>>6)&0x0007;
                if((instr>>5)&1){//bit[5]==1,为imm5
                    int imm5 = instr&0x001F;
                    imm5= sign_extend(imm5,5);
                    reg[DR] = reg[SR1] & imm5;
                }
                else{
                    int SR2 = instr&0x0007;
                    reg[DR] = reg[SR1] & reg[SR2];
                }
                setcc(DR);
                break;
            }
            case OP_NOT:{
                int DR = (instr>>9)&0x0007;
                int SR = (instr>>6)&0x0007;
                reg[DR] = (~reg[SR])&0xFFFF;
                setcc(DR);
                break;
            }  
            case OP_BR:{
                int n = instr>>11;
                int z = instr>>10;
                int p = instr>>9;
                if((n&&reg[R_COND]==C_N)|(z&&reg[R_COND]==C_Z)|(p&&reg[R_COND]==C_P)){
                    int PC_offset = sign_extend(instr&0x0111,9);
                    reg[R_PC]=(reg[R_PC]+PC_offset)&0xFFFF;
                }
                break;
            }
            case OP_JMP:{
                int BaseR = (instr>>6)&0x0007;
                reg[R_PC]=reg[BaseR];
                break;
            }  
            case OP_JSR:{
                reg[R_R7]=reg[R_PC];
                if((instr>>11)&0x1){//JSR指令
                    int PC_offset=sign_extend(instr&0x07FF,11);
                    reg[R_PC]=(reg[R_PC]+PC_offset)&0xFFFF;
                }
                else{//JSRR指令
                    int BaseR = (instr>>6)&0x0007;
                    reg[R_PC]=reg[BaseR];
                }
                break;
            }
            case OP_LD:{
                int DR = (instr>>9)&0x0007;
                int PC_offset = sign_extend(instr&0x01FF,9);
                reg[DR] = mem_read((reg[R_PC]+PC_offset)&0xFFFF);
                setcc(DR);
                break;
            }
            case OP_LDI:{
                int DR = (instr>>9)&0x0007;
                int PC_offset = sign_extend(instr&0x01FF,9);
                reg[DR]=mem_read(mem_read((reg[R_PC]+PC_offset)&0xFFFF));
                setcc(DR);
                break;
            }
            case OP_LDR:{
                int DR = (instr>>9)&0x0007;
                int PC_offset = sign_extend(instr&0x003F,6);
                int BaseR = (instr>>6)&0x0007;
                reg[DR]=mem_read((reg[BaseR]+PC_offset)&0xFFFF);
                setcc(DR);
                break;
            }
            case OP_LEA:{
                int DR = (instr>>9)&0x0007;
                int PC_offset = sign_extend(instr&0x01FF,9);
                reg[DR]=(reg[R_PC]+PC_offset)&0xFFFF;
                setcc(DR);
                break;
            }
            case OP_ST:{
                int SR = (instr>>9)&0x0007;
                int PC_offset = sign_extend(instr&0x01FF,9);
                mem_write((reg[R_PC]+PC_offset,reg[SR])&0xFFFF);
                break;
            }
            case OP_STI:{
                int SR = (instr>>9)&0x0007;
                int PC_offset = sign_extend(instr&0x01FF,9);
                mem_write(mem_read((reg[R_PC]+PC_offset)&0xFFFF),reg[SR]);
                break;
            }
            case OP_STR:{
                int DR = (instr>>9)&0x0007;
                int PC_offset = sign_extend(instr&0x003F,6);
                int BaseR = (instr>>6)&0x0007;
                mem_write((reg[BaseR]+PC_offset,reg[SR])&0xFFFF);
                break;
            }
            case OP_TRAP:{
                int trapvect8 = instr&0x00FF;
                switch(trapvect8){
                    case TRAP_GETC:{
                        reg[R_R0]=getchar();
                        break;
                    }
                    case TRAP_OUT:{
                        reg[R_R0]=reg[R_R0]&0x00FF;
                        putchar(reg[R_R0]);
                    }
                    case TRAP_PUTS:{
                        int addr = reg[R_R0];
                        while(mem_read(addr)!=0){
                            putchar(mem_read(addr));
                            addr++;
                        }
                    }
                    case TRAP_IN:{
                        cout<<"请输入一个字符:"<<endl;
                        reg[R_R0]=getchar();
                        putchar(reg[R_R0]);
                        break;
                    }
                    case TRAP_PUTSP:{
                        int addr = reg[R_R0];
                        int string_s = mem_read(addr);
                        while(1){
                            if(string_s&0x00FF)
                                putchar(string_s&0x00FF);
                            else break;
                            if(string_s>>8)
                                putchar(string_s>>8);
                            else break;
                            addr++;
                        }
                    }
                    case TRAP_HALT:{
                        running = 0;
                        cout<<"程序运行结束"<<endl;
                        break;
                    }
                    default:break;
                }
                break;    
            }
            case OP_RES:
            case OP_RTI:
            default:
                {
                break;
            }
        }//switch
    }//while
}


void show(){
    for(int i=0;i<20;i++){
        cout<<"#"<<" ";
    }
    cout<<"Registers: "<<endl;
    printf("R0 0x%04X   ",reg[R_R0]);
    printf("R1 0x%04X\n",reg[R_R1]);
    printf("R2 0x%04X   ",reg[R_R2]);
    printf("R3 0x%04X\n",reg[R_R3]);
    printf("R4 0x%04X   ",reg[R_R4]);
    printf("R5 0x%04X\n",reg[R_R5]);
    printf("R6 0x%04X   ",reg[R_R6]);
    printf("R7 0x%04X\n",reg[R_R7]);
    printf("PC 0x%04X   ",reg[R_PC]);
    switch (reg[R_COND]){
        case C_Z:{
            printf("CC Z\n");
            break;
        }
        case C_N:{
            printf("CC N\n");
            break;
        }
        case C_P:{
            printf("CC P\n");
            break;
        }
    }
    for(int i=0;i<20;i++){
        cout<<"#"<<" ";
    }
    cout<<"Memory: "<<endl;
    for(int i=0;i<20;i++){
        cout<<"#"<<" ";
    }
}

void Set_Reg(REG s,int val){
    switch(s){
        case R_R0:{
            reg[R_R0]=val&0xFFFF;break;
        }
        case R_R1:{
            reg[R_R1]=val&0xFFFF;break;
        }
        case R_R2:{
            reg[R_R2]=val&0xFFFF;break;
        }
        case R_R3:{
            reg[R_R3]=val&0xFFFF;break;
        }
        case R_R4:{
            reg[R_R4]=val&0xFFFF;break;
        }
        case R_R5:{
            reg[R_R5]=val&0xFFFF;break;
        }
        case R_R6:{
            reg[R_R6]=val&0xFFFF;break;
        }
        case R_R7:{
            reg[R_R7]=val&0xFFFF;break;
        }
        case R_PC:{
            reg[R_PC]=val&0xFFFF;break;
        }
        default:break;
    }
}

void Set_COND(char s){
    switch (s){
    case 'N':{
        reg[R_COND]=C_N;
        break;
    }
    case 'Z':{
        reg[R_COND]=C_Z;
        break;
    }
    case 'P':{
        reg[R_COND]=C_P;
        break;
    }    
    default:break;
}

void setbk(int val){    //设置断点，断点集按升序排列,重复的断点不再计入
    int i = 0;
    for(;i<numbk;i++){
        if(breakpoints[i]<val) {i++;continue;}
        if(breakpoints[i]==val) break;
        else{
            for(int j=numbk-1;j>i-1;j--){
                breakpoints[j+1]=breakpoints[j];
            }
            breakpoints[i]=val;
            numbk++;
            break;
        }
    }
    if(i==numbk){
        breakpoints[i]=val;
        numbk++;
    }
}

void execute_step(){
    while(getchar()=='1'){
        getchar();
        run_one();
    }
}

void execute_all(){
    
}



enum
{
    MR_KBSR = 0xFE00, /* keyboard status */
    MR_KBDR = 0xFE02  /* keyboard data */
};