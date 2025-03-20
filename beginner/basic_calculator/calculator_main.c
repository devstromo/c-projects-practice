#include <stdio.h>
#include "basic_calculator.h"

int main()
{
    int a, b, op;
    printf("Enter first integer number: ");
    scanf("%d", &a);
    printf("Enter second integer number: ");
    scanf("%d", &b);
    printf("Enter operation number\n1. Addition\n2. Subtraction\n3. Multiplication\n4. Division\n5. Power\n6. Mod\n");
    scanf("%d", &op);

    if (op == 4 && b == 0)
    {
        printf("\n\t\t\t\tDivision by zero is not allowed\n");
    }
    else
    {
        printf("\n\t\t\t\tResult: %.2f\n", calculate(a, b, op));
    }

    return 0;
}
