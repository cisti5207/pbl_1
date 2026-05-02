#ifndef LOGIN
#define LOGIN

#include "raylib.h"

#define PINKBLOSSOM  (Color) {255, 182, 193, 255}

Font SetFontUTF8(char *_font, int _fontSize);

void _DrawLoginBackground(Texture2D img, int _ScreenWidth, int _ScreenHeight, int _MonitorWidth, int _MonitorHeight);
void _DrawLoginForm(Texture2D img, int _ScreenWidth, int _ScreenHeight);

void DrawNameLibText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize);
void DrawWelcomeText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize);
void DrawLoginText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize);
void DrawLoginUsernameText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize);
void DrawLoginPasswordText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize);
void DrawRegisterText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize);
void DrawForgotPasswordText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize);
void DrawLoginButtonText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize);

#define WelcomeText "Welcome to Library!"
#define AboutWelcomeText "xxx"

#define NameLibText "HoanHoang_DUT"
#define LoginText "Login"
#define LoginUsernameText "Username"
#define LoginPasswordText "Password"
#define LoginButtonText "Login"

#define RegisterText "Register"
#define RegisterUsernameText "Username"
#define RegisterPasswordText "Password"
#define RegisterConfirmPasswordText "Confirm Password"
#define RegisterButtonText "Register"
#define CCCDText "CCCD"
#define DateOfBirthText "Date of Birth"

#define ForgotPasswordText "Forgot Password?"
#define ForgotPasswordUsernameText "Username"
#define ForgotPasswordCCCDText "CCCD"
#define ForgotPasswordDateOfBirthText "Date of Birth"
#define ForgotPasswordButtonText "Reset Password"
#define ForgotPasswordPasswordText "New Password"
#define ForgotPasswordConfirmPasswordText "Confirm Password"

#define BackToLoginText "Back to Login"
#define BackToLoginButtonText "Back to Login"

#endif // LOGIN