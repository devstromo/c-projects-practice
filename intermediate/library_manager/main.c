#include <stdio.h>

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