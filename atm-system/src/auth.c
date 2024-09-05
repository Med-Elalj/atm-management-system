#include <termios.h>
#include <sqlite3.h>
#include "header.h"

int loginMenu(char a[50], char pass[50])
{
    struct termios oflags, nflags;
    // Clear screen (for Unix-like systems)
    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\n");
    while (1)
    {
        printf("\n\t\t\t\t\t User Login:");
        scanf("%49s", a); // Read string and avoid buffer overflow
        clearBuffer();

        // Check for empty input
        if (strcmp(a, "") != 0)
        {
            break;
        }
    }

    // disabling echo
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
    {
        perror("tcsetattr");
        printf("%s:%s:", a, pass);
        return 1;
    }
    // printBytes(pass);
    while (1)
    {
        // printf("\n\t\t\t\t\t User Login:");
        printf("\n\n\n\n\n\t\t\t\tEnter the password to login:");
        scanf("%49s", pass); // Read string and avoid buffer overflow
        clearBuffer();

        // Check for empty input
        if (strcmp(pass, "") != 0)
        {
            break;
        }
    }
    // restore terminal
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
    {
        perror("tcsetattr");
        return 1;
    }
    return 0;
}

void getPassword(struct User *u, sqlite3 *db, char test[50])
{
    const char *select_sql = "SELECT password FROM users WHERE username = ?;";
    sqlite3_stmt *stmt;
    const unsigned char *password = NULL;
    int rc;
    printf("%s\n", "reached 1");
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return; // Return NULL to indicate failure
    }
    printf("%s\n", "reached 2");
    sqlite3_bind_text(stmt, 1, u->name, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        password = sqlite3_column_text(stmt, 0);
        if (password)
        {
            printf("%s\n%s\n", "reached 1", password);
            strncpy(test, (const char *)password, 49);
            test[49] = '\0'; //

            if (stmt != NULL)
            {
                sqlite3_finalize(stmt);
            }
            printf("Password for user '%s'", test);
            printf("Password for user '%s' is '%s' and ,'%s'\n", u->name, u->password, test);
            printf("Password matched,%d\n", strcmp((const char *)u->password, test));
            return;
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
    return;
}

int registerUser(char a[50], char pass[50])
{
    struct termios oflags, nflags;
    // char user[50], pass[50];
    // Clear screen (for Unix-like systems)
    start:
    system("clear");
    printf("note: feilds are limited to the first space char\n\n\n\t\t\t\t   Bank Management System\n");
    while (1)
    {
        printf("\n\t\t\t\t\t New User Login:");
        scanf("%49s", a); // Read string and avoid buffer overflow
        clearBuffer();

        // Check for empty input
        if (strcmp(a, "") != 0)
        {
            break;
        }
    }

    system("clear");
    // disabling echo
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    printf("note: feilds are limited to the first space char\n\n\n\t\t\t\t   Bank Management System\n");
    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
    {
        perror("tcsetattr");
        printf("%s:%s:", a, pass);
        return 1;
    }
    // printBytes(pass);
    while (1)
    {
        printf("\n\t\tUser Login: %s\n\nif you want to change it type cancel now",a);
        printf("\n\n\n\n\n\t\t\t\tEnter Password:");
        scanf("%49s", pass); // Read string and avoid buffer overflow
        clearBuffer();
        if (strcmp(pass ,"cancel")==0){
            goto start;
        }

        // Check for empty input
        if (strcmp(pass, "") != 0)
        {
            while (1)
            {
                system("clear");
                printf("note: feilds are limited to the first space char\n\n\n\t\t\t\t   Bank Management System\n");
                char passconf[50];
                // printf("\n\t\t\t\t\t User Login:");
                printf("\n\n\n\n\n\t\t\t\tConfirm Password:");
                scanf("%49s", passconf); // Read string and avoid buffer overflow
                clearBuffer();

                // Check for empty input
                if (strcmp(passconf, pass) == 0)
                {
                    goto done;
                }
                else
                {
                    system("clear");
                    printf("note: feilds are limited to the first space char\n\n\n\t\t\t\t   Bank Management System\n");
                    printf("\n\n\n\t\t\t\tPasswords do not match, please try again.\n");
                    break;
                }
            }
        }
    }
    done:
    // restore terminal
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
    {
        perror("tcsetattr");
        return 1;
    }
    return 0;
}
