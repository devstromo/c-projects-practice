#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * Determines the result of the game between the player and the computer.
 *
 * @param you The player's choice: 's' for stone, 'p' for paper, 'z' for scissors.
 * @param computer The computer's choice: 's' for stone, 'p' for paper, 'z' for scissors.
 * @return 1 if the player wins, 0 if the computer wins, and -1 for a draw.
 */
int game(char you, char computer)
{
    // If both player and computer choose the same, it's a draw
    if (you == computer)
        return -1;

    // Stone beats scissors, paper beats stone, scissors beats paper
    if (you == 's' && computer == 'p')
        return 0; // Player loses if they pick stone and the computer picks paper
    else if (you == 'p' && computer == 's')
        return 1; // Player wins if they pick paper and the computer picks stone

    if (you == 's' && computer == 'z')
        return 1; // Player wins if they pick stone and the computer picks scissors
    else if (you == 'z' && computer == 's')
        return 0; // Player loses if they pick scissors and the computer picks stone

    if (you == 'p' && computer == 'z')
        return 0; // Player loses if they pick paper and the computer picks scissors
    else if (you == 'z' && computer == 'p')
        return 1; // Player wins if they pick scissors and the computer picks paper
}

int main()
{
    char player, computer, result;

    // Initialize random seed based on the current time
    srand(time(NULL));

    // Generate a random number to select the computer's choice
    int n = rand() % 100;

    // Randomly assign 's', 'p', or 'z' to the computer based on the random number
    if (n < 33)
        computer = 's'; // Stone
    else if (n >= 33 && n < 66)
        computer = 'p'; // Paper
    else
        computer = 'z'; // Scissors

    // Prompt the player to enter their choice
    printf("\n\n\n\n\t\t\t\tEnter s for STONE, p for PAPER and z for SCISSOR\n\t\t\t\t\t\t\t");

    // Read player's choice
    scanf("%c", &player);

    // Get the result of the game
    result = game(player, computer);

    // Display the result based on the returned value
    if (result == -1)
    {
        printf("\n\n\t\t\t\tGame Draw!\n");
    }
    else if (result == 1)
    {
        printf("\n\n\t\t\t\tWow! You have won the game!\n");
    }
    else
    {
        printf("\n\n\t\t\t\tOh! You have lost the game!\n");
    }

    // Display both player and computer choices
    printf("\t\t\t\tYou choose : %c and Computer choose : %c\n", player, computer);

    return 0;
}