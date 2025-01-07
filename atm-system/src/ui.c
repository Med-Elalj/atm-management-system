#include "header.h"
#include <sqlite3.h>
#include <ctype.h>
#include <stdarg.h>

void createNewAcc(struct User u, sqlite3 *db)
{
    struct Account a;
    a.uId = u.id;
    a.creationDate = time(NULL) / 86400;
    int scan = 0;
noAccount:
    queryAccountType(a.accountType);
    system("clear");
    printf("\t\t\t===== New Account =====\n");
    printf("account type: %s\n", a.accountType);
    do
    {
        printf("\nEnter the country: ");
        clearBuffer();
        scan = scanf(" %50s", a.country);
    } while (scan == 0 || scan == 1 && validInput(a.country) == 0);
    char phone[12];
    do
    {
        printf("\nEnter the phone number: ");
        clearBuffer();
        scan = scanf("%11s", phone);
        a.phone = atoi(phone);
    } while ((strlen(phone) != 10 && strlen(phone) != 11 && scan == 1) || (a.phone <= 0 && scan == 1) || scan == 0);
    float b = 0.0;
    do
    {
        printf("\nEnter the initial deposit amount:");
        clearBuffer();
        scan = scanf("%f", &b);
        a.balance = b * 100;
    } while (scan == 0 || scan == 1 && a.balance <= 0);
    system("clear");
    if (confirm("Account Creation Canceled\n",
                    "\n\n\t\t\t===== Confirmation =====\n"
                    "\nPhone number: %d\nCountry : %s\nInitial balance : %d.%2d\nAccount Type : %s\n"
                    "\nIs this data correct ?\n Y : Yes\n N : No\n ",
                    a.phone, a.country, a.balance / 100, a.balance % 100, a.accountType))
    {
    db_createNewAccount(a, db);
    }
};

void queryAccountType(char type[7])
{

    int scan = 0;
    system("clear");
    printf("\nEnter the type of account:\n\t-> savings\n\t-> current\n\t-> fixed01(for 1 year)\n\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n\n\tEnter your choice:");
    clearBuffer();
    scan = scanf("%7s", type);
    while (scan != -1 && strcmp(type, "savings") != 0 && strcmp(type, "current") != 0 && strcmp(type, "fixed01") != 0 && strcmp(type, "fixed02") != 0 && strcmp(type, "fixed03") != 0)
    {
        printf("Invalid input! Enter a valid type of account: ");
        clearBuffer();
        scan = scanf("%7s", type);
    };
};

void updateAccount(struct User u, sqlite3 *db)
{
    int id = 0;
    int scan = 0;
    char data[12];
    char c;

    do
    {
        id = 0;
        printf("\nEnter the account number you want to update : ");
        clearBuffer();
        scan = scanf(" %d", &id);
    } while (scan == 0);

    do
    {
        c = 0;
        printf("witch info do you want to update ?\n\t[1] : Phone number\n\t[2] : Country of the Account\n");
        clearBuffer();
        scan = scanf(" %c", &c);
    } while (c != '1' && c != '2' && scan != -1);

    if (c == '1')
    {
        do
        {
            printf("\nEnter the new phone number: ");
            clearBuffer();
            scan = scanf("%11s", &data[0]);
        } while (scan == 0 || scan != -1 && (strlen(data) < 10 || !is_all_digits(data) || getchar() != 10));
    }
    else
    {
        queryAccountType(data);
    };

    if (confirm("Account data not updated.", "\nDo you want to update the information of account number %d to %s? (Y/N): ", id, data))
    {
        system("clear");
        db_updateAccount(db, c, data, id, u.id);
    }

};

int is_all_digits(const char *str)
{
    while (*str)
    {
        if (!isdigit(*str))
        {
            return 0;
        }
        str++;
    }
    return 1;
};

void makeTransaction(struct User u, sqlite3 *db)
{
    int amount, accID1, accID2 = 0;
    int scan = 0;
    do
    {
        printf("\nEnter the Origin account's account number : ");
        clearBuffer();
        scan = scanf(" %d", &accID2);
    } while (scan == 0 || accID2 <= 0 && scan != -1);
    do
    {
        printf("\nEnter the Destination account number : ");
        clearBuffer();
        scan = scanf(" %d", &accID1);
    } while (scan == 0 || accID2 <= 0 && scan != -1);
    do
    {
        printf("\nEnter the amount to transfer : ");
        clearBuffer();
        scan = scanf(" %d", &amount);
    } while (scan == 0);
    if (accID1 == accID2)
    {
        printf("\n\nCannot transfer to the same account.\n\n");
        return;
    }

    if (confirm("Transaction cancelled.\n\n","\nDo you want to proceed with the transaction? (Y/N): ")) {
        db_makeTransaction(db, accID1, accID2, amount, u.id);
    }
    return;
}

void removeAccount(struct User u, sqlite3 *db)
{
    int accID = 0;
    int scan = 0;
    do
    {
        printf("\nEnter the account number to delete : ");
        clearBuffer();
        scan = scanf(" %d", &accID);
    } while (scan == 0 || accID <= 0 && scan != -1);

    if (confirm("Deletion cancelled.\n\n","\nDo you want to proceed with the deletion? (Y/N): "))
    {
        db_removeAccount(db, accID, u.id);
    }
    return;

};

void transferAccount(struct User u, sqlite3 *db)
{
    int accID1 = 0;
    int scan = 0;
    do
    {
        printf("\nEnter the account number to be tanfered : ");
        clearBuffer();
        scan = scanf(" %d", &accID1);
    } while (scan == 0 || accID1 <= 0 && scan != -1);
    char uname2[50];
    do
    {
        printf("\nEnter the name of the recipient : ");
        clearBuffer();
        scan = scanf(" %49s", &uname2[0]);
    } while (scan == 0 || (strlen(&uname2[0]) <= 2) && scan != -1);
    if (scan == -1)
    {
        sqlite3_close(db);
        system("clear");
        exit(1);
    };

    return db_transferAccount(db, uname2, accID1, u.id);
}

void checkAllAccounts(struct User u, sqlite3 *db, int t)
{
    while(1){
        system("clear");
        db_printAccountsOfUser(u,db,t);
        if (confirm("","All of your(%s) accounts are listed above.\n\t Would You like to Re Fresh?\n\tY :Yes (Refrech).\n\tN : No (Back to menu).\n", u.name)){
            return;
        };
    }
};

int confirm(const char *cancel_msg,const char *format,  ...) {
    va_list args;
    va_start(args, format);
    int scan = 0;
    char c = 0;
    
    do
    {
        vprintf(format, args);
        clearBuffer();
        scan = scanf(" %c", &c);
    } while (scan == 0 || c != 'Y' && c != 'y' && c != 'n' && c != 'N' && scan != -1);
    
    va_end(args);
    
    if (c == 'n' || c == 'N' || scan == -1)
    {
        printf("\n\n%s\n\n",cancel_msg);
        return 0;
    }

    return 1;
}