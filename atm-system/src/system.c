#include "header.h"
#include <sqlite3.h>
#include <ctype.h>

const char *RECORDS = "./data/records.txt";

void createNewAcc(struct User u, sqlite3 *db)
{
    struct Account a;
    a.uId = u.id;
    a.creationDate = 1731679260 / 86400;
    int scan = 0;
noAccount:
    queryAccountType(a.accountType);
    system("clear");
    printf("\t\t\t===== New Account =====\n");
    time_t azer = a.creationDate * 86400;
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
    clearBuffer();
    int scan = scanf(" %c", &c);
    if (scan == -1)
    {
        exit(2);
    }
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
    int scan = 0;
    char phone[12];
    const const char *update_sql;
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
        update_sql = "UPDATE accounts SET phonenumber =? WHERE accID =? AND uID =? AND (SELECT COUNT(*) FROM accounts WHERE accID = ? AND uID = ?) = 1;";
        do
        {
            printf("\nEnter the new phone number: ");
            clearBuffer();
            scan = scanf("%11s", &phone[0]);
        } while (scan == 0 || scan != -1 && (strlen(phone) < 10 || !is_all_digits(phone) || getchar() != 10));
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
        clearBuffer();
        scan = scanf(" %c", &c);
    } while (c != 'y' && c != 'Y' && c != 'n' && c != 'N' && scan != -1);

    if (c == 'n' || c == 'N' || scan == -1)
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

    if (sqlite3_changes(db) == 0)
    {
        printf("\nErr : No changes were made.\n");
    }
    else
    {
        printf("\n\nInformation updated successfully!\n\n");
    }
    sqlite3_finalize(stmt);
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

    int rc;
    sqlite3_stmt *stmt = NULL;
    // Define the SQL statement with multiple queries
    const char *sql = "INSERT INTO transactions (accID1, accID2, amount) SELECT ?, ?, ? WHERE EXISTS (SELECT 1 FROM accounts WHERE accID = ? AND uID = ?);";

    // Prepare the SQL statement

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }

    // Bind the values to the statement
    sqlite3_bind_int(stmt, 1, accID1); // Bind accID1 to first "?"
    sqlite3_bind_int(stmt, 2, accID2); // Bind accID2 to second "?"
    sqlite3_bind_int(stmt, 3, amount); // Bind amount to third "?"
    sqlite3_bind_int(stmt, 4, accID1); // Bind accID1 to fourth "?"
    sqlite3_bind_int(stmt, 5, u.id);   // Bind u.id to fifth "?"

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));

        sqlite3_finalize(stmt);
        return;
    }
    printf("Execution succeeded\n");
    sqlite3_finalize(stmt);
    return;
};

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
    int rc;
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to start transaction: %s\n", sqlite3_errmsg(db));
        return;
    }
    // Delete the account from the database
    sqlite3_stmt *stmt1 = NULL;
    const char *sql1 = "DELETE FROM accounts WHERE accID =? AND uID =?;";
    rc = sqlite3_prepare_v2(db, sql1, -1, &stmt1, NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);
        return;
    }
    sqlite3_bind_int(stmt1, 1, accID);
    sqlite3_bind_int(stmt1, 2, u.id);
    rc = sqlite3_step(stmt1);
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt1);
        sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);
        return;
    };
    if (sqlite3_changes(db) == 0)
    {
        printf("\n\nNo account found with the given account number.\n\n");
        sqlite3_finalize(stmt1);
        sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);
        return;
    };
    rc = sqlite3_exec(db, "COMMIT;", 0, 0, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to commit transaction: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);
        return;
    };
    sqlite3_finalize(stmt1);
    printf("\n\nAccount deleted successfully.\n\n");
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
    sqlite3_stmt *stmt1 = NULL;
    const char *sql1 = "UPDATE accounts "
                       "SET uID = (SELECT uID FROM users WHERE uName = ?) "
                       "WHERE accID = ? AND uID = ?;";
    sqlite3_prepare_v2(db, sql1, -1, &stmt1, NULL);
    sqlite3_bind_text(stmt1, 1, uname2, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt1, 2, accID1);
    sqlite3_bind_int(stmt1, 3, u.id);
    int rc = sqlite3_step(stmt1);
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt1);
        return;
    }
    if (sqlite3_changes(db) == 0)
    {
        printf("\n\nNo account found with the given account number or user does not exist.\n\n");
        sqlite3_finalize(stmt1);
        return;
    };
    sqlite3_finalize(stmt1);
    printf("\n\nAccount transferred successfully.\n\n");
    return;
};