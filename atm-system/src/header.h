#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

struct Date
{
    int month, day, year;
};

// all fields for each record of an account
struct Record
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
int registerMenu(char a[50], char pass[50]);
void getPassword(struct User *u, sqlite3 *udb,char password[50]);
int registerUser(char a[50], char pass[50]);

// system function
void createNewAcc(struct User u);
void mainMenu(struct User u);
void checkAllAccounts(struct User u);

// database functions
sqlite3* dataBase(int i);
void get_current_date(struct Date *now);
void clearBuffer();

// cli functions
int isalphanum(int c);
void *getinput(char *in, int max);