#ifndef MAIN
#define MAIN

#define TRUE 1
#define FALSE 0

typedef struct 
{
    char _Name[200];
    char _Password[200];
    char _realName[200];
    char _Datebirth[50];
    char _CCCD[20];
    char _Role[20];
} User;

// ======================Console===================
void SetConsole();
void gotoxy(int, int);
void showCursor();
void hideCursor();


// =====================Hàm chính====================
void Title(int, int);
void Loading(int, int);

#define Nhappass    1
#define Changepass  2

void showUI_Login(int, int, int);
int changePassword(User *_User, int Soluong)
int saveAccount(User *_User, int Soluong);
int getAccount(User *_User, int *Soluong);

// ====================Hàm chung==================
void trim(char *_String);
int lenString(const char *_String)


#endif