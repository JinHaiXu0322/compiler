#include "add.h"
#include "fibonacci.h"
#include "factorial.h"

#define g1 10
int g2;

int main ()
{
    int a = 10;
    int c, d;
    int mul;
    int f;
    int i = 1;
    int j = 1;
    
    for(j=1; j > 5 && j < g1; j++)
    {
        f = factorial(j);
    }

	while(1){
        mul= i * add(a, g1);
        i++;
        if (mul > 100)   break; 
    }
    
    d = fibonacci(a);
    c = add(a, g1);
    c = ADD(c, g2);
    return c;
}
