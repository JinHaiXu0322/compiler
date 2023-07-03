## MiniC编译前端的实现

```
本实验为2023年春季学期西北工业大学计算机学院编译原理实验第3次实验，106个测试案例通过了105个，实验使用改造后的minic文法。
```

#### 功能介绍：

```
本实验实现了规定文法的编译器，使用flex和bison工具完成了一个C语言编译器，包含以下功能：
	1、IR生成
	2、符号表生成
	3、输出AST并显示
	4、实现函数定义，各种表达式的计算
	5、基本块划分并输出CFG等
其中源代码文件位于本文件下minic目录下，下面将介绍一些可能会用到的命令。
需要注意的是，命令中具体的文件信息仅代表文件类型，可根据实际情况替换成对应的文件。
```

#### 源码编译：

```
本程序支持使用cmake进行编译，也支持make进行编译，注意源码文件应不含中文路径，否则cmake可能报错。
cmake可使用以下两条命令或直接点击编译按钮实现（生成cmake_minic.exe）：
	cmake -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
	cmake --build cmake-build-debug --parallel
	或者直接使用vscode的编译按钮即可
make命令由于在windows系统下，故需要使用以下形式（生成minic.exe）：
	mingw32-make
```

#### 测试脚本运行:

```
进行测试前需要提前下载好老师的测试案例和工具。
所有测例：sh minicrun.sh
单个测例：sh minicrun.sh 65(指测试编号为65的测例)
多个测例：sh minicrun.sh 54 65(指测试编号从54到65的测例)
```

#### 测试案例返回值查看：

```
这个部分需要提前下载好老师提供的编译工具：
先用以下指令生成待测程序ir文件：
	.\minic.exe -i -o 014_add.ir .\function\014_add.c
再用指令工具执行该ir文件：
	.\IRCompiler\MINGW-x86_64\IRCompiler.exe -R 014_add.ir（这种命令针对没有输入文件的测试程序）
	.\IRCompiler\MINGW-x86_64\IRCompiler.exe -R 080_color.ir<.\function\080_color.in（这种命令针对有输入文件的测试程序）
最后查看返回值（windows系统中返回值存储在%errorlevel%中）：
	echo Returned Value is %errorlevel%
```

#### IR生成：

```
.\minic.exe -i -o 015.ir .\function\015_add2.c
```

#### 符号表生成：

```
.\minic.exe -s -o 013_symbol.txt .\function\013_var_defn_func.c
```

#### AST输出：

```
./minic -a -o ast_015.png function/015_add2.c
```

#### CFG输出：

```
.\minic.exe -c 函数名 -o 060_block.png .\function\060_scope.c
```
