#include <stdio.h>

typedef enum
{
    EMPTY,
    PLAYER_X,
    PLAYER_O
} Cell;

typedef struct
{
    Cell cells[3][3];
} Board;

void printBoard(Board *board);
void makeMove(Board *board, int row, int col, int player);
int checkWin(Board *board, int player);

int main()
{
    Board board;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            board.cells[i][j] = EMPTY;
        }
    }
    printf("Welcome to Simple Tic Tac Toe Game!\n");
    printf("Player 1 (X) - Player 2 (O)\n\n");
    printf("Initial board\n");
    printBoard(&board);
    int currentPlayer = PLAYER_X;
    int moves = 0;
    while (moves < 9)
    {
        int row, col;
        printf("Player %d, enter your move (row and column): ", currentPlayer == PLAYER_X ? 1 : 2);
        if (scanf("%d %d", &row, &col) != 2) {
            printf("Invalid input. Please enter two numbers.\n");
            while (getchar() != '\n');
            continue;
        }
        if (row < 1 || row > 3 || col < 1 || col > 3 || board.cells[row - 1][col - 1] != EMPTY) {
            printf("Invalid move. Try again.\n");
            continue;
        }
        makeMove(&board, row - 1, col - 1, currentPlayer);
        printBoard(&board);
        if (checkWin(&board, currentPlayer))
        {
            printf("Player %d wins!\n", currentPlayer == PLAYER_X ? 1 : 2);
            return 0;
        }
        currentPlayer = (currentPlayer == PLAYER_X) ? PLAYER_O : PLAYER_X;
        moves++;
    }
    printf("It's a draw!\n");
    return 0;
}

void printBoard(Board *board)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            char symbol = ' ';
            if (board->cells[i][j] == PLAYER_X)
                symbol = 'X';
            else if (board->cells[i][j] == PLAYER_O)
                symbol = 'O';

            printf(" %c ", symbol);
            if (j < 2)
                printf("|");
        }
        printf("\n");
        if (i < 2)
            printf("-----------\n");
    }
}

void makeMove(Board *board, int row, int col, int player)
{
    if (row < 0 || row >= 3 || col < 0 || col >= 3 || board->cells[row][col] != EMPTY)
    {
        printf("Invalid move. Try again.\n");
        return;
    }
    board->cells[row][col] = player;
}

int checkWin(Board *board, int player)
{
    for (int i = 0; i < 3; i++)
    {
        if ((board->cells[i][0] == player && board->cells[i][1] == player && board->cells[i][2] == player) ||
            (board->cells[0][i] == player && board->cells[1][i] == player && board->cells[2][i] == player))
        {
            return 1;
        }
    }
    if ((board->cells[0][0] == player && board->cells[1][1] == player && board->cells[2][2] == player) ||
        (board->cells[0][2] == player && board->cells[1][1] == player && board->cells[2][0] == player))
    {
        return 1;
    }
    return 0;
}