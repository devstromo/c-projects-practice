#include <stdio.h>
#include <stdlib.h>


typedef struct BankAccount
{
    int accountNumber;
    char accountHolder[50];
    double balance;
    char accountType[20]; // e.g., Savings, Checking
    char dateOpened[11]; // Format: YYYY-MM-DD
    char lastTransactionDate[11]; // Format: YYYY-MM-DD
} BankAccount;

// MAIN

int main() {
    printf("Hello to bank system!\n");
    
    return 0;
}