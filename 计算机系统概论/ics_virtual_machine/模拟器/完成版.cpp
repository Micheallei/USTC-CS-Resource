#include <iostream>
#include <fstream>
#include<stdio.h>
#include <string>
using namespace std;
#define MEMORY_MAX 65536

enum REG {
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
	R_COUNT//10
};

enum COND {
	C_N = 0,
	C_Z,
	C_P,
};

enum OPCODES {//指令顺序与其四位操作码的十进制数值对应
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

enum TRAP {
	TRAP_GETC = 0x20,//从键盘读入一个字符，但不会回显屏幕，ASCII码值给R0
	TRAP_OUT = 0x21,//将R0[7:0]的字符输出在屏幕上显示
	TRAP_PUTS = 0x22,//向屏幕写一个字符串，存放在连续内存中，起始地址由R0指定,直到0x0000时结束
	TRAP_IN = 0x23,//打印提示，等待键盘输入一个字符，并回显，装入R0
	TRAP_PUTSP = 0x24,//屏幕写一个字符串，每个内存有两个字符，先bit[7:0],再bit[15:8]，起始地址由R0决定，0x0000时结束
	TRAP_HALT = 0x25
};

int sign_extend(int x, int bit_count) {//将bit_count位的x有符号扩展为16位,x为指令
	if ((x >> (bit_count - 1)) & 1) {
		x = (x | (0xFFFF << bit_count)) & 0xFFFF;
	}
	return x;
}

int string_to_int(char* s) {
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


class Register {
public:
	int reg[R_COUNT];
	int breakpoints[20]; //断点集合
	int bknum;//断点个数
	Register();//构造函数
	void setcc(int r);
	void Set_Reg(int s, int val);
	int Get_Reg(int s);
	void clear_reg();
	void Set_COND(char s);
	void setbk(int address);
	void clearbk();
};

Register::Register() {
	for (int i = 0; i < 20; i++) { breakpoints[i] = 0; }
	bknum = 0;
	for (int i = 0; i < 8; i++) reg[i] = 0;
	reg[R_PC] = 0x3000;
	reg[R_COND] = C_N;
}
void Register::setcc(int r)    //根据相应寄存器的值来判定如何设置条件码
{
	if (reg[r] == 0)
	{
		reg[R_COND] = C_Z;
	}
	else if ((reg[r] >> 15)&0xFFFF) //右移15位还是正数说明第16位为1，则相当于是负数
	{
		reg[R_COND] = C_N;
	}
	else
	{
		reg[R_COND] = C_P;
	}
}

void Register::Set_Reg(int s, int val) {//设置寄存器，输入0-8，8为pc，及值
	switch (s) {
        case R_R0: {
            reg[R_R0] = val & 0xFFFF; break;
        }
        case R_R1: {
            reg[R_R1] = val & 0xFFFF; break;
        }
        case R_R2: {
            reg[R_R2] = val & 0xFFFF; break;
        }
        case R_R3: {
            reg[R_R3] = val & 0xFFFF; break;
        }
        case R_R4: {
            reg[R_R4] = val & 0xFFFF; break;
        }
        case R_R5: {
            reg[R_R5] = val & 0xFFFF; break;
        }
        case R_R6: {
            reg[R_R6] = val & 0xFFFF; break;
        }
        case R_R7: {
            reg[R_R7] = val & 0xFFFF; break;
        }
        case R_PC: {
            reg[R_PC] = val & 0xFFFF; break;
        }
        default:break;
    }
}

int Register::Get_Reg(int s) {//读取寄存器
	return reg[s];
}

void Register::clear_reg() {
	for (int i = 0; i < R_COUNT - 2;i++) reg[i] = 0;
	reg[R_COND] = C_Z;
	reg[R_PC] = 0x3000;
}

void Register::Set_COND(char s) {//输入N Z P
	switch (s) {
	case 'N': {
		reg[R_COND] = C_N;
		break;
	}
	case 'Z': {
		reg[R_COND] = C_Z;
		break;
	}
	case 'P': {
		reg[R_COND] = C_P;
		break;
	}
	default:break;
	}
}

void Register::setbk(int address) {    //设置断点，断点集按升序排列,重复的断点不再计入
    int i = 0;
    for (; i < bknum; i++) {
        if (breakpoints[i] < address) { i++; continue; }
        if (breakpoints[i] == address) break;
        else {
            for (int j = bknum - 1; j > i - 1; j--) {
                breakpoints[j + 1] = breakpoints[j];
            }
            breakpoints[i] = address;
            bknum++;
            break;
        }
    }
    if (i == bknum) {
        breakpoints[i] = address;
        bknum++;
    }
}

void Register::clearbk() {
    for (int i = 0; i < bknum; i++) breakpoints[i] = 0;
    bknum = 0;
}

class Memory {
private:
    int memory[MEMORY_MAX];
public:
    Memory();
    int mem_read(int address);
    void mem_write(int address, int value);
    void mem_clear();
};

Memory::Memory() {
    for (int i = 0; i < MEMORY_MAX; i++) { memory[i] = 0; }
}

int Memory::mem_read(int address) {//根据地址读取相应内存
    return memory[address];
}

void Memory::mem_write(int address, int value) {
    memory[address] = value;
}
void Memory::mem_clear() {
    for (int i = 0; i < MEMORY_MAX; i++) memory[i] = 0;
}

class LC_3 :public Register, public Memory {
private:
    int run();
    int state;//程序运行到halt时置1, 0为正常运行状态,
public:
    LC_3();
    void load_file();
    void show();
    void run_one();
    void run_all();
};

LC_3::LC_3(){ state = 0; }//state为0表示程序尚未结束

void LC_3::load_file() {//从文件把数据读入到内存中
    FILE* fp;
    fopen_s(&fp, "D:\\Microsoft Visual Studio\\MyProjects\\ics\\1.txt", "r");
    if (fp == NULL) {
        cout << "未成功打开文件" << endl;
        exit(-1);
    }
    char line[20] = {'\0'};
    fgets(line,19,fp);
    int PC_NOW;
    PC_NOW = string_to_int(line);
    Set_Reg(R_PC, PC_NOW);
    while (fgets(line, 19, fp)) {
        mem_write(PC_NOW, string_to_int(line));
        PC_NOW++;
    }
    fclose(fp);
}

void LC_3::show() {
    for (int i = 0; i < 80; i++) {
        cout << "#";
    }
    cout<<endl;
    cout << "Registers: " << endl;
    printf("R0 0x%04X   ", Get_Reg(R_R0));
    printf("R1 0x%04X\n", Get_Reg(R_R1));
    printf("R2 0x%04X   ", Get_Reg(R_R2));
    printf("R3 0x%04X\n", Get_Reg(R_R3));
    printf("R4 0x%04X   ", Get_Reg(R_R4));
    printf("R5 0x%04X\n", Get_Reg(R_R5));
    printf("R6 0x%04X   ", Get_Reg(R_R6));
    printf("R7 0x%04X\n", Get_Reg(R_R7));
    printf("PC 0x%04X   ", Get_Reg(R_PC));
    switch (Get_Reg(R_COND)) {
    case C_Z: {
        printf("CC Z\n");
        break;
    }
    case C_N: {
        printf("CC N\n");
        break;
    }
    case C_P: {
        printf("CC P\n");
        break;
    }
    }
    for (int i = 0; i < 80; i++) {
        cout << "#";
    }
    cout << endl;
    for (int i = 0; i < 80; i++) {
        cout << "#";
    }
    cout << endl;
    cout<<"breakpoints:"<<endl;
    int c=0;
    for(int i=0;i<bknum;i++) {
        printf("0x%04X  ",breakpoints[i]);
        c++;
        if(c%5==0)  cout<<endl;//5个断点一行
    }
    for (int i = 0; i < 80; i++) {
        cout << "#";
    }
    cout << endl;
    cout << "Memory: " << endl;
    printf("address     value");
    for(int i=0;i<10;i++){  //读取从当前PC位置起的十个内存空间
        printf("0x%04X      0x%04X\n",Get_Reg(R_PC)+i,mem_read(Get_Reg(R_PC)+i));
    }
    for (int i = 0; i < 80; i++) {
        cout << "#";
    }
    cout<<endl;
}

int LC_3::run() {
    int instr = mem_read(reg[R_PC]++);
    int op = instr >> 12; //取指令前四位，即操作符

    switch (op) {
    case OP_ADD: {
        int DR = (instr >> 9) & 0x0007;
        int SR1 = (instr >> 6) & 0x0007;
        if ((instr >> 5) & 1) {//bit[5]==1,为imm5
            int imm5 = instr & 0x001F;
            imm5 = sign_extend(imm5, 5);
            reg[DR] = (reg[SR1] + imm5) & 0xFFFF;
        }
        else {
            int SR2 = instr & 0x0007;
            reg[DR] = (reg[SR1] + reg[SR2]) & 0xFFFF;
        }
        setcc(DR);
        break;
    }
    case OP_AND: {
        int DR = (instr >> 9) & 0x0007;
        int SR1 = (instr >> 6) & 0x0007;
        if ((instr >> 5) & 1) {//bit[5]==1,为imm5
            int imm5 = instr & 0x001F;
            imm5 = sign_extend(imm5, 5);
            reg[DR] = (reg[SR1] & imm5)&0xFFFF;
        }
        else {
            int SR2 = instr & 0x0007;
            reg[DR] = (reg[SR1] & reg[SR2])&0xFFFF;
        }
        setcc(DR);
        break;
    }
    case OP_NOT: {
        int DR = (instr >> 9) & 0x0007;
        int SR = (instr >> 6) & 0x0007;
        reg[DR] = (~reg[SR]) & 0xFFFF;
        setcc(DR);
        break;
    }
    case OP_BR: {
        int n = (instr >> 11)&1;
        int z = (instr >> 10)&1;
        int p = (instr >> 9)&1;
        if ((n && reg[R_COND] == C_N) | (z && reg[R_COND] == C_Z) | (p && reg[R_COND] == C_P)) {
            int PC_offset = sign_extend(instr & 0x0111, 9);
            reg[R_PC] = (reg[R_PC] + PC_offset) & 0xFFFF;
        }
        break;
    }
    case OP_JMP: {
        int BaseR = (instr >> 6) & 0x0007;
        reg[R_PC] = reg[BaseR];
        break;
    }
    case OP_JSR: {
        reg[R_R7] = reg[R_PC];
        if ((instr >> 11) & 0x1) {//JSR指令
            int PC_offset = sign_extend(instr & 0x07FF, 11);
            reg[R_PC] = (reg[R_PC] + PC_offset) & 0xFFFF;
        }
        else {//JSRR指令
            int BaseR = (instr >> 6) & 0x0007;
            reg[R_PC] = reg[BaseR];
        }
        break;
    }
    case OP_LD: {
        int DR = (instr >> 9) & 0x0007;
        int PC_offset = sign_extend(instr & 0x01FF, 9);
        reg[DR] = mem_read((reg[R_PC] + PC_offset) & 0xFFFF);
        setcc(DR);
        break;
    }
    case OP_LDI: {
        int DR = (instr >> 9) & 0x0007;
        int PC_offset = sign_extend(instr & 0x01FF, 9);
        reg[DR] = mem_read(mem_read((reg[R_PC] + PC_offset) & 0xFFFF));
        setcc(DR);
        break;
    }
    case OP_LDR: {
        int DR = (instr >> 9) & 0x0007;
        int PC_offset = sign_extend(instr & 0x003F, 6);
        int BaseR = (instr >> 6) & 0x0007;
        reg[DR] = mem_read((reg[BaseR] + PC_offset) & 0xFFFF);
        setcc(DR);
        break;
    }
    case OP_LEA: {
        int DR = (instr >> 9) & 0x0007;
        int PC_offset = sign_extend(instr & 0x01FF, 9);
        reg[DR] = (reg[R_PC] + PC_offset) & 0xFFFF;
        setcc(DR);
        break;
    }
    case OP_ST: {
        int SR = (instr >> 9) & 0x0007;
        int PC_offset = sign_extend(instr & 0x01FF, 9);
        mem_write((reg[R_PC] + PC_offset) & 0xFFFF, reg[SR] & 0xFFFF);
        break;
    }
    case OP_STI: {
        int SR = (instr >> 9) & 0x0007;
        int PC_offset = sign_extend(instr & 0x01FF, 9);
        mem_write(mem_read((reg[R_PC] + PC_offset) & 0xFFFF), reg[SR] & 0xFFFF);
        break;
    }
    case OP_STR: {
        int SR = (instr >> 9) & 0x0007;
        int PC_offset = sign_extend(instr & 0x003F, 6);
        int BaseR = (instr >> 6) & 0x0007;
        mem_write((reg[BaseR] + PC_offset) & 0xFFFF, reg[SR] & 0xFFFF);
        break;
    }
    case OP_TRAP: {
        int trapvect8 = instr & 0x00FF;
        switch (trapvect8) {
        case TRAP_GETC: {
            reg[R_R0] = getchar();
            break;
        }
        case TRAP_OUT: {
            reg[R_R0] = reg[R_R0] & 0x00FF;
            putchar(reg[R_R0]);
            break;
        }
        case TRAP_PUTS: {
            int addr = reg[R_R0];
            while (mem_read(addr) != 0) {
                putchar(mem_read(addr));
                addr++;
            }
            break;
        }
        case TRAP_IN: {
            cout << "请输入一个字符:" << endl;
            reg[R_R0] = getchar();
            putchar(reg[R_R0]);
            break;
        }
        case TRAP_PUTSP: {
            int addr = reg[R_R0];
            int string_s = mem_read(addr);
            while (1) {
                if (string_s & 0x00FF)
                    putchar(string_s & 0x00FF);
                else break;
                if ((string_s >> 8)&0x00FF)
                    putchar((string_s >> 8)&0x00FF);
                else break;
                addr++;
            }
        }
        case TRAP_HALT: {
            cout << "程序运行结束" << endl;
            return 1;
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
    return 0;
}

void LC_3::run_one() {
    if (state) return;
    state = run();
    show();
}

void LC_3::run_all() {//运行到下一个断点处
    while (state == 0) {
        state = run();
        for (int i = 0; i < bknum; i++) {
            if (reg[R_PC] == breakpoints[i]) { state = 1; break; }
        }
    }
    show();
}


int main() {
    cout << "Welcome to LC_3 Simulator!" << endl;
    cout << "instructions:" << endl;
    for (int i = 0; i < 80; i++) {
        cout << "#";
    }
    cout << endl;
    cout << "--sb  " << "--sr  " << "--sm  " << endl;
    cout << "--l  " << "--e  " << "--E  " << endl;
    cout << "--cb  " << "--cm  " << "--cr  " << endl;
    cout << "--o  " << "--t  " << endl;
    for (int i = 0; i < 80; i++) {
        cout << "#";
    }
    cout << endl;
    LC_3 lc3;//初始化LC_3
    int flag = 1;
    while (flag) {
        for (int i = 0; i < 80; i++) {
            cout << "#";
        }
        cout << endl;
        cout << "your choice or 'h' to see instructions" << endl;
        char inst;
        scanf_s("%c", &inst,1);
        for (int i = 0; i < 80; i++) {
            cout << "#";
        }
        cout << endl;
        switch (inst) {
        case 'h': {
            getchar();
            for (int i = 0; i < 80; i++) {
                cout << "#";
            }
            cout << endl;
            cout << "--sb  " << "--sr  " << "--sm  " << endl;
            cout << "--l  " << "--e  " << "--E  " << endl;
            cout << "--cb  " << "--cm  " << "--cr  " << endl;
            cout << "--o  " <<"--v  "<< "--t  " << endl;
            for (int i = 0; i < 80; i++) {
                cout << "#";
            }
            cout << endl;
            break;
        }
        case 's': {
            char num;
            scanf_s("%c", &num,1);
            getchar();
            switch (num) {
            case 'b': {
                int addr;
                cout << "value:";
                scanf_s("%x", &addr);
                lc3.setbk(addr);
                break;
            }
            case 'r': {
                int num, val;
                cout << "regnum value:" << endl;
                scanf_s("%d %x", &num, &val);
                lc3.Set_Reg(num, val);
                break;
            }
            case 'm': {
                int addr, val;
                cout << "addr value:" << endl;
                scanf_s("%x %x", &addr, &val);
                lc3.mem_write(addr, val);
                break;
            }
            default:cout << "instruction wrong!" << endl;
            }
            getchar();
            break;
        }
        case 'e': {
            getchar();
            cout << "输入 1 来每次执行一步；输入 0 来停止" << endl;
            int n;
            scanf_s("%d", &n);
            getchar();
            while (n) {
                lc3.run_one();
                cout << "输入 1 来每次执行一步；输入 0 来停止:" << endl;
                scanf_s("%d", &n);
                getchar();
            }
            break;
        }
        case 'E': {
            getchar();
            lc3.run_all();
            break;
        }
        case 'l': {
            //char *s;
            //scanf_s("%s", s,256);
            getchar();
            lc3.load_file();
            break;
        }
        case 'c': {
            char num;
            scanf_s("%c", &num,1);
            getchar();
            switch (num) {
            case 'b': {
                lc3.clearbk();
                break;
            }
            case 'm': {
                lc3.mem_clear();
                break;
            }
            case 'r': {
                lc3.clear_reg();
                break;
            }
            default:cout << "instruction wrong!" << endl;
            }
            break;
        }
        case 'o': {
            getchar();
            lc3.show();
            break;
        }
        case 'v':{
            getchar();
            cout<<"address:"<<endl;
            int addr;
            scanf_s("%x",&addr);
            for (int i = 0; i < 80; i++) {
                cout << "#";
            }
            cout << endl;
            cout << "Memory: " << endl;
            printf("address     value");
            for(int i=0;i<10;i++){  //读取从当前PC位置起的十个内存空间
                printf("0x%04X      0x%04X\n",addr+i,mem_read(addr+i));
            }
            for (int i = 0; i < 80; i++) {
                cout << "#";
            }
            cout<<endl;
            break;
        }
        case 't': {
            getchar();
            flag = 0;
            break;
        }
        default:cout << "wrong instruction!" << endl;
        }//switch
    }//while
    return 0;
}