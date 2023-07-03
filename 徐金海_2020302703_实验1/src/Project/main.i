# 0 "main.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "main.c"
# 1 "add.h" 1



int add(int a, int b);
# 2 "main.c" 2

int g1 = 10;
int g2;

int main()
{
    int a = 10;
    int c;
    c = add(a, g1);
    c = ((c)+(g2));
    return c;
}
