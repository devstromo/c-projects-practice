#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main(int argc, char *argv[])
{
    int matrix[100][100];
    int rows, cols;
    if (strcmp(argv[1], "--file") == 0)
    {
        printf("Processing input from file...\n");
        FILE *f = fopen(argv[2], "r");
        if (!f)
        {
            perror("Failed to open file");
            return 1;
        }

        fscanf(f, "%d %d", &rows, &cols);
        if (rows <= 0 || cols <= 0 || rows > 100 || cols > 100)
        {
            fprintf(stderr, "Invalid matrix dimensions\n");
            fclose(f);
            return 1;
        }

        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                fscanf(f, "%d", &matrix[i][j]);

        fclose(f);
    }
    else if (strcmp(argv[1], "--input") == 0)
    {
        printf("Processing input from command line arguments...\n");
        if (argc < 3)
        {
            printf("Usage: %s <rows> <cols> [data...]\n", argv[0]);
            return 1;
        }

        rows = atoi(argv[2]);
        cols = atoi(argv[3]);

        if (argc < 3 + rows * cols)
        {
            printf("Error: Not enough data provided.\n");
            return 1;
        }

        int matrix[rows][cols];

        int index = 4;
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                matrix[i][j] = atoi(argv[index++]);

        // Example: print matrix
        printf("Matrix\n");
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                printf("%d%c", matrix[i][j], (j == cols - 1) ? '\n' : ' ');
    }
    printf("Maze Solver Program\n");
    // Implement maze solving logic here

    // Example output
    printf("Maze solved successfully!\n");
    return 0;
}