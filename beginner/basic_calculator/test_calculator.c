#include <stdio.h>
#include <assert.h>
#include "basic_calculator.h"

void test_calculator()
{
    assert(calculate(5, 3, 1) == 8);
    assert(calculate(5, 3, 2) == 2);
    assert(calculate(5, 3, 3) == 15);
    assert(calculate(6, 3, 4) == 2.0);
    assert(calculate(5, 0, 4) == 0);
    assert(calculate(5, 3, 99) == -999999);

    printf("All tests passed!\n");
}

int main()
{
    test_calculator();
    return 0;
}
