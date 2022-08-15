#!/usr/bin/env  python3
import array
import sys
import os

from collections import namedtuple


MAX_LINE_LENGTH = 4096
UNIX16_MAX = 2**16

REGS = {
    'R0':0x0,
    'R1':0x1,
    'R2':0x2,
    'R3':0x3,
    'R4':0x4,
    'R5':0x5,
    'R6':0x6,
    'R7':0x7,
}

TRAPS = {
    'GETC':0x20,
    'OUT':0x21,
    'PUTS':0x22,
    'IN':0x23,
    'PUTSP':0x24,
    'HALT':0x25,
}

OPS = {
    'BR':  0x0,    # 0b0000 branch
    'BRn': 0x0,    # 0b0000 branch if n
    'BRz': 0x0,    # 0b0000 branch
    'BRp': 0x0,    # 0b0000 branch
    'BRzp': 0x0,   # 0b0000 branch
    'BRnp': 0x0,   # 0b0000 branch
    'BRnz': 0x0,   # 0b0000 branch
    'BRnzp': 0x0,  # 0b0000 branch

    'ADD': 0x1,    # 0b0001 add
    'LD': 0x2,     # 0b0010 load
    'ST': 0x3,     # 0b0011 store
    'JSR': 0x4,    # 0b0100 jump register
    'JSRR': 0x4,   # 0b0100 jump register
    'AND': 0x5,    # 0b0101 bitwise and
    'LDR': 0x6,    # 0b0110 load register
    'STR': 0x7,    # 0b0111 store register
    'RTI': 0x8,    # 0b1000 unused
    'NOT': 0x9,    # 0b1001 bitwise not
    'LDI': 0xA,    # 0b1010 load indirect
    'STI': 0xB,    # 0b1011 store indirect
    'RET': 0xC,    # 0b1100 return
    'JMP': 0xC,    # 0b1100 jump
    'RES': 0xD,    # 0b1101 reserved (unused)
    'LEA': 0xE,    # 0b1110 load effective address
    'TRAP': 0xF,   # ob1111 execute trap

    **TRAPS //???
}

DOTS = {
    '.ORIG': None,
    '.END': None,
    '.FILL': None,
    '.BLKW': None,
    '.STRINGZ': None
}

class Type:
    LABEL = 'label'
    OP = 'op'
    DOT = 'dot'
    CONST = 'const'
    REG = 'reg'
    STR = 'str'

Token = namedtuple('Tok','t,v')

def tok_op_args(line):  #对每条指令的操作数分开
    args=[]
    for arg in line.split(','):#split：分割为列表
        arg=arg.strip() #去除头尾空格及换行符
        if not arg:
            continue
        if arg in REGS:
            args.append(Token(Type.REG,arg))
        elif arg.startswith('#'):
            args.append(Token(Type.CONST,int(arg[1:])))
        elif arg.startswith('x'):
            args.append(Token(Type.CONST,int('0'+arg,16)))
        elif arg.startswith('b'):
            args.append(Token(Type.CONST,int('0'+arg,2)))
        else:
            args.append(Token(Type.LABEL),arg))
    return args


def is_int(arg):
    try:
        int(arg)
        return True
    except ValueError:
        return False


def tok_dot_args(arg):#处理伪代码的参数
    arg = arg.strip()
    if not arg: #.END
        return []
    if arg.startswith('"'):#STRINGZ
        ???
        return [Token(Type.STR,arg.replace('"',''))]
    elif arg.startswith('\''):#STRINGZ
        ???
        return [Token(Type.STR,arg.replace('\'',''))]
    elif arg.startswith('x'):#.ORIG或.FILL
        return [Token(Type.CONST,int('0'+arg,16))]
    elif arg.startswith('b'):
        return [Token(Type.CONST,int('0'+arg,2))]
    elif arg.startswith('#'):
        return [Token(Type.CONST,int(arg,10))]
    else: #label
        return [Token(Type.LABEL,arg)]
    return []

def tok(line):#对每一行的处理,返回的时该行所有有效参数的列表，
    #且每个参数均用namedtuple了表示，打上标记
    line = line.strip() #去掉每行首位多余空格、换行符
    if not line or line.startswith(';'):#该行为空行或全为注释
        return None
    line,*_ = line.split(';',1) #去掉注释
    res = line.split(' ',1) #将该行第一个指令与其余分开，返回列表
 
    if len(res) == 1: #列表只有一个元素，即该行只有一个词
        token = res[0].strip()
        other = '' 
    else:
        token = res[0].strip()
        other = res[1].strip()

    if token in OPS:#开头是操作符
        return [Token(Type.OP, token)] + tok_op_args(other)
    elif token.startswith('.'):#开头是伪代码
        return [Token(Type.DOT, token.upper())] + tok_dot_args(other)
    else:#开头是label
        if other:#label后还有参数
            return [Token(Type.LABEL, token)] + tok(other)
        else:
            return [Token(Type.LABEL, token)]


def check_syntax(tokens):
    pass

def incr(lc,tokens):return lc+1

def asm_pass_one(code):#第一次遍历，构建符号表,并返回列表，
    #其元素为每一行的所有参数(均打上了标记),以及对应的地址(lc)
    #CONDS对伪代码和操作符行情况下的lc增加进行处理
    CONDS = {
        '.ORIG': lambda lc, tokens: tokens[1].v,
        '.BLKW': lambda lc, tokens: lc + tokens[1].v,
        '.FILL': incr,
        '.STRINGZ': lambda lc, tokens: lc + len(tokens[1].v) + 1,
        **{op: incr for op, _ in OPS.items()},
    }

    symbol_table={}
    lines = []
    lc = 0 #地址跟踪计数器，由.ORIG初始化，之后每识别出一条有效指令，则lc+1

    for line_number, line in enumerate(code.splitlines(), 1):
        #按整个汇编文本的每一行来分析
        assert len(line) < MAX_LINE_LENGTH, 'line is too long'
        tokens = tok(line)

        if not tokens:
            continue #当前扫描行为空或注释行
        if tokens[0].v == '.END':#扫描完毕
            break
    
        # 以f开头表示在字符串内支持大括号内的python表达式
        print(f'{hex(lc)} ({line_number}):{line.strip()}|{tokens}')
        #显示当前处理信息

        check_syntax(tokens)

        if tokens[0].t == Type.LABEL:#生成符号表的项
            symbol_table[tokens[0].v] = lc

        if tokens[0].t != Type.LABEL:#不是label，则应是伪代码或操作符
            lc = CONDS[tokens[0].v](lc,tokens)
        elif len(tokens) != 1:#是label，则看label后面的数据来增加lc
            lc = CONDS[tokens[1].v](lc,tokens[1:])

        lines.append((tokens, lc))

    print('LC:',hex(lc))
    return symbol_table,lines


#对不同操作符所在行的编码
def encode_ldr_str(op,sym,lc,toks):
    dr = REGS[tok[1].v]<<9

def encode_add_and(op,sym,lc,toks):
    pass

def encode_br(op,sym,lc,toks):
    pass

def encode_ld_st(op,sym,lc,toks):
    pass

def encode_traps(op,sym,lc,toks):
    pass



def asm_pass_two(symbol_table,lines):
    CONDS = {
        #fromlist:将参数list的每个元素加入到array末尾
        '.BLKW': lambda tokens, data, sym, lc:
        data.fromlist([0x0 for _ in range(tokens[1].v)]),

        '.FILL': lambda tokens, data, sym, lc: tokens[1].t == Type.CONST and
        data.append(tokens[1].v) or tokens[1].t == Type.LABEL and
        data.append(sym[tokens[1].v]),

        '.STRINGZ': lambda tokens, data, sym, lc:
        data.fromlist([c for c in tokens[1].v.encode()]) or data.append(0x0),

        **{
            op: lambda toks, data, sym, lc:
            data.append(encode[toks[0].v](OPS[toks[0].v] << 12, sym, lc, toks))
            for op, _ in OPS.items()
        }
    }
    data = array.array("H",[])
    data.append(lines[0][0][1].v)

    for tokens, lc in lines[1:]:
        if tokens[0].v == '.END':
            break
        print(tokens)

        if tokens[0].t != Type.LABEL:
            CONDS[tokens[0].v](tokens,data,symbol_table,lc)
        elif lend(tokens) != 1:
            CONDS[tokens[1].v](tokens[1:],data,symbol_table,lc)

    
    ?data.byteswap()?
    return data