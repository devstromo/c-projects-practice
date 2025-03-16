#include <stdio.h>

int main()
{
    int a, b, op;
    printf("Enter first integer number\n");
    scanf("%d", &a);
    printf("Enter second integer number\n");
    scanf("%d", &b);
    printf("Enter operation number\n1. Addition\n2. Subtraction\n3. Multiplication\n4. Division\n");
    scanf("%d", &op);
    switch (op)
    {
    case 1:
        printf("\n\t\t\t\tThe sum of %d and %d is %d\n", a, b, a + b);
        break;
    case 2:
        printf("\n\t\t\t\tThe difference of %d and %d is %d\n", a, b, a - b);
        break;
    case 3:
        printf("\n\t\t\t\tThe product of %d and %d is %d\n", a, b, a * b);
        break;
    case 4:
        if (b == 0)
            printf("\n\t\t\t\tDivision by zero is not allowed\n");
        else
            printf("\n\t\t\t\tThe division of %d and %d is %d\n", a, b, a / b);
        break;
    default:
        printf("\n\t\t\t\tInvalid operation\n");
    }

    return 0;
}
