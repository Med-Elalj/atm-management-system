#include "header.h"
#include <sqlite3.h>
#include <ctype.h>

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
conf:
    printf("\n\n\t\t\t===== Confirmation =====\n");
    printf("\nPhone number: %d\nCountry : %s\nInitial balance : %d.%2d\nAccount Type : %s\n", a.phone, a.country, a.balance / 100, a.balance % 100, a.accountType);
    printf("\nIs this data correct ?\n Y : Yes\n N : No\n Q : Back to Menu\n ");
    char confirm;

    clearBuffer();
    scan = scanf(" %c", &confirm);
    if (scan == -1)
    {
        exit(2);
    }
    while (getchar() != '\n')
        ;
    switch (confirm)
    {
    case 'N':
    case 'n':
        goto noAccount;
    case 'Q':
    case 'q':
        return;
    case 'Y':
    case 'y':
        system("clear");
        break;
    default:
        system("clear");
        printf("\nInvalid input! Please chonse from (Y/N/Q).\n");
        goto conf;
    }
    db_createNewAccount(a, db);
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
    char phone[12];
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
            scan = scanf("%11s", &phone[0]);
        } while (scan == 0 || scan != -1 && (strlen(phone) < 10 || !is_all_digits(phone) || getchar() != 10));
    }
    else
    {
        queryAccountType(phone);
    };
    int check;
    do
    {
        check = 0;
        printf("\nDo you want to update the information of account number %d to %s? (Y/N): ", id, phone);
        clearBuffer();
        scan = scanf(" %c", &check);
    } while (check != 'y' && check != 'Y' && check != 'n' && check != 'N' && scan != -1);

    if (check == 'n' || check == 'N' || scan == -1)
    {
        printf("\n\nPhone number not updated.\n\n");
        return;
    }

    if (scan == -1)
    {
        sqlite3_close(db);
        exit(1);
    };

    return db_updateAccount(db, c, phone, id, u.id);
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
    char c = 0;
    do
    {
        printf("\nDo you want to proceed with the transaction? (Y/N): ");
        clearBuffer();
        scan = scanf(" %c", &c);
    } while (scan == 0 || c != 'Y' && c != 'y' && c != 'n' && c != 'N' && scan != -1);

    if (c == 'n' || c == 'N' || scan == -1)
    {
        printf("\n\nTransaction cancelled.\n\n");
        return;
    }

    return db_makeTransaction(db, accID1, accID2, amount, u.id);
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

    char c = 0;
    do
    {
        printf("\nDo you want to proceed with the deletion? (Y/N): ");
        clearBuffer();
        scan = scanf(" %c", &c);
    } while (scan == 0 || c != 'Y' && c != 'y' && c != 'n' && c != 'N' && scan != -1);
    if (c == 'n' || c == 'N' || scan == -1)
    {
        printf("\n\nDeletion cancelled.\n\n");
        return;
    }
    return db_removeAccount(db, accID, u.id);
}

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