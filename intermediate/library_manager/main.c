#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_PATH_LEN 1024

typedef struct
{
    char title[100];
    char author[100];
    int year;
    char url[MAX_PATH_LEN];
} Book;

void ensure_bookstore_folder()
{
#ifdef _WIN32
    mkdir("bookstore");
#else
    mkdir("bookstore", 0755);
#endif
}

void copy_file(const char *source, const char *destination)
{
#ifdef _WIN32
    char command[MAX_PATH_LEN * 2];
    snprintf(command, sizeof(command), "copy \"%s\" \"%s\"", source, destination);
#else
    char command[MAX_PATH_LEN * 2];
    snprintf(command, sizeof(command), "cp \"%s\" \"%s\"", source, destination);
#endif
    printf("Copying from %s to %s...\n", source, destination);
    system(command);
}

void download_pdf(const char *url, const char *destination_path)
{
    char command[MAX_PATH_LEN * 2];
    snprintf(command, sizeof(command), "curl -L \"%s\" -o \"%s\"", url, destination_path);
    printf("Downloading from %s to %s...\n", url, destination_path);
    system(command);
}

int ends_with_pdf(const char *filename)
{
    size_t len = strlen(filename);
    return len >= 4 && strcmp(filename + len - 4, ".pdf") == 0;
}

void to_lowercase(char *str)
{
    for (; *str; ++str)
        *str = tolower(*str);
}

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

    printf("Enter book URL (or file path) [optional]: ");
    char url[MAX_PATH_LEN];
    fgets(url, sizeof(url), stdin); // flush leftover newline first
    fgets(url, sizeof(url), stdin); // now get the actual URL input

    // Remove trailing newline
    url[strcspn(url, "\n")] = '\0';

    if (strlen(url) > 0)
    {
        normalize_path(url, newBook.url, sizeof(newBook.url));
    }
    else
    {
        newBook.url[0] = '\0'; // set to empty string if user skipped input
    }

    if (strlen(newBook.url) > 0 && ends_with_pdf(newBook.url))
    {
        ensure_bookstore_folder();

        char dest_path[MAX_PATH_LEN];
        snprintf(dest_path, sizeof(dest_path), "bookstore/%s_%d.pdf", newBook.title, newBook.year);

        if (is_web_url(newBook.url))
        {
            download_pdf(newBook.url, dest_path);
        }
        else
        {
            copy_file(newBook.url, dest_path);
        }
    }

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

void viewBooks()
{
    printf("Viewing all books...\n");
    FILE *file = open_file("books.csv", "r");
    if (file == NULL)
    {
        printf("No books found (file missing).\n");
        return;
    }

    char line[256];
    int anyBooks = 0;

    while (fgets(line, sizeof(line), file))
    {
        if (anyBooks == 0 && strncmp(line, "Title", 5) == 0)
        {
            anyBooks = 1; // Skip header line
            continue;
        }
        printf("%s", line);
        anyBooks++;
    }

    if (anyBooks == 1)
    {
        printf("\n\nNo books found.\n\n");
    }
    fclose(file);
}

void searchBook()
{
    printf("Searching for a book...\n");
    char searchTerm[100];
    printf("Enter book title or author to search: ");
    scanf(" %[^\n]", searchTerm);

    FILE *file = open_file("books.csv", "r");
    if (file == NULL)
    {
        printf("No books found (file missing).\n");
        return;
    }

    char line[256];
    int found = 0;
    int anyBooks = 0;
    char searchTermLower[100];
    strcpy(searchTermLower, searchTerm);
    to_lowercase(searchTermLower);

    while (fgets(line, sizeof(line), file))
    {
        if (anyBooks == 0 && strncmp(line, "Title", 5) == 0)
        {
            anyBooks = 1;
            continue;
        }
        char lineCopy[256];
        strcpy(lineCopy, line);
        to_lowercase(lineCopy);

        if (strstr(lineCopy, searchTermLower) != NULL)
        {
            printf("%s", line);
            found = 1;
        }
        anyBooks++;
    }

    if (anyBooks == 1)
    {
        printf("\n\nNo books found.\n\n");
    }
    if (!found)
    {
        printf("No books found matching '%s'.\n", searchTerm);
    }

    fclose(file);
}

void removeBook()
{
    printf("Removing a book...\n");
    char searchTerm[100];
    printf("Enter book title or author to search: ");
    scanf(" %[^\n]", searchTerm);

    FILE *file = open_file("books.csv", "r");
    if (file == NULL)
    {
        printf("No books found (file missing).\n");
        return;
    }

    FILE *temp = open_file("temp_books.csv", "w");
    if (temp == NULL)
    {
        fclose(file);
        return;
    }

    char line[256];
    int found = 0;
    int isHeader = 1;

    char searchTermLower[100];
    strcpy(searchTermLower, searchTerm);
    to_lowercase(searchTermLower);

    while (fgets(line, sizeof(line), file))
    {
        if (isHeader)
        {
            fprintf(temp, "%s", line); // copy header as is
            isHeader = 0;
            continue;
        }

        char lineCopy[256];
        strcpy(lineCopy, line);
        to_lowercase(lineCopy);

        if (strstr(lineCopy, searchTermLower) != NULL)
        {
            printf("Deleted book: %s", line);
            found = 1;
            continue; // skip writing this line to temp file
        }

        fprintf(temp, "%s", line); // copy other lines
    }

    fclose(file);
    fclose(temp);

    if (!found)
    {
        printf("No books found matching '%s'.\n", searchTerm);
        remove("temp_books.csv"); // clean up temp file
    }
    else
    {
        if (remove("books.csv") != 0)
        {
            perror("Error deleting original file");
        }
        else if (rename("temp_books.csv", "books.csv") != 0)
        {
            perror("Error renaming temp file");
        }
        else
        {
            printf("Book removed successfully.\n");
        }
    }
}

void updateBookData()
{
    printf("Updating book data...\n");

    printf("Enter book title or author to update: ");
    char searchTerm[100];
    scanf(" %[^\n]", searchTerm);

    FILE *file = open_file("books.csv", "r");
    if (file == NULL)
    {
        printf("No books found (file missing).\n");
        return;
    }

    FILE *temp = open_file("temp_books.csv", "w");
    if (temp == NULL)
    {
        fclose(file);
        return;
    }

    char line[256];
    int isHeader = 1;
    int found = 0;

    while (fgets(line, sizeof(line), file))
    {
        if (isHeader)
        {
            fprintf(temp, "%s", line); // copy header as is
            isHeader = 0;
            continue;
        }

        char title[100], author[100], url[MAX_PATH_LEN];
        int year;

        // Parse the line
        sscanf(line, "\"%[^\"]\",\"%[^\"]\",%d,\"%[^\"]\"", title, author, &year, url);

        // Check if line matches search term (case-insensitive)
        char titleLower[100], authorLower[100], searchTermLower[100];
        strcpy(titleLower, title);
        strcpy(authorLower, author);
        strcpy(searchTermLower, searchTerm);
        to_lowercase(titleLower);
        to_lowercase(authorLower);
        to_lowercase(searchTermLower);

        if (strstr(titleLower, searchTermLower) != NULL || strstr(authorLower, searchTermLower) != NULL)
        {
            found = 1;
            printf("Current data:\nTitle: %s\nAuthor: %s\nYear: %d\nURL: %s\n", title, author, year, url);

            // Clear stdin leftover
            getchar();

            printf("Enter new title (or press Enter to keep current): ");
            char newTitle[100];
            fgets(newTitle, sizeof(newTitle), stdin);
            newTitle[strcspn(newTitle, "\n")] = 0; // Remove trailing newline
            if (strlen(newTitle) > 0)
                strcpy(title, newTitle);

            printf("Enter new author (or press Enter to keep current): ");
            char newAuthor[100];
            fgets(newAuthor, sizeof(newAuthor), stdin);
            newAuthor[strcspn(newAuthor, "\n")] = 0;
            if (strlen(newAuthor) > 0)
                strcpy(author, newAuthor);

            printf("Enter new year (or press Enter to keep current): ");
            char yearInput[10];
            fgets(yearInput, sizeof(yearInput), stdin);
            if (strlen(yearInput) > 1)
                sscanf(yearInput, "%d", &year);

            printf("Enter new URL (or press Enter to keep current): ");
            char newUrl[MAX_PATH_LEN];
            fgets(newUrl, sizeof(newUrl), stdin);
            newUrl[strcspn(newUrl, "\n")] = 0;
            if (strlen(newUrl) > 0)
                normalize_path(newUrl, url, sizeof(url));

            if (strlen(url) > 0 && ends_with_pdf(url))
            {
                ensure_bookstore_folder();

                char dest_path[MAX_PATH_LEN];
                snprintf(dest_path, sizeof(dest_path), "bookstore/%s_%d.pdf", title, year);

                if (is_web_url(url))
                {
                    download_pdf(url, dest_path);
                }
                else
                {
                    copy_file(url, dest_path);
                }
            }
        }

        // Write the (possibly updated) line to temp file
        fprintf(temp, "\"%s\",\"%s\",%d,\"%s\"\n", title, author, year, url);
    }

    fclose(file);
    fclose(temp);

    if (!found)
    {
        printf("No book found matching '%s'.\n", searchTerm);
        remove("temp_books.csv");
        return;
    }

    // Replace original file with updated temp file
    if (remove("books.csv") != 0)
    {
        perror("Error deleting original file");
    }
    else if (rename("temp_books.csv", "books.csv") != 0)
    {
        perror("Error renaming temp file");
    }
    else
    {
        printf("Book data updated successfully.\n");
    }

    printf("Returning to main menu...\n");
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
        printf("5. Update book data\n");
        printf("6. Exit\n");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            addBook();
            break;
        case 2:
            removeBook();
            break;
        case 3:
            searchBook();
            break;
        case 4:
            viewBooks();
            break;
        case 5:
            updateBookData();
            break;
        case 6:
            printf("Exiting the Library Management System. Goodbye!\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 6);

    return 0;
}