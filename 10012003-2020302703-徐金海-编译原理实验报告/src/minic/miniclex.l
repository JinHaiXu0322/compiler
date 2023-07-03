/* %{ %}段中定义一些全局变量，函数以及一些用于处理此法单元位置信息的代码 */
/*  
    1、包含了ly.h和yacc.tab.h两个头i文件
    2、定义并初始化全局变量yycolumn=1，用于记录此法单元所在的行号信息
    3、宏定义一个宏YY_USER_ACTION，它是一个词法分析器中的动作，它更新了yylloc变量的值，该变量表示当前词法单元的位置信息。
    通过设置yylloc的行号和列号等属性，可以跟踪词法单元在源代码中的位置。
    4、函数声明：这段代码声明了三个函数，分别是comment()、hex2int()和oct2int()。
    分别用于处理注释、16进制数和8进制数
*/
%{
#include "ast.h"
#include "yacc.tab.h"
int yycolumn=1;
#define YY_USER_ACTION yylloc.first_line=yylloc.last_line=yylineno; \
yylloc.first_column=yycolumn;	yylloc.last_column=yycolumn+yyleng-1; yycolumn+=yyleng;//每次匹配之前执行的动作,yylloc表示当前词法单元所对应的位置信息

void comment();
int hex2int(char * input);
int oct2int(char * input);
%}

/* 这是Flex的选项之一，用于启用行号计数 */
%option yylineno

/* 以下四行分别给出了标识符、整数（含8进制）、16进制整数、字符的正则式 */
id   [A-Za-z_][A-Za-z0-9_]*  
int    [0-9]+
hex [0-9a-fA-F]+
char [A-Z]|[a-z]|"!"|"@"|"#"|"$"|"%"|"^"|"&"|"*"|"("|")"|"_"|"-"|"+"|"="|"\\"|"{"|"}"|"["|"]"|":"|";"|"'"|"<"|">"|","|"."|"?"|"/"|"~"|"`"

/* 以下给出了一些可能存在的数据类型，保留字，运算符等的token定义，用于词法分析
"+="        {return ADDASS;}
"++"        {return INC;}
"-="        {return SUBASS;}
"--"        {return DEC;}
 */
%%


[0][x]{hex}        {yylval.data_int=hex2int(yytext); return INTEGER;}
[0]{int}        {yylval.data_int=oct2int(yytext); return INTEGER;}
{int}        {yylval.data_int=atoi(yytext); return INTEGER;}
"'"{char}"'" {yylval.data_char=yytext[1]; return CHAR;}
"void"        {yylval.data_str=yytext;return TYPE;}
"int"        {yylval.data_str=yytext;return TYPE;}
"char"       {yylval.data_str=yytext;return TYPE;}

"return"     {return RETURN;}
"if"         {return IF;}
"else"       {return ELSE;}
"while"      {return WHILE;}
"break"      {return BREAK;}
"continue"   {return CONTINUE;}

{id}       {yylval.data_str=yytext; return IDENTIFIER;}
";"			{return SEMICOLON;}
","			{return COMMA;}
">"|"<"|">="|"<="|"=="|"!=" {yylval.data_str=yytext;return RELOP;}
"="			{return ASSIGN;}
"+"			{return ADD;}
"-"			{return SUB;}
"*"			{return MUL;}
"/"			{return DIV;}
"%"         {return MOD;}
"&&"        {return AND;}
"||"        {return OR;}
"!"			{return NOT;}
"+="        {return ADDASS;}
"++"        {return INC;}
"-="        {return SUBASS;}
"--"        {return DEC;}
"("			{return LP;}//parentheses
")"			{return RP;}
"{"			{return LB;}//braces
"}"			{return RB;}
"["         {return LBT;}//brackets
"]"         {return RBT;}
"//"[^\n]*\n  ;
"/*"([^\*]|(\*)*[^\*/])*(\*)*"*/" ;
[\n]        {yycolumn=1;}   
[ \r\t]     {}   
.			{printf("Error type A :Mysterious character \"%s\"\n\t at Line %d\n",yytext,yylineno);}
%%

/* 
    yywrap()用于告知词法分析器是否应该继续进行的函数 
    默认情况下，词法分析器会在到达文件末尾时调用yywrap()函数。
    通过返回1，词法分析器将停止扫描，表示已经完成了所有的词法分析任务。
*/
int yywrap()
{
	return 1;
}

//以0开头的整数位8进制整数，将其化为10进制
int oct2int(char * input)
{
    int rt=0;
    for(int i=1;input[i]!='\0';i++)
    {
        int t;
        t = input[i] - '0';
        rt = rt*8+t;
    }
    return rt;
}

/* 
    以0x开头的整数为16进制整数，将其化为10进制
    注意数据的字母大小写，通过字符的ASCAII值计算到10进制    
*/
int hex2int(char * input)
{
    int rt=0;
    for(int i=2;input[i]!='\0';i++)
    {
        int t;
        if (input[i] <= '9')
        {
            t= input[i] - '0';
        }
        else if(input[i] >= 'A' && input[i] <= 'F')
        {
            t = input[i] - 'A' + 10;
        }
        else if(input[i] >= 'a' && input[i] <= 'f')
        {
            t = input[i] - 'a' + 10;
        }
        rt = rt*16+t;
    }
    return rt;
}
