import xml.dom.minidom
from lxml import etree
import sys

class LL1(object):
    def __init__(self, filename):
        self.grammar = list()
        self.f = open(filename, "r", encoding="utf8")
        for line in self.f.readlines():
            if line[:2] == '//' or not line or line[0] == '\n':
                continue
            else:
                self.grammar.append(line.strip('\n'))

        self.startVn = self.grammar[0].split("->")[0].split()[0]
        self.vn, self.vt = self.get_v()

        self.table = dict()
        self.initTable()
        print(self.vn)
        print(self.vt)
        print(self.grammar)


    def get_v(self):
        """
        get_V()函数得到终结符号集合和非终结符号结合
        :return: <set><set> 非总结符号集合和终结符号结合
        """
        vn, vt = set(), set()
        for c in self.grammar:
            left, right = c.split("->")
            vn.add(left.strip())
            r = right.split()
            for rr in r:
                vt.add(rr)
        vt -= vn
        return vn, vt

    def initTable(self):
        """
        初始化分析表函数
        :return: None
        """
        vtt = self.vt | set(["#"])
        vtt = vtt - set(["@"])
        for vn in self.vn:
            for vt in vtt:
                self.table[(vn, vt)] = "error"

    def first(self,ss):
        ans = set()
        if ss[0] in self.vt:
            ans |= set([ss[0]])
            return list(ans)

        if ss.__len__() == 1:
            for g in self.grammar:
                left, right = g.split("->")
                left = left.split()[0]  # <char>
                right = right.split()  # <list>

                if left == ss[0]:
                    times = 0
                    for i in right:
                        temp = (ans | set(self.first([i]))) - set(["@"])
                        ans = ans | temp
                        if "@" not in self.first([i]) and i != "@":
                            break
                        times += 1
                    if times == right.__len__():
                        ans.add("@")
            return list(ans)
        times = 0
        for i in ss:
            temp = (ans | set(self.first([i]))) - set(["@"])
            ans = ans | temp
            if "@" not in self.first([i]) and i != "@":
                break
            times += 1
        if times == ss.__len__():
            ans.add("@")
        return list(ans)

    def follow(self, ss):
        # print(ss)
        ans = set()
        if ss == "S":
            ans.add("#")
            return list(ans)
        for g in self.grammar:
            left, right = g.split("->")
            left = left.split()[0]
            right = right.split()
            if ss in right and right.index(ss) + 1 != right.__len__() and left != ss:
                ans = ans | set(self.first(right[right.index(ss) + 1:]))
                if "@" in self.first(right[right.index(ss) + 1:]):
                    ans = ans | set(self.follow(left))
            if ss in right and right.index(ss) + 1 == right.__len__() and left != ss:
                ans = ans | set(self.follow(left))
        ans -= set(["@"])
        return list(ans)


    def createTable(self):
        vtt = self.vt | set(["#"])
        vtt = vtt - set(["@"])
        for vn in self.vn:
            for vt in vtt:
                for g in self.grammar:
                    left, right = g.split("->")
                    left = left.split()  # <list>
                    right = right.split() # <list>
                    if vn == left[0]:
                        first_beta = self.first(right)
                        if vt in first_beta:
                            self.table[(vn, vt)] = g
                            # print(f"({vn}, {vt}): {g}")
                        if "@" in first_beta:
                            follow_beta = self.follow(vn)
                            for f in follow_beta:
                                self.table[(vn, f)] = g
                                # print(f"({vn}, {f}): {g}")
        print(self.table)


    def mainControl(self, in_file, out_file):
        dom = xml.dom.minidom.parse(in_file)
        root = dom.documentElement
        input_string = root.getElementsByTagName('value')
        input_type = root.getElementsByTagName('type')

        Cstring = []
        for i in range(len(input_string)):
            ctype = input_type[i].childNodes[0].nodeValue
            cvalue = input_string[i].childNodes[0].nodeValue
            if ctype == 'integer':
                Cstring.append("constants")
            elif ctype == 'character':
                Cstring.append("identifiers")
            else:
                Cstring.append(cvalue)
        chardict = {'integer', 'constants', 'identifiers'}


        vtt = self.vt | set(["#"])
        vtt = vtt - set(["@"])

        # Cstring = ['i', '+', 'i', '*', 'i']
        # Cstring = ['int', 'identifiers', '(', ')', '{', 'int', 'identifiers', '=', 'constants', ';', 'return', 'constants', ';', '}']
        Stack = []
        Vindex = 10001
        Stack.append(self.V_to_Vindex("#", Vindex))
        Vindex += 1
        Stack.append(self.V_to_Vindex(self.startVn, Vindex))
        Vindex += 1

        Cstring.append("#")
        i = 0


        root = etree.Element('ParserTree')

        root.set('name', out_file)
        locals()[Stack[-1]] = etree.SubElement(root, 'S')

        lastV = 'S' + '10002'
        while True:

            X = Stack[-1]
            a = Cstring[i]

            if self.Vindex_to_V(X) in vtt:
                if self.Vindex_to_V(X) == a and a == "#":
                    print("Successful!")
                    break
                elif self.Vindex_to_V(X) == a and a != "#":
                    aa = a
                    if a in chardict:
                        aa = input_string[i].childNodes[0].nodeValue

                    Stack.pop()
                    i += 1

                    locals()[a] = etree.SubElement(locals()[lastV], 'value')
                    locals()[a].text = aa
                    continue
                elif self.Vindex_to_V(X) != a:
                    print("Failed")
                    break
            elif self.Vindex_to_V(X) in self.vn:
                if self.table[(self.Vindex_to_V(X), a)] == "error":
                    print("Failed")
                    break
                else:
                    nodename = Stack[-1]
                    Stack.pop()

                    _, right = self.table[(self.Vindex_to_V(X), a)].split("->")
                    right = right.split() # <list>
                    if len(right) == 1 and right[0] == '@':
                        continue
                    else:
                        lastV = nodename
                        right.reverse()
                        ll = right.__len__()
                        for nodes in right:
                            Stack.append(self.V_to_Vindex(nodes, Vindex))
                            # if nodes in self.vn:
                            #     locals()[self.V_to_Vindex(nodes, Vindex)] = etree.Element(nodes)
                            #     locals()[nodename].append(locals()[self.V_to_Vindex(nodes, Vindex)])
                            Vindex += 1
                        for j in range(1, ll+1):
                            trueIndex = 0 - j
                            if self.Vindex_to_V(Stack[trueIndex]) in self.vn:
                                locals()[Stack[trueIndex]] = etree.SubElement(locals()[nodename], self.Vindex_to_V(Stack[trueIndex]))
                        # Stack.extend(right)
        tree = etree.ElementTree(root)
        tree.write(out_file, pretty_print=True, xml_declaration=True, encoding='utf-8')


    def V_to_Vindex(self, v, num):
        return v + str(num)

    def Vindex_to_V(self, v):
        v = v[: -5]
        return str(v)

if __name__ == "__main__":

    ll1 = LL1("grammar5")
    ll1.createTable()

    # testfile = open(sys.argv[1], 'r', encoding='utf8')
    # outfile = open(sys.argv[2], 'w+', encoding='utf8')
    ll1.mainControl(sys.argv[1], sys.argv[2])
