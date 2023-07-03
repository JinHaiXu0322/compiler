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
#include "graph.h"
#include "symbol.h"

/*
    这段代码用于处理逻辑与或运算，以下是函数的参数说明：
    and_or：待处理的AST节点
    true_label：表达式为真的标签
    false_label：表达式为假的标签
    cur_table：当前符号表的编号
    cur_level：当前符号表的层数
    cur_label：当前IR指令的标签
*/
int ast_or_calculator(Node *and_or, int true_label, int false_label, int cur_table, int cur_level, int cur_label)
{
    int rt_label = cur_label;
    int single_next_label = getNewLabel(); //当下的and_or 短路失败，判断第二个
    string first, second, finalrt, tt;
    if (and_or->children[0]->type == AST_OP_AND || and_or->children[0]->type == AST_OP_OR) {
        // 如果当前节点为AND
        if (and_or->type == AST_OP_AND) {
            int left_child_label = getNewLabel();
            // 递归调用自身，并将single_next_label和false_label作为参数，将结果赋值给rt_label
            rt_label = ast_or_calculator(and_or->children[0], single_next_label, false_label, cur_table, cur_level, left_child_label);
        }
        // 如果当前节点为OR
        else if (and_or->type == AST_OP_OR) {
            int left_child_label = getNewLabel();
            // 递归调用自身，并将single_next_label和false_label作为参数，将结果赋值给rt_label
            rt_label = ast_or_calculator(and_or->children[0], true_label, single_next_label, cur_table, cur_level, left_child_label);
        }
    }
    // 如果第一个子节点不是AND或OR
    else {
        // 处理第一个子节点
        first = exp_handler(and_or->children[0], cur_table, cur_level, cur_label, true_label, false_label);
        string first_type = getSymbol(first, cur_level, cur_table)->type;
        string first_cmp_result = first;
        // 如果第一个子节点是指针
        if (first_cmp_result[0] == '*') {
            // 创建一个名为t的临时变量，并将其添加到符号表中
            string t = "%t" + getUniqueIntTxt();
            appendSymbolTemp("i32", t, cur_level, cur_table);
            // 创建一条IR指令，将第一个子节点的值存储到t变量中
            irs[cur_table][cur_label].push_back(t + " = " + first_cmp_result);
            first_cmp_result = t;
        }
        // 根据逻辑运算符的不同，创建一条IR指令，并将其添加到irs数组的相应位置
        if (and_or->type == AST_OP_AND && and_or->parent->type != AST_OP_NOT) {
            irs[cur_table][cur_label].push_back("bc " + first_cmp_result + ", label .L" + int2str(single_next_label) + ", label .L" + int2str(false_label));
        } else if (and_or->type == AST_OP_AND && and_or->parent->type == AST_OP_NOT) {
            irs[cur_table][cur_label].push_back("bc " + first_cmp_result + ", label .L" + int2str(single_next_label) + ", label .L" + int2str(true_label));
        } else if (and_or->type == AST_OP_OR) {
            irs[cur_table][cur_label].push_back("bc " + first_cmp_result + ", label .L" + int2str(true_label) + ", label .L" + int2str(single_next_label));
        }
    }
    // 检查rt_label是否等于cur_label
    if (rt_label != cur_label) {
        cout << "cur_label:" << cur_label << " rt_label:" << rt_label << endl;
        // 如果当前IR指令的最后一条指令是br，则不添加新的IR指令
        if (irs[cur_table][cur_label].size() > 0 && irs[cur_table][cur_label].back().find("br") == 0) {
            /* code */
        }
        // 否则创建一条IR指令，并将其添加到irs数组中的相应位置
        else {
            irs[cur_table][cur_label].push_back("br label .L" + int2str(rt_label));
        }
    }
    // 处理第二个子节点
    if (and_or->children[1]->type != AST_OP_AND && and_or->children[1]->type != AST_OP_OR) {
        second = exp_handler(and_or->children[1], cur_table, cur_level, single_next_label);
        string second_cmp_result = second;
        if (second_cmp_result[0] == '*') {
            string t = "%t" + getUniqueIntTxt();
            appendSymbolTemp("i32", t, cur_level, cur_table);
            irs[cur_table][cur_label].push_back(t + " = " + second_cmp_result);
            second_cmp_result = t;
        }
        // 根据逻辑运算符的不同，创建一条IR指令，并将其添加到irs数组的相应位置
        if (and_or->type == AST_OP_AND && and_or->parent->type != AST_OP_NOT) {
            irs[cur_table][single_next_label].push_back("bc " + second_cmp_result + ", label .L" + int2str(true_label) + ", label .L" + int2str(false_label));
        } else if (and_or->type == AST_OP_OR) {
            irs[cur_table][single_next_label].push_back("bc " + second_cmp_result + ", label .L" + int2str(true_label) + ", label .L" + int2str(false_label));
        } else if (and_or->type == AST_OP_AND && and_or->parent->type == AST_OP_NOT) {
            irs[cur_table][single_next_label].push_back("bc " + second_cmp_result + ", label .L" + int2str(false_label) + ", label .L" + int2str(true_label));
        }
    } else {
        // 递归调用自身，并将true_label和false_label作为参数，将single_next_label作为cur_label
        ast_or_calculator(and_or->children[1], true_label, false_label, cur_table, cur_level, single_next_label);
    }

    return rt_label;
}

/*
    此函数用于处理其他表达式节点，返回生成的中间代码,参数说明：
    cur：当前节点，
    cur_table：当前符号表编号
    cur_level：当前作用域层数
    cur_label：当前标号
    true_label：True 标号
    false_label：False 标号
*/
string exp_handler(Node *cur, int cur_table, int cur_level, int cur_label, int true_label, int false_label)
{
    // 如果是赋值表达式
    if (cur->type == AST_OP_ASSIGN) {
        string le, ri;
        le = exp_handler(cur->children[0], cur_table, cur_level, cur_label);  // 处理左表达式
        ri = exp_handler(cur->children[1], cur_table, cur_level, cur_label);  // 处理右表达式
        string finalri = convertToTemp(ri, cur_table, cur_level, cur_label);  // 将右表达式转为临时变量
        irs[cur_table][cur_label].push_back(le + " = " + finalri);  // 添加赋值语句
        return "";
    }
    // 如果是算术运算表达式
    else if (cur->type == AST_OP_ADD || cur->type == AST_OP_SUB || cur->type == AST_OP_MUL || cur->type == AST_OP_DIV || cur->type == AST_OP_MOD) {
        string le, ri;
        le = exp_handler(cur->children[0], cur_table, cur_level, cur_label);  // 处理左表达式
        ri = exp_handler(cur->children[1], cur_table, cur_level, cur_label);  // 处理右表达式
        string finalle = convertToTemp(le, cur_table, cur_level, cur_label);  // 将左表达式转为临时变量
        string finalri = convertToTemp(ri, cur_table, cur_level, cur_label);  // 将右表达式转为临时变量
        string t = "%t" + getUniqueIntTxt();  // 生成一个唯一的临时变量名称
        appendSymbolTemp("i32", t, cur_level, cur_table);  // 添加该临时变量到符号表
        // 根据不同操作符生成不同的中间代码
        if (cur->type == AST_OP_ADD) {
            irs[cur_table][cur_label].push_back(t + "=add " + finalle + ", " + finalri);
        } else if (cur->type == AST_OP_SUB) {
            irs[cur_table][cur_label].push_back(t + "=sub " + finalle + ", " + finalri);
        } else if (cur->type == AST_OP_MUL) {
            irs[cur_table][cur_label].push_back(t + "=mul " + finalle + ", " + finalri);
        } else if (cur->type == AST_OP_DIV) {
            irs[cur_table][cur_label].push_back(t + "=div " + finalle + ", " + finalri);
        } else if (cur->type == AST_OP_MOD) {
            irs[cur_table][cur_label].push_back(t + "=mod " + finalle + ", " + finalri);
        }
        return t;
    }
    // 如果是关系运算表达式
    else if (cur->type == AST_OP_RELOP) {
        string le, ri;
        le = exp_handler(cur->children[0], cur_table, cur_level, cur_label);  // 处理左表达式
        ri = exp_handler(cur->children[1], cur_table, cur_level, cur_label);  // 处理右表达式
        string finalle = convertToTemp(le, cur_table, cur_level, cur_label);  // 将左表达式转为临时变量
        if (finalle[0] == '*') {
            string t = "%t" + getUniqueIntTxt();  // 生成一个唯一的临时变量名称
            appendSymbolTemp("i32", t, cur_level, cur_table);  // 添加该临时变量到符号表
            irs[cur_table][cur_label].push_back(t + " = " + finalle);
            finalle = t;
        }
        string finalri = convertToTemp(ri, cur_table, cur_level, cur_label);  // 将右表达式转为临时变量
        if (finalri[0] == '*') {
            string t = "%t" + getUniqueIntTxt();  // 生成一个唯一的临时变量名称
            appendSymbolTemp("i32", t, cur_level, cur_table);  // 添加该临时变量到符号表
            irs[cur_table][cur_label].push_back(t + " = " + finalri);
            finalri = t;
        }
        string t = "%t" + getUniqueIntTxt();  // 生成一个唯一的临时变量名称
        appendSymbolTemp("i1", t, cur_level, cur_table);  // 添加该临时变量到符号表
        string op = cur->data_str;  // 获取操作符
        // 根据不同操作符生成不同的中间代码
        if (op == ">") {
            irs[cur_table][cur_label].push_back(t + "=cmp gt " + finalle + ", " + finalri);
        } else if (op == "<") {
            irs[cur_table][cur_label].push_back(t + "=cmp lt " + finalle + ", " + finalri);
        } else if (op == ">=") {
            irs[cur_table][cur_label].push_back(t + "=cmp ge " + finalle + ", " + finalri);
        } else if (op == "<=") {
            irs[cur_table][cur_label].push_back(t + "=cmp le " + finalle + ", " + finalri);
        } else if (op == "==") {
            irs[cur_table][cur_label].push_back(t + "=cmp eq " + finalle + ", " + finalri);
        } else if (op == "!=") {
            irs[cur_table][cur_label].push_back(t + "=cmp ne " + finalle + ", " + finalri);
        }
        return t;
    }
    // 如果是逻辑操作表达式
    else if (cur->type == AST_OP_AND || cur->type == AST_OP_OR) {
        int and_or_in = ast_or_calculator(cur, true_label, false_label, cur_table, cur_level, cur_label);  // 处理逻辑操作表达式
    }
    // 如果是负数表达式
    else if (cur->type == AST_OP_NEG) {
        string child;
        child = exp_handler(cur->children[0], cur_table, cur_level, cur_label);  // 处理子表达式
        string t = "%t" + getUniqueIntTxt();  // 生成一个唯一的临时变量名称
        appendSymbolTemp("i32", t, cur_level, cur_table);  // 添加该临时变量到符号表
        irs[cur_table][cur_label].push_back(t + "=neg " + child);  // 生成中间代码
        return t;
    }
    // 如果是非操作表达式
    else if (cur->type == AST_OP_NOT) {
        int uminus_count = 1;
        Node *child = cur->children[0];
        while (child->type == AST_OP_NOT) {  // 处理可能存在的非操作
            uminus_count++;
            child = child->children[0];
        }
        string t;
        // 如果当前节点的第一个子节点是逻辑与运算符
        if (cur->children[0]->type == AST_OP_AND) {
            // 递归调用 exp_handler 函数处理左子节点，传入参数包括当前符号表、作用域层级、当前标签、真标签和假标签
            exp_handler(cur->children[0], cur_table, cur_level, cur_label, true_label, false_label);
        } else {
            string deep_child;
            // 递归调用 exp_handler 函数处理当前节点的子节点，返回的结果存储在 deep_child 变量中
            deep_child = exp_handler(child, cur_table, cur_level, cur_label);
            // 如果 deep_child 开头是 '*' 符号
            if (deep_child[0] == '*') {
                // 生成一个新的 i32 类型的临时变量，并将其添加到当前符号表中
                t = "%t" + getUniqueIntTxt();
                appendSymbolTemp("i32", t, cur_level, cur_table);
                // 将 deep_child 赋值给新的临时变量，并将赋值语句添加到当前符号表和标签对应的 IR 向量中
                irs[cur_table][cur_label].push_back(t + " = " + deep_child);
                deep_child = t;
            }
            // 生成一个新的 i1 类型的临时变量，并将其添加到当前符号表中
            t = "%t" + getUniqueIntTxt();
            appendSymbolTemp("i1", t, cur_level, cur_table);
            // 如果负号出现偶数次，将 deep_child 和 0 进行比较，并将比较结果赋值给新的临时变量
            if (uminus_count % 2 == 0) {
                irs[cur_table][cur_label].push_back(t + "=cmp ne " + deep_child + ", 0");
            }
            // 如果负号出现奇数次，将 deep_child 和 0 进行比较，并将比较结果赋值给新的临时变量
            else {
                irs[cur_table][cur_label].push_back(t + "=cmp eq " + deep_child + ", 0");
            }
        }
        return t;
    }
    // 如果当前节点是函数调用
    else if (cur->type == AST_OP_FUNC_CALL) {
        string _inname = "@" + cur->data_str;
        string _type;
        // 如果函数名是 getint、getch 或 getarray，返回类型为 i32
        if (cur->data_str == "getint" || cur->data_str == "getch" || cur->data_str == "getarray") {
            _type = "i32";
        }
        // 如果函数名是 putint、putch 或 putarray，返回类型为 void
        else if (cur->data_str == "putint" || cur->data_str == "putch" || cur->data_str == "putarray") {
            _type = "void";
        }
        // 否则，获取当前符号表中函数名对应的符号的类型，存储在 _type 变量中
        else {
            _type = getSymbol(_inname, cur_level, cur_table)->type;
        }
        string t;
        if (cur->children.size() == 0) {
            // 如果函数调用没有参数
            if (_type == "void") {
                // 添加一条 call 指令到当前符号表和标签对应的 IR 向量中，调用无参数函数
                irs[cur_table][cur_label].push_back("call " + _type + " " + _inname + "()");
                return "";
            } else {
                // 生成一个新的类型为 _type 的临时变量，并将其添加到当前符号表中
                t = "%t" + getUniqueIntTxt();
                appendSymbolTemp(_type, t, cur_level, cur_table);
                irs[cur_table][cur_label].push_back(t + "=call " + _type + " " + _inname + "()");
                return t;
            }
        } else {
            // 如果函数调用有参数
            Node *arg_list = cur->children[0];
            // 初始化函数调用指令字符串 rt，包含函数名和返回类型
            string rt = "call " + _type + " " + _inname + "(";
            // 遍历参数列表节点的子节点
            for (int i = 0; i < arg_list->children.size(); i++) {
                Node *arg = arg_list->children[i];
                string arg_txt = exp_handler(arg, cur_table, cur_level, cur_label);
                // 如果参数表达式字符串的第一个字符是数字，说明当前参数是一个整数常量，添加到函数调用指令字符串 rt 中
                if (arg_txt[0] <= '9' && arg_txt[0] >= '0') {
                    rt += "i32 " + arg_txt;
                }
                // 否则，说明当前参数是一个变量或数组，获取其对应的符号
                else {
                    Symbol *arg_sym = getSymbol(arg_txt, cur_level, cur_table);
                    // 如果当前参数是 getarray 或 putarray 函数的第二个参数，生成一个新的 i32 数组类型的临时变量，并将其添加到当前符号表中
                    if (cur->data_str == "getarray" || (cur->data_str == "putarray" && i == 1)) {
                        t = "%t" + getUniqueIntTxt();
                        appendSymbolTemp("i32", t + "[100]", cur_level, cur_table);
                        irs[cur_table][cur_label].push_back(t + " = " + arg_txt.substr(1));
                        rt += "i32 " + t + "[100]";
                    } else {
                        // 如果当前参数是 putint 函数的参数或者函数返回类型是 i32，且参数是一个指针类型的变量，生成一个新的 i32 类型的临时变量，并将其添加到当前符号表中
                        if (_type == "i32" && arg_txt[0] == '*' || cur->data_str == "putint") {
                            t = "%t" + getUniqueIntTxt();
                            appendSymbolTemp("i32", t, cur_level, cur_table);
                            irs[cur_table][cur_label].push_back(t + " = " + arg_txt);
                            rt += "i32 " + t;
                        }
                        // 如果当前参数是一个数组类型的变量，生成一个新的 i32* 类型的临时变量，并将其添加到当前符号表中
                        else if (arg_sym->name.find('[') < arg_sym->name.length()) {
                            t = "%t" + getUniqueIntTxt();
                            appendSymbolTemp("i32*", t, cur_level, cur_table);
                            irs[cur_table][cur_label].push_back(t + " = " + arg_txt);
                            rt += "i32* " + t;
                        }
                        // 如果当前参数是一个普通类型的变量，将其类型和表达式字符串添加到函数调用指令字符串 rt 中
                        else {
                            rt += arg_sym->type + " " + arg_txt;
                        }
                    }
                }
                if (i < arg_list->children.size() - 1) {
                    rt += ",";
                }
            }
            rt += ")";
            if (_type == "void") {
                irs[cur_table][cur_label].push_back(rt);
                return "";
            } else {
                t = "%t" + getUniqueIntTxt();
                appendSymbolTemp(_type, t, cur_level, cur_table);
                irs[cur_table][cur_label].push_back(t + " = " + rt);
                return t;
            }
        }
    }
    // 如果节点的类型为标识符，则在符号表中查找对应的变量，并返回变量名
    else if (cur->type == AST_OP_IDENTIFIER) {
        return getSymbolInname(cur->data_str, cur_level, cur_table);
    }
    // 如果节点的类型为整数，则返回该整数转换为字符串的结果
    else if (cur->type == AST_OP_INTEGER) {
        return int2str(cur->data_int);
    }
    // 如果节点的类型为数组，则需要对数组进行特殊处理，以获取对应的元素值
    else if (cur->type == AST_OP_ARRAY) {
        Node *ident_parent = cur;
        string name = "";
        int wei = 1;
        while (ident_parent->children.size() > 0 && ident_parent->children[0]->type == AST_OP_ARRAY) {
            ident_parent = ident_parent->children[0];
            wei++;
        }
        name = ident_parent->children[0]->data_str;
        // 创建空字符串kuohaonei，用于存储数组的下标部分
        string kuohaonei = "";
        // 如果数组只有一维，则执行以下代码
        if (wei == 1) {
            // 处理下标部分，得到相应的偏移量
            string pianyi_symbol = exp_handler(ident_parent->children[1], cur_table, cur_level, cur_label);
            string final_pianyi_symbol = convertToTemp(pianyi_symbol, cur_table, cur_level, cur_label);
            string t = "%t" + getUniqueIntTxt();
            appendSymbolTemp("i32", t, cur_level, cur_table);
            irs[cur_table][cur_label].push_back(t + "=mul " + final_pianyi_symbol + ", 4");
            // 根据偏移量和数组首地址计算出需要访问的元素的地址
            string t2 = "%t" + getUniqueIntTxt();
            appendSymbolTemp("i32*", t2, cur_level, cur_table);
            irs[cur_table][cur_label].push_back(t2 + "=add " + getSymbolInname(name, cur_level, cur_table) + ", " + t);
            // 返回地址所存储的值
            return "*" + t2;
        }
        // 如果数组不止一维，则执行以下代码
        else {
            // 获取数组的各个维度的大小，并记录在weiDetail中
            vector<int> weiDetail = getSymbolArrayWei(name, cur_table, cur_level);
            string finalrt = "";
            // 遍历各个维度，计算出相应的偏移量，并记录在finalrt中
            for (int i = 0; i < wei - 1; i++) {
                string pianyi_symbol;
                if (finalrt == "") {
                    pianyi_symbol = exp_handler(ident_parent->children[1], cur_table, cur_level, cur_label);
                } else {
                    pianyi_symbol = finalrt;
                }
                string t = "%t" + getUniqueIntTxt();
                appendSymbolTemp("i32", t, cur_level, cur_table);
                string final_pianyi_symbol = convertToTemp(pianyi_symbol, cur_table, cur_level, cur_label);
                irs[cur_table][cur_label].push_back(t + "=mul " + final_pianyi_symbol + ", " + int2str(weiDetail[i + 1]));
                string t2 = "%t" + getUniqueIntTxt();
                appendSymbolTemp("i32", t2, cur_level, cur_table);
                irs[cur_table][cur_label].push_back(t2 + "=add " + t + ", " + exp_handler(ident_parent->parent->children[1], cur_table, cur_level, cur_label));
                finalrt = t2;
                ident_parent = ident_parent->parent;
            }
            // 根据偏移量和数组首地址计算出需要访问的元素的地址
            string t = "%t" + getUniqueIntTxt();
            appendSymbolTemp("i32", t, cur_level, cur_table);
            irs[cur_table][cur_label].push_back(t + "=mul " + finalrt + ", 4");
            string t2 = "%t" + getUniqueIntTxt();
            appendSymbolTemp("i32*", t2, cur_level, cur_table);
            irs[cur_table][cur_label].push_back(t2 + "=add " + getSymbolInname(name, cur_level, cur_table) + ", " + t);
            // 返回地址所存储的值
            return "*" + t2;
        }
    }
    // 如果节点的类型为break，则将相应的IR指令添加到符号表中
    else if (cur->type == AST_OP_BREAK) {
        if (irs[cur_table][cur_label].size() > 0 && irs[cur_table][cur_label].back().find("br") == 0) {
            /* code */
        } else {
            irs[cur_table][cur_label].push_back("br label .L" + int2str(whileInfoStack.top().break_label));
        }
    }
    // 如果节点的类型为其他未定义类型，则返回空串
    return "";
}

/*
 * 下面这个就是大的handle器
 * 其中exp 相关挪到了 exp_handler 里面
 * 这个主要是要对着ast图来看
 * 毕竟写的比较奇怪，不是严格的按照递归的来写的
 */
void ana(Node *cur, int cur_table, int cur_level, int optional_finished_br_label)
{
    if (cur != NULL) {
        //如文本，就是一些DEF
        if (cur->type == AST_OP_EXT_VAR_DEF || cur->type == AST_OP_VAR_DEF || cur->type == AST_OP_PARAM_DEF) {
            //全局变量，应该是PROGRAM->SEGMENT_LIST->EXT_VAR_DEF
            // children[0]为type
            string _type = cur->children[0]->data_str;
            Node *secondChild = cur->children[1];
            if (secondChild->type == AST_OP_IDENTIFIER) {
                if (cur->type == AST_OP_EXT_VAR_DEF) {
                    appendSymbol(secondChild->data_str, _type, "GLOBAL VARIABLE", "@" + secondChild->data_str, 0, 0);
                } else if (cur->type == AST_OP_VAR_DEF) {
                    appendSymbol(secondChild->data_str, _type, "LOCAL VARIABLE", "%l" + getUniqueIntTxt(), cur_level, cur_table);
                } else if (cur->type == AST_OP_PARAM_DEF) {
                    //形参
                    string t, l;
                    l = "%l" + getUniqueIntTxt();
                    t = "%t" + getUniqueIntTxt();
                    appendSymbol(secondChild->data_str, _type, "FORMAL PARAMETER", l, cur_level, cur_table);
                    appendSymbol(secondChild->data_str, _type, "temporary", t, cur_level, cur_table);
                    //读取形参并转换为栈内变量
                    irs[cur_table][2].push_back(l + " = " + t);
                }
            } else if (secondChild->type == AST_OP_IDENTIFIER_P_ARRAY) {
                //仅用于形参 int a[]
                if (cur->type == AST_OP_PARAM_DEF) {
                    //按老师的ir编译器，这个一定要有容量且为0
                    string t, l;
                    l = "%l" + getUniqueIntTxt();
                    t = "%t" + getUniqueIntTxt();
                    appendSymbol(secondChild->data_str + "[0]", _type, "FORMAL PARAMETER", l, cur_level, cur_table);
                    //为了在printirs把参数打上[0]，所以这里inname写上了
                    appendSymbol(secondChild->data_str + "[0]", _type, "temporary", t + "[0]", cur_level, cur_table);
                    //读取形参并转换为栈内变量
                    irs[cur_table][2].push_back(l + " = " + t);
                } else {
                    //只是随便的一个error提示
                    //原则上是不会走到这里的
                    cout << "error @ random3224" << endl;
                }
            } else if (secondChild->type == AST_OP_IDENTIFIER_P_ARRAY_INTEGER) {
                // TODO
                //这个为形参 如 a[10]
                //但是好像目前测试样例还没有这种形式
                //所以就没有实现
                cout << "ERROR IDENTIFIER_P_ARRAY_INTEGER called。WAIT TO FUNC" << endl;
            } else if (secondChild->type == AST_OP_V_D_ASSIGN) {
                //定义时同时a = 5，如 int a = 5
                //但好像老师把这个测试样例给改没了
                if (cur->type == AST_OP_EXT_VAR_DEF) {
                    //目前测试样例（018）中，全局变量的 V_D_ASSIGN都是i32类型
                    appendSymbol(secondChild->data_str, _type, "GLOBAL VARIABLE", "@" + secondChild->data_str, 0, 0);
                    main_first_ir.push_back("@" + secondChild->data_str + " = " + int2str(secondChild->children[0]->data_int));
                } else if (cur->type == AST_OP_VAR_DEF) {
                    string t = "%l" + getUniqueIntTxt();
                    appendSymbol(secondChild->data_str, _type, "LOCAL VARIABLE", t, cur_level, cur_table);

                    //从 ASSIGN 里面复制并修改
                    string ri = exp_handler(secondChild->children[0], cur_table, cur_level, G_NOW_LABEL);
                    string finalri = convertToTemp(ri, cur_table, cur_level, G_NOW_LABEL);
                    irs[cur_table][G_NOW_LABEL].push_back(t + " = " + finalri);
                }
            } else {
                //这里应该是*_DEC_LIST
                if (secondChild->type != AST_OP_DEC_LIST && secondChild->type != AST_OP_EXT_DEC_LIST && secondChild->type != AST_OP_ARRAY) {
                    //不应该走到这个步骤。假如发生的花要再检查一下YACC
                    cout << "ERROR @DEC_LIST check" << endl;
                    cout << "Is: " << secondChild->type << " id:" << secondChild->id << endl;
                }

                for (auto it : secondChild->children) {
                    //这边是定义相关
                    if (it->type == AST_OP_IDENTIFIER) {

                        string _name = it->data_str;
                        if (it->parent->type == AST_OP_ARRAY) {
                            //这是一维数组
                            _name = _name + "[" + int2str(it->parent->children[1]->data_int) + "]";
                        }
                        if (cur->type == AST_OP_EXT_VAR_DEF) {
                            //普通全局变量
                            appendSymbol(_name, _type, "GLOBAL VARIABLE", "@" + it->data_str, 0, 0);
                        } else if (cur->type == AST_OP_VAR_DEF) {
                            //普通局部变量
                            appendSymbol(_name, _type, "LOCAL VARIABLE", "%l" + getUniqueIntTxt(), cur_level, cur_table);
                        }
                    } else if (it->type == AST_OP_ARRAY) {
                        // N维数组
                        Node *ident = it;
                        //下面这个是带 [1] 类型的
                        string name_with_wei = "";
                        //下面这个就是不带的
                        string name_no_wei = "";
                        while (ident->children.size() > 0 && ident->type == AST_OP_ARRAY) {
                            ident = ident->children[0];
                        }
                        name_with_wei = ident->data_str;
                        name_no_wei = ident->data_str;

                        ident = ident->parent;
                        while (ident->type == AST_OP_ARRAY) {
                            name_with_wei = name_with_wei + "[" + int2str(ident->children[1]->data_int) + "]";
                            ident = ident->parent;
                        }

                        if (cur->type == AST_OP_EXT_VAR_DEF) {
                            appendSymbol(name_with_wei, _type, "GLOBAL VARIABLE", "@" + name_no_wei, 0, 0);
                        } else if (cur->type == AST_OP_VAR_DEF) {
                            appendSymbol(name_with_wei, _type, "LOCAL VARIABLE", "%l" + getUniqueIntTxt(), cur_level, cur_table);
                        }
                    } else if (it->type == AST_OP_V_D_ASSIGN) {
                        //从不是LIST的那个代码里面复制修改
                        if (cur->type == AST_OP_EXT_VAR_DEF) {
                            //目前测试样例（018）中，全局变量的 V_D_ASSIGN都是i32类型
                            appendSymbol(it->data_str, _type, "GLOBAL VARIABLE", "@" + it->data_str, 0, 0);
                            main_first_ir.push_back("@" + it->data_str + " = " + int2str(it->children[0]->data_int));
                        } else if (cur->type == AST_OP_VAR_DEF) {
                            string t = "%l" + getUniqueIntTxt();
                            appendSymbol(it->data_str, _type, "LOCAL VARIABLE", t, cur_level, cur_table);

                            //从 ASSIGN 里面复制并修改
                            string ri = exp_handler(it->children[0], cur_table, cur_level, G_NOW_LABEL);
                            string finalri = convertToTemp(ri, cur_table, cur_level, G_NOW_LABEL);
                            irs[cur_table][G_NOW_LABEL].push_back(t + " = " + finalri);
                        }
                    }
                }
            }
        } else if (cur->type == AST_OP_PROGRAM) {
            //最顶上的头头
            ana(cur->children[0], 0, 0, -1);
        } else if (cur->type == AST_OP_SEGMENTS) {
            int _table = 0;
            for (auto it : cur->children) {
                //提升table.
                if (it->type == AST_OP_FUNC_DEF) {
                    _table++;
                    // label清2
                    G_NOW_LABEL = 2;
                    _used_max_label = 2;
                    _unique = 0;
                }

                ana(it, _table, cur_table, -1);
            }
        } else if (cur->type == AST_OP_FUNC_DEF_LITE) //函数声明
        {
            /*code*/
            //只存符号表就行
        } else if (cur->type == AST_OP_FUNC_DEF) //函数调用
        {
            //此时的TABLEID已经增加了，

            string _type = cur->children[0]->data_str;
            //存符号表，ref_table_id指示当前的这个func的临时函数要在哪个table_id里
            // func类似全局变量存table[0]
            appendSymbol(cur->children[1]->data_str, _type, "function", "@" + cur->children[1]->data_str, 0, 0, cur_table); // cur->children[1]为 FUNC_DEC

            if (_type != "void") {
                string __irtype = "";
                if (_type == "int") {
                    __irtype = "i32";
                } else if (_type == "char") {
                    __irtype = "i8";
                }

                //给返回值(local)插ir
                appendSymbol("RETURNED VALUE", _type, "RETURNED VALUE", "%l" + getUniqueIntTxt(), 1, cur_table);
            }

            ana(cur->children[1], cur_table, 1, -1); //可能会有形参
            ana(cur->children[2], cur_table, 0, -1); //为COMP_STM，在COMP_STM提升level                                                                       // cur->children[2] 为COMP_STM
        } else if (cur->type == AST_OP_FUNC_DEC) {
            for (auto it : cur->children) {
                ana(it, cur_table, 1, -1);
            }
        } else if (cur->type == AST_OP_PARAM_LIST) {
            for (auto it : cur->children) {
                ana(it, cur_table, 1, -1);
            }
        } else if (cur->type == AST_OP_COMP_STM) {
            //在FUNC_DEF下面
            //提升了level
            ana(cur->children[0], cur_table, cur_level + 1, optional_finished_br_label); //只有一个child，为STM_LIST
        } else if (cur->type == AST_OP_STM_LIST) {
            //在COMP_STM下面
            for (auto it : cur->children) {
                ana(it, cur_table, cur_level, optional_finished_br_label);
            }
            if (optional_finished_br_label > 0) {
                if (optional_finished_br_label == G_NOW_LABEL) {
                    optional_finished_br_label = 1;
                    cout << "WARNING!@optional_finished_br_label.ignore this br " << G_NOW_LABEL << endl;
                } else {
                    if (irs[cur_table][G_NOW_LABEL].size() > 0 && irs[cur_table][G_NOW_LABEL].back().find("br") == 0) {
                        /* code */
                    } else {
                        irs[cur_table][G_NOW_LABEL].push_back("br label .L" + int2str(optional_finished_br_label));
                    }
                }
            }
            if (cur->parent->parent->type == AST_OP_FUNC_DEF) {
                cout << "\x1b[32m" << "*-*—*—函数分析完成-*-*-*" << endl << "\x1b[0m";
                if (irs[cur_table][G_NOW_LABEL].size() > 0 && irs[cur_table][G_NOW_LABEL].back().find("br") == 0) {
                    /* code */
                } else {
                    irs[cur_table][G_NOW_LABEL].push_back("br label .L1");
                }
            }
        }
        //===Stmt
        else if (cur->type == AST_OP_EXP_STMT) {
            //只有一个childern且为 Exp
            exp_handler(cur->children[0], cur_table, cur_level, G_NOW_LABEL);
        } else if (cur->type == AST_OP_RETURN) {
            // return的时候不写exit相关，是在最终printir的时候再根据是否有返回值来打
            //只改写返回值local
            //只有一个childern且为 Exp
            string r = convertToTemp(exp_handler(cur->children[0], cur_table, cur_level, G_NOW_LABEL), cur_table, cur_level, G_NOW_LABEL);
            irs[cur_table][G_NOW_LABEL].push_back(SymbolTable[cur_table][0]->inname + " = " + r);
            if (irs[cur_table][G_NOW_LABEL].size() > 0 && irs[cur_table][G_NOW_LABEL].back().find("br") == 0) {
                /* code */
            } else {
                irs[cur_table][G_NOW_LABEL].push_back("br label .L1");
            }
        } else if (cur->type == AST_OP_RETURN_EMP) {
            //空返回
            if (irs[cur_table][G_NOW_LABEL].size() > 0 && irs[cur_table][G_NOW_LABEL].back().find("br") == 0) {
                /* code */
            } else {
                irs[cur_table][G_NOW_LABEL].push_back("br label .L1");
            }
        } else if (cur->type == AST_OP_IF) {

            // if里面为另一个label
            int _label = G_NOW_LABEL;
            int if_block_label = getNewLabel();
            //本if_then的外层
            int out_block_label = getNewLabel(); // optional_finished_br_label;
            string con = exp_handler(cur->children[0], cur_table, cur_level, G_NOW_LABEL, if_block_label, out_block_label);
            //为了防止里面有*导致老师的编译器不识别，所以再套一个tmp
            if (con[0] == '*') {
                string t = "%t" + getUniqueIntTxt();
                appendSymbolTemp("i32", t, cur_level, cur_table);
                irs[cur_table][_label].push_back(t + " = " + con);
                con = t;
            }
            // and or 因为它直接写br,所以空
            if (con != "") {
                irs[cur_table][_label].push_back("bc " + con + ", label .L" + int2str(if_block_label) + ", label .L" + int2str(out_block_label));
            }

            G_NOW_LABEL = if_block_label;
            ana(cur->children[1], cur_table, G_NOW_LABEL, out_block_label);

            G_NOW_LABEL = out_block_label;
        } else if (cur->type == AST_OP_IF_ELSE) {
            // if里面为另一个label
            int _label = G_NOW_LABEL;
            int if_true_block_label = getNewLabel();
            int if_false_block_label = getNewLabel();

            //下面指if-then-else外的第一个label
            int out_block_label = getNewLabel(); // optional_finished_br_label;
            /*if (optional_finished_br_label == -1)
            {
                out_block_label = getNewLabel();
            }*/
            string con = exp_handler(cur->children[0], cur_table, cur_level, G_NOW_LABEL, if_true_block_label, if_false_block_label);
            if (con[0] == '*') {
                string t = "%t" + getUniqueIntTxt();
                appendSymbolTemp("i32", t, cur_level, cur_table);
                irs[cur_table][_label].push_back(t + " = " + con);
                con = t;
            }
            // and or 因为它直接写br,所以空
            if (con != "") {
                irs[cur_table][_label].push_back("bc " + con + ", label .L" + int2str(if_true_block_label) + ", label .L" + int2str(if_false_block_label));
            }

            G_NOW_LABEL = if_true_block_label;
            ana(cur->children[1], cur_table, G_NOW_LABEL, out_block_label);

            G_NOW_LABEL = if_false_block_label;
            ana(cur->children[2], cur_table, G_NOW_LABEL, out_block_label);

            G_NOW_LABEL = out_block_label;
        } else if (cur->type == AST_OP_WHILE) {
            // while里面为另一个label
            int _label = G_NOW_LABEL;
            int while_condition_label = getNewLabel();
            // while 的判断要单独一个Label
            if (irs[cur_table][_label].size() > 0 && irs[cur_table][_label].back().find("br") == 0) {
                /* code */
            } else {
                irs[cur_table][_label].push_back("br label .L" + int2str(while_condition_label));
            }

            int while_block_label = getNewLabel();
            int out_block_label = getNewLabel(); // optional_finished_br_label;
            //为break continue存储数据
            WhileInfo *info = new WhileInfo();
            info->condition_label = while_condition_label;
            info->continue_label = while_block_label;
            info->break_label = out_block_label;
            whileInfoStack.push(*info);
            string con = exp_handler(cur->children[0], cur_table, cur_level, while_condition_label, while_block_label, out_block_label);
            if (con[0] == '*') {
                string t = "%t" + getUniqueIntTxt();
                appendSymbolTemp("i32", t, cur_level, cur_table);
                irs[cur_table][while_condition_label].push_back(t + " = " + con);
                con = t;
            }
            // and or 因为它直接写br,所以空
            if (con != "") {
                irs[cur_table][while_condition_label].push_back("bc " + con + ", label .L" + int2str(while_block_label) + ", label .L" + int2str(out_block_label));
            }

            G_NOW_LABEL = while_block_label;
            //结束回到 while_condition_label
            ana(cur->children[1], cur_table, G_NOW_LABEL, while_condition_label);
            //这个时候应该执行完了，G_NOW_LABEL
            G_NOW_LABEL = out_block_label;
            whileInfoStack.pop();
        }
    }
}