```yaml
%type  <node> Program Segments Bigdef Type ExtDecList FuncDec Blockstat VarList VarDec ParamVarDec ExtVar

%type  <node> ParamDef Stmt StmList DecList Dec Exp Args IntCount 

%token <data_int> INTEGER
%token <data_str> IDENTIFIER RELOP TYPE
%token <data_char> CHAR   
%token LP RP LB RB LBT RBT SEMICOLON COMMA
%token ADD  SUB MUL DIV MOD ASSIGN AND OR NOT IF ELSE WHILE RETURN BREAK CONTINUE

%left ASSIGN
%left OR
%left AND
%left RELOP
%left INC DEC
%left SUBASS ADDASS
%left ADD SUB
%left MUL DIV MOD

%right NEG NOT 

%nonassoc LOWER_THEN_ELSE
%nonassoc ELSE

Program: Segments 

Segments: {$$=NULL;} 
          |Bigdef Segments
          ; 

Bigdef: Type ExtDecList SEMICOLON 
        |error SEMICOLON
        |Type SEMICOLON {}
        |Type FuncDec Blockstat
        |Type FuncDec SEMICOLON 
        ;
 
ExtDecList: VarDec
            |VarDec COMMA ExtDecList
            ; 

Type: TYPE 
        ;    

VarDec: IDENTIFIER ASSIGN Exp
        |IDENTIFIER
        |VarDec LBT IntCount RBT 

ParamVarDec: IDENTIFIER 
             |IDENTIFIER LBT RBT        
             |IDENTIFIER LBT INTEGER RBT        
             ;

IntCount: INTEGER 
          ;

FuncDec: IDENTIFIER LP VarList RP
         |IDENTIFIER LP  RP
         ;  

VarList: ParamDef
         |ParamDef COMMA  VarList
         ;

ParamDef: Type ParamVarDec
          |Type                     
          ;

Blockstat: LB StmList RB
           ;

StmList: 
        |Stmt StmList
        ;

Stmt: Exp SEMICOLON
      |Blockstat
      |RETURN SEMICOLON
      |RETURN Exp SEMICOLON
      |IF LP Exp RP Stmt %prec LOWER_THEN_ELSE   
      |IF LP Exp RP Stmt ELSE Stmt
      |WHILE LP Exp RP Stmt
      |Type DecList SEMICOLON
      ;

DecList: Dec
         |Dec COMMA DecList
	     ;

Dec: VarDec
     ;

ExtVar: IDENTIFIER 
        |ExtVar LBT Exp RBT 
        ;


Exp: Exp ASSIGN Exp
     |Exp AND Exp
     |Exp OR Exp
     |Exp RELOP Exp
     |Exp ADD Exp
     |Exp SUB Exp
     |Exp MUL Exp
     |Exp DIV Exp
     |Exp MOD Exp
     |Exp ADDASS Exp
     |Exp INC
     |INC Exp
     |Exp SUBASS Exp
     |Exp DEC
     |DEC Exp
     |LP Exp RP
     |SUB Exp %prec NEG
     |NOT Exp
     |IDENTIFIER LP Args RP
     |IDENTIFIER LP RP
     |ExtVar
     |INTEGER
     |CHAR
     |BREAK
     |CONTINUE
     ;

Args:  Exp COMMA Args
       |Exp
       ;
```

