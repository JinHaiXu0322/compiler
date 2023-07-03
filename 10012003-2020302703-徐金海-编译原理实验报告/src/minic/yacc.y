/* 在解析过程中遇到错误应该显示详细的错误信息 */
%define parse.error verbose
/* 启用了位置跟踪功能，以便在错误报告中指示具体的源代码位置 */
%locations
/* 词法分析器和语法分析器之间共享代码 */
%{
#include "ast.h"
extern int yylineno;
extern FILE *yyin;
void yyerror(const char* fmt, ...);
%}
// 联合体声明，用于后续终结符和非终结符号属性指定使用
%union {
    int data_int;
    string data_str;
    char data_char;
    Node *node;
}

%type  <node> Program Segments Bigdef Type ExtDecList FuncDec Blockstat VarList VarDec ParamVarDec ExtVar

%type  <node> ParamDef Stmt StmList DecList Dec Exp Args IntCount 

/* 在词法分析阶段，当识别到一个整数时，其值将被保存为 data_int 类型的语义值。 */
%token <data_int> INTEGER

/* 在词法分析阶段，当识别到一个标识符或关系运算符时，相应的字符串将被保存为 data_str 类型的语义值。 */
%token <data_str> IDENTIFIER RELOP TYPE

/* 在词法分析阶段，当识别到一个字符时，其值将被保存为 data_char 类型的语义值 */
%token <data_char> CHAR   


/* 当识别到这些符号时，它们将作为对应的标记被传递给语法分析器进行进一步处理 */
%token LP RP LB RB LBT RBT SEMICOLON COMMA
//     (  )  {   }  [   ]     ;        ,  已在miniclex.l中定义

//      +    -   *   /   %     =    && ||  !  if else while return break continue
%token ADD  SUB MUL DIV MOD ASSIGN AND OR NOT IF ELSE WHILE RETURN BREAK CONTINUE

/* 以下是一些左结合运算符 */
%left ASSIGN
%left OR
%left AND
%left RELOP
%left INC DEC
%left SUBASS ADDASS
%left ADD SUB
%left MUL DIV MOD

/* 以下是一些右结合运算符 */
%right NEG NOT 

/* 非关联，用于在条件语句中指定“if-else”之间的优先级 */
%nonassoc LOWER_THEN_ELSE

/* 非关联，用于在条件语句中指定“else”与前面的“if”之间的优先级 */
%nonassoc ELSE

%%

/* ->segment->全局变量或者函数名定义或声明 */
/* 当语法分析器成功解析 Segments 并进入规则的动作部分时，将创建一个名为 AST_OP_PROGRAM 的 AST 节点，并将它作为整个程序的根节点 */
Program: Segments{ast_root = new_ast_node(AST_OP_PROGRAM,yylineno,$1,NULL,NULL);$$=ast_root;}; 

/*Segments 零个或多个Bigdef*/
/* 当语法分析器解析一个 Bigdef，并且后续还有更多的语句或定义时，将创建一个名为 AST_OP_SEGMENTS 的 AST 节点，并将它作为一个语句或定义序列的根节点 */
Segments: {$$=NULL;} 
        | Bigdef Segments {$$=new_ast_node(AST_OP_SEGMENTS,yylineno,$1,$2,NULL);}
        ; 

/*接Segment 表示全局变量，结构体，函数声明，函数定义*/
Bigdef: Type ExtDecList SEMICOLON {$$=new_ast_node(AST_OP_EXT_VAR_DEF,yylineno,$1,$2,NULL);}   //全局变量
        | error SEMICOLON   {$$=NULL; }
        //|Type SEMICOLON {} //结构体当前未实现
        |Type FuncDec Blockstat {$$=new_ast_node(AST_OP_FUNC_DEF,yylineno,$1,$2,$3);}         //函数定义
        |Type FuncDec SEMICOLON {$$=new_ast_node(AST_OP_FUNC_DEF_LITE,yylineno,$1,$2,NULL);}//函数声明
        ;
 
/*零个或多个变量的定义VarDec*/
ExtDecList: VarDec {$$=$1;} 
        | VarDec COMMA ExtDecList {$$=new_ast_node(AST_OP_EXT_DEC_LIST,yylineno,$1,$3,NULL);}
        ; 


/*类型描述符,如int float char*/
Type: TYPE {$$=new_ast_node(AST_OP_TYPE,yylineno,NULL,NULL,NULL);$$->data_str=$1;} 
        //|结构体
        ;    


/*对一个变量的定义*/ 
VarDec:  IDENTIFIER ASSIGN Exp {$$=new_ast_node(AST_OP_V_D_ASSIGN,yylineno,$3,NULL,NULL);$$->data_str=$1;}   //a = 5.测试样例里不存在a[4]=5一类数组
        | IDENTIFIER {$$=new_ast_node(AST_OP_IDENTIFIER,yylineno,NULL,NULL,NULL);$$->data_str=$1;}   //标识符，如a
        | VarDec LBT IntCount RBT        {$$=new_ast_node(AST_OP_ARRAY,yylineno,$1,$3,NULL);} //数组，如a[10]，可以叠来多重数组

        
/*形参相关*/
ParamVarDec:    IDENTIFIER {$$=new_ast_node(AST_OP_IDENTIFIER,yylineno,NULL,NULL,NULL);$$->data_str=$1;}   //标识符，如a
                | IDENTIFIER LBT RBT        {$$=new_ast_node(AST_OP_IDENTIFIER_P_ARRAY,yylineno,NULL,NULL,NULL);$$->data_str=$1;} //数组，如a[]，仅用于形参
                | IDENTIFIER LBT INTEGER RBT        {$$=new_ast_node(AST_OP_IDENTIFIER_P_ARRAY_INTEGER,yylineno,NULL,NULL,NULL);$$->data_str=$1;$$->data_int=$3;} //数组，如a[10]
                ;

/*用于辅助数组文法*/
IntCount: INTEGER {$$=new_ast_node(AST_OP_INTEGER,yylineno,NULL,NULL,NULL);$$->data_int=$1;}
        ;

/*函数头的定义*/ //a() or a(int b, int c)
FuncDec: IDENTIFIER LP VarList RP {$$=new_ast_node(AST_OP_FUNC_DEC,yylineno,$3,NULL,NULL);$$->data_str=$1;}//含参数函数
        |IDENTIFIER LP  RP   {$$=new_ast_node(AST_OP_FUNC_DEC,yylineno,NULL,NULL,NULL);$$->data_str=$1;}//无参数函数
        ;  

/*参数列表*/
VarList: ParamDef  {$$=$1;}//一个形参的定义
        | ParamDef COMMA  VarList  {$$=new_ast_node(AST_OP_PARAM_LIST,yylineno,$1,$3,NULL);}
        ;

/*一个形参的定义 int     */
ParamDef: Type ParamVarDec         {$$=new_ast_node(AST_OP_PARAM_DEF,yylineno,$1,$2,NULL);}
        | Type                     {$$=new_ast_node(AST_OP_PARAM_DEF_BUILTIN,yylineno,$1,NULL,NULL);}
        ;

/*花括号括起来的语句块*/
Blockstat: LB StmList RB    {$$=new_ast_node(AST_OP_COMP_STM,yylineno,$2,NULL,NULL);}
        ;

/*一系列语句列表*/
StmList: {$$=NULL; }  
        | Stmt StmList  {$$=new_ast_node(AST_OP_STM_LIST,yylineno,$1,$2,NULL);}
        ;

/*单条语句*/
Stmt: Exp SEMICOLON    {$$=new_ast_node(AST_OP_EXP_STMT,yylineno,$1,NULL,NULL);}//一条表达式
      | Blockstat      {$$=$1;}      //另一个语句块
      | RETURN SEMICOLON   {$$=new_ast_node(AST_OP_RETURN_EMP,yylineno,NULL,NULL,NULL);}//空返回语句
      | RETURN Exp SEMICOLON   {$$=new_ast_node(AST_OP_RETURN,yylineno,$2,NULL,NULL);}//返回语句
      | IF LP Exp RP Stmt %prec LOWER_THEN_ELSE   {$$=new_ast_node(AST_OP_IF,yylineno,$3,$5,NULL);} //if语句
      | IF LP Exp RP Stmt ELSE Stmt   {$$=new_ast_node(AST_OP_IF_ELSE,yylineno,$3,$5,$7);}//if-else 语句
      | WHILE LP Exp RP Stmt {$$=new_ast_node(AST_OP_WHILE,yylineno,$3,$5,NULL);}//while 语句
      | Type DecList SEMICOLON {$$=new_ast_node(AST_OP_VAR_DEF,yylineno,$1,$2,NULL);}
      ;

/*如int a,b,c*/
DecList: Dec  {$$=$1;}
        | Dec COMMA DecList  {$$=new_ast_node(AST_OP_DEC_LIST,yylineno,$1,$3,NULL);}
	;

/*变量名(或者带初始化不考虑)*/
Dec:   VarDec {$$=$1;}
       ;

/*表达式中可能遇到的IDENT和ARRAY（中括号里面可能是exp）*/
ExtVar:  IDENTIFIER {$$=new_ast_node(AST_OP_IDENTIFIER,yylineno,NULL,NULL,NULL);$$->data_str=$1;}   //标识符，如a
        | ExtVar LBT Exp RBT        {$$=new_ast_node(AST_OP_ARRAY,yylineno,$1,$3,NULL);} //数组，如a[10]，a[i]，a[i+1]
        ;

/*运算表达式*/
Exp:  //二元运算
        Exp ASSIGN Exp {$$=new_ast_node(AST_OP_ASSIGN,yylineno,$1,$3,NULL);}//二元运算
      | Exp AND Exp   {$$=new_ast_node(AST_OP_AND,yylineno,$1,$3,NULL);}//逻辑与
      | Exp OR Exp    {$$=new_ast_node(AST_OP_OR,yylineno,$1,$3,NULL);}//逻辑或
      | Exp RELOP Exp {$$=new_ast_node(AST_OP_RELOP,yylineno,$1,$3,NULL);$$->data_str=$2;}//关系表达式
      | Exp ADD Exp  {$$=new_ast_node(AST_OP_ADD,yylineno,$1,$3,NULL);}//五则则运算
      | Exp SUB Exp {$$=new_ast_node(AST_OP_SUB,yylineno,$1,$3,NULL);}
      | Exp MUL Exp  {$$=new_ast_node(AST_OP_MUL,yylineno,$1,$3,NULL);}
      | Exp DIV Exp   {$$=new_ast_node(AST_OP_DIV,yylineno,$1,$3,NULL);}
      | Exp MOD Exp   {$$=new_ast_node(AST_OP_MOD,yylineno,$1,$3,NULL);}
/*
      | Exp ADDASS Exp  {$$=new_ast_node("ADDASS",yylineno,$1,$3,NULL);} //复合加
      | Exp INC      {$$=new_ast_node("RINC",yylineno,$1,NULL,NULL);}  //右自增
      | INC Exp     {$$=new_ast_node("LINC",yylineno,$2,NULL,NULL);}  //左自增
      | Exp SUBASS Exp {$$=new_ast_node("SUBASS",yylineno,$1,$3,NULL);} //复合减
      | Exp DEC  {$$=new_ast_node("RDEC",yylineno,$1,NULL,NULL);} //自减
      | DEC Exp {$$=new_ast_node("LDEC",yylineno,$2,NULL,NULL);} //自减 
*/
      //一元运算
      | LP Exp RP     {$$=$2;}//括号表达式
      | SUB Exp %prec NEG   {$$=new_ast_node(AST_OP_NEG,yylineno,$2,NULL,NULL);}//取负
      | NOT Exp       {$$=new_ast_node(AST_OP_NOT,yylineno,$2,NULL,NULL);}//逻辑或
      
      //不包含运算符，较特殊的表达式
      | IDENTIFIER LP Args RP {$$=new_ast_node(AST_OP_FUNC_CALL,yylineno,$3,NULL,NULL);$$->data_str=$1;}  //函数调用(含参)
      | IDENTIFIER LP RP      {$$=new_ast_node(AST_OP_FUNC_CALL,yylineno,NULL,NULL,NULL);$$->data_str=$1;}//函数调用(无参)

      
      //最基本表达式
      | ExtVar  {$$=$1;}//"IDENT"或AST_OP_ARRAY数组
      | INTEGER           {$$=new_ast_node(AST_OP_INTEGER,yylineno,NULL,NULL,NULL);$$->data_int=$1;}      //整常数常量
      | CHAR          {$$=new_ast_node(AST_OP_CHAR,yylineno,NULL,NULL,NULL);$$->data_char=$1;} //字符常量
      | BREAK         {$$=new_ast_node(AST_OP_BREAK,yylineno,NULL,NULL,NULL);}
      | CONTINUE      {$$=new_ast_node(AST_OP_CONTINUE,yylineno,NULL,NULL,NULL);}
      ;

/*实参列表*/
Args:  Exp COMMA Args    {$$=new_ast_node(AST_OP_ARG_LIST,yylineno,$1,$3,NULL);}
       | Exp               {$$=new_ast_node(AST_OP_ARG_LIST,yylineno,$1,NULL,NULL);}
       ;
       
%%