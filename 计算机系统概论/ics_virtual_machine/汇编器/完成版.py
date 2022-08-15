###功能函数

def hex_to_hstr(s):  # 将16进制串转化为0x0000形式
    string = s[2:]
    if (len(string) < 4):
        string = (4 - len(string)) * '0' + string
    return '0x' + string


def p_int_to_bstr(s, n,line_num):  # 将非负整数s转化为n位无符号二进制串
    if s > (2 ** n - 1):
        raise UserWarning(f"<line {line_num}> the value is larger than unsigned {n} bit's value")
    a = bin(s)
    a = a[2:]
    if (len(a) < n):
        a = (n-len(a))*'0'+ a
    return a


def intTobin(val, n,line_num):  # 将整数转化为n位有符号二进制数
    if val >= 0:
        temp = p_int_to_bstr(val, n - 1,line_num)
        return '0' + temp
    else:
        val = abs(val)
        if val > 2 ** (n - 1):
            raise UserWarning(f"<line {line_num}> the value is larger than signed {n} bit's value")
        k = len(bin(val)[2:])
        s = bin(2 ** k - val)[2:]
        if(len(s)<k):
            s = (k-len(s))*'0' + s
        if (len(s) < n):
            s = (n - len(s)) * '1' + s
        return s


def register(arg, line_num):
    if (arg[0] == 'R'):
        return p_int_to_bstr(eval(arg[1]), 3,line_num)
    else:
        raise UserWarning(f'<line {line_num}> expect a register but failed')


def operands(arg, line_num):
    val = 0
    if (arg[0] == 'R'):
        return 3 * '0' + p_int_to_bstr(eval(arg[1]), 3,line_num)
    else:
        if arg[0] == '#':  # 5位立即数
            val = eval(arg[1:])
        else:
            raise UserWarning(f'<line {line_num}> expect a register or imm5 but failed')
        return '1' + intTobin(val, 5,line_num)


def PCoffset(arg, n, pc, line_num):  # 将label转化为n位PCoffset
    val = eval(arg) - pc
    return intTobin(val, n,line_num)


def trapvect(arg, line_num):
    val = eval('0' + arg)
    return p_int_to_bstr(val, 8,line_num)


def hex_to_bin(arg, line_num):  # 将x3000形式化为16位二进制串
    val = eval('0' + arg)
    return p_int_to_bstr(val, 16,line_num)


def str_to_bin(arg, line_num):  # 将一个字符串的每个字符都转换为16位ascII码值
    bin_line = ''
    for letter in arg:
        bin_line = bin_line + p_int_to_bstr(ord(letter), 16,line_num) + '\n'
    return bin_line


def PCoffset_6(arg, line_num):
    val = eval(arg[1:])
    return intTobin(val,6,line_num)

def get_symbol(symbol_table,line_num,arg):
    if arg not in symbol_table:
        raise UserWarning(f'<line {line_num}> label not existing')
    return symbol_table[arg]


###操作码字典
op_conds = {
    "ADD": "'0001' + register(line[1],line_num) + register(line[2],line_num) + operands(line[3],line_num)",
    "AND": "'0101' + register(line[1],line_num) + register(line[2],line_num) + operands(line[3],line_num)",
    "BRn": "'0000' + '100' + PCoffset(get_symbol(symbol_table,line_num,line[1]),9,pc,line_num)",
    "BRz": "'0000' + '010' + PCoffset(get_symbol(symbol_table,line_num,line[1]),9,pc,line_num)",
    "BRp": "'0000' + '001' + PCoffset(get_symbol(symbol_table,line_num,line[1]),9,pc,line_num)",
    "BRnz": "'0000' + '110' + PCoffset(get_symbol(symbol_table,line_num,line[1]),9,pc,line_num)",
    "BRnp": "'0000' + '101' + PCoffset(get_symbol(symbol_table,line_num,line[1]),9,pc,line_num)",
    "BRzp": "'0000' + '011' + PCoffset(get_symbol(symbol_table,line_num,line[1]),9,pc,line_num)",
    "BRnzp": "'0000' + '111' + PCoffset(get_symbol(symbol_table,line_num,line[1]),9,pc,line_num)",
    "JMP": "'1100' + '000' + register(line[1],line_num) + '000000'",
    "JSR": "'0100' + '1' + PCoffset(get_symbol(symbol_table,line_num,line[1]),11,pc,line_num)",
    "JSRR": "'0100' + '0' + '00' + register(line[1],line_num) + '000000'",
    "LD": "'0010' + register(line[1],line_num) + PCoffset(get_symbol(symbol_table,line_num,line[2]),9,pc,line_num)",
    "LDI": "'1010' + register(line[1],line_num) + PCoffset(get_symbol(symbol_table,line_num,line[2]),9,pc,line_num)",
    "LDR": "'0110' + register(line[1],line_num) + register(line[2],line_num) + PCoffset_6(line[3],line_num)",
    "LEA": "'1110' + register(line[1],line_num) + PCoffset(get_symbol(symbol_table,line_num,line[2]),9,pc,line_num)",
    "NOT": "'1001' + register(line[1],line_num) + register(line[2],line_num) + '111111'",
    "RET": "'1100' + '000' + '111' + '000000'",
    "RTI": "'1000' + '000000000000'",
    "ST": "'0011' + register(line[1],line_num) + PCoffset(get_symbol(symbol_table,line_num,line[2]),9,pc,line_num)",
    "STI": "'1011' + register(line[1],line_num) + PCoffset(get_symbol(symbol_table,line_num,line[2]),9,pc,line_num)",
    "STR": "'0111' + register(line[1],line_num) + register(line[2],line_num) + PCoffset_6(line[3],line_num)",
    "TRAP": "'1111' + '0000' + trapvect(line[1],line_num)",
    "GETC": "'1111' + '0000' + '00100000'",
    "OUT": "'1111' + '0000' + '00100001'",
    "PUTS": "'1111' + '0000' + '00100010'",
    "IN": "'1111' + '0000' + '00100011'",
    "PUTSP": "'1111' + '0000' + '00100100'" ,
    "HALT": "'1111' + '0000' + '00100101'",
    ".ORIG": "hex_to_bin(line[1],line_num)",
    ".FILL": "hex_to_bin(line[1],line_num)",
    ".BLKW": "('0'*16+'\n')*eval(line[1],line_num)",
    ".STRINGZ": "str_to_bin(line[1],line_num) + '0'*16",
    ".END": '1'
}

#每个操作符后面所需的参数个数
op_args_num = {
    "ADD": 3,
    "AND": 3,
    "BRn": 1,
    "BRz": 1,
    "BRp": 1,
    "BRnz": 1,
    "BRnp": 1,
    "BRzp": 1,
    "BRnzp": 1,
    "JMP": 1,
    "JSR": 1,
    "JSRR": 1,
    "LD": 2,
    "LDI": 2,
    "LDR": 3,
    "LEA": 2,
    "NOT": 2,
    "RET": 0,
    "RTI": 0,
    "ST": 2,
    "STI": 2,
    "STR": 3,
    "TRAP": 1,
    "GETC": 0,
    "OUT": 0,
    "PUTS": 0,
    "IN": 0,
    "PUTSP": 0,
    "HALT": 0,
    ".ORIG": 1,
    ".FILL": 1,
    ".BLKW": 1,
    ".STRINGZ": 1,
    ".END": '1'
}

###行处理函数
def deal_with_line(line):
    # 处理每一行信息，去左右空白，把所有词块分出来，返回成一个列表
    line = line.strip()
    if not line or line.startswith(';'):  # 该行为空或全为注释
        return None

    line, *anno = line.split(';', 1)  # 去掉注释
    line = line.strip()
    line = line + ' '
    args = []  # 用于返回每一行的结果
    sep = [' ', ',', '\n', '\t']  # 分词标志
    word_start = True
    strings_cond = False
    str_start = False
    k = 0
    for i, letter in enumerate(line):
        if letter == '"':
            strings_cond = True
            str_start = not str_start
        if not strings_cond:
            if letter not in sep:
                if word_start:
                    k = i
                    word_start = False
                continue
            if not word_start and letter in sep:
                args.append(line[k:i])
                word_start = True
        elif strings_cond:
            if not str_start and letter == '"':
                args.append(line[k:i])
                break
            if str_start and letter == '"':
                k = i + 1
    return args


###扫描函数
def asm_one(code):  # 以字典形式生成符号表，并返回元素只含每一行伪代码和指令的列表
    symbol_table = {}
    lc = 0
    start = False
    end = False
    clean_lines = []
    for line_num, line in enumerate(code.splitlines(), 1):
        args = deal_with_line(line)
        if not args:  # 该行为空，跳过
            continue
        if not start:  # 文件以.ORIG开头，并设置lc初始
            if args[0] == ".ORIG":
                lc = eval('0' + args[1])
                start = True
                clean_lines.append(args)
                continue
            else:
                raise UserWarning('There is no .ORIG at the beginning of the file')
        if args[0] in op_conds:#开头为操作符或伪代码
            if args[0] == ".END":
                end = True
                break
            else:
                if args[0] == '.BLKW':
                    lc = lc + eval(args[1])
                elif args[0] == '.STRINGZ':
                    lc = lc + len(args[1]) + 1
                else:
                    lc = lc + 1
                clean_lines.append(args)
                continue
        else:  # 开头为label
            symbol_table.setdefault(args[0], hex_to_hstr(hex(lc).upper()))
            clean_lines.append(args[1:])  # 将每行label去掉
            if args[1] == '.BLKW':
                lc = lc + eval(args[2])
            elif args[1] == '.STRINGZ':
                lc = lc + len(args[2]) + 1
            else:
                lc = lc + 1
    if not end:
        raise UserWarning('There is no .END at the end of the file')

    return symbol_table, clean_lines


def asm_two(symbol_table, lines):
    bin_lines = []
    final_lines = []
    pc = eval('0'+lines[0][1])  # pc起始地址
    for line_num,line in enumerate(lines,1):
        if line[0] not in op_conds:
            raise UserWarning(f'<line {line_num}> syntax error')
        if len(line)-1 < op_args_num[line[0]]:
            raise UserWarning(f'<line {line_num}> args missing')
        if len(line)-1 > op_args_num[line[0]]:
            raise UserWarning(f'<line {line_num}> args redundant')
        bin_lines.append(eval(op_conds[line[0]]))
        pc = pc + 1
    for line in bin_lines:
        if not line.endswith('\n'):
            final_lines.append(line+'\n')
        else:
            final_lines.append(line)
    return final_lines


def main():
    with open("D:\\Microsoft Visual Studio\\MyProjects\\ics\\2.asm", 'r') as f_asm:
        code = f_asm.read()
    symbol_table = {}
    lines = []
    bin_lines = []
    symbol_table, lines = asm_one(code)
    bin_lines = asm_two(symbol_table, lines)
    # 生成符号表文件.sym:
    with open("D:\\Microsoft Visual Studio\\MyProjects\\ics\\2.sym", 'w') as f_sym:
        f_sym.write("\\\\Symbol Name        Address\n")
        f_sym.write("\\\\-----------        -------\n")
        for key, value in symbol_table.items():
            f_sym.write(f'\\\\  {key}                 {value}\n')
    # 生成txt文件:
    with open("D:\\Microsoft Visual Studio\\MyProjects\\ics\\2.txt", 'w') as f_txt:
        for line in bin_lines:
            f_txt.write(line)

    with open("D:\\Microsoft Visual Studio\\MyProjects\\ics\\2.bin", 'w') as f_bin:
        for line in bin_lines:
            f_bin.write(line)


main()