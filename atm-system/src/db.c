#include <sqlite3.h>
#include <stdio.h>

int execute_sql(sqlite3 *db, const char *sql) {
    char *errMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    }
    return rc;
}

// Function to open a database and ensure the table exists
sqlite3* dataBase(int i) {
    sqlite3 *db;
    int rc;
    const char* dbName = (i == 0) ? "data/users.db" : "data/records.db";
    
    // Open the database
    rc = sqlite3_open(dbName, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    // If the parameter is 0, ensure the "users" table exists
    const char *createTableSQL;
    if (i == 0) {
        createTableSQL = 
            "CREATE TABLE IF NOT EXISTS users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "username TEXT NOT NULL UNIQUE, "
            "password TEXT NOT NULL);";
    } else {
        createTableSQL = 
            "CREATE TABLE IF NOT EXISTS accounts ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "user_id INTEGER NOT NULL, "
            "user_name TEXT NOT NULL, "
            "account_id INTEGER NOT NULL, "
            "date_of_creation DATE NOT NULL, "
            "country TEXT NOT NULL, "
            "phone_number TEXT NOT NULL, "
            "balance REAL NOT NULL, "
            "type_of_account TEXT NOT NULL);";
    }
        
        rc = execute_sql(db, createTableSQL);
        if (rc != SQLITE_OK) {
            sqlite3_close(db);
            return NULL;
        }
    
    return db;
}
#include "header.h"
#include <time.h>

void get_current_date(struct Date *now) {
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

void clearBuffer() {
    int c;
    // Read and discard characters until a newline or EOF is found
    while ((c = getchar()) != '\n' && c != EOF) {
        // No operation, just discard characters
    }
}