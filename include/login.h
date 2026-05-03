#ifndef LOGIN_H
#define LOGIN_H

#include "raylib.h"

#define PINKBLOSSOM  (Color) {255, 182, 193, 255}
#define COOLGREEN    (Color){46, 204, 113, 255}
#define LIGHTBROWN   (Color){205, 133, 63, 255}


#define WelcomeText "Welcome to Library!"
#define AboutWelcomeText "xxx"

#define NameLibText "HoanHoang_DUT"
#define LoginText "Login"
#define LoginUsernameText "USERNAME"
#define LoginPasswordText "PASSWORD"
#define LoginButtonText "Login"
#define LoginErrorText "Invalid username or password!"
#define LoginSuccessText "Login successful! Welcome to the library."
#define LoginEmptyText "Please enter both username and password."
#define LoginRegisterText "Don't have an account? Register"
#define LoginForgotPasswordText "Forgot your password?"

#define MAX_INPUT_USERNAME 256
#define MAX_INPUT_PASSWORD 256
#define MAX_INPUT_REALNAME 256
#define MAX_INPUT_CCCD 30
#define MAX_INPUT_DATEOFBIRTH 256
#define MAX_INPUT_ROLE 256


#define RegisterText "Register"
#define RegisterUsernameText "USERNAME"
#define RegisterPasswordText "PASSWORD"
#define RegisterConfirmPasswordText "CONFIRM PASSWORD"
#define RegisterButtonText "Register"
#define CCCDText "CCCD"
#define DateOfBirthText "Date of Birth"

#define ForgotPasswordText "Forgot Password?"
#define ForgotPasswordUsernameText "USERNAME"
#define ForgotPasswordCCCDText "CCCD"
#define ForgotPasswordDateOfBirthText "Date of Birth"
#define ForgotPasswordButtonText "Reset Password"
#define ForgotPasswordPasswordText "NEW PASSWORD"
#define ForgotPasswordConfirmPasswordText "CONFIRM PASSWORD"

#define BackToLoginText "Back to Login"
#define BackToLoginButtonText "Back to Login"

#define MAX_INPUT 256

typedef struct {
    Rectangle box;
    char text[MAX_INPUT];
    int length;
    bool isFocused;
} InputBox;

typedef struct {
    char username[MAX_INPUT_USERNAME];
    char password[MAX_INPUT_PASSWORD];
    char realName[MAX_INPUT_REALNAME];
    char cccd[MAX_INPUT_CCCD];
    char dateOfBirth[MAX_INPUT_DATEOFBIRTH];
    char role[MAX_INPUT_ROLE];
} Account;

typedef enum {
    LOGIN,
    REGISTER,
    FORGOT_PASSWORD
} LoginState;

typedef enum {
    LOGIN_SUCCESS,
    LOGIN_ERROR,
    LOGIN_EMPTY
} LoginResult;

typedef enum {
    FORGOT_PASSWORD_SUCCESS,
    FORGOT_PASSWORD_ERROR,
    FORGOT_PASSWORD_EMPTY
} ForgotPasswordResult;

Font SetFontUTF8(char *_font, int _fontSize);

void _DrawLoginBackground(Texture2D img, int _ScreenWidth, int _ScreenHeight, int _MonitorWidth, int _MonitorHeight);
void _DrawLoginForm(Texture2D img, int _ScreenWidth, int _ScreenHeight, InputBox *LoginBox, InputBox *PasswordBox, Account *accounts, int *accountCount);

void DrawNameLibText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize);
void DrawWelcomeText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize);

void DrawLoginText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize);
void DrawLoginUsernameText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize, InputBox *LoginBox);
void DrawLoginPasswordText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize, InputBox *PasswordBox);
void DrawLoginRegisterButtonText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize);
void DrawLoginForgotPasswordButtonText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize);
void DrawLoginButtonText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize, InputBox LoginBox, InputBox PasswordBox, Account *accounts, int *accountCount);

int CheckUsername(const char *inputUsername, const char *storedUsername);
int CheckPassword(const char *inputPassword, const char *storedPassword);
void GetAccountData(const char *filename, Account **accounts, int *accountCount);
void SaveAccountData(const char *filename, Account *accounts, int accountCount);
void trim(char *str);





void DeleteLastChar(InputBox *input);
void UpdateInputBox(InputBox *input);

#endif // LOGIN