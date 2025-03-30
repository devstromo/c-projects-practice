#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    srand(time(NULL));
    int r = (rand() % 100) + 1;
    printf("Welcom to random number guessing game\n");
    printf("Guess a number between 1 and 100\n");
    int guess = 0;
    int attempts = 0;
    while (guess != r)
    {
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
