import sys

class DFA:
    file_object = ''  # 文件句柄
    line_number = 0  # 记录行号
    state = 0  # 状态
    ResWord = ['int', 'if', 'then', 'else', 'end', 'repeat', 'until', '_Alignas', '_Alignof',
               'auto', 'short', 'long', 'float', 'double', 'struct', 'union', 'enum',
               'typedef', 'const', 'unsigned', 'signed', 'extern', 'static', 'void',
               'switch', 'for', 'case', 'do', 'while', 'goto', 'continue', 'break',
               'default', 'sizeof', 'return', 'define', 'include', 'inline', 'volatile',
               'char', '_Atomic', '_Bool', '_Complex', '_Generic', '_Imaginary', '_Noreturn',
               '_Static_assert', '_Thread_local']  # 保留字
    error_message = []  # 保存错误信息,存储元组,元组第一个参数是行号,第二个参数是错误字符
    annotate_message = []  # 注释信息,存储元组,元组第一个参数是行号,第二个参数是注释
    char_message = []  # 识别的字符串,存储元组,元组第一个参数是类型,第二个参数是该字符串

    def __init__(self, file_name, writefile):
        self.file_object = file_name  # 记录文件句柄
        self.state = 0                # 当前状态
        self.line_number = 0          # 当前行数
        self.error_message = []       # 记录错误信息
        self.annotate_message = []    # 记录注释信息
        self.char_message = []        # 记录字符
        self.word_number = 0
        self.writefile = writefile


    def Start_convert(self):
        for line in self.file_object:  # 一行行的处理
            self.state = 0
            #line = line.strip('\n')  # 去除换行fu
            self.line_number += 1  # 每处理一行行号加一
            line_length = len(line)
           # print(self.writefile)

            i = 0
            string = ''  # 存储一个字符串
            while i < line_length:
                ch = line[i]  # 读取该行的一个字符
                i += 1

                # state == 0 时的操作，最外层的判断语句相当于数据中心法中的主表
                if self.state == 0:
                    if ch == '\n':
                        break
                    if ch == ' ':
                        self.state = 0
                    elif ch == '_' or ch.isalpha():
                        self.state = 101             # accept 字符
                        string = string + ch

                    elif ch == '[' or ch == ']' or ch == '(' or ch == ')' or ch == '?' or ch == ':': # accept [, ], (, ), ?, :
                        string = string + ch
                        self.state = 301
                    elif ch == '&':
                        string = string + ch    # accept &
                        self.state = 302
                    elif ch == '+':             # accept +
                        string += ch
                        self.state = 304
                    elif ch == '-':             # accept -
                        string += ch
                        self.state = 306
                    elif ch == '=' or ch == '*' or ch == '%' or ch == '!' or ch == '^':   # accept  =, *, %, !, ^
                        string += ch
                        self.state = 309
                    elif ch == '/':             # accept /
                        string += ch
                        self.state = 311
                    elif ch == '>':             # accept >
                        string += ch
                        self.state = 315

                    elif ch == '<':             # accept <
                        string += ch
                        self.state = 317
                    elif ch == '#':             # accept #
                        string += ch
                        self.state = 319
                    elif ch == "&":             # accept &
                        string += ch
                        self.state = 321
                    elif ch == '|':             # accept |
                        string += ch
                        self.state = 323
                    elif ch == '}' or ch == '{' or ch == ',' or ch == ';':        # accept 分隔符
                        string += ch
                        self.print_token('separator', string, self.line_number)
                        i -= 1
                        string = ''
                        self.state = 401
                    elif ch == "'" or ch == '"':                                  # accept " '
                        string += ch
                        self.print_token('separator', string, self.line_number)
                        i -= 1
                        string = ''
                        self.state = 325
                    elif ch.isdigit() and ch != '0':
                        string += ch
                        self.state = 201
                    elif ch == '0':
                        string += ch
                        self.state = 203
                    elif ch == '.':
                        string += ch
                        self.state = 211
                elif self.state == 201:
                    if ch.isdigit():
                        string += ch
                        self.state = 201
                    else:
                        i -= 1
                        self.print_token('integer', string, self.line_number)
                        string = ''
                        self.state = 0
                elif self.state == 203:
                    if ch.isdigit() and ch != '8' and ch != '9':
                        string += ch
                        self.state = 203
                    elif ch == '8' or ch == '9':
                        string += ch
                        self.state = 209
                    elif ch == 'x' or ch == 'X':
                        string += ch
                        self.state = 205
                    elif ch == '.':
                        string += ch
                        self.state = 208
                    else:
                        i -= 1
                        self.print_token('integer', string, self.line_number)
                        string = ''
                        self.state = 0
                elif self.state == 205:
                    if self.ishex(ch):
                        string += ch
                        self.state = 206
                    else:
                        i -= 1
                        print('it is wrong--{}'.format(string))
                        string = ''
                        self.state = 0
                elif self.state == 206:
                    if self.ishex(ch):
                        string += ch
                        self.state = 206
                    else:
                        i -= 1
                        self.print_token('integer', string, self.line_number)
                        string = ''
                        self.state = 0
                elif self.state == 208:
                    if ch.isdigit():
                        string += ch
                        self.state = 208
                    elif ch == 'e' or ch == 'E':
                        string += ch
                        self.state = 210
                    else:
                        i -= 1
                        self.print_token('float', string, self.line_number)
                        string = ''
                        self.state = 0
                elif self.state == 209:
                    if ch.isdigit:
                        string += ch
                        self.state = 209
                    elif ch == '.':
                        string += ch
                        self.state = 208
                    elif ch == 'e' or ch == 'E':
                        string += ch
                        self.state = 210
                    else:
                        i -= 1
                        print('it is wrong--{}'.format(string))
                        self.state = 0
                elif self.state == 210:
                    if ch.isdigit():
                        string += ch
                        self.state = 212
                    else:
                        i -= 1
                        print('it is wrong--{}'.format(string))
                        string = ''
                        self.state = 0
                elif self.state == 211:
                    if ch.isdigit():
                        string += ch
                        self.state = 208
                    else:
                        i -= 1
                        print(string)
                        string = ''
                        self.state = 0
                elif self.state == 212:
                    if ch.isdigit():
                        string += ch
                        self.state = 212
                    else:
                        i -= 1
                        self.print_token('float', string, self.line_number)
                        string = 0
                        self.state = 0

                elif self.state == 101:
                    if ch == '_' or ch.isalpha() or ch.isdigit():
                        string = string + ch
                        self.state = 101
                    else:
                        i -= 1
                        if string in self.ResWord:
                            self.print_token("keyword", string, self.line_number)
                        else:
                            self.print_token("character", string, self.line_number)
                        string = ''
                        self.state = 0
                # 301 [ ] ( ) ? : 接受状态
                elif self.state == 301:
                    self.print_token('operator', string, self.line_number)
                    i -= 1
                    string = ''
                    self.state = 0

                elif self.state == 302:
                    if ch == '&' or ch == '=':
                        string += ch
                        self.state = 303
                    else:
                        # 接受 #
                        string = ''
                        i -= 1
                        self.state = 0

                # 303 ## #= 接受状态
                elif self.state == 303:
                    string = ''
                    i -= 1
                    self.state = 0
                elif self.state == 304:
                    if ch == '+' or ch == '=':
                        self.state = 305
                    else:
                        self.print_token('operator', string, self.line_number)
                        string = ''
                        i -= 1
                        self.state = 0

                elif self.state == 305:
                    string = ''
                    i -= 1
                    self.state = 0
                elif self.state == 306:
                    if ch == '-' or ch == '=' or ch == '>':
                        string += ch
                        self.state = 307
                    else:
                        self.print_token('operator', string, self.line_number)
                        string = ''    # accept -
                        i -= 1
                        self.state = 0
                elif self.state == 307:
                    self.print_token('operator', string, self.line_number)
                    i -= 1
                    string = ''
                    self.state = 0
                elif self.state == 309:
                    if ch == '=':
                        string += ch
                        self.state = 310
                    else:
                        self.print_token("operator", string, self.line_number)
                        string = ''
                        i -= 1
                        self.state = 0
                elif self.state == 310:
                    self.print_token("operator", string, self.line_number)
                    string = ''
                    i -= 1
                    self.state = 0
                elif self.state == 311:
                    if ch == '=':
                        string += ch
                        self.state = 314
                    else:
                        string = ''
                        i -= 1
                        self.state = 0
                elif self.state == 314:
                    string = ''
                    i -= 1
                    self.state = 0

                elif self.state == 315:
                    if ch == '>' or ch == '=':
                        string += ch
                        self.print_token('operator', string, self.line_number)
                        string = ''
                        self.state = 316
                    else:             # accept >
                        self.print_token('operator', string, self.line_number)
                        i -= 1
                        self.state = 0

                elif self.state == 316:   # accept >= >>
                    i -= 1
                    self.state = 0

                elif self.state == 317:
                    if ch == '=' or ch == '<':
                        string += ch
                        self.print_token('operator', string, self.line_number)
                        string = ''
                        self.state = 318
                    else:              # accept <
                        self.print_token('operator', string, self.line_number)
                        string = ''
                        i -= 1
                        self.state = 0

                elif self.state == 318:
                    i -= 1
                    self.state = 0

                elif self.state == 319:
                    if ch == '#':
                        string += ch
                        self.state = 320
                    else:
                        self.print_token('operator', string, self.line_number)
                        string = ''      # accept #
                        i -= 1
                        self.state = 0
                elif self.state == 320:  # accept ##
                    string = ''
                    i -= 1
                    self.state = 0
                elif self.state == 321:
                    if ch == '&' or ch == '=':
                        string += ch
                        self.state = 322
                    else:
                        string = ''       # accept &
                        i -= 1
                        self.state = 0
                elif self.state == 322:    # accept &=
                    string = ''
                    i -= 1
                    self.state = 0
                elif self.state == 323:
                    if ch == '|':        # accept ||
                        string += ch
                        self.state = 324
                    else:
                        string = ''
                        i -= 1
                        self.state = 0
                elif self.state == 401:  # accept {} ; ,
                    self.state = 0
                elif self.state == 325:
                    self.state = 0

    def print_token(self, token, value, line):
        self.word_number += 1
        print("  <token>", file=self.writefile)
        print("    <number>{}</number>".format(self.word_number), file=self.writefile)
        print("    <value>{}</value>".format(value), file=self.writefile)
        print("    <type>{}</type>".format(token), file=self.writefile)
        print("    <line>{}</line>".format(line), file=self.writefile)
        print("    <valid>{}</valid>".format("true"), file=self.writefile)
        print("  </token>", file=self.writefile)

    def ishex(self, ch):

        hexx = ['a', 'b', 'c', 'd', 'e', 'f'
                'A', 'B', 'C', 'D', 'E', 'F']
        if ch.isdigit() or ch in hexx:
            return True
        else:
            return False
    def hello(self):
        print(" duan dian")


if __name__ == "__main__":
    test_object = open(sys.argv[1])
    writefile = open(sys.argv[2], 'w+')
    # test_object = open("test.c")
    # writefile = open("output.xml", 'w+')
    print('<?xml version="1.0" encoding="UTF-8"?>', file=writefile)
    print('<project name="{}">'.format(sys.argv[2]), file=writefile)
    test = DFA(test_object, writefile)
    test.hello()
    test.Start_convert()
    test.hello()
    print('</project>', file=writefile)
