#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdarg>
#include <string>
#include <graphviz/gvc.h>
#include <graphviz/gvcext.h>
#include <vector>
#include <sstream>
#include <stack>
using namespace std;

#include "ir.h"
#include "symbol.h"

int node_num = 0;
int edge_num = 0;
// 用于给定义的临时变量标号
int _unique = 0;


#define YYSTYPE lex_data
#include "yacc.tab.h"
// #include "ir.h"

// 声明一个指向抽象语法树的根节点的指针
extern Node *ast_root;

// 2为正文
int _used_max_label = 1;

// 2为正文。在条件判断时候进不同block的时候要改这个，以让写irs的时候写对
int G_NOW_LABEL = 2;
string G_FUNC_BLOCK = "_";

// int数据变为string数据
string int2str(int num)
{
    stringstream ss;
    ss << num;
    return ss.str();
}

// 用作t临时变量编号的生成，其中_unique为上文中定义的全局变量
string getUniqueIntTxt()
{
    return int2str(_unique++);
}

// 用作label数字生成
int getNewLabel()
{
    return ++_used_max_label;
}

// yyerror函数用于在语法错误发生时输出错误信息
// 函数接受一个格式化字符串和可变参数列表，将错误信息输出到标准错误流(stderr)
// yylloc.first_line, yylloc.first_column由lex词法单元提供，用于定位词法单元的起始行号和其实列号
void yyerror(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "There is a grammar error at line %d column %d: ", yylloc.first_line, yylloc.first_column);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ".\n");
}

vector<Block *> BlockVec;

// 定义了一个名为SymbolTable的二维向量，其元素类型为指向Symbol结构体的指针
//[0] 为全局，后面为每个function一个。每个function按block来分level
vector<vector<Symbol *>> SymbolTable(1001);

// 0号ir表为全局变量declare,后续为每一个函数的
// 其中[1][0]为第一个函数形参declare主内容
// [1][1]为返回值以及后续根据符号表扫描插入的declare(局部和临时)
// [2]为正文（不含declare）
// [1][3]即为第三方.L3
vector<vector<vector<string>>> irs(1001);

// 这个一维向量用于存储每个作用域（scope）的返回值的中间代码.L1 return
vector<string> irreturn(1001);

// 因为有全局变量V_D_ASSIGN存在，所以全局变量的赋值要插在main头部
vector<string> main_first_ir;

// 定义一个名为whileInfoStack的堆栈，用于存储WhileInfo结构体的元素
// 其中包含条件标签(condition_label)、继续标签(continue_label)和跳出标签(break_label)
stack<WhileInfo> whileInfoStack;
//从ir语句中读取存在的Label数字
vector<int> getLabelsInIr(string ir)
{
    // 初始化一个空的整数向量rt，用于存储提取的标签
    vector<int> rt;
    // _start初始化为0表示从ir的最起始位置进行查找
    int _start = 0;
    int t = 0;
    // 使用find函数在IR代码字符串中查找.L的位置，从而找到标签的起始位置
    while (ir.find(".L", _start) < ir.length()) {
        //find用于在字符串中搜索指定的子字符串，并返回第一次出现的位置
        _start = ir.find(".L", _start);
        t = 0;
        // 如果当前字符是数字字符，则将其转换为对应的数值（ir[i] - '0'）
        // 然后乘以10（t = t * 10）并累加到变量t中，乘10的作用是正确地将.L后的数字串转化为数字，如.L123
        for (int i = _start + 2; i < ir.length() && ir[i] <= '9' && ir[i] >= '0'; i++) {
            t = t * 10 + (ir[i] - '0');
        }
        // 将解析得到的标签值t添加到结果向量rt中
        rt.push_back(t);
        //破坏_start当前指向的.L，继续进行while循环找到下一个
        _start++;
    }
    return rt;
}

// insertToLabels()用于将label值插入到给定的block中
void insertToLabels(Block *it, int label)
{
    // 检查块中的目标标签列表 to_labels 的大小。如果列表为空，
    // 即没有目标标签，那么直接将标签值 label 插入到列表中，作为第一个目标标签。
    if (it->to_labels.size() == 0)
        it->to_labels.push_back(label);

    // 如果列表不为空，判断是否已经存在与 label 相同的标签值，有则break，反之插入
    else {
        //用于检查当前label是否已经被插入
        bool finded = false;
        for (auto la : it->to_labels) {
            if (la == label) {
                finded = true;
                break;
            }
        }
        if (!finded)
            it->to_labels.push_back(label);
    }
}

// 将生成的中间代码以字符串的形式返回，用于ir的输出
string printIrs()
{
    // 定义一个空字符串 rt 作为最终生成的中间代码
    string rt = "";
    //当前的函数是不是要分析的
    bool bool_func_block = false;
    // irs[0][0]为全局变量的声明
    for (auto it : irs[0][0]) {
        rt = rt + it + "\n";
    }
    //下面这个是遍历存在每个函数对应的irs中的二维数组
    for (int _tableid = 1; irs[_tableid][2].size() > 0; _tableid++) {
        // 是一个字符串类型的变量，用于存储函数的返回类型
        string _func_return_type;
        //下面这个指对应的函数symbol
        Symbol *_it;
        // 根据当前迭代的表格ID，在全局符号表中找到对应的函数符号，并将其赋值给_it
        for (int i = 0; i < SymbolTable[0].size(); i++) {
            if (SymbolTable[0][i]->kind == "function" && SymbolTable[0][i]->ref_tableid == _tableid) {
                _it = SymbolTable[0][i];
            }
        }
        // 将当前函数的返回类型和输入参数的类型和名称添加到结果字符串 rt 中
        rt = rt + "define " + _it->type + " " + _it->inname + "(";
        if (_it->inname == "@" + G_FUNC_BLOCK)
            bool_func_block = true;
        else
            bool_func_block = false;
        bool first = true;
        // 遍历符号表
        for (int i = 0; i < SymbolTable[_it->ref_tableid].size(); i++) {
            if (SymbolTable[_it->ref_tableid][i]->kind == "temporary") {
                if (first) {
                    rt = rt + SymbolTable[_it->ref_tableid][i]->type + " " + SymbolTable[_it->ref_tableid][i]->inname;
                    first = false;
                } else {
                    rt = rt + "," + SymbolTable[_it->ref_tableid][i]->type + " " + SymbolTable[_it->ref_tableid][i]->inname;
                }
            }
        }
        rt = rt + ") {\n";
        // 如果当前函数是主函数 main，则将预先生成的 main_first_ir 中的中间代码添加到结果字符串 rt 中
        if (_it->inname == "@main") {
            for (auto it : main_first_ir) {
                rt = rt + it + "\n";
            }
        }
        // 将当前函数的各个阶段的中间代码添加到结果字符串 rt 中，并将其中的标签和标签对应的中间代码添加到 BlockVec 中
        for (auto it : irs[_tableid][0]) {
            rt = rt + it + "\n";
        }

        string rt_inname;
        // 如果当前函数有返回语句，则在函数退出前将返回语句的值赋给临时变量，并添加退出函数的中间代码到结果字符串 rt 中
        if (SymbolTable[_tableid].size() > 0 && SymbolTable[_tableid][0]->name == "RETURNED VALUE") {
            Symbol *tmp_rt = new Symbol();
            rt_inname = "%t" + int2str(SymbolTable[_tableid].size());
            tmp_rt->name = "temp_rt_" + rt_inname;
            tmp_rt->inname = rt_inname;
            tmp_rt->type = SymbolTable[_tableid][0]->type;
            tmp_rt->kind = "temporary";
            tmp_rt->level = 1;

            SymbolTable[_tableid].push_back(tmp_rt);
            irs[_tableid][1].push_back("declare " + tmp_rt->type + " " + rt_inname);
        }
        for (auto it : irs[_tableid][1]) {
            rt = rt + it + "\n";
        }
        // 将当前函数的中间代码添加到结果字符串 rt 中，并将其中的标签和标签对应的中间代码添加到 BlockVec 中
        Block *b = new Block();
        if (bool_func_block) {
            b->irs.push_back(".L2:");
            b->irs.push_back("entry");
            b->label = 2;
        }
        rt = rt + "entry\n";
        // 遍历当前函数的第二个阶段的中间代码
        for (auto it : irs[_tableid][2]) {
            rt = rt + it + "\n";
            if (bool_func_block) {
                b->irs.push_back(it);
                // 获取当前中间代码中包含的标签
                vector<int> labels = getLabelsInIr(it);
                // 遍历这个标签中的所有代码
                for (auto label : labels)
                    insertToLabels(b, label);
            }
        }
        if (bool_func_block)
            BlockVec.push_back(b);

        for (int i = 3; i < 200; i++) {
            // 如果当前阶段有中间代码
            if (irs[_tableid][i].size() > 0) {
                rt = rt + ".L" + int2str(i) + ":\n";
                // 创建一个新的 Block 对象 b
                b = new Block();
                // 在 b 中添加标签
                if (bool_func_block) {
                    b->label = i;
                    b->irs.push_back(".L" + int2str(b->label) + ":");
                }
                // 遍历当前阶段的中间代码
                for (int j = 0; j < irs[_tableid][i].size(); j++) {
                    rt = rt + irs[_tableid][i][j] + "\n";
                    if (bool_func_block) {
                        b->irs.push_back(irs[_tableid][i][j]);
                        // 获取当前中间代码中包含的标签
                        vector<int> labels = getLabelsInIr(irs[_tableid][i][j]);
                        for (auto label : labels)
                            insertToLabels(b, label);
                    }
                }
                // 将 b 添加到 BlockVec 中
                if (bool_func_block)
                    BlockVec.push_back(b);
            }
        }

        b = new Block();
        b->label = 1;
        b->irs.push_back(".L1:");
        // 如果当前函数有返回语句
        if (SymbolTable[_tableid].size() > 0 && SymbolTable[_tableid][0]->name == "RETURNED VALUE") {
            rt = rt + ".L1:\n";
            rt = rt + rt_inname + " = " + SymbolTable[_tableid][0]->inname + "\n";
            rt = rt + "exit " + rt_inname + "\n}\n";
            // 在 b 中添加临时变量的赋值语句和退出函数的中间代码
            if (bool_func_block) {
                b->irs.push_back(rt_inname + " = " + SymbolTable[_tableid][0]->inname);
                b->irs.push_back("exit " + rt_inname);
            }
        } else {
            rt = rt + ".L1:\nexit" + "\n}\n";
            // 在 b 中添加退出函数的中间代码
            if (bool_func_block) {
                b->irs.push_back("exit");
            }
        }
        if (bool_func_block)
            BlockVec.push_back(b);
    }
    // 返回最终生成的中间代码字符串 rt
    return rt;
}

//从a[x][y]转为a，用于返回数组名
string convertArrayToIdent(string input)
{
    string rt = "";

    // 遍历输入字符串中的每个字符
    for (int i = 0; i < input.length(); i++) {
        // 如果该字符不是左中括号 '['，则将其转化为一个字符串类型的对象 s
        if (input[i] != '[') {
            string s;
            s = input[i];
            // 将 s 追加到结果字符串 rt 的末尾
            rt = rt + s;
        }
        // 如果遇到了左中括号，就跳出循环，因为中括号后面的部分表示数组的下标，需要去除
        else {
            break;
        }
    }
    // 返回去除中括号后的标识符
    return rt;
}

//尽可能把全局或局部变量转换为临时，仅用于取值。支持数组指针(*开头)
string convertToTemp(string inname, int cur_table, int cur_level, int cur_label)
{
    string rt = inname;
    //*%t 数组指针
    if (inname[0] == '*' && inname[1] == '%' && inname[2] == 't') {
        string t = "%t" + getUniqueIntTxt();
        //这里的_type为类似i32型
        string _type = getSymbol(inname, cur_level, cur_table)->type;
        _type = _type.substr(0, _type.find('*'));
        appendSymbolTemp(_type, t, cur_level, cur_table);
        //写ir 临时=全局/局部
        irs[cur_table][cur_label].push_back(t + " = " + inname);
        rt = t;
    } else if ((inname[0] == '%' && inname[1] == 'l') || inname[0] == '@') {
        //右值为全局或局部，先取到临时
        string t = "%t" + getUniqueIntTxt();
        //读这个变量类型
        string _type = getSymbol(inname, cur_level, cur_table)->type;
        appendSymbolTemp(_type, t, cur_level, cur_table);
        //写ir 临时=全局/局部
        irs[cur_table][cur_label].push_back(t + " = " + inname);
        rt = t;
    }
    return rt;
}
