#include <stdio.h>

int main()
{
    double celsius, fahrenheit;
    int option;
    do
    {
        printf("Temperature Converter\n");
        printf("1. Celsius to Fahrenheit\n");
        printf("2. Fahrenheit to Celsius\n");
        printf("3. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &option);
        if (option == 1)
        {
            printf("Enter temperature in Celsius: ");
            scanf("%lf", &celsius);
            fahrenheit = (celsius * 9.0 / 5.0) + 32.0;
            printf("Temperature in Fahrenheit: %.2lf\n", fahrenheit);
        }
        else if (option == 2)
        {
            printf("Enter temperature in Fahrenheit: ");
            scanf("%lf", &fahrenheit);
            celsius = (fahrenheit - 32.0) * 5.0 / 9.0;
            printf("Temperature in Celsius: %.2lf\n", celsius);
        }
        else if (option != 3)
        {
            printf("Invalid option, please try again.\n");
        }
    } while (option != 3);

    return 0;
}
