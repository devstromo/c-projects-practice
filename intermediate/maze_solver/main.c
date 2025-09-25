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

// Position structure for coordinates
typedef struct
{
    int row;
    int col;
} Position;

// Queue structure for BFS
typedef struct
{
    Position items[MAX_QUEUE_SIZE];
    int front;
    int rear;
} Queue;

// Structure to store algorithm results for comparison
typedef struct
{
    bool path_found;
    int path_length;
    Position path[MAX_PATH];
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

bool enqueue(Queue *q, Position pos)
{
    if (q->rear >= MAX_QUEUE_SIZE)
        return false;
    q->items[q->rear] = pos;
    q->rear++;
    return true;
}

Position dequeue(Queue *q)
{
    Position p = q->items[q->front];
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
void copy_path(Position dest[MAX_PATH], Position src[MAX_PATH], int length)
{
    for (int i = 0; i < length; i++)
    {
        dest[i] = src[i];
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
int bfs(Position start, int rows, int cols, int matrix[100][100],
        int visited[100][100], Position path[10000], int *path_len,
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
    if (!enqueue(&q, start))
    {
        return -1; // Queue overflow
    }
    visited[start.row][start.col] = 1;

    int found_exit = 0;
    Position exit_pos = {-1, -1};

    while (!isQueueEmpty(&q))
    {
        Position current = dequeue(&q);
        int r = current.row;
        int c = current.col;

        // Check if we reached an exit
        if ((r == 0 || r == rows - 1 || c == 0 || c == cols - 1) &&
            (r != start.row || c != start.col))
        {
            found_exit = 1;
            exit_pos.row = r;
            exit_pos.col = c;
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
                Position new_pos = {new_r, new_c};
                if (!enqueue(&q, new_pos))
                {
                    return -1; // Queue overflow
                }
            }
        }
    }

    if (found_exit)
    {
        // Reconstruct path from exit to start
        Position curr = exit_pos;
        Position temp_path[10000];
        int temp_len = 0;

        while (curr.row != -1 && curr.col != -1)
        {
            temp_path[temp_len] = curr;
            temp_len++;

            Position next = {parent[curr.row][curr.col][0], parent[curr.row][curr.col][1]};
            curr = next;
        }

        // Reverse path to get start to exit
        *path_len = temp_len;
        for (int i = 0; i < temp_len; i++)
        {
            path[i] = temp_path[temp_len - 1 - i];
        }

        return 1;
    }

    return 0;
}

// Function to perform DFS and find path from start to exit
int dfs(Position pos, int rows, int cols, int matrix[100][100], int visited[100][100],
        Position path[10000], int *path_len, Position start)
{
    int r = pos.row, c = pos.col;
    // Check boundaries and walls
    if (!is_valid_position(r, c, rows, cols))
        return 0;
    if (matrix[r][c] == 1 || visited[r][c])
        return 0;

    // Mark current cell as visited
    visited[r][c] = 1;

    // Add current position to path
    path[*path_len] = pos;
    (*path_len)++;

    // If we reached a border cell different from start, we found an exit
    if ((r == 0 || r == rows - 1 || c == 0 || c == cols - 1) &&
        (r != start.row || c != start.col))
    {
        return 1;
    }

    for (int d = 0; d < 4; d++)
    {
        int new_r = r + DR[d];
        int new_c = c + DC[d];
        Position new_pos = {new_r, new_c};

        if (dfs(new_pos, rows, cols, matrix, visited, path, path_len, start))
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
    Position path[MAX_PATH];
    int path_len = 0;

    printf("\n=== ALGORITHM COMPARISON ===\n");
    printf("Testing both DFS and BFS from entrance (%d,%d)...\n\n", start_row, start_col);

    // Test DFS
    clear_visited(visited, rows, cols);
    path_len = 0;

    Position start_pos = {start_row, start_col};
    if (dfs(start_pos, rows, cols, matrix, visited, path, &path_len, start_pos))
    {
        dfs_result.path_found = true;
        dfs_result.path_length = path_len;
        copy_path(dfs_result.path, path, path_len);
    }

    // Test BFS
    clear_visited(visited, rows, cols);
    path_len = 0;
    int parent[MAX_SIZE][MAX_SIZE][2];

    int bfs_status = bfs(start_pos, rows, cols, matrix, visited, path, &path_len, parent);
    if (bfs_status == 1)
    {
        bfs_result.path_found = true;
        bfs_result.path_length = path_len;
        copy_path(bfs_result.path, path, path_len);
    }
    else if (bfs_status == -1)
    {
        fprintf(stderr, "Error: Queue overflow in BFS. Maze may be too large or have too many open cells.\n");
        return;
    }

    // Report results
    printf("DFS Results:\n");
    if (dfs_result.path_found)
    {
        printf("  ✓ Path found! Length: %d steps\n", dfs_result.path_length);
        printf("  Path: ");
        for (int i = 0; i < dfs_result.path_length; i++)
        {
            printf("(%d,%d)", dfs_result.path[i].row, dfs_result.path[i].col);
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
            printf("(%d,%d)", bfs_result.path[i].row, bfs_result.path[i].col);
            if (i < bfs_result.path_length - 1)
                printf(" -> ");
        }
        printf("\n");
    }
    else
    {
        printf("  ✗ No path found. The maze may be unsolvable or all exits are blocked.\n");
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
        printf("Only DFS found a path. BFS guarantees shortest path if one exists; check for implementation issues.\n");
    }
    else if (!dfs_result.path_found && bfs_result.path_found)
    {
        printf("Only BFS found a path. DFS may have limitations in certain mazes; verify the maze structure.\n");
    }
    else
    {
        printf("Neither algorithm found a path. The maze has no solution from the entrance to any exit.\n");
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
            fprintf(stderr, "Error: Could not read two integers for dimensions from file. Check file format (first line should be 'rows cols').\n");
            fclose(f);
            return 1;
        }
        if (rows < 1 || rows > 100 || cols < 1 || cols > 100)
        {
            fprintf(stderr, "Error: Dimensions %d x %d are out of range. Must be between 1 and 100.\n", rows, cols);
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
                    fprintf(stderr, "Error: Expected %d matrix values, but only %d found in file.\n", expected, count);
                    fclose(f);
                    return 1;
                }
                if (val != 0 && val != 1)
                {
                    fprintf(stderr, "Error: Invalid matrix value %d at position (%d,%d). Values must be 0 or 1.\n", val, i, j);
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
            fprintf(stderr, "Error: Too many matrix values in file. Expected exactly %d values.\n", expected);
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
            fprintf(stderr, "Error: Invalid rows value '%s'. Must be a valid integer.\n", argv[2]);
            return 1;
        }
        errno = 0;
        cols = strtol(argv[3], &endptr, 10);
        if (*endptr != '\0' || errno == ERANGE)
        {
            fprintf(stderr, "Error: Invalid cols value '%s'. Must be a valid integer.\n", argv[3]);
            return 1;
        }
        if (rows < 1 || rows > 100 || cols < 1 || cols > 100)
        {
            fprintf(stderr, "Error: Dimensions %d x %d are out of range. Must be between 1 and 100.\n", rows, cols);
            return 1;
        }

        int expected = rows * cols;
        if (argc != 4 + expected)
        {
            fprintf(stderr, "Error: Expected %d matrix values after dimensions, but got %d arguments.\n", expected, argc - 4);
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
                    fprintf(stderr, "Error: Invalid matrix value '%s' at position (%d,%d). Values must be 0 or 1.\n", argv[index], i, j);
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
        fprintf(stderr, "Error: No entrance found on maze boundary. Ensure at least one border cell is open (value 0).\n");
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
        Position path[10000];
        int path_len = 0;

        // Try to solve the maze
        Position start_pos = {start_row, start_col};
        if (use_dfs)
        {
            printf("Using DFS to find path from entrance (%d,%d)...\n", start_row, start_col);
            if (dfs(start_pos, rows, cols, matrix, visited, path, &path_len, start_pos))
            {
                printf("Path found! Length: %d steps\n", path_len);
                printf("Path from entrance to exit:\n");
                for (int i = 0; i < path_len; i++)
                {
                    printf("(%d,%d)", path[i].row, path[i].col);
                    if (i < path_len - 1)
                        printf(" -> ");
                }
                printf("\n");
            }
            else
            {
                printf("No path found from entrance to exit. The maze may be unsolvable or all exits are blocked.\n");
            }
        }
        else
        {
            // For BFS, we need a parent array to reconstruct the path
            int parent[100][100][2];
            printf("Using BFS to find shortest path from entrance (%d,%d)...\n", start_row, start_col);
            int bfs_status = bfs(start_pos, rows, cols, matrix, visited, path, &path_len, parent);
            if (bfs_status == 1)
            {
                printf("Shortest path found! Length: %d steps\n", path_len);
                printf("Path from entrance to exit:\n");
                for (int i = 0; i < path_len; i++)
                {
                    printf("(%d,%d)", path[i].row, path[i].col);
                    if (i < path_len - 1)
                        printf(" -> ");
                }
                printf("\n");
            }
            else if (bfs_status == -1)
            {
                fprintf(stderr, "Error: Queue overflow in BFS. Maze may be too large or have too many open cells.\n");
                return 1;
            }
            else
            {
                printf("No path found from entrance to exit. The maze may be unsolvable or all exits are blocked.\n");
            }
        }
    }

    return 0;
}