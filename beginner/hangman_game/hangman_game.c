#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORD_LENGTH 50
#define MAX_TRIES 6

struct WordHint {
    char word[MAX_WORD_LENGTH];
    char hint[MAX_WORD_LENGTH];
};
 
void displayWord(const char word[], const bool guessed[]);
 
// Function to draw the hangman
void drawHangman(int tries);

int main(int argc, char const *argv[])
{

    return 0;
}
