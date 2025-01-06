#include "header.h"
#include <sqlite3.h>
#include <ctype.h>

const char *RECORDS = "./data/records.txt";

void db_createNewAccount(struct Account a, sqlite3 *db)
{
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
}



void db_printAccountsOfUser(struct User u, sqlite3 *db, int t)
{
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
}

void db_updateAccount(sqlite3 * db, int c, char  phone[12], int id, int uid)
{
    const const char *update_sql;
    if (c == 1)
    {
        update_sql = "UPDATE accounts SET phonenumber =? WHERE accID =? AND uID =? AND (SELECT COUNT(*) FROM accounts WHERE accID = ? AND uID = ?) = 1;";
    } 
    else if (c==2)
    {
        update_sql = "UPDATE accounts SET country =? WHERE accID =? AND uID =? AND (SELECT COUNT(*) FROM accounts WHERE accID = ? AND uID = ?) = 1;";
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
    sqlite3_bind_int(stmt, 3, uid);
    sqlite3_bind_int(stmt, 4, id);
    sqlite3_bind_int(stmt, 5, uid);
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
}
;


void db_makeTransaction(sqlite3 *db, int accID1, int accID2, int amount, int uid)
{
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
    sqlite3_bind_int(stmt, 5, uid);    // Bind uid to fifth "?"

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

void db_removeAccount(sqlite3 *db, int accID, int uid)
{
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
    sqlite3_bind_int(stmt1, 2, uid);
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

void db_transferAccount(sqlite3 *db, char uname2[50], int accID1, int uid)
{
    sqlite3_stmt *stmt1 = NULL;
    const char *sql1 = "UPDATE accounts "
                       "SET uID = (SELECT uID FROM users WHERE uName = ?) "
                       "WHERE accID = ? AND uID = ?;";
    sqlite3_prepare_v2(db, sql1, -1, &stmt1, NULL);
    sqlite3_bind_text(stmt1, 1, uname2, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt1, 2, accID1);
    sqlite3_bind_int(stmt1, 3, uid);
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