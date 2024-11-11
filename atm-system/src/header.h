#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>

struct Date
{
    int month, day, year;
};


struct Account
{
    int accId;
    int uId;
    int creationDate;
    char country[50];
    int phone;
    char accountType[7];
    int balance;
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
void getuid(struct User *u, sqlite3 *db);

// system function
void createNewAcc(struct User u, sqlite3 *db);
void mainMenu(struct User u, sqlite3 *db);
void checkAllAccounts(struct User u,sqlite3 *db);
void queryAccountType(char type[7]);

// database functions
sqlite3* dataBase(int i);
void get_current_date(struct Date *now);
void clearBuffer();