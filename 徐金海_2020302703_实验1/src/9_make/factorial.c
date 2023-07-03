#include "factorial.h"

int factorial(int a)
{
    // 函数内部静态变量，用于统计函数被调用的次数
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