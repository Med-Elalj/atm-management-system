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
        } while (scanf("%11s", &phone[0]) == 0 || strlen(phone) < 10 || !is_all_digits(phone) || getchar() != 10);
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

void makeTransaction(struct User u, sqlite3 *db) /*(sqlite3 *db, double amount, int accID1, const char *uName, int accID2, int uID2)*/
{
    int amount, accID1, accID2 = 0;
    int scan = 0;
    do
    {
        printf("\nEnter the Origin account's account number : ");
        for (int r = fgetc(stdin); r != EOF && r != 10; r = fgetc(stdin))
            ;
        scan = scanf(" %d", &accID2);
    } while (scan == 0 || accID2 <= 0 && scan != -1);
    do
    {
        printf("\nEnter the Destination account number : ");
        for (int r = fgetc(stdin); r != EOF && r != 10; r = fgetc(stdin))
            ;
        scan = scanf(" %d", &accID1);
    } while (scan == 0 || accID2 <= 0 && scan != -1);
    do
    {
        printf("\nEnter the amount to transfer : ");
        for (int r = fgetc(stdin); r != EOF && r != 10; r = fgetc(stdin))
            ;
        scan = scanf("%d", &amount);
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
        for (int r = fgetc(stdin); r != EOF && r != 10; r = fgetc(stdin))
            ;
        scan = scanf(" %c", &c);
    } while (scan == 0 || c != 'Y' && c != 'y' && c != 'n' && c != 'N' && scan != -1);

    if (c == 'n' || c == 'N' || scan == -1)
    {
        printf("\n\nTransaction cancelled.\n\n");
        return;
    }
    int rc;

    // Begin transaction explicitly
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to start transaction: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_stmt *stmt[3] = {NULL, NULL, NULL};
    // Define the SQL statement with multiple queries
    const char *sql[3] = {"UPDATE accounts SET balance = balance + ? WHERE accID = ? AND uID = (SELECT uId FROM users WHERE uName = ?); ",
                          "UPDATE accounts SET balance = balance - ? WHERE accID = ? AND uID = ?; ",
                          "INSERT INTO transactions (accID1, accID2, amount, timestamp) VALUES (?, ?, ?, CURRENT_TIMESTAMP); "};

    // Prepare the SQL statement
    for (int i = 0; i < 3; i++)
    {

        rc = sqlite3_prepare_v2(db, sql[i], -1, &stmt[i], NULL);
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
            sqlite3_exec(db, "ROLLBACK;", 0, 0, 0); // Rollback if preparing fails
            return;
        }
    }

    // Bind the values to the statement
    sqlite3_bind_int(stmt[0], 1, amount);                        // Bind amount to first "?"
    sqlite3_bind_int(stmt[0], 2, accID1);                        // Bind accID1 to second "?"
    sqlite3_bind_text(stmt[0], 3, u.name, -1, SQLITE_TRANSIENT); // Bind uName to third "?"
    sqlite3_bind_int(stmt[1], 1, amount);                        // Bind amount to fourth "?"
    sqlite3_bind_int(stmt[1], 2, accID2);                        // Bind accID2 to fifth "?"
    sqlite3_bind_int(stmt[1], 3, u.id);                          // Bind uID2 to sixth "?"
    sqlite3_bind_int(stmt[2], 1, accID2);                        // Bind accID2 to seventh "?"
    sqlite3_bind_int(stmt[2], 2, accID1);                        // Bind accID1 to  eighth "?"
    sqlite3_bind_int(stmt[2], 3, amount);                        // Bind amount to tenth "?"

    // Execute the queries (the transaction)
    for (int i = 0; i < 3; i++)
    {
        rc = sqlite3_step(stmt[i]);
        if (rc != SQLITE_DONE)
        {
            fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
            for (int i1 = 0; i1 < 3; i1++)
            {
                sqlite3_finalize(stmt[i1]);
            };
            sqlite3_exec(db, "ROLLBACK;", 0, 0, 0); // Rollback if execution fails
            return;
        }
        printf("Execution %d \n", sqlite3_changes(db));
        if (sqlite3_changes(db) != 1)
        {
            printf("\n\nTransaction failed. Please try again.\n\n");
            for (int i1 = 0; i1 < 3; i1++)
            {
                sqlite3_finalize(stmt[i1]);
            };
            sqlite3_exec(db, "ROLLBACK;", 0, 0, 0); // Rollback if transaction fails
            return;
        }
    };

    // Commit the transaction
    rc = sqlite3_exec(db, "COMMIT;", 0, 0, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to commit transaction: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", 0, 0, 0); // Rollback if commit fails
        return;
    }

    // Finalize the statements
    for (int i1 = 0; i1 < 3; i1++)
    {
        sqlite3_finalize(stmt[i1]);
    };
    printf("\n\nTransaction successful.\n\n");
    return;
}

void removeAccount(struct User u, sqlite3 *db)
{
    int accID = 0;
    int scan = 0;
    do
    {
        printf("\nEnter the account number to delete : ");
        for (int r = fgetc(stdin); r != EOF && r != 10; r = fgetc(stdin))
            ;
        scan = scanf(" %d", &accID);
    } while (scan == 0 || accID <= 0 && scan != -1);

    char c = 0;
    do
    {
        printf("\nDo you want to proceed with the deletion? (Y/N): ");
        for (int r = fgetc(stdin); r != EOF && r != 10; r = fgetc(stdin))
            ;
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