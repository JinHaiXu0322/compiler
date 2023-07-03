int global_var_init = 10;   // 已初始化的全局变量
int global_var_uninit;      // 未初始化的全局变量

int add (int a, int b)
{
    return a + b;
}

int main()
{
    int sum = add (global_var_init, global_var_uninit);
    return 0;
}