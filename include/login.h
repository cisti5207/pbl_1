#ifndef LOGIN_H
#define LOGIN_H

#include "raylib.h"

#define MAX_USERNAME_LENGTH 256
#define MAX_PASSWORD_LENGTH 256
#define MAX_REALNAME_LENGTH 256
#define MAX_CCCD_LENGTH 256
#define MAX_DOB_LENGTH 256
#define MAX_ROLE_LENGTH 256

#define LEN_SHOW_USERNAME 29
#define LEN_SHOW_PASSWORD 49
#define LEN_SHOW_REALNAME 33
#define LEN_SHOW_CCCD 29
#define LEN_SHOW_DOB 21
#define LEN_SHOW_ROLE 13

#define MAX_INPUT 256

#define QUANTITY_FONT 100
#define FONT_SIZE 18
#define Arial "font/arial/arial.ttf"
#define ArialBold "font/arial/arialbd.ttf"
#define Cooper "font/cooper/COOPBL.ttf"
#define Times "font/times/times.ttf"
#define Impact "font/impact/impact.ttf"
#define Roboto "font/roboto/Roboto-Regular.ttf"
#define Roboto_Semibold "font/roboto/Roboto-Semibold.ttf"
#define Gill "font/gill/gillubcd.ttf"


#define BACKGROUND_IMG "img/image2.png"
#define FORM_LOGIN_IMG "img/image4.png"
#define WELCOME_TEXT "Welcome to HoanHoangLIB_DUT!"

#define LOGIN_TEXT "LOGIN"
#define LoginUserNAME_TEXT "Username:"
#define LoginPassword_TEXT "Password:"
#define LoginButton_TEXT "Login"
#define LoginRegisterText "Don't have an account? Register"
#define LoginForgotPasswordText "Forgot password?"
#define LoginSuccessText "Login successful!"
#define LoginFailedText "Invalid username or password. Please try again."
#define LoginEmptyText "Please enter both username and password."
#define LoginButtonText "Login"
#define ICON_USERNAME "img/icon1.png"
#define ICON_PASSWORD "img/icon2.png"
#define SHOW_PASSWORD "img/icon3.png"

#define REGISTER_TEXT "REGISTER"
#define FORGOT_PASSWORD_TEXT "FORGOT PASSWORD"

int InitLogin();

typedef struct Account {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char realName[MAX_REALNAME_LENGTH];
    char cccd[MAX_CCCD_LENGTH];
    char dateOfBirth[MAX_DOB_LENGTH];
    char role[MAX_ROLE_LENGTH];
} Account;

typedef struct Node {
    Account *account;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct Node *AccountList;
typedef struct Node *AccountPosition;

typedef struct {
    Rectangle box;
    char text[MAX_INPUT];
    int length;
    bool isFocused;
} InputBox;


typedef enum {
    LOGIN,
    REGISTER,
    FORGOT_PASSWORD
} LoginState;

typedef enum {
    LOGIN_IDLE,
    LOGIN_SUCCESS,
    LOGIN_EMPTY,
    LOGIN_FAILED
} LoginResult;

typedef enum {
    FORGOT_PASSWORD_IDLE,
    FORGOT_PASSWORD_SUCCESS,
    FORGOT_PASSWORD_EMPTY,
    FORGOT_PASSWORD_INVALID
} ForgotPasswordResult;

typedef enum{
    IDLE = 0,
    TRUE,
    FALSE,
    NOT_FOUND,
    FOUND,
    FAILED
} CheckResult;

void GetSize();

void DrawBackground();
void DrawForm();
void DrawLoginForm(InputBox *loginBox, InputBox *passwordBox, AccountList accounts, int accountCount, Texture2D IconUsername, Texture2D IconPassword, Texture2D ShowPasswordIcon);
void DrawLoginUsername(InputBox *LoginBox, Texture2D IconUsername);
void DrawLoginPassword(InputBox *PasswordBox, Texture2D IconPassword, Texture2D ShowPasswordIcon);
void DrawIcon(Rectangle box, Texture2D icon);

LoginResult CheckLogin(AccountList accounts, const char *username, const char *password);


int InsertAccount(AccountList pL, Account *e, AccountPosition p);
AccountList CreateHeaderNode();
int GetAccountData(const char *filename, AccountList accounts, int *accountCount);
int SaveAccountData(const char *filename, AccountList accounts, int accountCount);
int lenStringUTF8(const char *str);
void trim(char *str);
Font SetFontUTF8(const char *fontPath, int fontSize);
void DeleteLastChar(InputBox *input);
void UpdateInputBox(InputBox *input);
void DrawIcon(Rectangle box, Texture2D icon);
#endif // LOGIN