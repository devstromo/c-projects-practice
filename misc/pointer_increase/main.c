#include "stdio.h"

int main()
{
    int x = 5;
    int *p = &x;
    printf("Value of x: %d\n", *p++);
    int arr[] = {10, 20, 30};
    int *pa = arr;

    printf("%d\n", *pa++); // prints 10, p now points to arr[1]
    printf("%d\n", *pa++); // prints 20, p now points to arr[2]
    printf("%d\n", *pa);
    return 0;
}