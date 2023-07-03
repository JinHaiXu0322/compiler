# 普通班功能测试用例

## IR编译执行器说明

在IRCompiler文件夹下根据平台选择对应的文件夹下的自定义IR的编译执行器IRCompiler.exe，
该程序可以对IR进行编译与运行。

假定输入的IR文件名为test.ir，C源文件为test.c。请注意如下的命令在mingw运行终端上进行运行。

## mingw运行终端

在Git克隆的文件夹下会有一个mingw.bat的文件，该文件要与minicrun.sh在同一个文件夹下，其内容如下：

C:\LinuxEnv\msys64\msys2_shell.cmd -mingw64 -here  -defterm -no-start

这里假定msys2_shell.cmd的位置在C:\LinuxEnv\msys64文件夹下，请根据如下的方法修改。

方法：请查找之前解压compilertools工具集，然后在msys64下会存在文件msys2_shell.cmd文件，请获取
该文件的位置或路径调整mingw.bat。

双击mingw.bat即可进入mingw的运行终端。

## 脚本运行需安装的软件

在进入mingw的运行终端后，利用pacman安装相关的软件，如diff软件等，请注意32位或者64位MinGW

这里假定运行的是64位的编译器工具。

pacman -S mingw-w64-x86_64-diffutils

如git没有安装，建议安装git。

pacman -S git

### C程序到自定义IR

可进入IRCompiler\MINGW-x86_64下通过IRCompiler程序运行得出参考输出的IR。

./IRCompiler.exe -S -I -o test.ir test.c

供自己实现的minic编译器输出结果有误进行参考，切记不能用这里提供的IRCompiler用作你实现的编译器，
一旦发现0分处理。

### IR直接执行

./IRCompiler.exe -R test.ir

运行后可通过echo $?来获取main函数的返回值。

## 测试用例

这里提供了103个测试用例，在function文件夹下。

这里面没有const和变量初始化相关的用例。

### minicrun.sh使用

在运行前需要把你编写的minic编译器程序minic拷贝到Git克隆的文件夹下，保证与minicrun.sh在同一个文件夹下。

功能：先运行minic程序得出翻译后的IR，然后通过IRCompiler下提供的工具实现把Ir文件翻译并执行

命令：sh minicrun.sh 开始用例号 结束用例号

从开始用例号开始到结束用例号之间的所有用例进行编译执行

只运行1个用例，例如：sh minicrun.sh 0

运行一个序列的用例，例如: sh minicrun.sh 3 4

运行全部用例，例如：sh minicrun.sh 0 102

## 错误问题调查方法

如果运行出现问题，可按如下的步骤进行调查，假定你输出的IR名字为test.ir

1）利用IRCompiler工具把test.ir转换为test2.ir，看是否成功？

IRCompiler -S -I -o test2.ir test.ir

IRCompiler编译器会对test.ir进行编译检查错误，输出到test2.ir文件中。

请注意变量名会重新编号，注意与原始文件的不同。

2）把test2.ir转换成llvm格式的IR，看是否成功

IRCompiler -S -L -o test2.ll test2.ir

3）对llvm格式的IR进行编译，看是否错误

llc test2.ll

根据错误报告的行号定位llvm IR错误的位置，反推到test2.ir的错误地方。
