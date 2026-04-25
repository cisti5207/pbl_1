#ifndef MAIN
#define MAIN

#define TRUE 1
#define FALSE 0

typedef struct 
{
    char _Name[200];
    char _Password[200];
    char _readName[200];
    char _Datebirth[50];
    char _CCCD[20];
    char _Role[20];
} User;

void SetConsole();
void gotoxy(int, int);
void showCursor();
void hideCursor();

void Title(int, int);
void Loading(int, int);

#define Nhappass    1
#define Changepass  2

void showUI_Login(int, int, int);



#endif