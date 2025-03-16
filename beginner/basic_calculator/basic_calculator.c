#include <stdio.h>

// Function to perform calculations
double calculate(int a, int b, int op)
{
    switch (op)
    {
    case 1:
        return a + b;
    case 2:
        return a - b;
    case 3:
        return a * b;
    case 4:
        return (b != 0) ? (double)a / b : 0;
    default:
        return -999999;
    }
}

int main()
{
    int a, b, op;
    printf("Enter first integer number: ");
    scanf("%d", &a);
    printf("Enter second integer number: ");
    scanf("%d", &b);
    printf("Enter operation number\n1. Addition\n2. Subtraction\n3. Multiplication\n4. Division\n");
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
