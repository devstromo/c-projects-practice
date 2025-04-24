#include <stdio.h>

typedef struct Student
{
    int id;
    char name[50];
    char address[100];
    char phone[15];
} Student;

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

void saveLastId(int id)
{
    FILE *idFile = fopen("last_id.sequence", "w");
    if (idFile != NULL)
    {
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
            Student student;
            int id = getLastId();
            student.id = id + 1;
            printf("Enter student name: ");
            scanf("%s", student.name);
            printf("Enter student address: ");
            scanf("%s", student.address);
            printf("Enter student phone: ");
            scanf("%s", student.phone);
            fwrite(&student, sizeof(Student), 1, record);
            saveLastId(student.id);
            printf("Student added successfully.\n");
            break;
        case 2:
            printf("Viewing all students...\n");
            // View all students logic here
            fseek(record, 0, SEEK_SET); // Reset file pointer to the beginning
            Student tempStudent;
            while (fread(&tempStudent, sizeof(Student), 1, record) == 1)
            {
                printf("ID: %d\n", tempStudent.id);
                printf("Name: %s\n", tempStudent.name);
                printf("Address: %s\n", tempStudent.address);
                printf("Phone: %s\n", tempStudent.phone);
                printf("-------------------------\n");
            }
            if (ftell(record) == 0) // Check if the file is empty
            {
                printf("No students found.\n");
            }
            break;
        case 3:
            printf("Searching for a student...\n");
            // Search for a student logic here
            int searchId;
            printf("Enter student ID to search: ");
            scanf("%d", &searchId);
            fseek(record, 0, SEEK_SET); // Reset file pointer to the beginning
            int found = 0;
            while (fread(&tempStudent, sizeof(Student), 1, record) == 1)
            {
                if (tempStudent.id == searchId)
                {
                    printf("ID: %d\n", tempStudent.id);
                    printf("Name: %s\n", tempStudent.name);
                    printf("Address: %s\n", tempStudent.address);
                    printf("Phone: %s\n", tempStudent.phone);
                    found = 1;
                    break;
                }
            }
            if (!found)
            {
                printf("Student with ID %d not found.\n", searchId);
            }
            break;
        case 4:
            printf("Updating a student...\n");
            // Update student logic here
            break;
        case 5:
            printf("Deleting a student...\n");
            printf("Enter student ID to delete: ");
            int studentId;
            scanf("%d", &studentId);

            // Open the original record file for reading
            FILE *readFile = fopen("student_record.record", "r");
            if (readFile == NULL)
            {
                printf("Error opening student record file.\n");
                break;
            }

            // Create a temp file to write updated data
            FILE *tempFile = fopen("temp.record", "w");
            if (tempFile == NULL)
            {
                printf("Error creating temporary file.\n");
                fclose(readFile);
                break;
            }

            int studentFound = 0;
            Student tempStudent1;

            while (fread(&tempStudent1, sizeof(Student), 1, readFile) == 1)
            {
                if (tempStudent1.id == studentId)
                {
                    studentFound = 1; // Skip writing this record
                }
                else
                {
                    fwrite(&tempStudent1, sizeof(Student), 1, tempFile);
                }
            }

            fclose(readFile);
            fclose(tempFile);

            if (studentFound)
            {
                // Replace the original file with the new one
                remove("student_record.record");
                rename("temp.record", "student_record.record");
                printf("Student with ID %d deleted successfully.\n", studentId);
            }
            else
            {
                remove("temp.record"); // clean up unused temp file
                printf("Student with ID %d not found.\n", studentId);
            }
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