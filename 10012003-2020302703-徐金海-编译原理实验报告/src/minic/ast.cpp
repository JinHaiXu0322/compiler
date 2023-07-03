#include "ast.h"

// 定义了一个全局变量 node_id，用于给语法树节点分配唯一的标识符
int node_id = 0;

// 定义了一个指向语法树根节点的指针 ast_root，初始值为 NULL。它用于保存语法树的根节点，可以在构建语法树时进行更新
Node *ast_root = NULL;

// 定义了一个名为 new_ast_node 的函数，用于创建一个新的语法树节点。
// 它接受类型（type）、行号（lineno）以及最多三个子节点（s1、s2、s3）作为参数
Node *new_ast_node(enum ast_operator_type type, int lineno, Node *s1, Node *s2, Node *s3)
{
    // 创建了一个名为 node 的指针，指向通过 new 运算符创建的新的 Node 对象
    Node *node = new Node();
    // 将 node 的 id 属性设置为 node_id 的当前值，并将 node_id 自增。这样可以为每个节点分配唯一的标识符
    node->id = node_id++;
    // 将 node 的 type 属性设置为传递给函数的 type 参数的值，表示节点的类型
    node->type = type;
    // 开始时该节点无父节点
    node->parent = NULL;

    if (s1 != NULL) {
        node->children.push_back(s1);
        s1->parent = node;
    }
    if (s2 != NULL) {
        node->children.push_back(s2);
        s2->parent = node;
    }
    if (s3 != NULL) {
        node->children.push_back(s3);
        s3->parent = node;
    }
    return node;
}
