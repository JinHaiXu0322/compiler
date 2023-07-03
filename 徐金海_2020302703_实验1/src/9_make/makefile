test: test.o add.o factorial.o fibonacci.o
	gcc -o test test.o add.o factorial.o fibonacci.o
test.o: test.s
	gcc -c -o test.o test.s
add.o: add.s
	gcc -c -o add.o add.s
factorial.o: factorial.s
	gcc -c -o factorial.o factorial.s
fibonacci.o: fibonacci.s
	gcc -c -o fibonacci.o fibonacci.s
test.s: test.i
	gcc -S -o test.s test.i
add.s: add.i
	gcc -S -o add.s add.i
factorial.s: factorial.i
	gcc -S -o factorial.s factorial.i
fibonacci.s: fibonacci.i
	gcc -S -o fibonacci.s fibonacci.i
test.i: test.c
	gcc -E -o test.i test.c
add.i: add.c
	gcc -E -o add.i add.c
factorial.i: factorial.c
	gcc -E -o factorial.i factorial.c
fibonacci.i: fibonacci.c
	gcc -E -o fibonacci.i fibonacci.c
clean:
	rm *.o *.i *.s test