#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stdbool.h>

#define MAX_QUEUE_SIZE 10000
#define MAX_SIZE 100
#define MAX_PATH 10000

// Fix the matrix declaration issue
int matrix[MAX_SIZE][MAX_SIZE]; // Use consistently
// Direction arrays for both DFS and BFS
static const int DR[] = {-1, 0, 1, 0};
static const int DC[] = {0, 1, 0, -1};

// Queue structure for BFS
typedef struct
{
    int row;
    int col;
} Point;

typedef struct
{
    Point items[MAX_QUEUE_SIZE];
    int front;
    int rear;
} Queue;

// Queue operations
void initQueue(Queue *q)
{
    q->front = q->rear = 0;
}

bool isQueueEmpty(Queue *q)
{
    return q->front == q->rear;
}

bool enqueue(Queue *q, int row, int col)
{
    if (q->rear >= MAX_QUEUE_SIZE)
        return false;
    q->items[q->rear].row = row;
    q->items[q->rear].col = col;
    q->rear++;
    return true;
}

Point dequeue(Queue *q)
{
    Point p = q->items[q->front];
    q->front++;
    return p;
}

bool parse_algorithm_flag(int argc, char *argv[], int start_index)
{
    return (start_index < argc && strcmp(argv[start_index], "--bfs") == 0);
}

bool is_valid_position(int r, int c, int rows, int cols) {
    return (r >= 0 && r < rows && c >= 0 && c < cols);
}

// BFS function to find shortest path
int bfs(int start_row, int start_col, int rows, int cols, int matrix[100][100],
        int visited[100][100], int path[10000][2], int *path_len,
        int parent[100][100][2])
{
    Queue q;
    initQueue(&q);

    // Initialize visited and parent arrays
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            parent[i][j][0] = parent[i][j][1] = -1;
        }
    }
    // Start BFS
    enqueue(&q, start_row, start_col);
    visited[start_row][start_col] = 1;

    int found_exit = 0;
    int exit_row = -1, exit_col = -1;

    while (!isQueueEmpty(&q))
    {
        Point current = dequeue(&q);
        int r = current.row;
        int c = current.col;

        // Check if we reached an exit
        if ((r == 0 || r == rows - 1 || c == 0 || c == cols - 1) &&
            (r != start_row || c != start_col))
        {
            found_exit = 1;
            exit_row = r;
            exit_col = c;
            break;
        }

        // Try all four directions
        for (int d = 0; d < 4; d++)
        {
            int new_r = r + DR[d];
            int new_c = c + DC[d];

            if (is_valid_position(new_r, new_c, rows, cols) &&
                matrix[new_r][new_c] == 0 && !visited[new_r][new_c])
            {
                visited[new_r][new_c] = 1;
                parent[new_r][new_c][0] = r;
                parent[new_r][new_c][1] = c;
                enqueue(&q, new_r, new_c);
            }
        }
    }

    if (found_exit)
    {
        // Reconstruct path from exit to start
        int curr_r = exit_row;
        int curr_c = exit_col;
        int temp_path[10000][2];
        int temp_len = 0;

        while (curr_r != -1 && curr_c != -1)
        {
            temp_path[temp_len][0] = curr_r;
            temp_path[temp_len][1] = curr_c;
            temp_len++;

            int next_r = parent[curr_r][curr_c][0];
            int next_c = parent[curr_r][curr_c][1];
            curr_r = next_r;
            curr_c = next_c;
        }

        // Reverse path to get start to exit
        *path_len = temp_len;
        for (int i = 0; i < temp_len; i++)
        {
            path[i][0] = temp_path[temp_len - 1 - i][0];
            path[i][1] = temp_path[temp_len - 1 - i][1];
        }

        return 1;
    }

    return 0;
}

// Function to perform DFS and find path from start to exit
int dfs(int r, int c, int rows, int cols, int matrix[100][100], int visited[100][100],
        int path[10000][2], int *path_len, int start_row, int start_col)
{
    // Check boundaries and walls
    if (!is_valid_position(r, c, rows, cols)) return 0;
    if (matrix[r][c] == 1 || visited[r][c])
        return 0;

    // Mark current cell as visited
    visited[r][c] = 1;

    // Add current position to path
    path[*path_len][0] = r;
    path[*path_len][1] = c;
    (*path_len)++;

    // If we reached a border cell different from start, we found an exit
    if ((r == 0 || r == rows - 1 || c == 0 || c == cols - 1) &&
        (r != start_row || c != start_col))
    {
        return 1;
    }

    for (int d = 0; d < 4; d++)
    {
        int new_r = r + DR[d];
        int new_c = c + DC[d];

        if (dfs(new_r, new_c, rows, cols, matrix, visited, path, path_len, start_row, start_col))
        {
            return 1;
        }
    }

    // If no path is found, backtrack
    (*path_len)--;
    return 0;
}

int main(int argc, char *argv[])
{
    int rows, cols;
    bool use_dfs = true;
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

        if (strcmp(argv[3], "--bfs") == 0)
        {
            use_dfs = false;
        }
        else
        {
            use_dfs = true;
        }
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

        printf("Matrix\n");
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                printf("%d%c", matrix[i][j], (j == cols - 1) ? '\n' : ' ');
        if (strcmp(argv[4], "--bfs") == 0)
        {
            use_dfs = false; // Placeholder for other algorithms
        }
        else
        {
            use_dfs = true;
        }
    }
    printf("Maze Solver Program\n");

    // Find entrance (first open cell on border)
    int start_row = -1, start_col = -1;

    // Check first and last column
    for (int i = 0; i < rows && start_row == -1; i++)
    {
        if (matrix[i][0] == 0)
        {
            start_row = i;
            start_col = 0;
        }
        else if (matrix[i][cols - 1] == 0)
        {
            start_row = i;
            start_col = cols - 1;
        }
    }

    // Check first and last row if no entrance found
    if (start_row == -1)
    {
        for (int j = 0; j < cols && start_row == -1; j++)
        {
            if (matrix[0][j] == 0)
            {
                start_row = 0;
                start_col = j;
            }
            else if (matrix[rows - 1][j] == 0)
            {
                start_row = rows - 1;
                start_col = j;
            }
        }
    }

    if (start_row == -1)
    {
        printf("No entrance found on maze boundary!\n");
        return 1;
    }

    // Initialize visited array and path
    int visited[100][100] = {0};
    int path[10000][2];
    int path_len = 0;

    // Try to solve the maze
    if (use_dfs)
    {
        printf("Using DFS to find path from entrance (%d,%d)...\n", start_row, start_col);
        if (dfs(start_row, start_col, rows, cols, matrix, visited, path, &path_len, start_row, start_col))
        {
            printf("Path found!\n");
            printf("Path from entrance to exit:\n");
            for (int i = 0; i < path_len; i++)
            {
                printf("(%d,%d)", path[i][0], path[i][1]);
                if (i < path_len - 1)
                    printf(" -> ");
            }
            printf("\n");
        }
        else
        {
            printf("No path found from entrance to exit!\n");
        }
    }
    else
    {
        // For BFS, we need a parent array to reconstruct the path
        int parent[100][100][2];
        printf("Using BFS to find shortest path from entrance (%d,%d)...\n", start_row, start_col);
        if (bfs(start_row, start_col, rows, cols, matrix, visited, path, &path_len, parent))
        {
            printf("Shortest path found!\n");
            printf("Path from entrance to exit:\n");
            for (int i = 0; i < path_len; i++)
            {
                printf("(%d,%d)", path[i][0], path[i][1]);
                if (i < path_len - 1)
                    printf(" -> ");
            }
            printf("\n");
        }
        else
        {
            printf("No path found from entrance to exit!\n");
        }
    }

    return 0;
}