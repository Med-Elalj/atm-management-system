#include <sqlite3.h>
#include <stdio.h>

int execute_sql(sqlite3 *db, const char *sql)
{
    char *errMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    }
    return rc;
}

// Function to open a database and ensure the table exists
sqlite3 *dataBase(int i)
{
    sqlite3 *db;
    int rc;
    const char *dbName = "data/main.db";

    // Open the database
    rc = sqlite3_open(dbName, &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    // If the parameter is 0, ensure the "users" table exists
    const char *createTableSQL;
    if (i == 0)
    {
        createTableSQL =
            "CREATE TABLE IF NOT EXISTS users ("
            "uId INTEGER PRIMARY KEY AUTOINCREMENT, "
            "uName VARCHAR(50) NOT NULL UNIQUE, "
            "uPassword VARCHAR(50) NOT NULL);";
    }
    else
    {
        createTableSQL =
            "CREATE TABLE IF NOT EXISTS accounts ("
            "accID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "uID INTEGER NOT NULL, "
            "creation_Date INTEGER NOT NULL, "
            "country VARCHAR(50) NOT NULL, "
            "phonenumber VARCHAR(20) NOT NULL, "
            "acc_type VARCHAR(7) CHECK(acc_type IN ('savings', 'fixed01', 'fixed02', 'fixed03', 'current')) NOT NULL, "
            "balance DECIMAL(15, 2), "
            "FOREIGN KEY(uID) REFERENCES users(uId) "
            ");";
    }

    rc = execute_sql(db, createTableSQL);
    printf("test:%d\n", rc);
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db);
        return NULL;
    }

    return db;
}
#include "header.h"
#include <time.h>

void get_current_date(struct Date *now)
{
    time_t t;
    struct tm *tm_info;

    // Get the current time
    time(&t);

    // Convert to local time
    tm_info = localtime(&t);

    // Extract month, day, and year
    now->month = tm_info->tm_mon + 1;
    now->day = tm_info->tm_mday;
    now->year = tm_info->tm_year + 1900;
}

void clearBuffer()
{
    int c;
    // Read and discard characters until a newline or EOF is found
    while ((c = getchar()) != '\n' && c != EOF)
    {
        // No operation, just discard characters
    }
}