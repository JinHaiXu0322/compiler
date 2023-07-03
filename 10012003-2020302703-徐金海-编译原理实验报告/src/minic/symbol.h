#ifndef SYMBOL_H
#define SYMBOL_H

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

//写符号表，kind 自定义，会写ir里面的declare
void appendSymbol(string name, string type, string kind, string inname, int level, int tableId, int reftableid = -1);
// 根据符号表中变量名字和作用域级别，获取对应变量的临时变量名
void appendSymbolTemp(string type, string inname, int level, int tableId);
// 根据符号表中数组类型符号的名称获取其各维度的大小
vector<int> getSymbolArrayWei(string name, int tableId, int level);
//传入符号，支持依据数组ident查询,返回inname,测试用例中不存在同函数同level相同名
string getSymbolInname(string name, int level, int tableId);
//支持数组指针 *开头
Symbol *getSymbol(string inname, int level, int tableId);
// 用于生成符号表的具体内容
string printSymbol();

#endif // SYMBOL_H