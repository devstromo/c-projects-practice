#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <errno.h>
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

// Structure to store algorithm results for comparison
typedef struct
{
    bool path_found;
    int path_length;
    int path[MAX_PATH][2];
    const char *algorithm_name;
} PathResult;

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

bool is_valid_position(int r, int c, int rows, int cols)
{
    return (r >= 0 && r < rows && c >= 0 && c < cols);
}

// Function to copy path for result storage
void copy_path(int dest[MAX_PATH][2], int src[MAX_PATH][2], int length)
{
    for (int i = 0; i < length; i++)
    {
        dest[i][0] = src[i][0];
        dest[i][1] = src[i][1];
    }
}

// Function to clear visited array
void clear_visited(int visited[MAX_SIZE][MAX_SIZE], int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            visited[i][j] = 0;
        }
    }
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
    if (!is_valid_position(r, c, rows, cols))
        return 0;
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

// Function to run both algorithms and compare results
void compare_algorithms(int start_row, int start_col, int rows, int cols)
{
    PathResult dfs_result = {false, 0, {0}, "DFS"};
    PathResult bfs_result = {false, 0, {0}, "BFS"};

    int visited[MAX_SIZE][MAX_SIZE] = {0};
    int path[MAX_PATH][2];
    int path_len = 0;

    printf("\n=== ALGORITHM COMPARISON ===\n");
    printf("Testing both DFS and BFS from entrance (%d,%d)...\n\n", start_row, start_col);

    // Test DFS
    clear_visited(visited, rows, cols);
    path_len = 0;

    if (dfs(start_row, start_col, rows, cols, matrix, visited, path, &path_len, start_row, start_col))
    {
        dfs_result.path_found = true;
        dfs_result.path_length = path_len;
        copy_path(dfs_result.path, path, path_len);
    }

    // Test BFS
    clear_visited(visited, rows, cols);
    path_len = 0;
    int parent[MAX_SIZE][MAX_SIZE][2];

    if (bfs(start_row, start_col, rows, cols, matrix, visited, path, &path_len, parent))
    {
        bfs_result.path_found = true;
        bfs_result.path_length = path_len;
        copy_path(bfs_result.path, path, path_len);
    }

    // Report results
    printf("DFS Results:\n");
    if (dfs_result.path_found)
    {
        printf("  ✓ Path found! Length: %d steps\n", dfs_result.path_length);
        printf("  Path: ");
        for (int i = 0; i < dfs_result.path_length; i++)
        {
            printf("(%d,%d)", dfs_result.path[i][0], dfs_result.path[i][1]);
            if (i < dfs_result.path_length - 1)
                printf(" -> ");
        }
        printf("\n");
    }
    else
    {
        printf("  ✗ No path found\n");
    }

    printf("\nBFS Results:\n");
    if (bfs_result.path_found)
    {
        printf("  ✓ Path found! Length: %d steps\n", bfs_result.path_length);
        printf("  Path: ");
        for (int i = 0; i < bfs_result.path_length; i++)
        {
            printf("(%d,%d)", bfs_result.path[i][0], bfs_result.path[i][1]);
            if (i < bfs_result.path_length - 1)
                printf(" -> ");
        }
        printf("\n");
    }
    else
    {
        printf("  ✗ No path found\n");
    }

    // Comparison analysis
    printf("\n=== COMPARISON ANALYSIS ===\n");
    if (dfs_result.path_found && bfs_result.path_found)
    {
        printf("Both algorithms found a path!\n");
        printf("DFS path length: %d steps\n", dfs_result.path_length);
        printf("BFS path length: %d steps\n", bfs_result.path_length);

        if (bfs_result.path_length < dfs_result.path_length)
        {
            printf("BFS found a shorter path by %d steps (BFS guarantees shortest path)\n",
                   dfs_result.path_length - bfs_result.path_length);
        }
        else if (dfs_result.path_length < bfs_result.path_length)
        {
            printf("DFS found a shorter path by %d steps (unusual - check implementation)\n",
                   bfs_result.path_length - dfs_result.path_length);
        }
        else
        {
            printf("Both algorithms found paths of equal length!\n");
        }
    }
    else if (dfs_result.path_found && !bfs_result.path_found)
    {
        printf("Only DFS found a path (implementation error in BFS?)\n");
    }
    else if (!dfs_result.path_found && bfs_result.path_found)
    {
        printf("Only BFS found a path (implementation error in DFS?)\n");
    }
    else
    {
        printf("Neither algorithm found a path - maze has no solution\n");
    }
}

int main(int argc, char *argv[])
{
    int rows, cols;
    bool use_dfs = true;
    bool compare_mode = false;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s --file <filename> [--bfs] [--compare] or %s --input <rows> <cols> <matrix_values...> [--bfs] [--compare]\n", argv[0], argv[0]);
        return 1;
    }

    // Check for compare flag
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--compare") == 0)
        {
            compare_mode = true;
            break;
        }
    }

    if (strcmp(argv[1], "--file") == 0)
    {
        if (argc < 3)
        {
            fprintf(stderr, "Error: --file requires a filename\n");
            return 1;
        }
        printf("Processing input from file...\n");
        FILE *f = fopen(argv[2], "r");
        if (!f)
        {
            perror("Failed to open file");
            return 1;
        }

        int read_dims = fscanf(f, "%d %d", &rows, &cols);
        if (read_dims != 2)
        {
            fprintf(stderr, "Error: Failed to read dimensions from file\n");
            fclose(f);
            return 1;
        }
        if (rows < 1 || rows > 100 || cols < 1 || cols > 100)
        {
            fprintf(stderr, "Error: Dimensions must be between 1 and 100\n");
            fclose(f);
            return 1;
        }

        int expected = rows * cols;
        int count = 0;
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                int val;
                if (fscanf(f, "%d", &val) != 1)
                {
                    fprintf(stderr, "Error: Not enough matrix values in file\n");
                    fclose(f);
                    return 1;
                }
                if (val != 0 && val != 1)
                {
                    fprintf(stderr, "Error: Matrix values must be 0 or 1\n");
                    fclose(f);
                    return 1;
                }
                matrix[i][j] = val;
                count++;
            }
        }
        // Check for extra values
        int extra;
        if (fscanf(f, "%d", &extra) == 1)
        {
            fprintf(stderr, "Error: Too many matrix values in file\n");
            fclose(f);
            return 1;
        }

        if (!compare_mode)
        {
            use_dfs = !parse_algorithm_flag(argc, argv, 3);
        }
        fclose(f);
    }
    else if (strcmp(argv[1], "--input") == 0)
    {
        if (argc < 4)
        {
            fprintf(stderr, "Error: --input requires at least rows and cols\n");
            return 1;
        }
        printf("Processing input from command line arguments...\n");

        char *endptr;
        errno = 0;
        rows = strtol(argv[2], &endptr, 10);
        if (*endptr != '\0' || errno == ERANGE)
        {
            fprintf(stderr, "Error: Invalid rows value\n");
            return 1;
        }
        errno = 0;
        cols = strtol(argv[3], &endptr, 10);
        if (*endptr != '\0' || errno == ERANGE)
        {
            fprintf(stderr, "Error: Invalid cols value\n");
            return 1;
        }
        if (rows < 1 || rows > 100 || cols < 1 || cols > 100)
        {
            fprintf(stderr, "Error: Dimensions must be between 1 and 100\n");
            return 1;
        }

        int expected = rows * cols;
        if (argc != 4 + expected)
        {
            fprintf(stderr, "Error: Incorrect number of matrix values provided\n");
            return 1;
        }

        int index = 4;
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                errno = 0;
                int val = strtol(argv[index], &endptr, 10);
                if (*endptr != '\0' || errno == ERANGE || (val != 0 && val != 1))
                {
                    fprintf(stderr, "Error: Matrix values must be 0 or 1\n");
                    return 1;
                }
                matrix[i][j] = val;
                index++;
            }
        }

        printf("Matrix\n");
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                printf("%d%c", matrix[i][j], (j == cols - 1) ? '\n' : ' ');

        if (!compare_mode)
        {
            use_dfs = !parse_algorithm_flag(argc, argv, 4 + rows * cols);
        }
    }
    else
    {
        fprintf(stderr, "Usage: %s --file <filename> [--bfs] [--compare] or %s --input <rows> <cols> <matrix_values...> [--bfs] [--compare]\n", argv[0], argv[0]);
        return 1;
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

    // Run comparison mode or single algorithm
    if (compare_mode)
    {
        compare_algorithms(start_row, start_col, rows, cols);
    }
    else
    {
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
                printf("Path found! Length: %d steps\n", path_len);
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
                printf("Shortest path found! Length: %d steps\n", path_len);
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
    }

    return 0;
}