#include <stdio.h>
#include <math.h>
#include "basic_calculator.h"

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
    case 5:
        return (double) pow(a, b); 
    default:
        return -999999;
    }
}
