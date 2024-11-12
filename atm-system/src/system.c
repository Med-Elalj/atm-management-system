#include "header.h"
#include <sqlite3.h>
#include <ctype.h>

const char *RECORDS = "./data/records.txt";

void createNewAcc(struct User u, sqlite3 *db)
{
    struct Account a;
    a.uId = u.id;
    a.creationDate = 1731679260 / 86400;
    int s = 0;
noAccount:
    queryAccountType(a.accountType);
    system("clear");
    printf("\t\t\t===== New Account =====\n");
    time_t azer = a.creationDate * 86400;
    printf("account type: %s\n", a.accountType);
    do
    {
        printf("\nEnter the country: ");
        s = scanf(" %50s", a.country);
        while (getchar() != '\n')
            ;

    } while (s == 0 || s == 1 && validInput(a.country) == 0);
    char phone[12];
    do
    {
        printf("\nEnter the phone number: ");
        s = scanf("%11s", phone);
        while (getchar() != '\n')
            ;
        a.phone = atoi(phone);
    } while ((strlen(phone) != 10 && strlen(phone) != 11 && s == 1) || (a.phone <= 0 && s == 1) || s == 0);
    float b = 0.0;
    do
    {
        printf("\nEnter the initial deposit amount:");
        s = scanf("%f", &b);
        while (getchar() != '\n')
            ;
        a.balance = b * 100;
    } while (s == 0 || s == 1 && a.balance <= 0);
    system("clear");
    if (s == -1)
    {
        exit(2);
    }
conf:
    printf("\n\n\t\t\t===== Confirmation =====\n");
    printf("\nPhone number: %d\nCountry : %s\nInitial balance : %d.%2d\nAccount Type : %s\n", a.phone, a.country, a.balance / 100, a.balance % 100, a.accountType);
    printf("\nIs this data correct ?\n Y : Yes\n N : No\n Q : Back to Menu\n ");
    char confirm;
    scanf(" %c", &confirm);
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
    // Add the account to the database

    const char *insert_sql = "INSERT INTO accounts(uId, creation_Date, country, phonenumber, acc_type, balance) "
                             "VALUES(?,?,?,?,?,?);";
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt, 1, a.uId);
    sqlite3_bind_int(stmt, 2, a.creationDate);
    sqlite3_bind_text(stmt, 3, a.country, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, a.phone);
    sqlite3_bind_text(stmt, 5, a.accountType, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 6, a.balance / 100.0);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Failed to insert user: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
    printf("\n\nAccount created successfully!\n\n");
};

void queryAccountType(char type[7])
{
    system("clear");
    printf("\nEnter the type of account:\n\t-> savings\n\t-> current\n\t-> fixed01(for 1 year)\n\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n\n\tEnter your choice:");
    scanf("%7s", type);
    while (strcmp(type, "savings") != 0 && strcmp(type, "current") != 0 && strcmp(type, "fixed01") != 0 && strcmp(type, "fixed02") != 0 && strcmp(type, "fixed03") != 0)
    {
        printf("Invalid input! Enter a valid type of account: ");
        scanf("%7s", type);
    };
};

void checkAllAccounts(struct User u, sqlite3 *db, int t)
{
checkAllAccounts:
    system("clear");
    const const char *select_sql, *pTempl0, *pTempl1;
    switch (t)
    {
    case 0:
        select_sql = "SELECT accID FROM accounts WHERE uId =?;";
        pTempl0 = "Account numbers\n";
        pTempl1 = "%d \n";
        break;
    case 1:
        select_sql = "SELECT * FROM accounts WHERE uId =?;";
        pTempl0 = "Account number  Date of creation  Country  Phone      Account type   Balance \n";
        pTempl1 = "%14d  %10.10s  %4s %8.8s %11s     %7s     %5.10d.%2.2d \n";
        break;
    default:
        return;
    }
    sqlite3_stmt *stmt;
    int rc;
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        while (getchar() != 10)
        {
        };
        return;
    }
    sqlite3_bind_int(stmt, 1, u.id);

    printf("%s", pTempl0);
    for (rc = sqlite3_step(stmt); rc == SQLITE_ROW; rc = sqlite3_step(stmt))
    {
        // Extract the integer value from the first column (id)
        long int time = sqlite3_column_int(stmt, 2) * 86400;
        char *f = ctime(&time);
        f[10] = '\0';
        f[24] = '\0';
        int b = sqlite3_column_int(stmt, 6);
        if (t == 0)
        {
            printf(pTempl1, sqlite3_column_int(stmt, 0));
        }
        else
        {
            printf(pTempl1,
                   sqlite3_column_int(stmt, 0), f, &f[20], sqlite3_column_text(stmt, 3),
                   sqlite3_column_text(stmt, 4), sqlite3_column_text(stmt, 5), b / 100, b % 100);
        }
    }

    // Check if there was an error with the query execution
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Execution finished with error: %s\n", sqlite3_errmsg(db));
    }

    // Finalize the statement and close the database
    sqlite3_finalize(stmt);
    printf("\n");
end:
    printf("All of your(%s) accounts are listed above.\n\tR : Refrech.\n\tQ : Quit.\n", u.name);
    char c;
    printf("%d", scanf(" %c", &c));
    switch (c)
    {
    case 'q':
    case 'Q':
        return;
    case 'r':
    case 'R':
        goto checkAllAccounts;
    default:
        goto end;
    }
};

void updateAccount(struct User u, sqlite3 *db)
{
    int id = 0;
    char phone[12];
    const const char *update_sql;
    char c;

    do
    {
        id = 0;
        printf("\nEnter the account number you want to update : ");
        while (getchar() != '\n')
            ;

    } while (scanf("%d", &id) == 0);

    do
    {
        c = 0;
        printf("witch info do you want to update ?\n\t[1] : Phone number\n\t[2] : Country of the Account\n");
        scanf(" %c", &c);
        while (getchar() != '\n')
            ;
    } while (c != '1' && c != '2');

    if (c == '1')
    {
        update_sql = "UPDATE accounts SET phonenumber =? WHERE accID =? AND uID =? AND (SELECT COUNT(*) FROM accounts WHERE accID = ? AND uID = ?) = 1;";
        do
        {
            printf("\nEnter the new phone number: ");
            while (getchar() != '\n')
                ;
        } while (scanf("%11s", &phone[0]) == 0 || strlen(phone) < 10 || !is_all_digits(phone) || getchar() != 0);
    }
    else
    {
        update_sql = "UPDATE accounts SET country =? WHERE accID =? AND uID =? AND (SELECT COUNT(*) FROM accounts WHERE accID = ? AND uID = ?) = 1;";
        queryAccountType(phone);
    };
    do
    {
        c = 0;
        printf("\nDo you want to update the information of account number %d to %s? (Y/N): ", id, phone);
        scanf(" %c", &c);
        while (getchar() != '\n')
            ;
    } while (c != 'y' && c != 'Y' && c != 'n' && c != 'N');
    if (c == 'n' || c == 'N')
    {
        printf("\n\nPhone number not updated.\n\n");
        return;
    }
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare_v2(db, update_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, phone, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, id);
    sqlite3_bind_int(stmt, 3, u.id);
    sqlite3_bind_int(stmt, 4, id);
    sqlite3_bind_int(stmt, 5, u.id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Failed to update : %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    printf("%d", rc);
    sqlite3_finalize(stmt);
    printf("\n\nPhone number updated successfully!\n\n");
    return;
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
}