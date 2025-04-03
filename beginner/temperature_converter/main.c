#include <stdio.h>

int main()
{
    double celsius, fahrenheit, kelvin;
    int option;
    do
    {
        printf("Temperature Converter\n");
        printf("1. Celsius to Fahrenheit\n");
        printf("2. Fahrenheit to Celsius\n");
        printf("3. Celsius to Kelvin\n");
        printf("4. Kelvin to Celsius\n");
        printf("5. Fahrenheit to Kelvin\n");
        printf("6. Kelvin to Fahrenheit\n");
        printf("7. Exit\n");
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
        else if (option == 3)
        {
            printf("Enter temperature in Celsius: ");
            scanf("%lf", &celsius);
            kelvin = celsius + 273.15;
            printf("Temperature in Kelvin: %.2lf\n", kelvin);
        }
        else if (option == 4)
        {
            printf("Enter temperature in Kelvin: ");
            scanf("%lf", &kelvin);
            celsius = kelvin - 273.15;
            printf("Temperature in Celsius: %.2lf\n", celsius);
        }
        else if (option == 5)
        {
            printf("Enter temperature in Fahrenheit: ");
            scanf("%lf", &fahrenheit);
            kelvin = (fahrenheit - 32.0) * 5.0 / 9.0 + 273.15;
            printf("Temperature in Kelvin: %.2lf\n", kelvin);
        }
        else if (option == 6)
        {
            printf("Enter temperature in Kelvin: ");
            scanf("%lf", &kelvin);
            fahrenheit = (kelvin - 273.15) * 9.0 / 5.0 + 32.0;
            printf("Temperature in Fahrenheit: %.2lf\n", fahrenheit);
        }
        else if (option == 7)
        {
            printf("Exiting...\n");
        }
        else
        {
            printf("Invalid option. Please try again.\n");
        }
    } while (option != 7);

    return 0;
}
