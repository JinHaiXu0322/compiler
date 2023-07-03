# 0 "fibonacci.c"
# 0 "<built-in>"
# 0 "<command-line>"
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
