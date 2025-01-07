#include "header.h"
#include <sqlite3.h>

void mainMenu(struct User u, sqlite3 *db)
{
    system("clear");
    int option;
    while (1)
    {
        printf("\n\n\t\t======= ATM =======\n\n");
        printf("\n\t\t-->> Feel free to choose one of the options below <<--\n");
        printf("\n\t\t[1]- Create a new account\n");
        printf("\n\t\t[2]- Update account information\n");
        printf("\n\t\t[3]- Check accounts\n");
        printf("\n\t\t[4]- Check list of owned account\n");
        printf("\n\t\t[5]- Make Transaction\n");
        printf("\n\t\t[6]- Remove existing account\n");
        printf("\n\t\t[7]- Transfer ownership\n");
        printf("\n\t\t[8]- Exit\n");
        clearBuffer();
        if (scanf("%d", &option) == -1)
        {
            sqlite3_close(db);
            exit(1);
        };

        switch (option)
        {
        case 1:
            createNewAcc(u, db);
            system("clear");
            break;
        case 2:
            // student TODO : add your **Update account information** function
            updateAccount(u, db);
            system("clear");
            break;
        case 3:
            // student TODO : add your **Check the details of existing accounts** function
            checkAllAccounts(u, db, 0);
            system("clear");
            break;
        case 4:
            // student DONE : add your **Check list of owned accounts** function
            checkAllAccounts(u, db, 1);
            system("clear");
            break;
        case 5:
            // student TODO : add your **Make transaction** function
            makeTransaction(u, db);
            system("clear");
            break;
        case 6:
            // student TODO : add your **Remove existing account** function
            removeAccount(u, db);
            system("clear");
            break;
        case 7:
            // student TODO : add your **Transfer owner** function
            transferAccount(u, db);
            system("clear");
            break;
        case 8:
            return;
            system("clear");
        default:
            system("clear");
            printf("Invalid operation!\n");
        };
    };
};

// return 0 to continue sucessfully
// return 1 to exit program
int initMenu(struct User *u, sqlite3 *db)
{
    int option = 0;
    system("clear");
    while (1)
    {
        printf("\n\n\t\t======= ATM =======\n");
        printf("\n\t\t-->> Feel free to login / register :\n");
        printf("\n\t\t[1]- login\n");
        printf("\n\t\t[2]- register\n");
        printf("\n\t\t[3]- exit\n ");
        clearBuffer();
        if (scanf("%d", &option) == -1)
        {
            return 1;
        };

        switch (option)
        {
        case 1:
            if (loginMenu(u->name, u->password) == 1)
            {
                return 1;
            }
            const char *password = getPassword(u, db);
            if (password == NULL || strcmp(password, "") == 0 || strcmp(u->password, password) != 0)
            {
                printf("\n\nWelcome %s", u->name);
                free((void *)password);
                return getuid(u, db);
            }
            else
            {
                system("clear");
                printf("\nWrong password or User Name\n");
                free((void *)password);
                continue;
            }

        case 2:

            if (registerMenu(u->name, u->password, db) == 0)
            {
                system("clear");
                printf("\nRegistration Successful!");
            }
            else
            {
                system("clear");
                printf("\n\nRegistration Failed!");
            };
            continue;

        case 3:
            system("clear");
            return 1;
        default:
            system("clear");
            printf("Insert a valid operation!\n");
        };
    };
};

int main()
{
    system("clear");
    struct User u;
    sqlite3 *db = NULL;
    // Initialize databases
    dataBase(0, &db);
    dataBase(1, &db);
    dataBase(2, &db);
    dataBase(3, &db);

    // Check if databases were opened successfully
    if (db == NULL)
    {
        fprintf(stderr, "Error opening database.\n");
        return 1;
    }
    printf("Database %p,\n", db);
    // Initialize user and show menu
    switch (initMenu(&u, db))
    {
        case 0:
            mainMenu(u, db);
            sqlite3_close(db);
            return 0;
        case 1:
            sqlite3_close(db);
            return 1;
    }
}
