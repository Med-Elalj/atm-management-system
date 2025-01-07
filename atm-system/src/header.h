#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>
#include <stdarg.h>

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
int insertNewUser(sqlite3 *db, char a[50], char pass[50]);
const char *getPassword(struct User *u, sqlite3 *udb);
int getuid(struct User *u, sqlite3 *db);
int validInput(char *input);
int checkUsername(const char *username, sqlite3 *db);

// authentication ui functions
int registerMenu(char a[50], char pass[50], sqlite3 *db);

// system function
void db_createNewAccount(struct Account a, sqlite3 *db);
void db_printAccountsOfUser(struct User u, sqlite3 *db, int t);
void db_updateAccount(sqlite3 * db, int c, char  phone[12], int id, int uid);
void db_makeTransaction(sqlite3 *db, int accID1, int accID2, int amount, int uid);
void db_removeAccount(sqlite3 *db, int accID, int azer);
void db_transferAccount(sqlite3 *db, char uname2[50], int accID1, int azer);

// database functions
void dataBase(int i, sqlite3 **db);
void get_current_date(struct Date *now);
void clearBuffer();

// ui functions
void createNewAcc(struct User u, sqlite3 *db);
void queryAccountType(char type[7]);
void updateAccount(struct User u,sqlite3 *db);
int is_all_digits(const char *str);
void makeTransaction(struct User u, sqlite3 *db);
void removeAccount(struct User u,sqlite3 *db);
void transferAccount(struct User u,sqlite3 *db);
void checkAllAccounts(struct User u,sqlite3 *db,int t);
int confirm(const char *cancel_msg, const char *format, ...);

void mainMenu(struct User u, sqlite3 *db);