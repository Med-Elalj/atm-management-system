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
int validInput(char *input);

// system function
void createNewAcc(struct User u, sqlite3 *db);
void mainMenu(struct User u, sqlite3 *db);
void checkAllAccounts(struct User u,sqlite3 *db,int t);
void queryAccountType(char type[7]);
void updateAccount(struct User u,sqlite3 *db);
int is_all_digits(const char *str);
void makeTransaction(struct User u, sqlite3 *db);
void removeAccount(struct User u,sqlite3 *db);

// database functions
void dataBase(int i, sqlite3 **db);
void get_current_date(struct Date *now);
void clearBuffer();