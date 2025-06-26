#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LEN 1024

typedef struct
{
    char title[100];
    char author[100];
    int year;
    char url[200];
} Book;

void initBookDB()
{
    FILE *file = fopen("books.csv", "r");
    if (file == NULL)
    {
        file = fopen("books.csv", "w");
        fprintf(file, "Title,Author,Year,URL\n");
        fclose(file);
    }
    else
    {
        fclose(file);
    }
}

FILE *open_file(const char *path, const char *mode) {
    FILE *file = fopen(path, mode);
    if (file == NULL) {
        printf("Error: Cannot open file '%s' with mode '%s'\n", path, mode);
    }
    return file;
}

int is_web_url(const char *input) {
    return strncmp(input, "http://", 7) == 0 || strncmp(input, "https://", 8) == 0;
}

// Normalize file:// URI into local path
void normalize_path(const char *input, char *out) {
#ifdef _WIN32
    // Windows: file:///C:/Users/... -> C:\Users\...
    if (strncmp(input, "file:///", 8) == 0) {
        strcpy(out, input + 8);
        for (char *p = out; *p; ++p)
            if (*p == '/')
                *p = '\\';
    } else {
        strcpy(out, input);
    }
#else
    // Linux/macOS: file:///home/user/... -> /home/user/...
    if (strncmp(input, "file://", 7) == 0) {
        strcpy(out, input + 7);
    } else {
        strcpy(out, input);
    }
#endif
}

void addBook()
{
    // This function will handle adding a new book to the library.
    // It will prompt the user for book details and store them in the library's collection.
    printf("Adding a new book...\n");
    // Implementation goes here
}

int main()
{
    initBookDB();
    int choice;
    printf("Library Management System\n");

    do
    {
        printf("Welcome to the Library Management System!\n");
        printf("Please select an option:\n");
        printf("1. Add a new book\n");
        printf("2. Remove a book\n");
        printf("3. Search for a book\n");
        printf("4. View all books\n");
        printf("5. Exit\n");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            addBook();
            break;
        case 2:
            // Code to remove a book
            break;
        case 3:
            // Code to search for a book
            break;
        case 4:
            // Code to view all books
            break;
        case 5:
            printf("Exiting the Library Management System. Goodbye!\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 5);

    return 0;
}