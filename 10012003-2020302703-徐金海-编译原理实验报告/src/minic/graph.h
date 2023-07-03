#ifndef GRAPH_H
#define GRAPH_H

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

// 递归生成 AST 中所有节点，并设置节点之间的边
void genNodes(Agraph_t *g, Agnode_t *parent_gv, Node *next_node);
// 对AST进行处理，相邻的相同类型的节点合并成一个节点，减少其节点数量
void ast(Node *cur);
// 将 if 和 while 语句内部的语句列表包裹在一个 COMP_STM 和 STM_LIST 中，从而使得语法树更加规范化，方便后续的处理
void ast2(Node *cur);

// GRAPH_H
#endif 