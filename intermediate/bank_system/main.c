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
    }
    else
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
    }
    else
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
    }
    else
    {
        perror("Error initializing ledger entry sequence file");
    }
}

int getLastLedgerEntryId()
{
    FILE *idFile = fopen("ledger_entry.seq", "r");
    int id = 0;
    if (idFile != NULL)
    {
        fscanf(idFile, "%d", &id);
        fclose(idFile);
    }
    return id;
}

void saveLastLedgerEntryId(int id)
{
    FILE *idFile = fopen("ledger_entry.seq", "w");
    if (idFile != NULL)
    {
        fprintf(idFile, "%d", id);
        fclose(idFile);
    }
    else
    {
        perror("Error saving last ledger entry ID");
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
    }
    else
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
    }
    else
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

void writeBankAccountToCSV(BankAccount *account, int accountNumber)
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
        fprintf(file, "%d,\"%s\",%.2f,\"%s\",\"%s\",\"%s\"\n",
                accountNumber,
                accountHolder,
                account->balance,
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

void addNewAccount()
{
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
    int accountNumber = getLastBankAccountId() + 1;
    writeBankAccountToCSV(&newAccount, accountNumber);
    saveLastBankAccountId(accountNumber);
    printf("Account added successfully!\n");
}

void viewAllAccounts()
{
    printf("Viewing all accounts...\n");
    FILE *recordView = fopen("accounts.csv", "r");
    if (recordView == NULL)
    {
        printf("No accounts found (file missing).\n");
        return;
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
}

void searchForAccount()
{
    printf("Searching for an account...\n");
    FILE *recordAccountView = fopen("accounts.csv", "r");
    if (recordAccountView == NULL)
    {
        printf("No accounts found (file missing).\n");
        return;
    }
    char accountLine[256];
    int anyAccount = 0;
    char searchName[50];
    printf("Enter account holder name to search: ");
    scanf(" %[^\n]", searchName);
    while (fgets(accountLine, sizeof(accountLine), recordAccountView))
    {
        if (anyAccount == 0 && accountLine[0] == 'A')
        {
            anyAccount = 1;
            continue;
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
    FILE *recordView = fopen("accounts.csv", "r");
    if (recordView == NULL)
    {
        printf("No accounts found (file missing).\n");
        return;
    }

    printf("Enter account number to update: ");
    int accountNumberToUpdate;
    scanf("%d", &accountNumberToUpdate);

    FILE *tempFile = fopen("temp_accounts.csv", "w");
    if (tempFile == NULL)
    {
        printf("Error creating temporary file.\n");
        fclose(recordView);
        return;
    }

    char updateLine[256];
    int anyUpdate = 0;
    int accountFound = 0;

    BankAccount updatedAccount;

    while (fgets(updateLine, sizeof(updateLine), recordView))
    {
        if (anyUpdate == 0 && updateLine[0] == 'A')
        {
            anyUpdate = 1;
            fprintf(tempFile, "%s", updateLine);
            continue;
        }

        int accountNumber;
        sscanf(updateLine, "%d", &accountNumber);

        if (accountNumber != accountNumberToUpdate)
        {
            fprintf(tempFile, "%s", updateLine);
        }
        else
        {
            if (!accountFound)
            {
                accountFound = 1;

                updatedAccount.accountNumber = accountNumber;

                printf("Enter new account holder name: ");
                scanf(" %[^\n]", updatedAccount.accountHolder);

                printf("Enter new balance: ");
                scanf("%lf", &updatedAccount.balance);

                printf("Enter new account type (e.g., Savings, Checking): ");
                scanf(" %[^\n]", updatedAccount.accountType);

                getCurrentDate(updatedAccount.dateOpened, sizeof(updatedAccount.dateOpened));
                updatedAccount.lastTransactionDate[0] = '\0';

                fprintf(
                    tempFile,
                    "%d,\"%s\",%.2f,\"%s\",\"%s\",\"%s\"\n",
                    updatedAccount.accountNumber,
                    updatedAccount.accountHolder,
                    updatedAccount.balance,
                    updatedAccount.accountType,
                    updatedAccount.dateOpened,
                    updatedAccount.lastTransactionDate);
            }
            else
            {
                continue;
            }
        }
    }

    fclose(recordView);
    fclose(tempFile);

    if (!accountFound)
    {
        printf("\n\nNo accounts found with the number '%d'.\n\n", accountNumberToUpdate);
        remove("temp_accounts.csv");
        return;
    }

    if (remove("accounts.csv") != 0)
    {
        perror("Error al eliminar accounts.csv");
        return;
    }
    if (rename("temp_accounts.csv", "accounts.csv") != 0)
    {
        perror("Error al renombrar temp_accounts.csv a accounts.csv");
        return;
    }

    printf("Account with number %d updated successfully!\n", accountNumberToUpdate);
}

void withdrawMoney()
{
    printf("Withdrawing money...\n");

    FILE *recordView = fopen("accounts.csv", "r");
    if (recordView == NULL)
    {
        printf("No accounts found (file missing).\n");
        return;
    }

    printf("Enter account number to withdraw money: ");
    int accountNumberToWithdrawMoney;
    scanf("%d", &accountNumberToWithdrawMoney);

    FILE *tempFile = fopen("temp_accounts.csv", "w");
    if (tempFile == NULL)
    {
        printf("Error creating temporary file.\n");
        fclose(recordView);
        return;
    }

    char updateLine[256];
    int anyUpdate = 0;
    int accountFound = 0;
    int accountHasNoBalance = 0;
    double withdrawalAmount;

    BankAccount searchedAccount;

    while (fgets(updateLine, sizeof(updateLine), recordView))
    {
        if (anyUpdate == 0 && updateLine[0] == 'A')
        {
            anyUpdate = 1;
            fprintf(tempFile, "%s", updateLine);
            continue;
        }

        int accountNumber;
        sscanf(updateLine, "%d", &accountNumber);

        if (accountNumber != accountNumberToWithdrawMoney)
        {
            fprintf(tempFile, "%s", updateLine);
        }
        else
        {
            if (!accountFound)
            {
                accountFound = 1;

                searchedAccount.accountNumber = accountNumber;
                if (searchedAccount.balance == 0.0)
                {
                    printf("Account with number %d has no balance.\n", accountNumberToWithdrawMoney);
                    fprintf(tempFile, "%s", updateLine);
                    accountHasNoBalance = 1;
                    continue;
                }
                printf("Enter amount to withdraw: ");
                scanf("%lf", &withdrawalAmount);
                if (searchedAccount.balance < withdrawalAmount)
                {
                    printf("Insufficient balance for withdrawal.\n");
                    fprintf(tempFile, "%s", updateLine);
                    continue;
                }
                searchedAccount.balance -= withdrawalAmount;
                getCurrentDate(searchedAccount.lastTransactionDate, sizeof(searchedAccount.lastTransactionDate));

                fprintf(
                    tempFile,
                    "%d,\"%s\",%.2f,\"%s\",\"%s\",\"%s\"\n",
                    searchedAccount.accountNumber,
                    searchedAccount.accountHolder,
                    searchedAccount.balance,
                    searchedAccount.accountType,
                    searchedAccount.dateOpened,
                    searchedAccount.lastTransactionDate);
            }
            else
            {
                continue;
            }
        }
    }

    fclose(recordView);
    fclose(tempFile);

    if (!accountFound)
    {
        printf("\n\nNo accounts found with the number '%d'.\n\n", accountNumberToWithdrawMoney);
        remove("temp_accounts.csv");
        return;
    }

    if (remove("accounts.csv") != 0)
    {
        perror("Error al eliminar accounts.csv");
        return;
    }
    if (rename("temp_accounts.csv", "accounts.csv") != 0)
    {
        perror("Error al renombrar temp_accounts.csv a accounts.csv");
        return;
    }

    if (accountHasNoBalance)
    {
        printf("Account with number %d has no balance. Withdrawal not processed.\n", accountNumberToWithdrawMoney);
        return;
    }

    printf("Account with number %d updated successfully!\n", accountNumberToWithdrawMoney);
}

void depositMoney()
{
    printf("Depositing money...\n");

    FILE *recordView = fopen("accounts.csv", "r");
    if (recordView == NULL)
    {
        printf("No accounts found (file missing).\n");
        return;
    }

    printf("Enter account number to deposit money: ");
    int accountNumberToDepositMoney;
    scanf("%d", &accountNumberToDepositMoney);

    FILE *tempFile = fopen("temp_accounts.csv", "w");
    if (tempFile == NULL)
    {
        printf("Error creating temporary file.\n");
        fclose(recordView);
        return;
    }

    char updateLine[256];
    int anyUpdate = 0;
    int accountFound = 0;
    double depositAmount;
    BankAccount searchedAccount;
    while (fgets(updateLine, sizeof(updateLine), recordView))
    {
        if (anyUpdate == 0 && updateLine[0] == 'A')
        {
            anyUpdate = 1;
            fprintf(tempFile, "%s", updateLine);
            continue;
        }
        int accountNumber;
        sscanf(updateLine, "%d", &accountNumber);

        if (accountNumber != accountNumberToDepositMoney)
        {
            fprintf(tempFile, "%s", updateLine);
        }
        else
        {
            if (!accountFound)
            {
                accountFound = 1;

                sscanf(updateLine, "%d,\"%49[^\"]\",%lf,\"%19[^\"]\",\"%10[^\"]\",\"%10[^\"]\"",
                       &searchedAccount.accountNumber,
                       searchedAccount.accountHolder,
                       &searchedAccount.balance,
                       searchedAccount.accountType,
                       searchedAccount.dateOpened,
                       searchedAccount.lastTransactionDate);

                printf("Enter amount to deposit: ");
                scanf("%lf", &depositAmount);
                searchedAccount.balance += depositAmount;
                getCurrentDate(searchedAccount.lastTransactionDate, sizeof(searchedAccount.lastTransactionDate));

                fprintf(
                    tempFile,
                    "%d,\"%s\",%.2f,\"%s\",\"%s\",\"%s\"\n",
                    searchedAccount.accountNumber,
                    searchedAccount.accountHolder,
                    searchedAccount.balance,
                    searchedAccount.accountType,
                    searchedAccount.dateOpened,
                    searchedAccount.lastTransactionDate);
            }
            else
            {
                continue;
            }
        }
    }
    fclose(recordView);
    fclose(tempFile);
    if (!accountFound)
    {
        printf("\n\nNo accounts found with the number '%d'.\n\n", accountNumberToDepositMoney);
        remove("temp_accounts.csv");
        return;
    }
    if (remove("accounts.csv") != 0)
    {
        perror("Error al eliminar accounts.csv");
        return;
    }
    if (rename("temp_accounts.csv", "accounts.csv") != 0)
    {
        perror("Error al renombrar temp_accounts.csv a accounts.csv");
        return;
    }
    printf("Account with number %d updated successfully!\n", accountNumberToDepositMoney);
}

void transferringMoney()
{
    printf("Transferring money...\n");

    int sourceAccountNumber, destinationAccountNumber;
    double transferAmount;

    printf("Enter source account number: ");
    scanf("%d", &sourceAccountNumber);
    printf("Enter destination account number: ");
    scanf("%d", &destinationAccountNumber);
    printf("Enter amount to transfer: ");
    scanf("%lf", &transferAmount);

    FILE *original = fopen("accounts.csv", "r");
    if (!original)
    {
        printf("No accounts found (file missing).\n");
        return;
    }

    BankAccount source, destination;
    int foundSource = 0, foundDestination = 0;
    char line[256];

    // Read all accounts first to find both accounts
    while (fgets(line, sizeof(line), original))
    {
        BankAccount temp;
        int accountNumber;
        sscanf(line, "%d,\"%49[^\"]\",%lf,\"%19[^\"]\",\"%10[^\"]\",\"%10[^\"]\"",
               &temp.accountNumber,
               temp.accountHolder,
               &temp.balance,
               temp.accountType,
               temp.dateOpened,
               temp.lastTransactionDate);

        if (temp.accountNumber == sourceAccountNumber)
        {
            source = temp;
            foundSource = 1;
        }
        else if (temp.accountNumber == destinationAccountNumber)
        {
            destination = temp;
            foundDestination = 1;
        }
    }
    fclose(original);

    if (!foundSource)
    {
        printf("Source account not found.\n");
        return;
    }
    if (!foundDestination)
    {
        printf("Destination account not found.\n");
        return;
    }
    if (source.balance < transferAmount)
    {
        printf("Insufficient funds.\n");
        return;
    }

    // Update balances
    source.balance -= transferAmount;
    destination.balance += transferAmount;
    getCurrentDate(source.lastTransactionDate, sizeof(source.lastTransactionDate));
    getCurrentDate(destination.lastTransactionDate, sizeof(destination.lastTransactionDate));

    // Now write all accounts back, replacing updated source and destination
    original = fopen("accounts.csv", "r");
    FILE *temp = fopen("temp_accounts.csv", "w");
    if (!original || !temp)
    {
        printf("Error accessing files.\n");
        if (original)
            fclose(original);
        if (temp)
            fclose(temp);
        return;
    }

    while (fgets(line, sizeof(line), original))
    {
        BankAccount tempAcc;
        int accountNumber;
        sscanf(line, "%d,\"%49[^\"]\",%lf,\"%19[^\"]\",\"%10[^\"]\",\"%10[^\"]\"",
               &tempAcc.accountNumber,
               tempAcc.accountHolder,
               &tempAcc.balance,
               tempAcc.accountType,
               tempAcc.dateOpened,
               tempAcc.lastTransactionDate);

        if (tempAcc.accountNumber == sourceAccountNumber)
        {
            fprintf(temp, "%d,\"%s\",%.2f,\"%s\",\"%s\",\"%s\"\n",
                    source.accountNumber,
                    source.accountHolder,
                    source.balance,
                    source.accountType,
                    source.dateOpened,
                    source.lastTransactionDate);
        }
        else if (tempAcc.accountNumber == destinationAccountNumber)
        {
            fprintf(temp, "%d,\"%s\",%.2f,\"%s\",\"%s\",\"%s\"\n",
                    destination.accountNumber,
                    destination.accountHolder,
                    destination.balance,
                    destination.accountType,
                    destination.dateOpened,
                    destination.lastTransactionDate);
        }
        else
        {
            fputs(line, temp);
        }
    }

    fclose(original);
    fclose(temp);

    remove("accounts.csv");
    rename("temp_accounts.csv", "accounts.csv");

    printf("Transfer completed successfully.\n");

    // Log the transaction in the ledger
    LedgerEntry entry;
    entry.transactionId = getLastLedgerEntryId() + 1;
    entry.fromAccount = sourceAccountNumber;
    entry.toAccount = destinationAccountNumber;
    entry.amount = transferAmount;
    getCurrentDate(entry.date, sizeof(entry.date));
    snprintf(entry.note, sizeof(entry.note), "Transfer from %d to %d", sourceAccountNumber, destinationAccountNumber);
    writeLedgerEntryToCSV(&entry);
    saveLastLedgerEntryId(entry.transactionId);
    printf("Transaction logged successfully with ID %d.\n", entry.transactionId);
}

void viewTransactionHistory()
{
    printf("Viewing transaction history...\n");

    FILE *ledgerFile = fopen("ledger.csv", "r");
    if (ledgerFile == NULL)
    {
        printf("No transaction history found (file missing).\n");
        return;
    }

    char line[256];
    int any = 0;
    while (fgets(line, sizeof(line), ledgerFile))
    {
        if (any == 0 && line[0] == 'T')
        {
            any = 1;
            continue;
        }
        printf("%s", line);
        any++;
    }
    if (any == 1)
    {
        printf("\n\nNo transactions found.\n\n");
    }
    fclose(ledgerFile);
}

void viewAccountBalance()
{
    printf("Viewing account balance...\n");
    FILE *recordAccountView = fopen("accounts.csv", "r");
    if (recordAccountView == NULL)
    {
        printf("No accounts found (file missing).\n");
        return;
    }
    char accountLine[256];
    int accountFound = 0;
    char searchName[50];
    BankAccount searchedAccount;
    printf("Enter account holder name to get balance: ");
    scanf(" %[^\n]", searchName);
    while (fgets(accountLine, sizeof(accountLine), recordAccountView))
    {
        if (accountFound == 0 && accountLine[0] == 'A')
        {
            continue;
        }
        if (strstr(accountLine, searchName) != NULL)
        {
            accountFound = 1;
            sscanf(accountLine, "%d,\"%49[^\"]\",%lf,\"%19[^\"]\",\"%10[^\"]\",\"%10[^\"]\"",
                   &searchedAccount.accountNumber,
                   searchedAccount.accountHolder,
                   &searchedAccount.balance,
                   searchedAccount.accountType,
                   searchedAccount.dateOpened,
                   searchedAccount.lastTransactionDate);
            printf("\nBalance for account holder '%s' (Account Number: %d): %.2f\n",
                   searchedAccount.accountHolder,
                   searchedAccount.accountNumber,
                   searchedAccount.balance);
        }
    }
    if (accountFound == 0)
    {
        printf("\n\nNo accounts found with the name '%s'.\n\n", searchName);
    }
    fclose(recordAccountView);
}

// MAIN
int main()
{

    FILE *idFile = fopen("bank_account.seq", "r");
    if (idFile == NULL)
    {
        initBankAccountSequenceFile();
        idFile = fopen("bank_account.seq", "r");
    }

    initBankAccountDB();
    FILE *ledgerFile = fopen("ledger_entry.seq", "r");
    if (ledgerFile == NULL)
    {
        initLedgerEntrySequenceFile();
        ledgerFile = fopen("ledger_entry.seq", "r");
    }
    initLedgerDB();

    int option;

    do
    {
        printf("\n\nWelcome to the Bank Account Management System!\n\n");
        printf("1. Add a new account\n");
        printf("2. View all accounts\n");
        printf("3. Search for an account\n");
        printf("4. Update an account\n");
        printf("5. Delete an account\n");
        printf("6. Withdraw money\n");
        printf("7. Deposit money\n");
        printf("8. Transfer money\n");
        printf("9. View transaction history\n");
        printf("10. View account balance\n");
        printf("11. Exit\n");
        printf("Please select an option (1-11): ");
        scanf("%d", &option);
        if (option < 1 || option > 11)
        {
            printf("Invalid option. Please select a number between 1 and 6.\n");
            return 1;
        }
        switch (option)
        {
        case 1:
            addNewAccount();
            break;
        case 2:
            viewAllAccounts();
            break;
        case 3:
            searchForAccount();
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
            withdrawMoney();
            break;
        case 7:
            depositMoney();
            break;
        case 8:
            transferringMoney();
            break;
        case 9:
            viewTransactionHistory();
            break;
        case 10:
            viewAccountBalance();
            break;
        case 11:
            printf("Exiting the program...\n");
            break;
        default:
            printf("Invalid option. Please select a number between 1 and 11.\n");
        }
    } while (option != 11);

    return 0;
}