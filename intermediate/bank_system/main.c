#include <stdio.h>
#include <stdlib.h>

typedef struct BankAccount
{
    int accountNumber;
    char accountHolder[50];
    double balance;
    char accountType[20];         // e.g., Savings, Checking
    char dateOpened[11];          // Format: YYYY-MM-DD
    char lastTransactionDate[11]; // Format: YYYY-MM-DD
} BankAccount;

void initBankAccountSequenceFile()
{
    FILE *idFile = fopen("bank_account.seq", "w");
    if (idFile != NULL)
    {
        fprintf(idFile, "%d", 0); // Initialize with 0
        fclose(idFile);
    }
}

void initLedgerEntrySequenceFile()
{
    FILE *idFile = fopen("ledger_entry.seq", "w");
    if (idFile != NULL)
    {
        fprintf(idFile, "%d", 0); // Initialize with 0
        fclose(idFile);
    }
}

// MAIN

int main()
{

    // Initialize the sequence files if they don't exist
    FILE *idFile = fopen("bank_account.seq", "r");
    if (idFile == NULL)
    {
        initBankAccountSequenceFile();
        idFile = fopen("bank_account.seq", "r");
    }
    FILE *ledgerFile = fopen("ledger_entry.seq", "r");
    if (ledgerFile == NULL)
    {
        initLedgerEntrySequenceFile();
        ledgerFile = fopen("ledger_entry.seq", "r");
    }

    int option;

    do
    {
        printf("Welcome to the Bank Account Management System!\n");
        printf("1. Add a new account\n");
        printf("2. View all accounts\n");
        printf("3. Search for an account\n");
        printf("4. Update an account\n");
        printf("5. Delete an account\n");
        printf("6. Exit\n");
        printf("Please select an option (1-6): ");
        scanf("%d", &option);
        if (option < 1 || option > 6)
        {
            printf("Invalid option. Please select a number between 1 and 6.\n");
            return 1;
        }
        switch (option)
        {
        case 1:
            printf("Adding a new account...\n");
            // Code to add a new account
            break;
        case 2:
            printf("Viewing all accounts...\n");
            // Code to view all accounts
            break;
        case 3:
            printf("Searching for an account...\n");
            // Code to search for an account
            break;
        case 4:
            printf("Updating an account...\n");
            // Code to update an account
            break;
        case 5:
            printf("Deleting an account...\n");
            // Code to delete an account
            break;
        case 6:
            printf("Exiting the program...\n");
            break;
        default:
            printf("Invalid option. Please select a number between 1 and 6.\n");
        }
    } while (option != 6);

    return 0;
}