#include "add.h"

int g1 = 10;
int g2;

int main()
{
    int a = 10;
    int c;
    c = add(a, g1);
    c = ADD(c, g2);
    return c;
}