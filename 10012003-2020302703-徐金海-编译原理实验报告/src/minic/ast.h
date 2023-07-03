#ifndef AST_H
#define AST_H

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;

enum ast_operator_type {
    // Program
    AST_OP_PROGRAM,
    // Segments
    AST_OP_SEGMENTS,
    // EXT_VAR_DEF
    AST_OP_EXT_VAR_DEF,
    // FUNC_DEF
    AST_OP_FUNC_DEF,
    // FUNC_DEF_LITE
    AST_OP_FUNC_DEF_LITE,
    // EXT_DEC_LIST
    AST_OP_EXT_DEC_LIST,
    // Type
    AST_OP_TYPE,
    // V_D_ASSIGN
    AST_OP_V_D_ASSIGN,
    // IDENTIFIER
    AST_OP_IDENTIFIER,
    // Array
    AST_OP_ARRAY,
    // IDENTIFIER_P_ARRAY
    AST_OP_IDENTIFIER_P_ARRAY,
    // IDENTIFIER_P_ARRAY_INTEG
    AST_OP_IDENTIFIER_P_ARRAY_INTEGER,
    // INTEGER
    AST_OP_INTEGER,
    // RELOP
    AST_OP_RELOP,
    // FUNC_CALL
    AST_OP_FUNC_CALL,
    // CHAR
    AST_OP_CHAR,
    // break
    AST_OP_BREAK,
    // CONTINUE
    AST_OP_CONTINUE,
    // ARG_LIST  
    AST_OP_ARG_LIST,
    // FUNC_DEC
    AST_OP_FUNC_DEC,
    // PARAM_LIST
    AST_OP_PARAM_LIST,
    // PARAM_DEF
    AST_OP_PARAM_DEF,
    //PARAM_DEF_BUILTIN
    AST_OP_PARAM_DEF_BUILTIN,
    AST_OP_COMP_STM,
    AST_OP_STM_LIST,
    AST_OP_EXP_STMT,
    AST_OP_RETURN_EMP,
    AST_OP_RETURN,
    AST_OP_IF,
    AST_OP_IF_ELSE,
    AST_OP_WHILE,
    // VAR_DEF 
    AST_OP_VAR_DEF,
    // DEC_LIST
    AST_OP_DEC_LIST,
    //&&
    AST_OP_AND,
    //||
    AST_OP_OR,
    // 非
    AST_OP_NOT,
    // 二元运算符+
    AST_OP_ADD,
    // 二元运算符*
    AST_OP_MUL,
    // 二元运算符-
    AST_OP_SUB,
    // 二元运算符/
    AST_OP_DIV,
    // 二元运算符%
    AST_OP_MOD,
    // 一元运算符-
    AST_OP_NEG,
    // 赋值语句
    AST_OP_ASSIGN,
    // 非法运算符
    AST_OP_MAX,
};


// 定义节点的结构体，其中包含以下信息
typedef struct node_s {
    // 指向父节点的指针
    struct node_s *parent;
    //存储子节点的向量
    std::vector<struct node_s *> children;
    //节点的类型，使用字符串表示
    enum ast_operator_type type;
    //存储字符串类型数据的成员变量
    string data_str;
    //存储整数类型数据的成员变量
    int data_int;
    //存储字符类型数据的成员变量
    char data_char;
    //节点的唯一标识符
    int id;
}Node;

// 下面定义了另外一个结构体，用于在词法分析阶段存储词法单元的数据
struct lex_data {
    //存储整数类型数据的成员变量
    int data_int;
    //存储字符串类型数据的成员变量
    string data_str;
    //存储字符类型数据的成员变量
    char data_char;
    //指向语法树节点的指针
    Node *node;
};

#define YYSTYPE lex_data

// 声明了一个名为 ast_root 的全局变量，类型为 Node*，表示它是一个指向 Node 类型对象的指针
// 其在ast.cpp中被定义
extern Node *ast_root;

// 告诉编译器将后面的函数声明按照 C 的方式进行链接，而不是 C++ 的方式。
// 在 C++ 中，函数名会根据命名空间、重载和其他特性进行修饰，以支持函数重载和名称空间。而在 C 中，函数名没有这样的修饰。
extern "C"
{
    int yywrap(void);
    int yylex(void);
}

// 以下函数的作用是创建一个新的抽象语法树（AST）节点
Node *new_ast_node(enum ast_operator_type type,
    int lineno,
    Node *s1,
    Node *s2,
    Node *s3);

// AST_H
#endif 