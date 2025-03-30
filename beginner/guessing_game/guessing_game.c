#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    srand(time(NULL));
    int r = (rand() % 100) + 1;
    printf("Welcom to random number guessing game\n");
    printf("First, select the mode option:\n\t1. Chill(No limit)\n\t2. Normal(10 attempts)\n\t3. Hard(5 attempts)\n");
    int mode = 0;
    scanf("%d", &mode);
    int attemptsLimit = 0;
    if (mode == 1)
    {
        printf("You have selected Chill mode\n");
    }
    else if (mode == 2)
    {
        printf("You have selected Normal mode\n");
        attemptsLimit = 10;
    }
    else if (mode == 3)
    {
        printf("You have selected Hard mode\n");
        attemptsLimit = 5;
    }
    else
    {
        printf("Invalid mode selected. Exiting...\n");
        return 1;
    }
    printf("Guess a number between 1 and 100\n");
    int guess = 0;

    int attempts = 0;
    while (guess != r && (attemptsLimit == 0 || attempts < attemptsLimit))
    {
        if (attemptsLimit > 0)
        {
            printf("You have %d attempts left\n", attemptsLimit - attempts);
        }
        printf("Enter your guess: ");
        scanf("%d", &guess);
        attempts++;
        if (guess < r)
        {
            printf("Too low! Try again.\n");
        }
        else if (guess > r)
        {
            printf("Too high! Try again.\n");
        }
        else
        {
            printf("Congratulations! You guessed the number in %d attempts.\n", attempts);
            break;
        }
        if (attemptsLimit > 0 && attempts >= attemptsLimit)
        {
            printf("Sorry, you've used all your attempts. The number was %d.\n", r);
            break;
        }
    }
    printf("Would you like to play again? (y/n): ");
    char playAgain;
    scanf(" %c", &playAgain);
    if (playAgain == 'y' || playAgain == 'Y')
    {
        main(); // Restart the game
    }
    else
    {
        printf("Thank you for playing!\n");
    }
    return 0;
}
