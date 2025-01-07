#include "header.h"
#include <sqlite3.h>
#include <termios.h>

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
    } while ((a == "" || a == NULL || !validInput(a)) && scan != -1);

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
    printf("\n\n\n\n\n\t\t\t\tEnter the password to login:");
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
    if (scan == -1)
    {
        return 0;
    }
    return 1;
}

int registerMenu(char uName[50], char pass[50], sqlite3 *db)
{
    int scan = 0;
    struct termios oflags, nflags;
    int azer = 1;

    while (azer)
    {
        system("clear");
        printf("\n\n\n\t\t\t\t   Bank Management System\nValid Login contains between 2 and 49 alphabetical charachters\n\t\t\t\t\t Insert Your User Login:");
        do
        {
            clearBuffer();
            scan = scanf("%49s", uName);
        } while (scan != -1 && (uName == "" || uName == NULL || !validInput(uName)));
        if (checkUsername(uName, db))
        {
            system("clear");
            printf("Username already exists. Please choose a different one.\n");
            for (int i = 0; i < 50; i++)
            {
                uName[i] = '\0';
            };
            continue;
        }
        system("clear");
        if (confirm("", "\rCurrent User Login: '%s'\nDo you want to keep it? (y/n): ", uName))
        {
            break;
        }
    }
    // disabling echo
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
    {
        perror("tcsetattr");
        return 1;
    }

    system("clear");
    while (1)
    {
        printf("\n\n\n\n\n\t\t\t\tEnter Your password :");
        do
        {
            clearBuffer();
            scan != scanf("%49s", pass);
        } while (scan != -1 && (pass == "" || pass == NULL || !validInput(pass)));
        char ver_pass[50];
        printf("\n\n\n\n\n\t\t\t\tConfirm Your password :");
        do
        {
            clearBuffer();
            scan = scanf("%49s", ver_pass);
        } while (scan != -1 && (ver_pass == "" || ver_pass == NULL || !validInput(ver_pass)));
        if (scan == -1)
        {
            return 1;
        }
        if (strcmp(pass, ver_pass) != 0)
        {
            system("clear");
            printf("Passwords do not match.\n");
            continue;
        }
        break;
    }
    // restore terminal
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
    {
        perror("tcsetattr");
        return 1;
    }
    return insertNewUser(db, uName, pass);
}
