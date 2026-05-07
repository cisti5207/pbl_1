#include "raylib.h"
#include <stdio.h>
#include "login.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

Vector2 mousePosition;
Font font[6];

LoginState LOGIN_STATE = LOGIN;
LoginResult LOGIN_RESULT = LOGIN_EMPTY;
ForgotPasswordResult FORGOT_PASSWORD_RESULT = FORGOT_PASSWORD_EMPTY;


int InitLogin() {
    Account *_accounts = NULL;
    int _accountCount = 0;
    GetAccountData("data\\accounts.txt", &_accounts, &_accountCount);
    printf("Total accounts loaded: %d\n", _accountCount);


    font[0] = SetFontUTF8("font/arial/arial.ttf", 24);
    font[1] = SetFontUTF8("font/cooper/COOPBL.ttf", 100);
    font[2] = SetFontUTF8("font/times/times.ttf", 50);
    font[3] = SetFontUTF8("font/impact/impact.ttf", 50);
    font[4] = SetFontUTF8("font/roboto/Roboto-Regular.ttf", 50);
    font[5] = SetFontUTF8("font/gill/gillubcd.ttf", 100);

    Texture2D ImgForFormLogin = LoadTexture("img/image1.png");
    Texture2D ImgForBackground = LoadTexture("img/image2.png");

    int _ScreenWidth = GetScreenWidth();
    int _ScreenHeight = GetScreenHeight();

    int _MonitorWidth = GetMonitorWidth(0);
    int _MonitorHeight = GetMonitorHeight(0);

    for (int i = 0; i < 6; i++) {
        if (font[i].texture.id == 0) {
            printf("Failed to load font!\n");
            return -1;
        }
    }

    ShowCursor();

    InputBox LoginBox = { 0 };
    InputBox PasswordBox = { 0 };
    
    while (!WindowShouldClose()) {
        if (IsWindowResized()) {
            _ScreenWidth = GetScreenWidth();
            _ScreenHeight = GetScreenHeight();
        }
        mousePosition = GetMousePosition();

        BeginDrawing();
        ClearBackground(BLACK);
        
        _DrawLoginBackground(ImgForBackground, _ScreenWidth, _ScreenHeight, _MonitorWidth, _MonitorHeight);

        _DrawLoginForm(ImgForFormLogin, _ScreenWidth, _ScreenHeight, &LoginBox, &PasswordBox, _accounts, &_accountCount);

        DrawFPS(10, 10);
        EndDrawing();

        if (LOGIN_RESULT == LOGIN_SUCCESS) {
            break; // Thoát vòng lặp nếu đăng nhập thành công
        }
    }

    for (int i = 0; i < 6; i++) {
        UnloadFont(font[i]);
    }

    ClearBackground(BLACK);

    return LOGIN_RESULT;
}

Font SetFontUTF8(char *_font, int _fontSize)
{
    int codepoints[] = {
    // Basic Latin (ký tự thường)
    32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
    48,49,50,51,52,53,54,55,56,57,
    58,59,60,61,62,63,64,
    65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,
    81,82,83,84,85,86,87,88,89,90,
    91,92,93,94,95,96,
    97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,
    113,114,115,116,117,118,119,120,121,122,
    123,124,125,126,

    // Vietnamese Latin bổ sung
    192,193,194,195,200,201,202,204,205,210,211,212,213,
    217,218,221,
    224,225,226,227,232,233,234,236,237,242,243,244,245,
    249,250,253,
    258,259,272,273,296,297,416,417,431,432,

    // Vietnamese đầy đủ dấu (quan trọng nhất)
    7840,7841,7842,7843,7844,7845,7846,7847,7848,7849,
    7850,7851,7852,7853,7854,7855,7856,7857,7858,7859,
    7860,7861,7862,7863,7864,7865,7866,7867,7868,7869,
    7870,7871,7872,7873,7874,7875,7876,7877,7878,7879,
    7880,7881,7882,7883,7884,7885,7886,7887,7888,7889,
    7890,7891,7892,7893,7894,7895,7896,7897,7898,7899,
    7900,7901,7902,7903,7904,7905,7906,7907,7908,7909,
    7910,7911,7912,7913,7914,7915,7916,7917,7918,7919,
    7920,7921,7922,7923,7924,7925,7926,7927,7928,7929
    };

    int codepointCount = sizeof(codepoints) / sizeof(codepoints[0]);

    Font finalFont = LoadFontEx(_font, _fontSize, codepoints, codepointCount);

    return finalFont;
}

void _DrawLoginBackground(Texture2D img, int _ScreenWidth, int _ScreenHeight, int _MonitorWidth, int _MonitorHeight) {
    float _ScaleX = (float)_MonitorWidth / img.width;
    float _ScaleY = (float)_MonitorHeight / img.height;

    float _Scale = (_ScaleX > _ScaleY) ? _ScaleX : _ScaleY;

    int _ImgWidth = img.width * _Scale;
    int _ImgHeight = img.height * _Scale;

    int _ImgX = (_ScreenWidth - _ImgWidth) * 0.5f;
    int _ImgY = (_ScreenHeight - _ImgHeight) * 0.5;

    DrawTextureEx(img, (Vector2){_ImgX, _ImgY}, 0.0f, _Scale, WHITE);
    DrawRectangleGradientV(0, 0, _ScreenWidth, _ScreenHeight, Fade(WHITE, 0.2f), Fade(BLACK, 0.2f));
}

void _DrawLoginForm(Texture2D img, int _ScreenWidth, int _ScreenHeight, InputBox *LoginBox, InputBox *PasswordBox, Account *accounts, int *accountCount) {
    int _FormWidth = _ScreenWidth * 0.7f;   
    int _FormHeight = _ScreenHeight * 0.7f;

    int _FormX = (_ScreenWidth - _FormWidth) * 0.5f;
    int _FormY = (_ScreenHeight - _FormHeight) * 0.5f;

    DrawRectangleRounded((Rectangle){_FormX + 2, _FormY + 2, _FormWidth + 2, _FormHeight + 2}, 0.05f, 10, Fade(DARKBLUE, 0.8f));
    DrawRectangleRounded((Rectangle){_FormX, _FormY, _FormWidth, _FormHeight}, 0.05f, 10, WHITE);
    DrawRectangleRoundedLinesEx((Rectangle){_FormX, _FormY, _FormWidth, _FormHeight}, 0.05f, 10, 2.0f, Fade(DARKBLUE, 0.8f));

    DrawRectangleRounded((Rectangle){_FormX, _FormY, _FormWidth / 2, _FormHeight}, 0.06f, 10, DARKBLUE);

    DrawNameLibText                     (_FormX, _FormY, _FormWidth, _FormHeight, _FormHeight * 0.06f);
    DrawWelcomeText                     (_FormX, _FormY, _FormWidth, _FormHeight, _FormHeight * 0.06f);
    DrawLoginText                       (_FormX, _FormY, _FormWidth, _FormHeight, _FormHeight * 0.1f);
    DrawLoginUsernameText               (_FormX, _FormY, _FormWidth, _FormHeight, _FormHeight * 0.02f, LoginBox);
    DrawLoginPasswordText               (_FormX, _FormY, _FormWidth, _FormHeight, _FormHeight * 0.02f, PasswordBox);
    DrawLoginRegisterButtonText         (_FormX, _FormY, _FormWidth, _FormHeight, _FormHeight * 0.02f);
    DrawLoginForgotPasswordButtonText   (_FormX, _FormY, _FormWidth, _FormHeight, _FormHeight * 0.02f);
    DrawLoginButtonText                 (_FormX, _FormY, _FormWidth, _FormHeight, _FormHeight * 0.04f, *LoginBox, *PasswordBox, accounts, accountCount);
}

void DrawNameLibText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize) {
    int _TextWidth = MeasureText(NameLibText, _FontSize);
    int _TextX = _FormX + (_FormWidth / 2 - _TextWidth) * 0.5f + _FormWidth * 0.5f;
    int _TextY = _FormY + _FormHeight * 0.1f;
    DrawTextEx(font[1], NameLibText, (Vector2){_TextX, _TextY}, _FontSize, 2, BLACK);
}

void DrawWelcomeText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize) {
    Vector2 _TextWidth = MeasureTextEx(font[1], WelcomeText, _FontSize, 2);
    int _TextX = _FormX + (_FormWidth / 2 - _TextWidth.x) * 0.5f;
    int _TextY = _FormY + _FormHeight * 0.1f;
    DrawTextEx(font[1], WelcomeText, (Vector2){_TextX, _TextY}, _FontSize, 2, WHITE);
    DrawLine(_TextX, _TextY + _FontSize + 2, _TextX + _TextWidth.x, _TextY + _FontSize + 2, WHITE);
}

void DrawLoginText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize) {
    int _TextWidth = MeasureText(LoginText, _FontSize);
    int _TextX = _FormX + _FormWidth/2 + (_FormWidth / 2 - _TextWidth) * 0.5f;
    int _TextY = _FormY + _FormHeight * 0.2f;
    DrawTextEx(font[1], LoginText, (Vector2){_TextX, _TextY}, _FontSize, 2, BLACK);
}

void DrawLoginUsernameText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize, InputBox *LoginBox) {
    LoginBox->box = (Rectangle){
        _FormX + _FormWidth * 0.55f - 10, 
        _FormY + _FormHeight * 0.5f - 10, 
        _FormWidth * 0.4f + 20, 
        50
    };

    int _TextX = _FormX + _FormWidth * 0.55f;
    int _TextY = _FormY + _FormHeight * 0.5f;

    Rectangle textRect = { 
        _TextX - 10, 
        _TextY - 10, 
        _FormWidth * 0.4f, 
        _FontSize + 20 
    };

    if (CheckCollisionPointRec(mousePosition, textRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        LoginBox->isFocused = true;
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        LoginBox->isFocused = false;
    }

    DrawRectangleRoundedLinesEx(textRect, 0.2f, 15, 2.0f, BLACK);
    DrawRectangleRounded(textRect, 0.1f, 15, (LoginBox->isFocused || strlen(LoginBox->text) > 0) ? Fade(WHITE, 0.5f) : LIGHTGRAY);

    

    if (LoginBox->isFocused) {
        UpdateInputBox(LoginBox);

        DrawTextEx(font[0], LoginBox->text, (Vector2){_TextX, _TextY}, _FontSize + 5, 1, BLACK);

        float w = MeasureTextEx(font[0], LoginBox->text, _FontSize + 5, 1).x;
        if ((int)(GetTime()*2) % 2 == 0) {
            DrawRectangle(_TextX + w + 2, _TextY, 2, _FontSize + 5, BLACK);
        }
    }
    else if (strlen(LoginBox->text) > 0) {
        DrawTextEx(font[0], LoginBox->text, (Vector2){_TextX, _TextY}, _FontSize + 5, 1, BLACK);
    }
    else
    {
        DrawTextEx(font[0], LoginUsernameText, (Vector2){_TextX, _TextY}, _FontSize, 1, BLACK);
    }
}

void DrawLoginPasswordText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize, InputBox *PasswordBox) {
    PasswordBox->box = (Rectangle){
        _FormX + _FormWidth * 0.55f - 10, 
        _FormY + _FormHeight * 0.6f - 10, 
        _FormWidth * 0.4f + 20, 
        50
    };

    int _TextX = _FormX + _FormWidth * 0.55f;
    int _TextY = _FormY + _FormHeight * 0.6f;

    Rectangle textRect = { 
        _TextX - 10, 
        _TextY - 10, 
        _FormWidth * 0.4f, 
        _FontSize + 20 
    };

    if (CheckCollisionPointRec(mousePosition, textRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        PasswordBox->isFocused = true;
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        PasswordBox->isFocused = false;
    }

    DrawRectangleRoundedLinesEx(textRect, 0.2f, 15, 2.0f, BLACK);
    DrawRectangleRounded(textRect, 0.1f, 15, (PasswordBox->isFocused || strlen(PasswordBox->text) > 0) ? Fade(WHITE, 0.5f) : LIGHTGRAY);

    if (PasswordBox->isFocused) {
        UpdateInputBox(PasswordBox);

        // Hiển thị dấu '*' thay vì ký tự thực
        char maskedText[MAX_INPUT];
        for (int i = 0; i < PasswordBox->length; i++) {
            maskedText[i] = '*';
        }
        maskedText[PasswordBox->length] = '\0';

        DrawTextEx(font[0], maskedText, (Vector2){_TextX, _TextY}, _FontSize + 5, 1, BLACK);
    }
    else if (strlen(PasswordBox->text) > 0) {
        char maskedText[MAX_INPUT];
        for (int i = 0; i < PasswordBox->length; i++) {
            maskedText[i] = '*';
        }
        maskedText[PasswordBox->length] = '\0';

        DrawTextEx(font[0], maskedText, (Vector2){_TextX, _TextY}, _FontSize + 5, 1, BLACK);
    }
    else
    {
        DrawTextEx(font[0], LoginPasswordText, (Vector2){_TextX, _TextY}, _FontSize, 1, BLACK);
    }
    
}

void DrawLoginRegisterButtonText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize) {
    int _TextWidth = MeasureText(LoginRegisterText, _FontSize);
    int _TextX = _FormX + _FormWidth * 0.55f;
    int _TextY = _FormY + _FormHeight * 0.67f;
    DrawTextEx(font[2], LoginRegisterText, (Vector2){_TextX, _TextY}, _FontSize, 2, BLACK);
    DrawLine(_TextX, _TextY + _FontSize + 2, _TextX + _TextWidth, _TextY + _FontSize + 2, BLACK);
}

void DrawLoginForgotPasswordButtonText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize) {
    int _TextWidth = MeasureText(LoginForgotPasswordText, _FontSize);
    int _TextX = _FormX + _FormWidth * 0.55f;
    int _TextY = _FormY + _FormHeight * 0.7f;
    DrawTextEx(font[2], LoginForgotPasswordText, (Vector2){_TextX, _TextY}, _FontSize, 2, BLACK);
    DrawLine(_TextX, _TextY + _FontSize + 2, _TextX + _TextWidth, _TextY + _FontSize + 2, BLACK);
}

void DrawLoginButtonText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize, InputBox LoginBox, InputBox PasswordBox, Account *accounts, int *accountCount) {
    Vector2 _TextWidth = MeasureTextEx(font[1], LoginButtonText, _FontSize, 2);
    Rectangle buttonRect = {
        _FormX + (_FormWidth / 2 - _TextWidth.x) * 0.5f + _FormWidth * 0.5f - 20,
        _FormY + _FormHeight * 0.8f - 10,
        _TextWidth.x + 40,
        _TextWidth.y + 20
    };

    Vector2 _Text = (Vector2){buttonRect.x + 20, buttonRect.y + 10};

    if (CheckCollisionPointRec(mousePosition, buttonRect)) {
        DrawRectangleRounded(buttonRect, 0.2f, 10, Fade(COOLGREEN, 0.8f));

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
        {
            if (LoginBox.length > 0 && PasswordBox.length > 0) 
            {
                // Kiểm tra thông tin đăng nhập
                for (int i = 0; i < *accountCount; i++) 
                {
                    if (CheckUsername(LoginBox.text, accounts[i].username) && CheckPassword(PasswordBox.text, accounts[i].password)) 
                    {
                        LOGIN_RESULT = LOGIN_SUCCESS;
                        printf("Login successful! Welcome, %s.\n", accounts[i].realName);
                        return; // Thoát hàm sau khi đăng nhập thành công
                    }
                }
                // Đăng nhập thất bại
                printf("Login failed! Invalid username or password.\n");
            }
            else {
                DrawTextEx(font[1], LoginEmptyText, (Vector2){_FormX + _FormWidth * 0.55f, _FormY + _FormHeight * 0.8f}, _FontSize, 2, RED);
                printf("Login failed! Please enter both username and password.\n");
            }
        }
        DrawTextEx(font[1], LoginButtonText, _Text, _FontSize, 2, Fade(BLACK, 0.8f));
    } else {
        DrawRectangleRounded(buttonRect, 0.2f, 10, COOLGREEN);
        DrawTextEx(font[1], LoginButtonText, _Text, _FontSize, 2, BLACK);
    }
    LOGIN_RESULT = LOGIN_ERROR;
    return;
}

// ===== HÀM PHỤ TRỢ: XÓA KÝ TỰ UTF-8 =====
void DeleteLastChar(InputBox *input) {
    if (input->length > 0) {
        input->length--;
        // Lùi lại để xóa trọn vẹn ký tự UTF-8 (có thể chiếm 2-3 byte)
        while (input->length > 0 && (input->text[input->length] & 0xC0) == 0x80) {
            input->length--;
        }
        input->text[input->length] = '\0';
    }
}

// ===== UPDATE INPUT =====
void UpdateInputBox(InputBox *input) {
    // ===== XỬ LÝ XÓA (BACKSPACE) =====
    // 1. Dùng GetKeyPressed() lấy sạch hàng đợi phím. 
    // Việc này đảm bảo bắt dính lệnh Backspace tàng hình do Unikey gửi.
    int keycode = GetKeyPressed();
    while (keycode > 0) {
        if (keycode == KEY_BACKSPACE) {
            DeleteLastChar(input);
        }
        keycode = GetKeyPressed(); // Lấy phím tiếp theo trong hàng đợi
    }

    // 2. Vẫn giữ IsKeyPressedRepeat để người dùng có thể đè phím xóa liên tục
    if (IsKeyPressedRepeat(KEY_BACKSPACE)) {
        DeleteLastChar(input);
    }

    // ===== XỬ LÝ NHẬP CHỮ UTF-8 =====
    int charKey = GetCharPressed();
    while (charKey > 0) {
        int size = 0;
        const char *utf8 = CodepointToUTF8(charKey, &size);

        if (input->length + size < MAX_INPUT) {
            memcpy(&input->text[input->length], utf8, size);
            input->length += size;
            input->text[input->length] = '\0';
        }
        
        charKey = GetCharPressed();
    }
}

int CheckUsername(const char *inputUsername, const char *storedUsername) {
    if (strcmp(inputUsername, storedUsername) == 0) {
        return 1; // Username matches
    }
    return 0; // Username does not match
}

int CheckPassword(const char *inputPassword, const char *storedPassword) {
    if (strcmp(inputPassword, storedPassword) == 0) {
        return 1; // Password matches
    }
    return 0; // Password does not match
}

void GetAccountData(const char *filename, Account **accounts, int *accountCount) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open account data file.\n");
        return;
    }

    Account tempAccount;

    fscanf (file, "Số lượng: %d\n", accountCount);

    int i = 0;
    *accounts = malloc((*accountCount) * sizeof(Account));

    while (fscanf(file, " | %[^|]| %[^|]| %[^|]| %[^|]| %[^|]| %[^|]|", tempAccount.username, tempAccount.password, tempAccount.realName, tempAccount.cccd, tempAccount.dateOfBirth, tempAccount.role) == 6) {
        //| 102250016                     | 1                                                 | Lê Hữu Hoàng                      | 26/10/2007                    | 049107000125          | Administrator |
        trim(tempAccount.username);
        trim(tempAccount.password);
        trim(tempAccount.realName);
        trim(tempAccount.cccd);
        trim(tempAccount.dateOfBirth);
        trim(tempAccount.role);
        (*accounts)[i] = tempAccount;
        i++;
        printf ("%d: %s | %s | %s | %s | %s | %s |\n", i, (*accounts)[i-1].username, (*accounts)[i-1].password, (*accounts)[i-1].realName, (*accounts)[i-1].cccd, (*accounts)[i-1].dateOfBirth, (*accounts)[i-1].role);
    }

    if (i != *accountCount) {
        printf("Warning: Account count mismatch. Expected %d but read %d.\n", *accountCount, i);
    }
    else
    {
        printf("Successfully loaded %d accounts.\n", *accountCount);
    }
    fclose(file);
}

void trim(char *str) {
    // Xóa khoảng trắng ở đầu
    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    // Xóa khoảng trắng ở cuối
    char *end = str + strlen(str) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0';

    // Dịch chuỗi về đầu mảng nếu cần
    if (start != str) {
        memmove(str, start, end - start + 2);
    }
}