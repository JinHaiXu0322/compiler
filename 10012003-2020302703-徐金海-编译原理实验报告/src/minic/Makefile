CFLAGS = -static-libstdc++ -static-libgcc -w -O2 -Wno-write-strings
#-g表示调试
LEX=flex
YACC=bison
CC=g++
OBJECT=minic

$(OBJECT): lex.yy.o  yacc.tab.o ast.o ir.o minic_calculator.o graph.o block.o symbol.o main.o
	$(CC) $(CFLAGS) lex.yy.o yacc.tab.o ast.o ir.o minic_calculator.o graph.o block.o symbol.o main.o -o $(OBJECT) $(shell pkg-config --cflags --libs libgvc)
	@rm -f  *.o *.tab.c *.tab.h *.yy.c

main.o: main.cpp ir.h minic_calculator.h graph.h
	$(CC) $(CFLAGS) -c main.cpp

symbol.o: symbol.cpp symbol.h ir.h
	$(CC) $(CFLAGS) -c symbol.cpp	

block.o: block.cpp block.h ir.h
	$(CC) $(CFLAGS) -c block.cpp	

graph.o: graph.cpp graph.h ir.h
	$(CC) $(CFLAGS) -c graph.cpp	

minic_calculator.o: minic_calculator.cpp minic_calculator.h ir.h symbol.h
	$(CC) $(CFLAGS) -c minic_calculator.cpp

ir.o: ir.cpp ir.h
	$(CC) $(CFLAGS) -c ir.cpp

lex.yy.o: lex.yy.c  yacc.tab.h ast.h
	$(CC) $(CFLAGS) -c lex.yy.c

yacc.tab.o: yacc.tab.c ast.h
	$(CC) $(CFLAGS) -c yacc.tab.c

ast.o: ast.cpp ast.h
	$(CC) $(CFLAGS) -c ast.cpp

yacc.tab.c  yacc.tab.h: yacc.y
	$(YACC) -d yacc.y -Wno-conflicts-sr

lex.yy.c: miniclex.l
	$(LEX) miniclex.l

clean:
	@rm -f $(OBJECT)  *.o *.tab.c *.tab.h *.yy.c *.png sym.txt
