#### 以下是关于本次实验的一些说明：

```
1、实验环境：Windows + 老师提供的compilertools
2、当前目录下存放的calculator-flex-bison中存放的是计算器源码文件，编译前请自行删除其中的cmake-build-debug目录，否则可能会出现报错。
3.当前目录下存放的testfile中存放的是测试文件，其中.txt文件是用来测试计算器的计算功能，,pdf是与之对应的AST。文件与其具体测试的功能具有以下对应关系：
	combineTest.txt------整数和实数混合运算测试
	doubleTest.txt------小数点表示的实数测试
	integerAllFuncTest.txt------强化计算器整数类型数据完整功能测试
	integerTest.txt------原有计算器的测试
	nagNumTest.txt------求负运算测试
	scientificNotationTest.txt------科学计数法表示的实数测试
4、编译指令：
	cmake -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
	cmake --build cmake-build-debug --parallel
5、测试过程中用到的指令（注意自行替换文件名）：
	.\cmake-build-debug\calculator-flex-bison.exe -a test.txt
	.\cmake-build-debug\calculator-flex-bison.exe -a -o test.pdf test.txt
	.\cmake-build-debug\calculator-flex-bison.exe -r test.txt
	.\cmake-build-debug\calculator-flex-bison.exe -r -o ir.txt test.txt
	.\cmake-build-debug\calculator-flex-bison.exe -R test.txt
```

#### 头文件说明：

| 文件名            | 功能                                                         |
| ----------------- | ------------------------------------------------------------ |
| IRCode.h          | 中间代码生成部分，定义了一个描述中间代码的类 InterCode，以及与其相关的函数和变量。 |
| IRInst.h          | 中间代码指令部分，定义了在中间代码中可以使用的各种指令和操作符，以及与其相关的函数和变量。 |
| ast.h             | 抽象语法树部分，定义了处理抽象语法树的各种结构体和函数，以及与其相关的变量和宏定义。 |
| calculator_lex.h  | 词法分析器部分，用于声明识别和处理表达式中的单词；           |
| calculator_yacc.h | 语法解析器部分，用于定义对输入表达式进行解析和树构建的函数及相关数据类型的初始化等； |
| common.h          | 通用函数部分，包含了对抽象语法树和单词进行操作的函数的定义。 |
| expr.h            | AST树表达式计算部分。                                        |
| graph.h           | 输出AST树部分，实现了将抽象语法树以文本格式输出到文件中的函数。 |
| lexer.h           | 词法分析程序的头文件，包括一些宏定义和函数声明。             |
| parser.h          | 语法分析程序的头文件，定义了语法分析器相关的数据结构和函数。 |
| symbol.h          | 符号表部分，定义了在编译过程中会用到的数据结构和函数，包括临时数值、常量和变量型的数值，以及数值的查找和创建等操作。 |

