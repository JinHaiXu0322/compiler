
int main() {
    int i = 1;
    int sum = 0;
    while (i <= 10) {
        sum += i * i;
        i++;
    }
 
    int j;
    int sum2 = 0;
    for (j = 1; j <= 10; j++) {
        sum2 += j * j;
    }

    return 0;
}