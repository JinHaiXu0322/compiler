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
#include "minic_calculator.h"
#include "graph.h"
#include "symbol.h"
#include "block.h"

int main(int argc, char *argv[])
{
    for (int i = 0; i < irs.size(); i++) {
        irs[i].resize(200);
    }
    string filename = "test.c";
    string dumpfile = "_dump.ir";
    string symbolfile = "_sym.txt";
    string astfile = "_ast.png";
    string blockfile = "_block.png";
    bool nofixast = false;
    if (argc == 1) {
        dumpfile = "dump.ir";
        astfile = "ast.png";
    } else if (argc == 2) {
        filename = (argv[1]);
        dumpfile = "dump.ir";
        astfile = "ast.png";
    } else if (argc == 5) {
        filename = (argv[4]);
        if (strcmp(argv[1], "-s") == 0) {
            //输出符号表
            symbolfile = argv[3];
        } else if (strcmp(argv[1], "-i") == 0) {
            //输出ir
            dumpfile = (argv[3]);
        } else if (strcmp(argv[1], "-a") == 0) {
            //输出ast树
            astfile = (argv[3]);
        } else if (strcmp(argv[1], "-anofix") == 0) {
            //输出ast树(no fix)
            astfile = (argv[3]);
            nofixast = true;
        }
    } else if (argc == 6) {
        //基本块划分
        G_FUNC_BLOCK = (argv[2]);
        blockfile = (argv[4]);
        filename = (argv[5]);
    }
    const char *sFile = (char *)filename.c_str();
    FILE *fp = fopen(sFile, "r");
    if (fp == NULL) {
        printf("cannot open %s\n", sFile);
        return -1;
    }
    extern FILE *yyin;
    // yyin和yyout都是FILE*类型
    yyin = fp;
    // yacc会从yyin读取输入，yyin默认是标准输入，这里改为磁盘文件。yacc默认向yyout输出，可修改yyout改变输出目的

    printf("\x1b[33m /********----开始解析 %s----********/\n \x1b[0m", sFile);
    yyparse();
    //使yacc开始读取输入和解析，它会调用lex的yylex()读取记号
    printf("\x1b[33m /********----%s 解析完成----********/\n \x1b[0m", sFile);
    fclose(fp);
    Agraph_t *g = agopen((char *)"ast", Agdirected, nullptr);
    Agraph_t *g_block = agopen((char *)"block", Agdirected, nullptr);
    if (!nofixast) {
        //修复ast树
        // cout << "\x1b[34m" << "******开始修复语法树......******" << endl << "\x1b[0m";
        ast(ast_root);
        ast2(ast_root);
        // cout << "\x1b[34m" << "******修复语法树完成......******" << endl << "\x1b[0m";
    }
    //==
    Agnode_t *gv_node = agnode(g, (char *)int2str(node_num++).c_str(), 1);
    agsafeset(gv_node, "shape", "plaintext", "");
    agsafeset(gv_node, "fontsize", "20", "");
    agsafeset(gv_node, (char *)"label", "minic_ast", "");
    //==
    genNodes(g, NULL, ast_root);
    cout << "\x1b[32m" << "*-*—*—开始构造语法树-*-*-*" << endl << "\x1b[0m";
    if (astfile[0] != '_') {
        GVC_t *gv = gvContext();
        gvLayout(gv, g, "dot");
        gvRenderFilename(gv, g, "png", astfile.c_str());
        agclose(g);
        gvFreeContext(gv);
        cout << "\x1b[32m" << "*-*—*—语法树输出完成-*-*-*" << endl << "\x1b[0m";
        if (argc == 5) {
            return 0;
        }
    }
    // ir分析
    ana(ast_root, 0, 0, -1);
    cout << "\x1b[32m" << "*-*—*—ir生成完成-*-*-*" << endl << "\x1b[0m";
    string dumpir = printIrs();
    string dumpsymbol = printSymbol();
    if (dumpfile[0] != '_') {
        ofstream ofs(dumpfile, ios::out);
        ofs << dumpir;
        ofs.close();
    } else if (symbolfile[0] != '_') {
        ofstream ofs(symbolfile, ios::out);
        ofs << dumpsymbol;
        ofs.close();
        cout << "\x1b[32m" << "*-*—*—符号表已生成-*-*-*" << endl << "\x1b[0m";
    }
    if (G_FUNC_BLOCK[0] != '_') {
        genBlock(g_block);
        GVC_t *gv = gvContext();
        gvLayout(gv, g_block, "dot");
        gvRenderFilename(gv, g_block, "png", blockfile.c_str());
        agclose(g_block);
        gvFreeContext(gv);
        cout << "\x1b[32m" << "*-*—*—完成基本块划分-*-*-*" << endl << "\x1b[0m";
    }

    return 0;
}