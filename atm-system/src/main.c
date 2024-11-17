#include "header.h"
#include <sqlite3.h>

void mainMenu(struct User u, sqlite3 *db)
{
    system("clear");
    while (1)
    {
        printf("%d %s\n", u.id, u.name);
        int option;
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
        scanf("%d", &option);

        switch (option)
        {
        case 1:
            createNewAcc(u, db);
            break;
        case 2:
            // student TODO : add your **Update account information** function
            updateAccount(u, db);
            // here
            break;
        case 3:
            // student TODO : add your **Check the details of existing accounts** function
            // here
            checkAllAccounts(u, db,0);
            // system("clear");
            break;
        case 4:
            // student DONE : add your **Check list of owned accounts** function
            checkAllAccounts(u, db,1);
            printf("\n\t");
            // system("clear");
            break;
        case 5:
            // student TODO : add your **Make transaction** function
            makeTransaction(u, db);
            // here
            break;
        case 6:
            // student TODO : add your **Remove existing account** function
            removeAccount(u, db);
            // here
            break;
        case 7:
            // student TODO : add your **Transfer owner** function
            transferAccount(u, db);
            // here
            break;
        case 8:
            exit(1);
            // break;
        default:
            printf("Invalid operation!\n");
        };
    };
};

int initMenu(struct User *u, sqlite3 *db)
{
    int r = 0;
    int option = 5;
    printf("%d\n", option);
    // system("clear");
    printf("\n\n\t\t======= ATM =======\n");
    printf("\n\t\t-->> Feel free to login / register :\n");
    printf("\n\t\t[1]- login\n");
    printf("\n\t\t[2]- register\n");
    printf("\n\t\t[3]- exit\n ");
    while (!r)
    {
        if (scanf("%d", &option) == -1)
        {
            exit(1);
        };
        switch (option)
        {
        case 1:
            r = 1;
            r = loginMenu(u->name, u->password);
            const char *password = getPassword(u, db);
            if (password == NULL || (password, "") == 0)
            {
                r = 0;
            };
            if (r != 1)
            {
                printf("\n\nLogin Failed!");
                free((void *)password);
                return 1;
            };
            if (strcmp(u->password, password) == 0)
            {
                printf("\n\nWelcome %s", u->name);
            }
            else
            {
                system("clear");
                printf("\nWrong password or User Name\n");
                free((void *)password);
                return 1;
            }
            free((void *)password);
            return 0;
        case 2:
            // // student TODO : add your **Registration** function
            // // here
            if (registerMenu(u->name, u->password, db) == 0)
            {
                printf("\nRegistration Successful!");
            }
            else
            {
                printf("\n\nRegistration Failed!");
            };
            r = 0;
            return 0;
        case 3:
            system("clear");
            exit(1);
            return 0;
        default:
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
    dataBase(0,&db);
    dataBase(1,&db);
    dataBase(2,&db);

    // Check if databases were opened successfully
    if (db == NULL)
    {
        fprintf(stderr, "Error opening database.\n");
        return 1; // Exit with an error code
    }
    printf("Database %p,\n",db);
    // Initialize user and show menu
    for (; initMenu(&u, db) > 0;)
    {
    };
    getuid(&u, db);
    mainMenu(u, db);
    // u.id = 1;
    // checkAllAccounts(u,db);
    // printf("Database\n");
    // Close databases
    sqlite3_close(db);
    // sqlite3_close(db);

    return 0;
}
