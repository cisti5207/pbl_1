#ifndef LOGIN_H
#define LOGIN_H

#include "raylib.h"
#include "libmanage.h"

#define MAX_LOGIN_USERNAME_LENGTH 256
#define MAX_LOGIN_PASSWORD_LENGTH 256
#define MAX_LOGIN_REALNAME_LENGTH 256
#define MAX_LOGIN_CCCD_LENGTH 256
#define MAX_LOGIN_DOB_LENGTH 256
#define MAX_LOGIN_ROLE_LENGTH 256

#define LEN_SHOW_USERNAME 29
#define LEN_SHOW_PASSWORD 49
#define LEN_SHOW_REALNAME 33
#define LEN_SHOW_CCCD 29
#define LEN_SHOW_DOB 21
#define LEN_SHOW_ROLE 13

#define LOGIN_BACKGROUND_IMG "img/image2.png"
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



typedef struct Account {
    char username[MAX_LOGIN_USERNAME_LENGTH];
    char password[MAX_LOGIN_PASSWORD_LENGTH];
    char realName[MAX_LOGIN_REALNAME_LENGTH];
    char cccd[MAX_LOGIN_CCCD_LENGTH];
    char dateOfBirth[MAX_LOGIN_DOB_LENGTH];
    char role[MAX_LOGIN_ROLE_LENGTH];
} Account;

typedef struct Node {
    Account *account;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct Node *AccountList;
typedef struct Node *AccountPosition;

typedef enum {
    LOGIN_IDLE,
    LOGIN_SUCCESS,
    LOGIN_EMPTY,
    LOGIN_FAILED
} LoginResult;

int InitLogin(Account *account_return);

void LOGIN_DrawBackground(Size LoginSize, Texture2D Img_Bg);
void LOGIN_DrawForm(Rectangle Form, Texture2D Img_FormLogin, Font *_Font);
void LOGIN_DrawLoginForm(InputBox *_loginBox, InputBox *_passwordBox, AccountList accounts, int accountCount, Texture2D _IconUsername, Texture2D _IconPassword, Texture2D _ShowPasswordIcon, Font *_Font, Rectangle Form, Vector2 mouse, LoginResult *LOGIN_RESULT);
void LOGIN_DrawLoginUsername(InputBox *_LoginBox, Texture2D _IconUsername, Rectangle Form, Vector2 mouse, Font *_Font);
void LOGIN_DrawLoginPassword(InputBox *_PasswordBox, Texture2D _IconPassword, Texture2D _ShowPasswordIcon, Rectangle Form, Font *_Font, Vector2 mouse);
LoginResult CheckLogin(AccountList accounts, const char *username, const char *password);

int InsertAccount(AccountList pL, Account *e, AccountPosition p);
AccountList CreateHeaderNode();
int LOGIN_GetAccountData(const char *filename, AccountList accounts, int *accountCount);
int LOGIN_SaveAccountData(const char *filename, AccountList accounts, int accountCount);
CheckResult Get_account_return(AccountList _account, Account *account, const char *username);

#endif // LOGIN