#ifndef IR_H
#define IR_H

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
#include "ast.h"

extern int node_num;
extern int edge_num;
// 用于给定义的临时变量标号
extern int _unique;

#define YYSTYPE lex_data
#include "yacc.tab.h"
// #include "ir.h"

extern Node *ast_root;
extern int _used_max_label;
extern int G_NOW_LABEL;
extern string G_FUNC_BLOCK;

// 符号表，全局变量维护，之后每个函数维护一个，函数内部的block层次用level表示
typedef struct symbol_s {
    //符号的名称
    string name;
    //符号的类型:int void
    string type;
    //符号的种类:全局变量、局部变量、临时变量、\TEMPORARY VARIABLE\FORMAL PARAMETER\返回值（常量不需要？）、函数、临时
    string kind;
    //符号所在的作用域的名称:内部存的名字 
    string inname;
    //符号所在的层级
    int level;
    //符号关联的符号表的标识符，默认为-1
    int ref_tableid = -1;
} Symbol;

typedef struct WhileInfo_s {
    //条件语句对应的标签值
    int condition_label;
    //循环中continue语句对应的标签值
    int continue_label;
    //循环中break语句对应的标签值
    int break_label;
} WhileInfo;

//为基本块划分而产生的
typedef struct block_s {
    //块的标签值  
    int label;
    //存储中间代码的向量（字符串形式）
    vector<string> irs;
    //跳转目标标签的向量，用于插入label值
    vector<int> to_labels;
} Block;


extern vector<Block *> BlockVec;
extern vector<vector<Symbol *>> SymbolTable;
extern vector<vector<vector<string>>> irs;
extern vector<string> irreturn;
extern vector<string> main_first_ir;
extern stack<WhileInfo> whileInfoStack;
// yyerror函数用于在语法错误发生时输出错误信息
void yyerror(const char *, ...);
vector<int> getSymbolArrayWei(string name, int tableId, int level);
// exp_handler函数的作用是处理表达式节点，生成相应的中间代码。它根据当前节点的类型和子节点递归地处理表达式的各个部分，并返回生成的中间代码
string exp_handler(Node *cur, int cur_table, int cur_level, int cur_label, int true_label = -1, int false_label = -1);

// int数据变为string数据
string int2str(int);
// 用作t临时变量编号的生成，其中_unique为上文中定义的全局变量
string getUniqueIntTxt();
// 用作label数字生成
int getNewLabel();
//从ir语句中读取存在的Label数字
vector<int> getLabelsInIr(string ir);
// insertToLabels()用于将label值插入到给定的block中
void insertToLabels(Block *it, int label);
// 将生成的中间代码以字符串的形式返回，用于ir的输出
string printIrs();
//从a[x][y]转为a，用于返回数组名
string convertArrayToIdent(string input);
//尽可能把全局或局部变量转换为临时，仅用于取值。支持数组指针(*开头)
string convertToTemp(string inname, int cur_table, int cur_level, int cur_label);

#endif // IR_H