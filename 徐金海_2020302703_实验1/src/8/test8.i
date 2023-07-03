# 0 "test8.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "test8.c"
int global_var_init = 10;
int global_var_uninit;

int add (int a, int b)
{
    return a + b;
}

int main()
{
    int sum = add (global_var_init, global_var_uninit);
    return 0;
}
