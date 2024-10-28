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
            wordList[count].word[MAX_WORD_LENGTH - 1] = '\0';  // Null-terminate

            strncpy(wordList[count].hint, hint, MAX_WORD_LENGTH - 1);
            wordList[count].hint[MAX_WORD_LENGTH - 1] = '\0';  // Null-terminate

            count++;
        }
    }

    fclose(file);
    return count;
}

int main(int argc, char const *argv[])
{
    struct WordHint wordList[MAX_WORDS];
    const int wordCount = loadWordsFromFile("./words.csv", wordList, MAX_WORDS);

    for (int i = 0; i < wordCount; i++) {
        printf("Word: %s, Hint: %s\n", wordList[i].word, wordList[i].hint);
    }

    return 0;
}
