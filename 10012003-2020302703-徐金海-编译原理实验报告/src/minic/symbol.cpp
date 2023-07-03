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
void appendSymbol(string name, string type, string kind, string inname, int level, int tableId, int reftableid)
{
    Symbol *rt = new Symbol();
    rt->name = name;
    // 根据传入的参数 type，将其转换为对应的 IR 类型，并设置为 rt 的 type 属性
    if (type == "int")
        rt->type = "i32";
    else if (type == "char")
        rt->type = "i8";
    else if (type == "void")
        rt->type = "void";
    else if (type[0] == 'i')
        rt->type = type;

    rt->kind = kind;
    rt->inname = inname;
    rt->level = level;
    rt->ref_tableid = reftableid;
    SymbolTable[tableId].push_back(rt);
    string inname_fix_for_array = rt->inname;
    // 如果该符号是数组类型，则根据其数组维度生成相应的中间代码，并将其添加到 inname_fix_for_array 中
    if (rt->name.find('[') < rt->name.length()) {
        //定义的时候补上[1][2]一类的
        vector<int> wei = getSymbolArrayWei(rt->name.substr(0, rt->name.find('[')), tableId, level);
        for (auto it : wei)
            inname_fix_for_array = inname_fix_for_array + "[" + int2str(it) + "]";
    }
    // 根据符号的种类生成相应的中间代码，并将其添加到对应的符号表中
    if (kind == "GLOBAL VARIABLE") {
        // tableId 应该恒定为0
        irs[tableId][0].push_back("declare " + rt->type + " " + inname_fix_for_array);
    } else if (kind == "LOCAL VARIABLE") {
        irs[tableId][1].push_back("declare " + rt->type + " " + inname_fix_for_array);
    } else if (kind == "FORMAL PARAMETER") {
        // tableId 应该>0
        irs[tableId][0].push_back("declare " + rt->type + " " + inname_fix_for_array);
    } else if (kind == "RETURNED VALUE") {
        irs[tableId][1].push_back("declare " + rt->type + " " + rt->inname);
    }
}

//为临时变量写符号表和对应IR声明
void appendSymbolTemp(string type, string inname, int level, int tableId)
{
    Symbol *rt = new Symbol();
    rt->name = "temp_" + inname;
    // 根据传入的参数 type，将其转换为对应的 IR 类型，并设置为 rt 的 type 属性
    if (type == "int")
        rt->type = "i32";
    else if (type == "char")
        rt->type = "i8";
    else if (type == "void")
        rt->type = "void";
    else if (type[0] == 'i')
        rt->type = type;
    rt->kind = "temporary variable";
    rt->inname = inname;
    rt->level = level;
    rt->ref_tableid = 0;
    SymbolTable[tableId].push_back(rt);
    // 生成相应的中间代码，并将其添加到 irs 数组中
    irs[tableId][1].push_back("declare " + rt->type + " " + rt->inname);
}

//数组ident查询,,仅传入数组，如a[3][4]传a,返回{3,4}
vector<int> getSymbolArrayWei(string name, int tableId, int level)
{
    string _name = "";
    vector<int> rt;
    // 先在局部同作用域 level 中查找同名数组类型符号
    auto table = SymbolTable[tableId];
    for (auto it : table) {
        if (convertArrayToIdent(it->name) == name && it->level == level) {
            _name = it->name;
        }
    }
    if (_name == "") {
        // 如果在局部作用域中没有找到同名符号，则降低作用域级别往下查找
        for (int _level = level - 1; _level >= 0; _level--) {
            auto table = SymbolTable[tableId];
            for (auto it : table) {
                if (convertArrayToIdent(it->name) == name && it->level == _level) {
                    _name = it->name;
                }
            }
        }
    }
    if (_name == "") {
        // 如果在当前符号表和所有作用域中都没有找到同名符号，则在全局作用域中查找
        auto table = SymbolTable[0];
        for (auto it : table) {
            if (convertArrayToIdent(it->name) == name) {
                _name = it->name;
            }
        }
    }
    // 如果仍然没有找到同名符号，则输出错误信息
    if (_name == "") {
        cout << "error @getSymbolArrayWei" << endl;
    }
    int tmp_sum = 0;
    bool working = false;
    // 遍历 _name 中的每个字符
    for (int i = 0; i < _name.length(); i++) {
        // 如果遇到了左中括号，则将 tmp_sum 初始化为 0，working 设置为 true
        if (_name[i] == '[') {
            tmp_sum = 0;
            working = true;
        }
        // 如果 working 为 true 且当前字符是数字，则将 tmp_sum 乘以 10 并加上当前数字
        else if (working && _name[i] >= '0' && _name[i] <= '9') {
            tmp_sum = tmp_sum * 10 + (_name[i] - '0');
        }
        // 如果遇到了右中括号，则将 working 设置为 false，将 tmp_sum 添加到向量 rt 中
        else if (_name[i] == ']') {
            working = false;
            rt.push_back(tmp_sum);
        }
    }
    return rt;
}

// 根据符号表中变量名字和作用域级别，获取对应变量的临时变量名
string getSymbolInname(string name, int level, int tableId)
{
    auto table = SymbolTable[tableId];
    // 先在当前符号表中查找同名变量，并且作用域级别相同的变量
    for (auto it : table) {
        if (convertArrayToIdent(it->name) == name && it->level == level) {
            return it->inname;
        }
    }
    // 如果在当前符号表中没有找到同名变量，则降低作用域级别往下查找
    for (int _level = level - 1; _level >= 0; _level--) {
        auto table = SymbolTable[tableId];
        for (auto it : table) {
            if (convertArrayToIdent(it->name) == name && it->level == _level) {
                return it->inname;
            }
        }
    }
    // 如果在所有作用域中都没有找到对应的变量，则在全局作用域中查找同名变量
    table = SymbolTable[0];
    for (auto it : table) {
        if (convertArrayToIdent(it->name) == name) {
            return it->inname;
        }
    }
    // 如果仍然没有找到同名变量，则输出错误信息并返回空字符串
    cout << "error @getSymbolInname" << endl;
    return "";
}

//支持数组指针 *开头
Symbol *getSymbol(string inname, int level, int tableId)
{
    auto table = SymbolTable[tableId];
    // 如果符号名的第一个字符为'*'，则将其删除
    if (inname[0] == '*') {
        inname = inname.substr(1);
    }
    // 先在局部同作用域 level 中查找同名数组类型符号
    for (auto it : table) {
        if (it->inname == inname && it->level == level) {
            return it;
        }
    }
    //局部没有就降level往下面找
    for (int _level = level - 1; _level >= 0; _level--) {
        for (auto it : table) {
            if (it->inname == inname && it->level == _level) {
                return it;
            }
        }
    }
    // 如果在当前符号表和其下层符号表中都没有找到同名符号，则在全局符号表中查找
    table = SymbolTable[0];
    for (auto it : table) {
        if (it->inname == inname) {
            return it;
        }
    }
    // 如果在任何符号表中都没有找到同名符号，则打印错误信息并返回空指针
    cout << "error @getSymbol" << endl;
    return NULL;
}

// 用于生成符号表的具体内容
string printSymbol()
{
    string rt = "";
    for (int i = 0; i < SymbolTable.size(); i++) {
        if (SymbolTable[i].size() > 0) {
            rt = rt + "*****************************符号表[" + int2str(i) + "]*****************************\n";
            // rt = rt + "[name] [type] [kind] [inname] [level] [ref_tableid]?\n";
            for (int j = 0; j < SymbolTable[i].size(); j++) {
                rt = rt + SymbolTable[i][j]->name + ": | [" + SymbolTable[i][j]->type + "] | [" + SymbolTable[i][j]->kind + "] | [" + SymbolTable[i][j]->inname + "] | [" + int2str(SymbolTable[i][j]->level) + " (" + int2str(SymbolTable[i][j]->ref_tableid) + ")]\n";
            }
        }
    }
    return rt;
}