#ifndef BLOCK_H
#define BLOCK_H

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
#include "symbol.h"

// 用于生成CFG
void genBlock(Agraph_t *g);

// BLOCK_H
#endif 