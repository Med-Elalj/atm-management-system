#include "header.h"
#include <sqlite3.h>

sqlite3 *udb;
sqlite3 *adb;

void mainMenu(struct User u)
{
    int option;
    system("clear");
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
        createNewAcc(u);
        break;
    case 2:
        // student TODO : add your **Update account information** function
        // here
        break;
    case 3:
        // student TODO : add your **Check the details of existing accounts** function
        // here
        break;
    case 4:
        checkAllAccounts(u);
        break;
    case 5:
        // student TODO : add your **Make transaction** function
        // here
        break;
    case 6:
        // student TODO : add your **Remove existing account** function
        // here
        break;
    case 7:
        // student TODO : add your **Transfer owner** function
        // here
        break;
    case 8:
        exit(1);
        break;
    default:
        printf("Invalid operation!\n");
    }
};

int initMenu(struct User *u)
{
    int r = 0;
    int option;
    // system("clear");
    printf("\n\n\t\t======= ATM =======\n");
    printf("\n\t\t-->> Feel free to login / register :\n");
    printf("\n\t\t[1]- login\n");
    printf("\n\t\t[2]- register\n");
    printf("\n\t\t[3]- exit\n");
    while (!r)
    {
        scanf("%d", &option);
        switch (option)
        {
        case 1:
            r = 1;
            r = loginMenu(u->name, u->password);
            const char *password = getPassword(u, udb);
            if (strcmp(password, "") == 0)
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
                printf("\nWrong password or User Name\n");
                free((void *)password);
                return 1;
            }
            free((void *)password);
            return 0;
        case 2:
            // // student TODO : add your **Registration** function
            // // here
            if (registerMenu(u->name, u->password, udb) == 0)
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
    struct User u;

    // Initialize databases
    udb = dataBase(0);
    adb = dataBase(1);

    // Check if databases were opened successfully
    if (udb == NULL || adb == NULL)
    {
        fprintf(stderr, "Error opening databases.\n");
        return 1; // Exit with an error code
    }

    // Initialize user and show menu
    for (; initMenu(&u) > 0;)
    {
    };
    mainMenu(u);

    // Close databases
    sqlite3_close(udb);
    sqlite3_close(adb);

    return 0;
}
