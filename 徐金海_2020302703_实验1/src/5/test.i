# 0 "test.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "test.c"
# 1 "add.h" 1



int add(int a, int b);
# 2 "test.c" 2
# 1 "fibonacci.h" 1



int fibonacci(int a);
# 3 "test.c" 2
# 1 "factorial.h" 1



int factorial(int a);
# 4 "test.c" 2


int g2;

int main ()
{
    int a = 10;
    int c, d;
    int mul;
    int f;
    int i = 1;
    int j = 1;

    for(j=1; j > 5 && j < 10; j++)
    {
        f = factorial(j);
    }

 while(1){
        mul= i * add(a, 10);
        i++;
        if (mul > 100) break;
    }

    d = fibonacci(a);
    c = add(a, 10);
    c = ((c)+(g2));
    return c;
}
