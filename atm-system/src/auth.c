#include <termios.h>
#include <sqlite3.h>
#include "header.h"
#include <ctype.h>

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

int loginMenu(char a[50], char pass[50])
{
    int scan = 0;
    struct termios oflags, nflags;

    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Login:");
    do
    {
        clearBuffer();
        scan = scanf("%49s", a);
    } while ((a == "" || a == NULL || !validInput(a))&& scan != -1);

    // disabling echo
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
    {
        perror("tcsetattr");
        return 0;
    }
    // printBytes(pass);
    printf("\n\n\n\n\n\t\t\t\tEnter the password to login:");
    // pass = "";
    do
    {
        clearBuffer();
        scan = scanf("%49s", pass);
    } while (scan != -1 && (pass == "" || pass == NULL || !validInput(pass)));
    // restore terminal
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
    {
        perror("tcsetattr");
        return 0;
    }
    if (scan == -1){
        return 0;
    }
    return 1;
}

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

int registerMenu(char a[50], char pass[50], sqlite3 *db)
{
    int scan = 0;
    struct termios oflags, nflags;

start:
    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\nValid Login contains between 2 and 49 alphabetical charachters\n\t\t\t\t\t Insert Your User Login:");
    do
    {
        clearBuffer();
        scan = scanf("%49s", a);
    } while (scan!=-1 && (a == "" || a == NULL || !validInput(a)));
    if (checkUsername(a, db))
    {
        system("clear");
        printf("Username already exists. Please choose a different one.\n");
        for (int i = 0; i < 50; i++)
        {
            a[i] = '\0';
        };
        goto start;
    }
    system("clear");
    char confirm;
    do
    {
        system("clear");
        printf("\rCurrent User Login: '%s'\n", a);
        printf("Do you want to keep it? (y/n): ");
        clearBuffer();
        scan = scanf(" %c", &confirm);
        if (confirm == 'n' || confirm == 'N')
        {
            goto start;
        }
    } while (scan != -1 && (confirm != 'y' && confirm != 'Y'));

    // disabling echo
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
    {
        perror("tcsetattr");
        return 0;
    }
pass:
    system("clear");
    printf("\n\n\n\n\n\t\t\t\tEnter Your password :");
    do
    {
        clearBuffer();
        scan != scanf("%49s", pass);
    } while (scan!=-1&& (pass == "" || pass == NULL || !validInput(pass)));
    char ver_pass[50];
    printf("\n\n\n\n\n\t\t\t\tConfirm Your password :");
    do
    {
        clearBuffer();
        scan = scanf("%49s", ver_pass);
    } while (scan != -1 && (ver_pass == "" || ver_pass == NULL || !validInput(ver_pass)));
    if (scan == -1) {
        exit(1);
    }
    if (strcmp(pass, ver_pass) != 0)
    {
        printf("Passwords do not match.\n");
        goto pass;
    }
    // restore terminal
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
    {
        perror("tcsetattr");
        return 1;
    }
    // insert new user into database
    const char *insert_sql = "INSERT INTO users(uName, uPassword) VALUES(?,?);";
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    sqlite3_bind_text(stmt, 1, a, -1, SQLITE_TRANSIENT);
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

void getuid(struct User *u,sqlite3 *db) {
    const char *get_uId = "SELECT uId FROM users WHERE uName = ?;";
    sqlite3_stmt *stmt;
    int rc;
    rc = sqlite3_prepare_v2(db, get_uId, -1, &stmt, 0);
    if (rc!= SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    rc = sqlite3_bind_text(stmt, 1, u->name, -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind parameter: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
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
};