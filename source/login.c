#include "raylib.h"
#include "login.h"
#include "libmanage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

Vector2 LOGIN_mousePosition;
Texture2D LOGIN_ImgForBackground;
Texture2D LOGIN_ImgForFormLogin;
Font LOGIN_Font[QUANTITYLOGIN_FONT];

LoginResult             LOGIN_RESULT;

int LOGIN_ScreenWidth;
int LOGIN_ScreenHeight;

int LOGIN_MonitorWidth;
int LOGIN_MonitorHeight;

int LOGIN_FormWidth;
int LOGIN_FormHeight;

int LOGIN_FormX;
int LOGIN_FormY;

int InitLogin(Account *account_return)
{
    LOGIN_RESULT = LOGIN_IDLE;
    AccountList _accounts = CreateHeaderNode();
    if (_accounts == NULL) {
        printf("Failed to create header node for accounts.\n");
        return FAILED;
    }

    int _accountCount = 0;
    printf("Loading account data...\n");
    if (LOGIN_GetAccountData("data\\accounts.txt", _accounts, &_accountCount) == FAILED) {
        printf("Failed to load account data.\n");
        return FAILED;
    }
    LOGIN_ImgForBackground = LoadTexture(LOGIN_BACKGROUND_IMG);
    LOGIN_ImgForFormLogin = LoadTexture(FORM_LOGIN_IMG);

    LOGIN_MonitorWidth = GetMonitorWidth(0);
    LOGIN_MonitorHeight = GetMonitorHeight(0);

    LOGIN_GetSize(&LOGIN_ScreenWidth, &LOGIN_ScreenHeight, &LOGIN_FormX, &LOGIN_FormY, &LOGIN_FormWidth, &LOGIN_FormHeight);

    LOGIN_Font[0] = SetFontUTF8(ArialBold, 100);
    LOGIN_Font[1] = SetFontUTF8(Roboto_Semibold, 30);

    InputBox _LoginBox = { 0 }; 
        Texture2D _IconUsername = LoadTexture(ICON_USERNAME);

    InputBox _PasswordBox = { 0 }; 
        Texture2D _IconPassword = LoadTexture(ICON_PASSWORD);
        Texture2D _ShowPasswordIcon = LoadTexture(SHOW_PASSWORD);

    while (!WindowShouldClose() && LOGIN_RESULT != LOGIN_SUCCESS) {
        if (IsWindowResized())
        {
            LOGIN_GetSize(&LOGIN_ScreenWidth, &LOGIN_ScreenHeight, &LOGIN_FormX, &LOGIN_FormY, &LOGIN_FormWidth, &LOGIN_FormHeight);
        }

        LOGIN_mousePosition = GetMousePosition();

        BeginDrawing();
        ClearBackground(BLACK);
        
        LOGIN_DrawBackground();
        LOGIN_DrawForm();
        LOGIN_DrawLoginForm(&_LoginBox, &_PasswordBox, _accounts, _accountCount, _IconUsername, _IconPassword, _ShowPasswordIcon);

        EndDrawing();
    }

    LOGIN_SaveAccountData("data\\accounts.txt", _accounts, _accountCount);

    for (int i = 0; i < 2; i++)
        UnloadFont(LOGIN_Font[i]);

    UnloadTexture(LOGIN_ImgForBackground);
    UnloadTexture(LOGIN_ImgForFormLogin);
    UnloadTexture(_IconUsername);
    UnloadTexture(_IconPassword);
    UnloadTexture(_ShowPasswordIcon);


    Get_account_return(_accounts, account_return, _LoginBox.text);

    return LOGIN_SUCCESS;
}

void LOGIN_DrawBackground() {
    float _scaleX = (float)LOGIN_MonitorWidth / LOGIN_ImgForBackground.width;
    float _scaleY = (float)LOGIN_MonitorHeight / LOGIN_ImgForBackground.height;
    
    float _scale = (_scaleX > _scaleY) ? _scaleX : _scaleY;

    Rectangle source = {
        0,
        0,
        LOGIN_ImgForBackground.width,
        LOGIN_ImgForBackground.height
    };

    Rectangle dest = {
        (LOGIN_ScreenWidth - LOGIN_ImgForBackground.width * _scale) / 2,
        (LOGIN_ScreenHeight - LOGIN_ImgForBackground.height * _scale) / 2,
        LOGIN_ImgForBackground.width * _scale,
        LOGIN_ImgForBackground.height * _scale
    };

    Vector2 origin = { 0, 0 };
    
    DrawTexturePro(LOGIN_ImgForBackground, source, dest, origin, 0.0f, WHITE);
    DrawRectangleGradientV(0, 0, LOGIN_ScreenWidth, LOGIN_ScreenHeight, Fade(WHITE, 0.0f), Fade(BLACK, 0.5f));
}
void LOGIN_DrawForm()
{    Rectangle FormRect = { 
        LOGIN_FormX, 
        LOGIN_FormY, 
        LOGIN_FormWidth, 
        LOGIN_FormHeight 
    };

    Rectangle FormRectLoginRight = { 
        LOGIN_FormX + LOGIN_FormWidth / 2, 
        LOGIN_FormY, 
        LOGIN_FormWidth / 2, 
        LOGIN_FormHeight 
    };

    Rectangle FormRectLoginLeft = { 
        LOGIN_FormX, 
        LOGIN_FormY, 
        LOGIN_FormWidth / 2, 
        LOGIN_FormHeight 
    };

    Rectangle BoxWelcomeText = {
        LOGIN_FormX + (LOGIN_FormWidth / 2) * 0.1f,
        LOGIN_FormY + LOGIN_FormHeight * 0.3f,
        LOGIN_FormWidth / 2 * 0.8f,
        LOGIN_FormHeight * 0.5f,
    };

    DrawRectangleRec(FormRect, WHITE);

    float _ScaleTextureY = FormRectLoginRight.height / LOGIN_ImgForFormLogin.height;
    float _ScaleTextureX = FormRectLoginRight.width / LOGIN_ImgForFormLogin.width;

    float _ScaleTexture = (_ScaleTextureX > _ScaleTextureY) ? _ScaleTextureX : _ScaleTextureY;

    Rectangle source = {
        LOGIN_ImgForFormLogin.width * 0.2f,
        LOGIN_ImgForFormLogin.height - LOGIN_FormHeight / _ScaleTexture,
        FormRectLoginRight.width / _ScaleTexture,
        FormRectLoginRight.height / _ScaleTexture
    };

    Rectangle dest = {
        LOGIN_FormX,
        LOGIN_FormY,
        source.width * _ScaleTexture,
        source.height * _ScaleTexture
    };

    Vector2 origin = { 0, 0 };

    DrawTexturePro(LOGIN_ImgForFormLogin, source, dest, origin, 0.0f, WHITE);
    
    
    
    Vector2 _TextWidth = MeasureTextEx(LOGIN_Font[0], WELCOME_TEXT, LOGIN_FormHeight * 0.06f, 2);
    
    int LOGIN_TextPerRectX = _TextWidth.x / (LOGIN_FormWidth / 2);
    int LOGIN_FontSize = LOGIN_FormHeight * 0.06f * 0.7f / LOGIN_TextPerRectX;

    _TextWidth = MeasureTextEx(LOGIN_Font[0], WELCOME_TEXT, LOGIN_FontSize, 2);

    Vector2 _Text = {
        LOGIN_FormX + (LOGIN_FormWidth / 2 - _TextWidth.x) * 0.5f,
        LOGIN_FormY + LOGIN_FormHeight * 0.1f
    };

    Vector2 _ShadowText = {
        _Text.x - 1,
        _Text.y
    };

    DrawRectangleGradientV(FormRectLoginLeft.x, FormRectLoginLeft.y, FormRectLoginLeft.width, FormRectLoginLeft.height, Fade(PINK, 0.2f), Fade(BLACK, 0.4f));
    DrawRectangleRounded(BoxWelcomeText, 0.1f, 100, Fade(WHITE, 0.5f));
    DrawTextEx(LOGIN_Font[0], WELCOME_TEXT, _ShadowText, LOGIN_FontSize + 2, 1, YELLOW);
    DrawTextEx(LOGIN_Font[0], WELCOME_TEXT, _Text, LOGIN_FontSize, 2, Fade(BLACK, 0.8f));
    
}

void LOGIN_DrawLoginForm(InputBox *_loginBox, InputBox *_passwordBox, AccountList accounts, int accountCount, Texture2D _IconUsername, Texture2D _IconPassword, Texture2D _ShowPasswordIcon) {
    Vector2 _TextWidth = MeasureTextEx(LOGIN_Font[0], LOGIN_TEXT, LOGIN_FormHeight * 0.1f, 2);
    Vector2 _Text = {
        LOGIN_FormX + LOGIN_FormWidth/2 + (LOGIN_FormWidth / 2 - _TextWidth.x) * 0.5f,
        LOGIN_FormY + LOGIN_FormHeight * 0.2f
    };

    DrawTextEx(LOGIN_Font[0], LOGIN_TEXT, _Text, LOGIN_FormHeight * 0.1f, 2, BLACK);

    LOGIN_DrawLoginUsername(_loginBox, _IconUsername);
    LOGIN_DrawLoginPassword(_passwordBox, _IconPassword, _ShowPasswordIcon);

    Vector2 _TextWidthButton = MeasureTextEx(LOGIN_Font[1], LoginButtonText, LOGIN_FormHeight * 0.04f, 2);
    Rectangle buttonRect = {
        LOGIN_FormX + (LOGIN_FormWidth / 2 - _TextWidthButton.x) * 0.5f + LOGIN_FormWidth * 0.5f - 20,
        LOGIN_FormY + LOGIN_FormHeight * 0.8f - 10,
        _TextWidthButton.x + 40,
        _TextWidthButton.y + 20
    };

    DrawRectangleRounded(buttonRect, 0.1f, 15, (CheckCollisionPointRec(LOGIN_mousePosition, buttonRect)) ? Fade(WHITE, 0.5f) : LIGHTGRAY);
    DrawRectangleRoundedLinesEx(buttonRect, 0.1f, 15, 2.0f, BLACK);

    Vector2 _TextButton = {
        buttonRect.x + 20,
        buttonRect.y + 10
    };

    DrawTextEx(LOGIN_Font[1], LoginButtonText, _TextButton, LOGIN_FormHeight * 0.04f, 2, BLACK);

    Vector2 _TextWidthLoginResult;
    Vector2 _TextLoginResult;
    switch(LOGIN_RESULT)
    {
        case LOGIN_IDLE: 
            break;
        case LOGIN_SUCCESS: 
            _TextWidthLoginResult = MeasureTextEx(LOGIN_Font[1], LoginSuccessText, LOGIN_FormHeight * 0.02f, 2);
            
            _TextLoginResult = (Vector2){
                LOGIN_FormX + (LOGIN_FormWidth / 2 - _TextWidthLoginResult.x) * 0.5f + LOGIN_FormWidth * 0.5f,
                LOGIN_FormY + LOGIN_FormHeight * 0.75f
            };

            DrawTextEx(LOGIN_Font[1], LoginSuccessText, _TextLoginResult, LOGIN_FormHeight * 0.02f, 2, GREEN); 
            break;
        case LOGIN_EMPTY: 
            _TextWidthLoginResult = MeasureTextEx(LOGIN_Font[1], LoginEmptyText, LOGIN_FormHeight * 0.02f, 2);
            _TextLoginResult = (Vector2){
                LOGIN_FormX + (LOGIN_FormWidth / 2 - _TextWidthLoginResult.x) * 0.5f + LOGIN_FormWidth * 0.5f,
                LOGIN_FormY + LOGIN_FormHeight * 0.75f
            };
            DrawTextEx(LOGIN_Font[1], LoginEmptyText, _TextLoginResult, LOGIN_FormHeight * 0.02f, 2, ORANGE); 
            break;
        case LOGIN_FAILED: 
            _TextWidthLoginResult = MeasureTextEx(LOGIN_Font[1], LoginFailedText, LOGIN_FormHeight * 0.02f, 2);
            _TextLoginResult = (Vector2){
                LOGIN_FormX + (LOGIN_FormWidth / 2 - _TextWidthLoginResult.x) * 0.5f + LOGIN_FormWidth * 0.5f,
                LOGIN_FormY + LOGIN_FormHeight * 0.75f
            };
            DrawTextEx(LOGIN_Font[1], LoginFailedText, _TextLoginResult, LOGIN_FormHeight * 0.02f, 2, RED); 
            break;
    }

    if (CheckCollisionPointRec(LOGIN_mousePosition, buttonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (_loginBox -> length > 0 && _passwordBox -> length > 0) {
            LOGIN_RESULT = CheckLogin(accounts, _loginBox->text, _passwordBox->text);
        }
        else {
            LOGIN_RESULT = LOGIN_EMPTY;
        }
    }

}
void LOGIN_DrawLoginUsername(InputBox *_LoginBox, Texture2D _IconUsername) {
    _LoginBox->box = (Rectangle){
        LOGIN_FormX + LOGIN_FormWidth * 0.55f, 
        LOGIN_FormY + LOGIN_FormHeight * 0.5f, 
        LOGIN_FormWidth * 0.4f,
        LOGIN_FormHeight * 0.06f
    };

    Rectangle IconBox_Username = {
        LOGIN_FormX + LOGIN_FormWidth * 0.55f,
        LOGIN_FormY + LOGIN_FormHeight * 0.5f,
        _LoginBox->box.height,
        _LoginBox->box.height
    };

    Vector2 _Text = {
        _LoginBox->box.x + IconBox_Username.width + 10,
        _LoginBox->box.y + (_LoginBox->box.height - MeasureTextEx(LOGIN_Font[0], "a", FONT_SIZE, 1).y) / 2
    };

    DrawRectangleRounded(_LoginBox->box, 0.1f, 15, (CheckCollisionPointRec(LOGIN_mousePosition, _LoginBox->box) || _LoginBox->isFocused || _LoginBox->length > 0) ? WHITE : LIGHTGRAY);
    
    DrawRectangleRec(IconBox_Username, LIGHTGRAY);
    DrawRectangleRoundedLinesEx(_LoginBox->box, 0.1f, 1, 2.0f, BLACK);

    DrawLineEx((Vector2){IconBox_Username.x + IconBox_Username.width, IconBox_Username.y}, (Vector2){IconBox_Username.x + IconBox_Username.width, IconBox_Username.y + IconBox_Username.height}, 2.0f, BLACK);

    DrawIcon(IconBox_Username, _IconUsername);

    if (CheckCollisionPointRec(LOGIN_mousePosition, _LoginBox->box) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        _LoginBox->isFocused = true;
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        _LoginBox->isFocused = false;
    }

    if (_LoginBox->isFocused) {
        UpdateInputBox(_LoginBox);

        DrawTextEx(LOGIN_Font[0], _LoginBox->text, _Text, FONT_SIZE, 1, BLACK);

        float w = MeasureTextEx(LOGIN_Font[0], _LoginBox->text,FONT_SIZE, 1).x;
        if ((int)(GetTime()*2) % 2 == 0) {
            DrawRectangle(_Text.x + w + 2, _Text.y, 2,FONT_SIZE, BLACK);
        }
    }
    else if (_LoginBox -> length > 0) {
        DrawTextEx(LOGIN_Font[0], _LoginBox->text, _Text,FONT_SIZE, 1, BLACK);
    }
    

}
void LOGIN_DrawLoginPassword(InputBox *_PasswordBox, Texture2D _IconPassword, Texture2D _ShowPasswordIcon) {
    _PasswordBox->box = (Rectangle){
        LOGIN_FormX + LOGIN_FormWidth * 0.55f, 
        LOGIN_FormY + LOGIN_FormHeight * 0.6f, 
        LOGIN_FormWidth * 0.4f,
        LOGIN_FormHeight * 0.06f
    };

    Rectangle IconBox_Password = {
        LOGIN_FormX + LOGIN_FormWidth * 0.55f,
        LOGIN_FormY + LOGIN_FormHeight * 0.6f,
        _PasswordBox->box.height,
        _PasswordBox->box.height
    };
    Rectangle Show_PasswordBox = {
        _PasswordBox->box.x + _PasswordBox->box.width - IconBox_Password.width,
        _PasswordBox->box.y,
        IconBox_Password.width,
        IconBox_Password.height
    };

    Vector2 _Text = {
        _PasswordBox->box.x + IconBox_Password.width + 10,
        _PasswordBox->box.y + (_PasswordBox->box.height - MeasureTextEx(LOGIN_Font[0], "a", FONT_SIZE, 1).y) / 2
    };

    DrawRectangleRounded(_PasswordBox->box, 0.1f, 15, (CheckCollisionPointRec(LOGIN_mousePosition, _PasswordBox->box) || _PasswordBox->isFocused || _PasswordBox->length > 0) ? WHITE : LIGHTGRAY);
    
    DrawRectangleRec(IconBox_Password, LIGHTGRAY);
    DrawRectangleRoundedLinesEx(_PasswordBox->box, 0.1f, 1, 2.0f, BLACK);
    
    DrawLineEx((Vector2){IconBox_Password.x + IconBox_Password.width, IconBox_Password.y}, (Vector2){IconBox_Password.x + IconBox_Password.width, IconBox_Password.y + IconBox_Password.height}, 2.0f, BLACK);
    
    DrawIcon(IconBox_Password, _IconPassword);
    DrawIcon(Show_PasswordBox, _ShowPasswordIcon);
    if (CheckCollisionPointRec(LOGIN_mousePosition, _PasswordBox->box) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        _PasswordBox->isFocused = true;
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        _PasswordBox->isFocused = false;
    }

    if (_PasswordBox->isFocused) {
        UpdateInputBox(_PasswordBox);

        char maskedText[MAX_INPUT];
        float w = MeasureTextEx(LOGIN_Font[0], _PasswordBox->text, FONT_SIZE, 1).x;
        memset(maskedText, '*', _PasswordBox->length);
        maskedText[_PasswordBox->length] = '\0';

        if (CheckCollisionPointRec(LOGIN_mousePosition, Show_PasswordBox) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            DrawTextEx(LOGIN_Font[0], _PasswordBox->text, _Text, FONT_SIZE, 1, BLACK);
            w = MeasureTextEx(LOGIN_Font[0], _PasswordBox->text,FONT_SIZE, 1).x;
            if ((int)(GetTime()*2) % 2 == 0) {
                DrawRectangle(_Text.x + w + 2, _Text.y, 2,FONT_SIZE, BLACK);
            }
        }
        else {
            DrawTextEx(LOGIN_Font[0], maskedText, _Text, FONT_SIZE, 1, BLACK);
            w = MeasureTextEx(LOGIN_Font[0], maskedText, FONT_SIZE, 1).x;
            if ((int)(GetTime()*2) % 2 == 0) {
                DrawRectangle(_Text.x + w + 2, _Text.y, 2, FONT_SIZE, BLACK);
            }
        }
    }

    else if (_PasswordBox -> length > 0) {
        char maskedText[MAX_INPUT];
        memset(maskedText, '*', _PasswordBox->length);
        maskedText[_PasswordBox->length] = '\0';

        DrawTextEx(LOGIN_Font[0], (CheckCollisionPointRec(LOGIN_mousePosition, _PasswordBox->box) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) ? _PasswordBox->text : maskedText, _Text, FONT_SIZE, 1, BLACK);
    }
}
LoginResult CheckLogin(AccountList accounts, const char *username, const char *password) {
    Node *current = accounts->next; // Bắt đầu từ node đầu tiên sau header

    while (current != NULL) {
        if (strcmp(current->account->username, username) == 0) {
            if (strcmp(current->account->password, password) == 0) {
                return LOGIN_SUCCESS;
            }
            else
            {
                return LOGIN_FAILED;
            }
        }
        current = current->next;
    }

    return LOGIN_FAILED; // Không tìm thấy tài khoản
}

int InsertAccount(AccountList pL, Account *e, AccountPosition p)
{
    Node *newNode = (Node *)malloc(sizeof(Node));

    if (newNode == NULL) {
        return FALSE; // Failed to allocate memory
    }

    if (p == NULL)
    {
        p = pL; // Insert at the beginning if p is NULL
    }

    newNode->account = e;
    newNode ->next = p->next;
    newNode ->prev = p;
    if (p->next != NULL) {
        p->next->prev = newNode;
    }
    p->next = newNode;

    return TRUE; // Successfully inserted
}
AccountList CreateHeaderNode() {
    Node *header = (Node *)malloc(sizeof(Node));
    
    if (header == NULL) {
        return NULL; // Failed to allocate memory
    }

    header -> next = NULL;
    header -> prev = NULL;

    return header;
}
int LOGIN_GetAccountData(const char *filename, AccountList accounts, int *accountCount) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Failed to open account data file.\n");
        return FAILED;
    }

    int count = 0;
    int expectedCount; fscanf(file, "Số lượng: %d\n", &expectedCount);

    //| 102250016                     | 1                                                 | Lê Hữu Hoàng                      | 26/10/2007                    | 049107000125          | Administrator |
    while (!feof(file)) {
        Account *tempAccount = (Account *)malloc(sizeof(Account));
        if (tempAccount == NULL) {
            printf("Failed to allocate memory for temporary account.\n");
            fclose(file);
            return FAILED;
        }
        if (fscanf(file, " | %[^|]| %[^|]| %[^|]| %[^|]| %[^|]| %[^|]|", tempAccount->username, tempAccount->password, tempAccount->realName, tempAccount->cccd, tempAccount->dateOfBirth, tempAccount->role) == 6) {
            trim(tempAccount->username);
            trim(tempAccount->password);
            trim(tempAccount->realName);
            trim(tempAccount->cccd);
            trim(tempAccount->dateOfBirth);
            trim(tempAccount->role);

            if (InsertAccount(accounts, tempAccount, NULL) == FALSE) {
                printf("Failed to insert account into list.\n");
                free(tempAccount);
                fclose(file);
                return FAILED;
            }
            count++;
        }
    }

    fclose(file);
    *accountCount = count;
    if (count != expectedCount) {
        printf("Warning: Account count mismatch. Expected %d but read %d.\n", expectedCount, count);
        return FAILED;
    }
    else
    {
        printf("Successfully loaded %d accounts.\n", count);
        return TRUE;
    }
}
int LOGIN_SaveAccountData(const char *filename, AccountList accounts, int accountCount) {
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        printf("Failed to open account data file for writing.\n");
        return FAILED;
    }

    fprintf(file, "Số lượng: %d\n", accountCount);

    Node *current = accounts;
    
    while (current -> next != NULL) current = current -> next;

    while (current != accounts) {
        Account *acc = current->account;
        fprintf(file, "| %s%*s | %s%*s | %s%*s | %s%*s | %s%*s | %s%*s |\n", 
            acc->username, LEN_SHOW_USERNAME - lenStringUTF8(acc->username), "",
            acc->password, LEN_SHOW_PASSWORD - lenStringUTF8(acc->password), "",
            acc->realName, LEN_SHOW_REALNAME - lenStringUTF8(acc->realName), "",
            acc->cccd, LEN_SHOW_CCCD - lenStringUTF8(acc->cccd), "",
            acc->dateOfBirth, LEN_SHOW_DOB - lenStringUTF8(acc->dateOfBirth), "",
            acc->role, LEN_SHOW_ROLE - lenStringUTF8(acc->role), ""
        );
        current = current -> prev;
    }

    fclose(file);
    return TRUE;
}

void LOGIN_GetSize(int *screenWidth, int *screenHeight, int *formX, int *formY, int *formWidth, int *formHeight) {
    *screenWidth = GetScreenWidth();
    *screenHeight = GetScreenHeight();

    *formWidth = *screenWidth * 0.8f;
    *formHeight = *screenHeight * 0.8f;

    *formX = (*screenWidth - *formWidth) / 2;
    *formY = (*screenHeight - *formHeight) / 2;
}
CheckResult Get_account_return(AccountList _account, Account *account, const char *username){
    Account A;

    _account = _account -> next;
    while (_account != NULL)
    {
        A = *(_account -> account);
        if (strcmp(username, A.username) == 0){
            *account = A;
            return TRUE;
        }
        _account = _account -> next;
    }   

    return FAILED;
}