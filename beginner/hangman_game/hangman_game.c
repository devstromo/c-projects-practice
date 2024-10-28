#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORD_LENGTH 100
#define MAX_WORDS 100
#define MAX_TRIES 6

struct WordHint {
    char word[MAX_WORD_LENGTH];
    char hint[MAX_WORD_LENGTH];
};

void displayWord(const char word[], const bool guessed[]);

void drawHangman(int tries);

int loadWordsFromFile(const char *filename, struct WordHint wordList[], const int maxWords) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Unable to open file");
        return 0;
    }

    int count = 0;
    char line[2 * MAX_WORD_LENGTH];

    while (fgets(line, sizeof(line), file) && count < maxWords) {
        line[strcspn(line, "\n")] = '\0';

        const char *word = strtok(line, ",");
        const char *hint = strtok(NULL, ",");

        if (word && hint) {
            strncpy(wordList[count].word, word, MAX_WORD_LENGTH - 1);
            wordList[count].word[MAX_WORD_LENGTH - 1] = '\0'; // Null-terminate

            strncpy(wordList[count].hint, hint, MAX_WORD_LENGTH - 1);
            wordList[count].hint[MAX_WORD_LENGTH - 1] = '\0'; // Null-terminate

            count++;
        }
    }

    fclose(file);
    return count;
}

int main(void) {
    struct WordHint wordList[MAX_WORDS];
    const int wordCount = loadWordsFromFile("./words.csv", wordList, MAX_WORDS);

    srand(time(NULL));
    const int wordIndex = rand() % 4;

    const char *secretWord = wordList[wordIndex].word;
    const char *hint = wordList[wordIndex].hint;

    int wordLength = strlen(secretWord);
    char guessedWord[MAX_WORD_LENGTH] = {0};
    bool guessedLetters[26] = {false};

    printf("Welcome to Hangman!\n");
    printf("Hint: %s\n", hint);

    int tries = 0;

    while (tries < MAX_TRIES) {
        printf("\n");
        displayWord(guessedWord, guessedLetters);
        drawHangman(tries);

        char guess;
        printf("Enter a letter: ");
        scanf(" %c", &guess);
        guess = tolower(guess);

        if (guessedLetters[guess - 'a']) {
            printf("You've already guessed that letter. "
                "Try again.\n");
            continue;
        }

        guessedLetters[guess - 'a'] = true;

        bool found = false;
        for (int i = 0; i < wordLength; i++) {
            if (secretWord[i] == guess) {
                found = true;
                guessedWord[i] = guess;
            }
        }

        if (found) {
            printf("Good guess!\n");
        } else {
            printf("Sorry, the letter '%c' is not in the "
                   "word.\n",
                   guess);
            tries++;
        }

        if (strcmp(secretWord, guessedWord) == 0) {
            printf("\nCongratulations! You've guessed the "
                   "word: %s\n",
                   secretWord);
            break;
        }
    }

    if (tries >= MAX_TRIES) {
        printf("\nSorry, you've run out of tries. The word "
               "was: %s\n",
               secretWord);
    }
    return 0;
}

void displayWord(const char word[], const bool guessed[]) {
    printf("Word: ");
    for (int i = 0; word[i] != '\0'; i++) {
        if (guessed[word[i] - 'a']) {
            printf("%c ", word[i]);
        } else {
            printf("_ ");
        }
    }
    printf("\n");
}

void drawHangman(const int tries) {
    const char *hangmanParts[]
            = {
                "     _________", "    |         |",
                "    |         O", "    |        /|\\",
                "    |        / \\", "    |"
            };

    printf("\n");
    for (int i = 0; i <= tries; i++) {
        printf("%s\n", hangmanParts[i]);
    }
}
