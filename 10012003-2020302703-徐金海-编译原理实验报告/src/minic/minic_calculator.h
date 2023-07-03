#ifndef MINIC_H
#define MINIC_H

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

// 用于处理与、或以及单目运算符
int ast_or_calculator(Node *and_or, int true_label, int false_label, int cur_table, int cur_level, int cur_label);
// 此函数用于处理其他表达式节点，返回生成的中间代码
string exp_handler(Node *cur, int cur_table, int cur_level, int cur_label, int true_label, int false_label);
// 处理跳转及其他
void ana(Node *cur, int cur_table, int cur_level, int optional_finished_br_label);

// MINIC_H
#endif 