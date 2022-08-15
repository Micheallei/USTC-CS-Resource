from graphviz import Graph
def separator(line, sep=[' ','\n']):#分词函数，将文件中每一行的两个数提取出来，返回一个列表
    result = []
    i = 0
    while line[i] not in sep:       #找第一个数的切片
        i = i+1
    result.append(line[0:i])
    j = i+1
    while j < len(line) and line[j] not in sep:       #找第二个数的切片
        j = j + 1
    result.append(line[i+1:j])
    return result

dot = Graph('UDN')                  #构造无向图

with open('D:/Microsoft Visual Studio/MyProjects/wf_ep4/wf_ep4/test_2.txt', 'r') as f:
    num = int(f.readline())
    for k in range(num):             #创建点
        dot.node(name=str(k))
    for line in f:                  #创建边
        list_1 = separator(line)
        dot.edge(list_1[0], list_1[1])

dot.view()