# 1 "fibonacci.c"
# 1 "<built-in>"
# 1 "<命令行>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<命令行>" 2
# 1 "fibonacci.c"
# 1 "fibonacci.h" 1



int fibonacci(int a);
# 2 "fibonacci.c" 2

int fibonacci(int n) {
    if (n <= 1)
        return n;
    else
        return fibonacci(n-1) + fibonacci(n-2);
}
