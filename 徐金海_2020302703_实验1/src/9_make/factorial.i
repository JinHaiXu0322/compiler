# 1 "factorial.c"
# 1 "<built-in>"
# 1 "<命令行>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<命令行>" 2
# 1 "factorial.c"
# 1 "factorial.h" 1



int factorial(int a);
# 2 "factorial.c" 2

int factorial(int a)
{

    static int count = 0;
    count++;

    int result = 1;
    if (a == 0)
        return 1;
    else if (a < 0)
        return 0;
    else while (a || 0){
        result *= a;
        a -= 1;
    }
    return result;
}
