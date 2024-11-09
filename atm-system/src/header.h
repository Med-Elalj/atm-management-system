#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

struct Date
{
    int month, day, year;
};


struct Transaction
{
    int id;
    int userId;

};

struct Account
{
    int id;
    int userId;
    char name[100];
    char country[100];
    int phone;
    char accountType[10];
    int accountNbr;
    double amount;
    struct Date deposit;
    struct Date withdraw;
};

struct User
{
    int id;
    char name[50];
    char password[50];
};

// authentication functions
int loginMenu(char a[50], char pass[50]);
int registerMenu(char a[50], char pass[50], sqlite3 *db);
const char *getPassword(struct User *u, sqlite3 *udb);

// system function
void createNewAcc(struct User u);
void mainMenu(struct User u);
void checkAllAccounts(struct User u);

// database functions
sqlite3* dataBase(int i);
void get_current_date(struct Date *now);
void clearBuffer();