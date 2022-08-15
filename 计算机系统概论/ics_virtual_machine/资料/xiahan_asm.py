#文件名后缀解析/操作
def GetSuffix(filename):
    point = filename.rfind(".")
    if point == -1:
        return ""
    else:
        return filename[point+1:]
def ChangeSuffix(filename,newsuffix):
    point = filename.rfind(".")
    return filename[:point]+'.'+newsuffix

#词法分析
def sp(s):
    """切分元素"""
    blank = ['\t',' ','\n',',']
    in_quotes_area = False
    in_blank_area = True
    s += blank[0]
    ans = []
    for j,c in enumerate(s):
        if c in blank and not in_quotes_area:
            if not in_blank_area:
                ans.append(s[i:j])
                in_blank_area = True
        elif c == '"':
            in_quotes_area = not in_quotes_area
        if c not in blank:
            if in_blank_area:
                i = j
                in_blank_area = False
    return ans

#语法分析
def DelComment(l):
    """删除注释"""
    comment_start = len(l)
    for i,elem in enumerate(l):
        if elem[0] == ';':
            comment_start = i
            break
    return l[:comment_start]
def cut(lines):
    """删除'.END'之后的内容"""
    end_place = -1
    for i,line in enumerate(lines):
        if line[0] == '.END':
            end_place = i
            if len(line) != 1:
                raise UserWarning("'.END' is not expected to take arguments!")
            break
    if end_place == -1:
        raise UserWarning("A program should ended with '.END'")
    return lines[:end_place]

#编码转换
def imp(x):
    """求二进制数-x的等长补码表示"""
    n = len(x)
    return bin(2**n - eval("0b" + x))[-n:]
def h2b(x,n):
    """将以'x'开头的16进制数翻译为n位2进制原码"""
    ans = bin(eval('0'+x))[2:]
    if(len(ans) > n):
        raise UserWarning("Hexadecimal number %d has more than %d bits!"%(x,n))
    else:
        return (n-len(ans))*'0'+ans
def h2d(x):
    """读取一个以'x'开头的16进制数"""
    return eval('0'+x)
def d2ib(x,n):
    """将10进制数翻译为n位2进制补码"""
    v = eval(x)
    ans = bin(abs(v))[2:]
    ans = (n - len(ans))*'0' + ans
    if(v < 0):
        return imp(ans)
    else:
        return ans
def d2ub(x,n):
    """将10进制数翻译为n位2进制原码"""
    v = eval(x)
    ans = bin(v)[2:]
    ans = (n - len(ans)) * '0' + ans
    return ans
def c2b(c):
    """转化字符为16位的2进制ascii码"""
    return d2ub(str(ord(c)),16)
def s2b(s):
    """将字符串转化为多行16位的ascii码"""
    ans = ''
    for c in s:
        ans += c2b(c)+'\n'
    return ans
def b2h(s,n = 4):
    """将01字符串转化为16进制字符串"""
    ans = hex(eval('0b'+s))[2:].upper()
    ans = (n - len(ans))*'0' + ans
    return ans
    
#文本处理
def deln(s):
    """如果字符串以'\n'结尾，则去掉"""
    if s[-1] == '\n':
        return s[:-1]
    else :
        return s
def addn(s):
    """使字符串统一以'\n'结尾"""
    if s[-1] == '\n':
        return s
    else:
        return s + '\n'
def normalize(ins):
    """将汇编指令大小写格式统一化"""
    if ins[:2] == "BR":
        return ins.upper()
    else:
        return ins

####################################################
#标签表
label_dict = dict()

#指令翻译模式
op_dict = {
"ADD"       : """ "0001" + reg(line[1]) + reg(line[2]) + op(line[3]) """,
"AND"       : """ "0101" + reg(line[1]) + reg(line[2]) + op(line[3]) """,
"BRN"       : """ "0000100" + pcoffset(line[1],9) """,
"BRZ"       : """ "0000010" + pcoffset(line[1],9) """,
"BRP"       : """ "0000001" + pcoffset(line[1],9) """,
"BRNZ"      : """ "0000110" + pcoffset(line[1],9) """,
"BRNP"      : """ "0000101" + pcoffset(line[1],9) """,
"BRZP"      : """ "0000011" + pcoffset(line[1],9) """,
"BRNZP"     : """ "0000111" + pcoffset(line[1],9) """,
"JMP"       : """ "1100000" + reg(line[1]) + "000000" """,
"JSR"       : """ "01001" + pcoffset(line[1],11) """,
"JSRR"      : """ "0100000"+reg(line[1])+"000000" """,
"LD"        : """ "0010" + reg(line[1]) + pcoffset(line[2],9) """,
"LDI"       : """ "1010" + reg(line[1]) + pcoffset(line[2],9) """,
"LDR"       : """ "0110" + reg(line[1]) + reg(line[2]) + offset(line[3]) """,
"LEA"       : """ "1110" + reg(line[1]) + pcoffset(line[2],9) """,
"NOT"       : """ "1001" + reg(line[1]) + reg(line[2]) + "111111" """,
"RET"       : """ "1100000111000000" """,
"RTI"       : """ "1000000000000000" """,
"ST"        : """ "0011" + reg(line[1]) + pcoffset(line[2],9) """,
"STI"       : """ "1011" + reg(line[1]) + pcoffset(line[2],9) """,
"STR"       : """ "0111" + reg(line[1]) + reg(line[2]) + offset(line[3]) """,
"TRAP"      : """ "11110000" + h2b(line[1],8) """,
"HALT"      : """ "1111000000100101" """,
".ORIG"     : """ raise UserWarning("'.ORIG' can be only used at the beginning of the program!") """,
".FILL"     : """ h2b(line[1],16) """,
".BLKW"     : """ addn('0'*16) * eval(line[1]) """,
".STRINGZ"  : """ s2b(eval(line[1])) + '0'*16""",
".END"      : """ raise UserWarning('.END' can be only used at the end of the program!) """
}

#计算当前行指令对应机器码的占用行数
rows_num_dict = {
".BLKW" : "eval(line[1])",
".STRINGZ" : "len(eval(line[1]))+1"
}

####################################################
def parser(filename):
    if GetSuffix(filename) != "asm":
        raise UserWarning("Input file name should be ended with '.asm'!")
    with open(filename,"r") as f_asm:
        bin_lines = []
        lines = f_asm.readlines()
        clean_lines = []                    #处理过后的、只剩label和指令
        for line in lines:
            cln_line = DelComment(sp(line)) #将一行删除注释，且分割元素，返回列表
            if cln_line != []:
                clean_lines.append(cln_line)
        clean_lines = cut(clean_lines)
        
        orig = h2d(clean_lines[0][1])       #程序起始地址
        clean_lines = clean_lines[1:]       #删去.ORIG所在行
        
        ############################################
        ##第一遍扫描
        ############################################
        cur = orig   #cur为当前扫描的行所在内存地址
        for i,line in enumerate(clean_lines):
            if normalize(line[0]) not in op_dict:   #即该行开始为label
                if len(line) == 1:
                    raise UserWarning("".join(line)+"\nLabel should not be attach before a blank line!")
                if normalize(line[1]) in op_dict:
                    label_dict[line[0]] = cur           #记录label
                    line = clean_lines[i] = line[1:]    #从clean_lines和line中删去label
                else:
                    raise UserWarning("".join(line)+"\nEach line is expected to have only one label!")
            cur += eval(rows_num_dict.setdefault(line[0],"1"))
        ############################################
        ##解析函数
        ############################################
        def reg(s):
            if s[0] != 'R':
                raise UserWarning("Register name must be headed with 'R'!")
            return d2ub(s[1:],3)
        def imm(s,n):
            if s[0] != '#':
                raise UserWarning("Immediates must be headed with '#'!")
            return d2ib(s[1:],n)
        def op(s):
            if s[0] == 'R':
                return "000"+reg(s)
            else:
                return '1'+imm(s,5)
        def offset(s):
            return imm(s,6)
        def pcoffset(s,n):
            if s[0] == "#":
                return imm(s,n)
            else:
                return d2ib(str(label_dict[s] - cur - 1),n)
        ############################################
        ##第二遍扫描
        ############################################
        cur = orig
        for line in clean_lines:
            bin_lines.append(addn(eval(op_dict[normalize(line[0])])))
            cur += eval(rows_num_dict[line[0]])
        #格式统一化处理
        bin_lines = [addn(d2ub(str(orig),16))] + bin_lines
        bin_lines = sp(''.join(bin_lines))
        ############################################
        ##写入
        ############################################
        with open(ChangeSuffix(filename,"bin"),"w") as f_bin:
            for line in bin_lines:
                f_bin.write(addn(line))
        with open(ChangeSuffix(filename,"hex"),"w") as f_hex:
            for line in bin_lines:
                f_hex.write(addn(b2h(line)))
        with open(ChangeSuffix(filename,"obj"),"wb") as f_bin:
            for line in bin_lines:
                f_bin.write(bytearray.fromhex(b2h(line)))
def main():
    filename = input("Name of file : ")
    parser(filename)
    input("\nPress any key to continue......\n")

main()