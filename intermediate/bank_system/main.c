#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>

typedef struct BankAccount
{
    int accountNumber;
    char accountHolder[50];
    double balance;
    char accountType[20];         // e.g., Savings, Checking
    char dateOpened[11];          // Format: YYYY-MM-DD
    char lastTransactionDate[11]; // Format: YYYY-MM-DD
} BankAccount;

typedef struct LedgerEntry
{
    int transactionId;
    int fromAccount;
    int toAccount;
    double amount;
    char date[11];  // Format: YYYY-MM-DD
    char note[100]; // Optional note: e.g., "transfer", "deposit", etc.
} LedgerEntry;

void initBankAccountSequenceFile()
{
    FILE *idFile = fopen("bank_account.seq", "w");
    if (idFile != NULL)
    {
        fprintf(idFile, "%d", 0); // Initialize with 0
        fclose(idFile);
    } else
    {
        perror("Error initializing bank account sequence file");
    }
}

int getLastBankAccountId()
{
    FILE *idFile = fopen("bank_account.seq", "r");
    int id = 0;
    if (idFile != NULL)
    {
        fscanf(idFile, "%d", &id);
        fclose(idFile);
    }
    return id;
}

void saveLastBankAccountId(int id)
{
    FILE *idFile = fopen("bank_account.seq", "w");
    if (idFile != NULL)
    {
        fprintf(idFile, "%d", id);
        fclose(idFile);
    } else
    {
        perror("Error saving last bank account ID");
    }
}

void initLedgerEntrySequenceFile()
{
    FILE *idFile = fopen("ledger_entry.seq", "w");
    if (idFile != NULL)
    {
        fprintf(idFile, "%d", 0); // Initialize with 0
        fclose(idFile);
    } else
    {
        perror("Error initializing ledger entry sequence file");
    }
}

void initBankAccountDB()
{
    FILE *file = fopen("accounts.csv", "r");
    if (file == NULL)
    {
        file = fopen("accounts.csv", "w");
        fprintf(file, "AccountNumber,AccountHolder,Balance,AccountType,DateOpened,LastTransactionDate\n");
        fclose(file);
    } else
    {
        fclose(file);
    }
}

void initLedgerDB()
{
    FILE *file = fopen("ledger.csv", "r");
    if (file == NULL)
    {
        file = fopen("ledger.csv", "w");
        fprintf(file, "TransactionId,FromAccount,ToAccount,Amount,Date,Note\n");
        fclose(file);
    } else
    {
        fclose(file);
    }
}

void escapeCsvField(char *dest, const char *src)
{
    while (*src)
    {
        if (*src == '"')
        {
            *dest++ = '"'; // double the quote
        }
        *dest++ = *src++;
    }
    *dest = '\0';
}

void writeBankAccountToCSV(BankAccount *account)
{
    FILE *file = fopen("accounts.csv", "a");
    if (file != NULL)
    {
        char accountHolder[100];
        escapeCsvField(accountHolder, account->accountHolder);
        char accountType[100];
        escapeCsvField(accountType, account->accountType);
        char dateOpened[100];
        escapeCsvField(dateOpened, account->dateOpened);
        char lastTransactionDate[100];
        escapeCsvField(lastTransactionDate, account->lastTransactionDate);
        int accountNumber = getLastBankAccountId() + 1;
        saveLastBankAccountId(accountNumber);
        fprintf(file, "%d,\"%s\",%.2f,\"%s\",\"%s\",\"%s\"\n",
                accountNumber,
                accountHolder,
                0.0,
                accountType,
                dateOpened,
                lastTransactionDate);
        fclose(file);
    }
}

void writeLedgerEntryToCSV(LedgerEntry *entry)
{
    FILE *file = fopen("ledger.csv", "a");
    if (file != NULL)
    {
        char note[100];
        escapeCsvField(note, entry->note);
        fprintf(file, "%d,%d,%d,%.2f,%s,\"%s\"\n",
                entry->transactionId,
                entry->fromAccount,
                entry->toAccount,
                entry->amount,
                entry->date,
                note);
        fclose(file);
    }
}

void getCurrentDate(char *buffer, size_t size)
{
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, size, "%Y-%m-%d", tm_info);
}

void deleteAccountByNumber()
{
    FILE *recordDelete = fopen("accounts.csv", "r");
    if (recordDelete == NULL)
    {
        printf("No accounts found (file missing).\n");
        return;
    }

    printf("Enter account number to delete: ");
    int accountNumberToDelete;
    scanf("%d", &accountNumberToDelete);

    FILE *tempFile = fopen("temp_accounts.csv", "w");
    if (tempFile == NULL)
    {
        printf("Error creating temporary file.\n");
        fclose(recordDelete);
        return;
    }

    char deleteLine[256];
    int anyDelete = 0;
    int accountFound = 0;

    while (fgets(deleteLine, sizeof(deleteLine), recordDelete))
    {
        // Si es la línea del encabezado (empieza con 'A'), la copiamos y seguimos
        if (anyDelete == 0 && deleteLine[0] == 'A')
        {
            anyDelete = 1;
            fprintf(tempFile, "%s", deleteLine);
            continue;
        }

        // Para cada línea de datos, extraemos el accountNumber (antes de la primera coma)
        int accountNumber;
        sscanf(deleteLine, "%d", &accountNumber);
        if (accountNumber != accountNumberToDelete)
        {
            // Si no coincide con el número a borrar, la copiamos al temp
            fprintf(tempFile, "%s", deleteLine);
        }
        else
        {
            // Si coincide, marcamos que encontramos la cuenta
            accountFound = 1;
        }
    }

    fclose(recordDelete);
    fclose(tempFile);

    // Intentamos eliminar el CSV original
    if (remove("accounts.csv") != 0)
    {
        perror("Error al eliminar accounts.csv");
        // Opcionalmente podrías renombrar temp de vuelta o abortar aquí
    }
    // Intentamos renombrar temp a accounts.csv
    if (rename("temp_accounts.csv", "accounts.csv") != 0)
    {
        perror("Error al renombrar temp_accounts.csv a accounts.csv");
    }

    if (!accountFound)
    {
        printf("\n\nNo accounts found with the number '%d'.\n\n", accountNumberToDelete);
    }
    else
    {
        printf("Account with number %d deleted successfully!\n", accountNumberToDelete);
    }
}

void updateAccountByNumber()
{
    // This function is a placeholder for updating an account by number.
    // The implementation would involve reading the account, modifying it,
    // and then writing it back to the file.
    printf("Update account functionality is not yet implemented.\n");
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

    // Initialize the database file if it doesn't exist
    initBankAccountDB();
    FILE *ledgerFile = fopen("ledger_entry.seq", "r");
    if (ledgerFile == NULL)
    {
        initLedgerEntrySequenceFile();
        ledgerFile = fopen("ledger_entry.seq", "r");
    }
    // Initialize the ledger database file if it doesn't exist
    initLedgerDB();

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
            BankAccount newAccount;
            printf("Enter account holder name: ");
            scanf(" %[^\n]", newAccount.accountHolder);
            printf("Enter balance: ");
            scanf("%lf", &newAccount.balance);
            printf("Enter account type (e.g., Savings, Checking): ");
            scanf(" %[^\n]", newAccount.accountType);
            getCurrentDate(newAccount.dateOpened, sizeof(newAccount.dateOpened));
            newAccount.lastTransactionDate[0] = '\0'; // Initialize to empty string
            writeBankAccountToCSV(&newAccount);
            printf("Account added successfully!\n");
            // Code to add a new account
            break;
        case 2:
            printf("Viewing all accounts...\n");
            FILE *recordView = fopen("accounts.csv", "r");
            if (recordView == NULL)
            {
                printf("No accounts found (file missing).\n");
                break;
            }
            char line[256];
            int any = 0;
            while (fgets(line, sizeof(line), recordView))
            {
                if (any == 0 && line[0] == 'A') // Check if it's the header line
                {
                    any = 1;  // Mark that we have printed the header
                    continue; // Skip printing the header again
                }
                printf("%s", line);
                any++;
            }
            if (any == 1)
            {
                printf("\n\nNo accounts found.\n\n");
            }
            fclose(recordView);
            break;
        case 3:
            printf("Searching for an account...\n");
            FILE *recordAccountView = fopen("accounts.csv", "r");
            if (recordAccountView == NULL)
            {
                printf("No accounts found (file missing).\n");
                break;
            }
            char accountLine[256];
            int anyAccount = 0;
            char searchName[50];
            printf("Enter account holder name to search: ");
            scanf(" %[^\n]", searchName);
            while (fgets(accountLine, sizeof(accountLine), recordAccountView))
            {
                if (anyAccount == 0 && accountLine[0] == 'A') // Check if it's the header line
                {
                    anyAccount = 1; // Mark that we have printed the header
                    continue;       // Skip printing the header again
                }
                if (strstr(accountLine, searchName) != NULL)
                {
                    printf("%s", accountLine);
                    anyAccount++;
                }
            }
            if (anyAccount == 1)
            {
                printf("\n\nNo accounts found with the name '%s'.\n\n", searchName);
            }
            fclose(recordAccountView);
            break;
        case 4:
            printf("Updating an account...\n");
            updateAccountByNumber();
            break;
        case 5:
            printf("Deleting an account...\n");
            deleteAccountByNumber();
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