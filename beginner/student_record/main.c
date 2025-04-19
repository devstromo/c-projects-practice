#include <stdio.h>


void initSequenceFile()
{
    FILE *idFile = fopen("last_id.sequence", "w");
    if (idFile != NULL)
    {
        fprintf(idFile, "%d", 0); // Initialize with 0
        fclose(idFile);
    }
}

int getLastId()
{
    FILE *idFile = fopen("last_id.sequence", "r");
    int id = 0;
    if (idFile != NULL)
    {
        fscanf(idFile, "%d", &id);
        fclose(idFile);
    }
    return id;
}

void saveLastId(int id) {
    FILE *idFile = fopen("last_id.txt", "w");
    if (idFile != NULL) {
        fprintf(idFile, "%d", id);
        fclose(idFile);
    }
}

int main()
{
    int n;
    FILE *record = fopen("student_record.record", "a+");
    if (record == NULL)
    {
        printf("Error opening file.\n");
        return 1;
    }

    // Initialize the sequence file if it doesn't exist
    FILE *idFile = fopen("last_id.sequence", "r");
    if (idFile == NULL)
    {
        initSequenceFile();
        idFile = fopen("last_id.sequence", "r");
    }


    do
    {
        printf("Student Record System\n");
        printf("Welcome to the Student Record System!\n");
        printf("What do you want to do?\n");
        printf("1. Add a student\n");
        printf("2. View all students\n");
        printf("3. Search for a student\n");
        printf("4. Update a student\n");
        printf("5. Delete a student\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &n) != 1)
        {
            printf("Invalid input. Please enter a number.\n");
            return 1;
        }
        if (n < 1 || n > 6)
        {
            printf("Invalid choice. Please enter a number between 1 and 6.\n");
            return 1;
        }
        switch (n)
        {
        case 1:
            printf("Adding a student...\n");
            // Add student logic here
            break;
        case 2:
            printf("Viewing all students...\n");
            // View all students logic here
            break;
        case 3:
            printf("Searching for a student...\n");
            // Search for a student logic here
            break;
        case 4:
            printf("Updating a student...\n");
            // Update student logic here
            break;
        case 5:
            printf("Deleting a student...\n");
            // Delete student logic here
            break;
        case 6:
            printf("Exiting the program...\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    } while (n != 6);

    fclose(record);
    fclose(idFile);

    return 0;
}