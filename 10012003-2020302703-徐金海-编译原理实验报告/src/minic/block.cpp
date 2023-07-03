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

//生成graphviz示例的block基本块划分图
//其中的blockvt
//是在 printirs 函数里面整好的
void genBlock(Agraph_t *g)
{
    // 创建存放节点指针的数组
    vector<Agnode_t *> aNode;
    // 设置数组大小
    aNode.resize(1001);
    // 记录节点数量
    int node_num = 0;
    // 遍历基本块数组
    for (int i = 0; i < BlockVec.size(); i++) {
        // 如果基本块为空或者不包含任何指令，则跳过此次循环
        if (BlockVec[i] == NULL || BlockVec[i]->irs.size() == 0)
            continue;
        // 获取当前基本块
        Block *block = BlockVec[i];
        // 用于存放基本块的代码
        string code = "";
        // 记录代码行数
        int line_i = 0;

        // 遍历基本块中的指令，其中非转义\l表示graphviz换行左对齐
        for (auto line : block->irs)
            // 将指令添加到代码字符串中
            code = code + "\\l" + int2str((line_i++)) + ". " + line;
        // 添加 Graphviz 换行符
        code = code + "\\l";
        // 创建 Graphviz 节点
        Agnode_t *gv_node = agnode(g, (char *)int2str(node_num++).c_str(), 1);
        // 设置节点形状为 record
        agsafeset(gv_node, "shape", "record", "ellipse");
        // 设置节点标签为代码字符串
        agsafeset(gv_node, (char *)"label", (char *)(code).c_str(), "");
        // 将节点指针添加到 aNode 数组中
        aNode[block->label] = gv_node;
    }
    //后面再循环一次来补连线，再次遍历基本块数组
    for (int i = 0; i < BlockVec.size(); i++) {
        // 如果基本块为空或者不包含任何指令，则跳过此次循环
        if (BlockVec[i] == NULL || BlockVec[i]->irs.size() == 0)
            continue;
        // 获取当前基本块
        Block *block = BlockVec[i];
        // 遍历当前基本块的子节点
        for (auto child : block->to_labels)
            // 创建 Graphviz 边，表示从当前节点到子节点的控制流
            agedge(g, aNode[block->label], aNode[child], NULL, 1);
    }
}