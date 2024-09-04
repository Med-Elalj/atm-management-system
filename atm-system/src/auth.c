#include <termios.h>
#include <sqlite3.h>
#include "header.h"

char *USERS = "./data/users.txt";
void printBytes(const char *str) {
    int c ;
    while ((c = getchar()) != '\n' && c != EOF) {
        printf("hapend'%d'", c);
    }
    return;
}
int loginMenu(char a[50], char pass[50])
{
    struct termios oflags, nflags;

    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Login:");
    do {
        clearBuffer();
        scanf("%s", a);
    } while (a == "" || a ==NULL);

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
    do {
        clearBuffer();
        scanf("%s", pass);
    } while (pass == "" || pass ==NULL);

    // restore terminal
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
    {
        perror("tcsetattr");
        return 0;
    }
    return 1;
}

const char *getPassword(struct User *u, sqlite3 *db)
{
    const char *select_sql = "SELECT password FROM users WHERE username = ?;";
    sqlite3_stmt *stmt;
    const unsigned char *password = NULL;
    int rc;
    char *test;

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
        password = sqlite3_column_text(stmt, 0);
        if (password)
        {
            snprintf(test, sizeof(test), "%s", password);
            sqlite3_finalize(stmt);
            printf("Password for user '%s'", test);
            printf("Password for user '%s' is '%s' and ,'%s'\n", u->name, u->password, test);
            printf("Password matched,%d\n", strcmp((const char *)u->password, test));
            return test;
        }
    }
    else if (rc == SQLITE_DONE)
    {
        printf("No user found with username '%s'.\n", u->name);
    }
    else
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    }
    if (stmt != NULL)
    {
        sqlite3_finalize(stmt);
    }
    printf("Password for user '%s', '%s'\n", u->name, "");
    return "";
}