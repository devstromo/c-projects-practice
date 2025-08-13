#include "stdio.h"

int main()
{
    int x = 5;
    int *p = &x;
    printf("Value of x: %d\n", *p++);

    return 0;
}