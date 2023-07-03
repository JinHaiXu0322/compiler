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
#include <unordered_map>

/*ast_operator_type类型转为string*/
std::string OperatorTypeToString(ast_operator_type op)
{
    // 定义一个静态无序映射 opToString，将 AST 节点的操作类型转化为字符串表示
    static const std::unordered_map<ast_operator_type, std::string> opToString {
        { AST_OP_PROGRAM, "PROGRAM" },
            // 程序节点
        { AST_OP_SEGMENTS, "SEGMENTS" },
            // 代码段节点
        { AST_OP_EXT_VAR_DEF, "EXT_VAR_DEF" },
            // 外部变量声明节点
        { AST_OP_FUNC_DEF, "FUNC_DEF" },
            // 函数定义节点
        { AST_OP_FUNC_DEF_LITE, "FUNC_DEF_LITE" },
            // 函数声明节点
        { AST_OP_EXT_DEC_LIST, "EXT_DEC_LIST" },
            // 外部声明列表节点
        { AST_OP_TYPE, "TYPE" },
            // 类型节点
        { AST_OP_V_D_ASSIGN, "V_D_ASSIGN" },
            // 变量或数组初始化节点
        { AST_OP_IDENTIFIER, "IDENTIFIER" },
            // 标识符节点
        { AST_OP_ARRAY, "ARRAY" },
            // 数组节点
        { AST_OP_IDENTIFIER_P_ARRAY, "IDENTIFIER_P_ARRAY" },
            // 标识符和数组节点
        { AST_OP_IDENTIFIER_P_ARRAY_INTEGER, "IDENTIFIER_P_ARRAY_INTEGER" },
            // 标识符、数组和整数节点
        { AST_OP_INTEGER, "INTEGER" },
            // 整数节点
        { AST_OP_RELOP, "RELOP" },
            // 关系运算符节点
        { AST_OP_FUNC_CALL, "FUNC_CALL" },
            // 函数调用节点
        { AST_OP_CHAR, "CHAR" },
            // 字符节点
        { AST_OP_BREAK, "BREAK" },
            // break 语句节点
        { AST_OP_CONTINUE, "CONTINUE" },
            // continue 语句节点
        { AST_OP_ARG_LIST, "ARG_LIST" },
            // 参数列表节点
        { AST_OP_FUNC_DEC, "FUNC_DEC" },
            // 函数声明节点
        { AST_OP_PARAM_LIST, "PARAM_LIST" },
            // 参数列表节点
        { AST_OP_PARAM_DEF, "PARAM_DEF" },
            // 参数定义节点
        { AST_OP_PARAM_DEF_BUILTIN, "PARAM_DEF_BUILTIN" },
            // 内置类型的参数定义节点
        { AST_OP_COMP_STM, "COMP_STM" },
            // 复合语句节点
        { AST_OP_STM_LIST, "STM_LIST" },
            // 语句列表节点
        { AST_OP_EXP_STMT, "EXP_STMT" },
            // 表达式语句节点
        { AST_OP_RETURN_EMP, "RETURN_EMP" },
            // 空返回语句节点
        { AST_OP_RETURN, "RETURN" },
            // 返回语句节点
        { AST_OP_IF, "IF" },
            // if 语句节点
        { AST_OP_IF_ELSE, "IF_ELSE" },
            // if-else 语句节点
        { AST_OP_WHILE, "WHILE" },
            // while 语句节点
        { AST_OP_VAR_DEF, "VAR_DEF" },
            // 变量定义节点
        { AST_OP_DEC_LIST, "DEC_LIST" }
            // 声明列表节点
    };

    auto it = opToString.find(op);
    if (it != opToString.end()) {
        return it->second;
    }
    return "";
}


// 用于生成 AST 中表示终结符的节点，并设置节点的样式和标签
Agnode_t *genGraphNode(Agnode_t *node, string c)
{
    // 设置节点样式
    agsafeset(node, (char *)"style", (char *)"filled", (char *)"");
    agsafeset(node, (char *)"fillcolor", (char *)"yellow", (char *)"");
    agsafeset(node, (char *)"fontcolor", (char *)"black", (char *)"");
    agsafeset(node, (char *)"fontname", (char *)"Sans Bold", (char *)"");
    // 设置节点标签
    agsafeset(node, (char *)"label", (char *)c.c_str(), "");
    agsafeset(node, (char *)"shape", (char *)"box", (char *)"");
    return node;
}

// 递归生成 AST 中所有节点，并设置节点之间的边
void genNodes(Agraph_t *g, Agnode_t *parent_gv, Node *next_node)
{
    // 创建新节点，并设置样式和标签
    Agnode_t *gv_node = agnode(g, (char *)int2str(node_num++).c_str(), 1);
    // 如果该节点是叶子节点
    if (next_node->children.size() == 0) {
        // 根据节点类型和数据设置节点标签
        if (next_node->type == AST_OP_CONTINUE) {
            genGraphNode(gv_node, "continue");
        } else if (next_node->type == AST_OP_BREAK) {
            genGraphNode(gv_node, "break");
        } else if (next_node->type == AST_OP_FUNC_DEC) {
            genGraphNode(gv_node, next_node->data_str + "()");
        } else if (next_node->type == AST_OP_FUNC_CALL) {
            genGraphNode(gv_node, next_node->data_str + "()");
        } else if (next_node->data_str != "") {
            genGraphNode(gv_node, next_node->data_str);
        } else if (to_string(next_node->data_int) != "") {
            genGraphNode(gv_node, std::to_string(next_node->data_int));
        } else if (to_string(next_node->data_char) != "") {
            genGraphNode(gv_node, std::to_string(next_node->data_char));
        }
    }
    // 如果该节点有子节点
    else {
        // 根据节点类型设置节点标签
        switch (next_node->type) {
            // 加法
        case AST_OP_ADD:
            genGraphNode(gv_node, "+");
            break;
            // 减法
        case AST_OP_SUB:
            genGraphNode(gv_node, "-");
            break;
            // 乘法           
        case AST_OP_MUL:
            genGraphNode(gv_node, "*");
            break;
            // 取负
        case AST_OP_NEG:
            // 除法
        case AST_OP_DIV:
            genGraphNode(gv_node, "-");
            break;
            // 取余
        case AST_OP_MOD:
            genGraphNode(gv_node, "%");
            break;
            // 赋值语句
        case AST_OP_ASSIGN:
            genGraphNode(gv_node, "=");
            break;
            // 逻辑或
        case AST_OP_OR:
            genGraphNode(gv_node, "||");
            break;
            // 逻辑非
        case AST_OP_NOT:
            genGraphNode(gv_node, "!");
            break;
            // 逻辑与
        case AST_OP_AND:
            genGraphNode(gv_node, "&&");
            break;
            // 函数定义或函数调用
        case AST_OP_FUNC_DEC:
        case AST_OP_FUNC_CALL:
            genGraphNode(gv_node, next_node->data_str + "()");
            break;
            // if 语句
        case AST_OP_IF:
            genGraphNode(gv_node, "IF");
            break;
            // if-else 语句
        case AST_OP_IF_ELSE:
            genGraphNode(gv_node, "IF_ELSE");
            break;
            // while 语句
        case AST_OP_WHILE:
            genGraphNode(gv_node, "WHILE");
            break;
            // 变量声明赋值语句
        case AST_OP_V_D_ASSIGN:
            genGraphNode(gv_node, next_node->data_str);
            break;
            // 关系运算符
        case AST_OP_RELOP:
            genGraphNode(gv_node, next_node->data_str.c_str());
            break;
            // 返回语句
        case AST_OP_RETURN:
            genGraphNode(gv_node, "return");
            break;
            // 数组访问
        case AST_OP_ARRAY:
            genGraphNode(gv_node, "[]");
            break;
        default:
            // 如果节点类型未被上述 case 匹配到，则设置节点标签为运算符类型
            agsafeset(gv_node, (char *)"label", (char *)(OperatorTypeToString(next_node->type).c_str()), "");
            agsafeset(gv_node, (char *)"shape", (char *)"ellipse", (char *)"");
            break;
        }
    }

    if (parent_gv != NULL) {
        // 创建从父节点到当前节点的边
        agedge(g, parent_gv, gv_node, NULL, 1);
    }
    for (int i = 0; i < next_node->children.size(); i++) {
        // 递归生成所有子节点的 AST 图形化视图
        genNodes(g, gv_node, next_node->children[i]);
    }
}

// 对AST进行处理，相邻的相同类型的节点合并成一个节点，减少其节点数量
void ast(Node *cur)
{
    // 获取当前节点的子节点数量
    int ch_size = cur->children.size();
    // 遍历所有子节点
    for (int i = 0; i < cur->children.size(); i++) {
        // 获取当前子节点
        Node *nod = cur->children[i];
        // 判断是否可以将当前子节点与下一个子节点合并
        if ((cur->type == AST_OP_SEGMENTS || cur->type == AST_OP_STM_LIST || cur->type == AST_OP_EXT_DEC_LIST || cur->type == AST_OP_DEC_LIST || cur->type == AST_OP_PARAM_LIST || cur->type == AST_OP_ARG_LIST) && nod->type == cur->type && nod->data_char == cur->data_char && nod->data_int == cur->data_int && nod->data_str == cur->data_str) {
            // 将被合并的子节点的所有子节点添加到当前子节点的子节点列表中
            for (int i = 0; i < nod->children.size(); i++) {
                nod->children[i]->parent = cur;
                cur->children.push_back(nod->children[i]);
            }
            // 删除被合并的子节点
            auto iter = cur->children.erase(std::begin(cur->children) + i); // 删除第 i 个元素
            i = 0;
        }
    }
    // 对当前节点的每个子节点递归调用 ast() 函数，以实现对子树的优化操作
    for (int i = 0; i < cur->children.size(); i++) {
        ast(cur->children[i]);
    }
}


// 将 if 和 while 语句内部的语句列表包裹在一个 COMP_STM 和 STM_LIST 中，从而使得语法树更加规范化，方便后续的处理
void ast2(Node *cur)
{
    // 强制将if、while语句内部套进COMP_STM 和 STM_LIST
    for (int i = 0; i < cur->children.size(); i++) {
        // 获取当前子节点
        Node *nod = cur->children[i];
        // 如果当前子节点是 if 语句，且第二个子节点不是 COMP_STM 类型
        if (nod->type == AST_OP_IF && nod->children[1]->type != AST_OP_COMP_STM) {
            // 新建 COMP_STM 和 STM_LIST 节点
            Node *n1 = new Node();
            n1->type = AST_OP_COMP_STM;
            Node *n2 = new Node();
            n2->type = AST_OP_STM_LIST;
            n1->children.push_back(n2);
            n2->parent = n1;

            // 将原来的语句列表添加到 STM_LIST 中
            n1->parent = cur;
            n2->children.push_back(nod->children[1]);

            // 删除原来的语句列表，并将新建的 COMP_STM 节点作为 if 语句的第二个子节点
            auto iter = nod->children.erase(std::begin(nod->children) + 1); // 删除第二个元素
            nod->children.push_back(n1);
        }
        // 如果当前子节点是 if_else 语句，且第二个子节点不是 COMP_STM 类型
        if (nod->type == AST_OP_IF_ELSE && nod->children[1]->type != AST_OP_COMP_STM) {
            // 新建 COMP_STM 和 STM_LIST 节点
            Node *n1 = new Node();
            n1->type = AST_OP_COMP_STM;
            Node *n2 = new Node();
            n2->type = AST_OP_STM_LIST;
            n1->children.push_back(n2);
            n2->parent = n1;

            // 将原来的语句列表添加到 STM_LIST 中
            n1->parent = cur;
            n2->children.push_back(nod->children[1]);

            // 删除原来的语句列表，并将新建的 COMP_STM 节点作为 if_else 语句的第二个子节点
            auto iter = nod->children.erase(std::begin(nod->children) + 1); // 删除第二个元素
            nod->children.insert(nod->children.begin() + 1, n1); // 在第二个元素前插入新节点
        }
        // 如果当前子节点是 if_else 语句，且第三个子节点不是 COMP_STM 类型
        if (nod->type == AST_OP_IF_ELSE && nod->children[2]->type != AST_OP_COMP_STM) {
            // 新建 COMP_STM 和 STM_LIST 节点
            Node *n1 = new Node();
            n1->type = AST_OP_COMP_STM;
            Node *n2 = new Node();
            n2->type = AST_OP_STM_LIST;
            n1->children.push_back(n2);
            n2->parent = n1;

            // 将原来的语句列表添加到 STM_LIST 中
            n1->parent = cur;
            n2->children.push_back(nod->children[2]);

            // 删除原来的语句列表，并将新建的 COMP_STM 节点作为 if_else 语句的第三个子节点
            auto iter = nod->children.erase(std::begin(nod->children) + 2); // 删除第三个元素
            nod->children.insert(nod->children.begin() + 2, n1); // 在第三个元素前插入新节点
        }
        // 如果当前子节点是 while 语句，且第二个子节点不是 COMP_STM 类型
        if (nod->type == AST_OP_WHILE && nod->children[1]->type != AST_OP_COMP_STM) {
            // 新建 COMP_STM 和 STM_LIST 节点
            Node *n1 = new Node();
            n1->type = AST_OP_COMP_STM;
            Node *n2 = new Node();
            n2->type = AST_OP_STM_LIST;
            n1->children.push_back(n2);
            n2->parent = n1;
            n1->parent = cur;
            n2->children.push_back(nod->children[1]);

            // 删除原来的语句列表，并将新建的 COMP_STM 节点作为 while 语句的第二个子节点
            auto iter = nod->children.erase(std::begin(nod->children) + 1); // 删除第二个元素
            nod->children.push_back(n1);
        }
    }

    // 对当前节点的每个子节点都递归地调用 ast2 函数，实现对子树的优化操作
    for (int i = 0; i < cur->children.size(); i++) {
        ast2(cur->children[i]);
    }
}

