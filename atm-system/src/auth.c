#include <sqlite3.h>
#include "header.h"
#include <ctype.h>
#include <string.h>

char *USERS = "./data/users.txt";

void printBytes(const char *str)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        printf("hapend'%d'", c);
    }
    return;
};

// valid input is regex /[\w\d ]{2,49}/
int validInput(char *input)
{
    int i;
    for (i = 0; i < strlen(input); i++)
    {
        if (!isalpha(input[i])|| input[i]==' ')
        {
            return 0;
        }
    }
    return strlen(input) >= 2 && strlen(input) <= 49;
};


const char *getPassword(struct User *u, sqlite3 *db)
{
    const char *select_sql = "SELECT uPassword FROM users WHERE uName = ?;";
    sqlite3_stmt *stmt;
    const unsigned char *password_column = NULL;
    int rc;
    char *password = calloc(50, sizeof(char));

    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return NULL; // Return NULL to indicate failure
    }

    sqlite3_bind_text(stmt, 1, u->name, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        password_column = sqlite3_column_text(stmt, 0);
        if (password_column)
        {
            snprintf(password, sizeof(password), "%s", password_column);
            sqlite3_finalize(stmt);
            return password;
        }
    }
    if (stmt != NULL)
    {
        sqlite3_finalize(stmt);
    }
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    };
    return NULL;
}

int checkUsername(const char *username, sqlite3 *db)
{
    const char *select_sql = "SELECT COUNT(*) FROM users WHERE uName =?;";
    sqlite3_stmt *stmt;
    int rc;
    int count;

    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return count > 0;
    }
    else
    {
        // Handle errors or no rows returned
        fprintf(stderr, "Failed to execute query: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0; // Return 0 if the query failed
    };
};

int insertNewUser(sqlite3 *db, char uName[50], char pass[50])
{
    // insert new user into database
    const char *insert_sql = "INSERT INTO users (uName, uPassword) VALUES (?,?);";
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    sqlite3_bind_text(stmt, 1, uName, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pass, -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Failed to insert user: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 1;
    }
    sqlite3_finalize(stmt);
    return 0;
};

int getuid(struct User *u,sqlite3 *db) {
    const char *get_uId = "SELECT uId FROM users WHERE uName = ?;";
    sqlite3_stmt *stmt;
    int rc;
    rc = sqlite3_prepare_v2(db, get_uId, -1, &stmt, 0);
    if (rc!= SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    rc = sqlite3_bind_text(stmt, 1, u->name, -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind parameter: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 1;
    }
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        // If we got a row, fetch the uId
        u->id = sqlite3_column_int(stmt, 0);
    } else {
        // If no row is returned, handle the error or case where the user doesn't exist
        if (rc == SQLITE_DONE) {
            fprintf(stderr, "User not found: %s\n", u->name);
        } else {
            fprintf(stderr, "Failed to execute query: %s\n", sqlite3_errmsg(db));
        }
    }

    // Finalize the statement to free resources
    sqlite3_finalize(stmt);
    return 0;
};