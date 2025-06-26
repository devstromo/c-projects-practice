#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LEN 1024

typedef struct
{
    char title[100];
    char author[100];
    int year;
    char url[MAX_PATH_LEN];
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

FILE *open_file(const char *path, const char *mode)
{
    FILE *file = fopen(path, mode);
    if (file == NULL)
    {
        printf("Error: Cannot open file '%s' with mode '%s'\n", path, mode);
    }
    return file;
}

int is_web_url(const char *input)
{
    return strncmp(input, "http://", 7) == 0 || strncmp(input, "https://", 8) == 0;
}

// Normalize file:// URI into local path
void normalize_path(const char *input, char *output, size_t max_len)
{
    if (strncmp(input, "file:///", 8) == 0)
    {
        // Windows-style path
        strncpy(output, input + 8, max_len - 1);
#ifdef _WIN32
        // Convert forward slashes to backslashes
        for (char *p = output; *p; ++p)
            if (*p == '/')
                *p = '\\';
#endif
    }
    else if (strncmp(input, "file://", 7) == 0)
    {
        // Unix-style path
        strncpy(output, input + 7, max_len - 1);
    }
    else
    {
        // Leave other URLs or paths as-is
        strncpy(output, input, max_len - 1);
    }
    output[max_len - 1] = '\0'; // Always null-terminate
}

void addBook()
{
    printf("Adding a new book...\n");
    Book newBook;

    printf("Enter book title: ");
    scanf(" %[^\n]", newBook.title);
    printf("Enter book author: ");
    scanf(" %[^\n]", newBook.author);
    printf("Enter book year: ");
    if (scanf("%d", &newBook.year) != 1)
    {
        printf("Invalid year input.\n");
        return;
    }

    printf("Enter book URL (or file path): ");
    char url[MAX_PATH_LEN];
    scanf(" %[^\n]", url);

    normalize_path(url, newBook.url, sizeof(newBook.url));

    FILE *file = open_file("books.csv", "a");
    if (file != NULL)
    {
        fprintf(file, "\"%s\",\"%s\",%d,\"%s\"\n",
                newBook.title,
                newBook.author,
                newBook.year,
                newBook.url);
        fclose(file);
        printf("Book saved to database.\n");
    }

    printf("\nBook added:\nTitle: %s\nAuthor: %s\nYear: %d\nURL: %s\n",
           newBook.title, newBook.author, newBook.year, newBook.url);
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