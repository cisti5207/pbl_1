#include "raylib.h"
#include <stdio.h>
#include "login.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

Vector2 mousePosition;
Texture2D ImgForBackground;
Texture2D ImgForFormLogin;
Font _Font[QUANTITY_FONT];

LoginState              LOGIN_STATE             = LOGIN;
LoginResult             LOGIN_RESULT            = LOGIN_IDLE;
ForgotPasswordResult    FORGOT_PASSWORD_RESULT  = FORGOT_PASSWORD_IDLE;
CheckResult             CHECK_RESULT            = IDLE;

int _ScreenWidth;
int _ScreenHeight;

int _MonitorWidth;
int _MonitorHeight;

int _FormWidth;
int _FormHeight;

int _FormX;
int _FormY;

int InitLogin()
{
    AccountList _accounts = CreateHeaderNode();
    if (_accounts == NULL) {
        printf("Failed to create header node for accounts.\n");
        return FAILED;
    }

    int _accountCount = 0;
    printf("Loading account data...\n");
    if (GetAccountData("data\\accounts.txt", _accounts, &_accountCount) == FAILED) {
        printf("Failed to load account data.\n");
        return FAILED;
    }
    ImgForBackground = LoadTexture(BACKGROUND_IMG);
    ImgForFormLogin = LoadTexture(FORM_LOGIN_IMG);

    _MonitorWidth = GetMonitorWidth(0);
    _MonitorHeight = GetMonitorHeight(0);

    GetSize();

    _Font[0] = SetFontUTF8(ArialBold, 100);
    _Font[1] = SetFontUTF8(Roboto_Semibold, 30);

    InputBox LoginBox = { 0 }; 
        Texture2D IconUsername = LoadTexture(ICON_USERNAME);

    InputBox PasswordBox = { 0 }; 
        Texture2D IconPassword = LoadTexture(ICON_PASSWORD);
        Texture2D ShowPasswordIcon = LoadTexture(SHOW_PASSWORD);

    while (!WindowShouldClose()) {
        if (IsWindowResized())
        {
            GetSize();
        }
        mousePosition = GetMousePosition();

        BeginDrawing();
        ClearBackground(BLACK);
        
        DrawBackground();
        DrawForm();
        DrawLoginForm(&LoginBox, &PasswordBox, _accounts, _accountCount, IconUsername, IconPassword, ShowPasswordIcon);

    if (LOGIN_RESULT == LOGIN_SUCCESS)  break;
        EndDrawing();
    }

    SaveAccountData("data\\accounts.txt", _accounts, _accountCount);
    return LOGIN_SUCCESS;
}

void GetSize()
{
    _ScreenWidth = GetScreenWidth();
    _ScreenHeight = GetScreenHeight();

    _FormWidth = _ScreenWidth * 0.7f;
    _FormHeight = _ScreenHeight * 0.7f;

    _FormX = (_ScreenWidth - _FormWidth) * 0.5f;
    _FormY = (_ScreenHeight - _FormHeight) * 0.5f;
}

void DrawBackground() {
    float _scaleX = (float)_MonitorWidth / ImgForBackground.width;
    float _scaleY = (float)_MonitorHeight / ImgForBackground.height;
    
    float _scale = (_scaleX > _scaleY) ? _scaleX : _scaleY;

    Rectangle source = {
        0,
        0,
        ImgForBackground.width,
        ImgForBackground.height
    };

    Rectangle dest = {
        (_ScreenWidth - ImgForBackground.width * _scale) / 2,
        (_ScreenHeight - ImgForBackground.height * _scale) / 2,
        ImgForBackground.width * _scale,
        ImgForBackground.height * _scale
    };

    Vector2 origin = { 0, 0 };
    
    DrawTexturePro(ImgForBackground, source, dest, origin, 0.0f, WHITE);
    DrawRectangleGradientV(0, 0, _ScreenWidth, _ScreenHeight, Fade(WHITE, 0.0f), Fade(BLACK, 0.5f));
}
void DrawForm()
{    Rectangle FormRect = { 
        _FormX, 
        _FormY, 
        _FormWidth, 
        _FormHeight 
    };

    Rectangle FormRectLoginRight = { 
        _FormX + _FormWidth / 2, 
        _FormY, 
        _FormWidth / 2, 
        _FormHeight 
    };

    Rectangle FormRectLoginLeft = { 
        _FormX, 
        _FormY, 
        _FormWidth / 2, 
        _FormHeight 
    };

    Rectangle BoxWelcomeText = {
        _FormX + (_FormWidth / 2) * 0.1f,
        _FormY + _FormHeight * 0.3f,
        _FormWidth / 2 * 0.8f,
        _FormHeight * 0.5f,
    };

    DrawRectangleRec(FormRect, WHITE);

    float _ScaleTextureY = FormRectLoginRight.height / ImgForFormLogin.height;
    float _ScaleTextureX = FormRectLoginRight.width / ImgForFormLogin.width;

    float _ScaleTexture = (_ScaleTextureX > _ScaleTextureY) ? _ScaleTextureX : _ScaleTextureY;

    Rectangle source = {
        ImgForFormLogin.width * 0.2f,
        ImgForFormLogin.height - _FormHeight / _ScaleTexture,
        FormRectLoginRight.width / _ScaleTexture,
        FormRectLoginRight.height / _ScaleTexture
    };

    Rectangle dest = {
        _FormX,
        _FormY,
        source.width * _ScaleTexture,
        source.height * _ScaleTexture
    };

    Vector2 origin = { 0, 0 };

    DrawTexturePro(ImgForFormLogin, source, dest, origin, 0.0f, WHITE);
    
    
    
    Vector2 _TextWidth = MeasureTextEx(_Font[0], WELCOME_TEXT, _FormHeight * 0.06f, 2);
    
    int TextPerRectX = _TextWidth.x / (_FormWidth / 2);
    int _FontSize = _FormHeight * 0.06f * 0.7f / TextPerRectX;

    _TextWidth = MeasureTextEx(_Font[0], WELCOME_TEXT, _FontSize, 2);

    Vector2 _Text = {
        _FormX + (_FormWidth / 2 - _TextWidth.x) * 0.5f,
        _FormY + _FormHeight * 0.1f
    };

    Vector2 _ShadowText = {
        _Text.x - 1,
        _Text.y
    };

    DrawRectangleGradientV(FormRectLoginLeft.x, FormRectLoginLeft.y, FormRectLoginLeft.width, FormRectLoginLeft.height, Fade(PINK, 0.2f), Fade(BLACK, 0.4f));
    DrawRectangleRounded(BoxWelcomeText, 0.1f, 100, Fade(WHITE, 0.5f));
    DrawTextEx(_Font[0], WELCOME_TEXT, _ShadowText, _FontSize + 2, 1, YELLOW);
    DrawTextEx(_Font[0], WELCOME_TEXT, _Text, _FontSize, 2, Fade(BLACK, 0.8f));
    
}

void DrawLoginForm(InputBox *loginBox, InputBox *passwordBox, AccountList accounts, int accountCount, Texture2D IconUsername, Texture2D IconPassword, Texture2D ShowPasswordIcon) {
    Vector2 _TextWidth = MeasureTextEx(_Font[0], LOGIN_TEXT, _FormHeight * 0.1f, 2);
    Vector2 _Text = {
        _FormX + _FormWidth/2 + (_FormWidth / 2 - _TextWidth.x) * 0.5f,
        _FormY + _FormHeight * 0.2f
    };

    DrawTextEx(_Font[0], LOGIN_TEXT, _Text, _FormHeight * 0.1f, 2, BLACK);

    DrawLoginUsername(loginBox, IconUsername);
    DrawLoginPassword(passwordBox, IconPassword, ShowPasswordIcon);

    Vector2 _TextWidthButton = MeasureTextEx(_Font[1], LoginButtonText, _FormHeight * 0.04f, 2);
    Rectangle buttonRect = {
        _FormX + (_FormWidth / 2 - _TextWidthButton.x) * 0.5f + _FormWidth * 0.5f - 20,
        _FormY + _FormHeight * 0.8f - 10,
        _TextWidthButton.x + 40,
        _TextWidthButton.y + 20
    };

    DrawRectangleRounded(buttonRect, 0.1f, 15, (CheckCollisionPointRec(mousePosition, buttonRect)) ? Fade(WHITE, 0.5f) : LIGHTGRAY);
    DrawRectangleRoundedLinesEx(buttonRect, 0.1f, 15, 2.0f, BLACK);

    Vector2 _TextButton = {
        buttonRect.x + 20,
        buttonRect.y + 10
    };

    DrawTextEx(_Font[1], LoginButtonText, _TextButton, _FormHeight * 0.04f, 2, BLACK);

    Vector2 _TextWidthLoginResult;
    Vector2 _TextLoginResult;
    switch(LOGIN_RESULT)
    {
        case LOGIN_IDLE: 
            break;
        case LOGIN_SUCCESS: 
            _TextWidthLoginResult = MeasureTextEx(_Font[1], LoginSuccessText, _FormHeight * 0.02f, 2);
            
            _TextLoginResult = (Vector2){
                _FormX + (_FormWidth / 2 - _TextWidthLoginResult.x) * 0.5f + _FormWidth * 0.5f,
                _FormY + _FormHeight * 0.75f
            };

            DrawTextEx(_Font[1], LoginSuccessText, _TextLoginResult, _FormHeight * 0.02f, 2, GREEN); 
            break;
        case LOGIN_EMPTY: 
            _TextWidthLoginResult = MeasureTextEx(_Font[1], LoginEmptyText, _FormHeight * 0.02f, 2);
            _TextLoginResult = (Vector2){
                _FormX + (_FormWidth / 2 - _TextWidthLoginResult.x) * 0.5f + _FormWidth * 0.5f,
                _FormY + _FormHeight * 0.75f
            };
            DrawTextEx(_Font[1], LoginEmptyText, _TextLoginResult, _FormHeight * 0.02f, 2, ORANGE); 
            break;
        case LOGIN_FAILED: 
            _TextWidthLoginResult = MeasureTextEx(_Font[1], LoginFailedText, _FormHeight * 0.02f, 2);
            _TextLoginResult = (Vector2){
                _FormX + (_FormWidth / 2 - _TextWidthLoginResult.x) * 0.5f + _FormWidth * 0.5f,
                _FormY + _FormHeight * 0.75f
            };
            DrawTextEx(_Font[1], LoginFailedText, _TextLoginResult, _FormHeight * 0.02f, 2, RED); 
            break;
    }

    if (CheckCollisionPointRec(mousePosition, buttonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (loginBox -> length > 0 && passwordBox -> length > 0) {
            LOGIN_RESULT = CheckLogin(accounts, loginBox->text, passwordBox->text);
        }
        else {
            LOGIN_RESULT = LOGIN_EMPTY;
        }
    }

}
void DrawLoginUsername(InputBox *LoginBox, Texture2D IconUsername) {
    LoginBox->box = (Rectangle){
        _FormX + _FormWidth * 0.55f, 
        _FormY + _FormHeight * 0.5f, 
        _FormWidth * 0.4f,
        _FormHeight * 0.06f
    };

    Rectangle IconBox_Username = {
        _FormX + _FormWidth * 0.55f,
        _FormY + _FormHeight * 0.5f,
        LoginBox->box.height,
        LoginBox->box.height
    };

    Vector2 _Text = {
        LoginBox->box.x + IconBox_Username.width + 10,
        LoginBox->box.y + (LoginBox->box.height - MeasureTextEx(_Font[0], "a", FONT_SIZE, 1).y) / 2
    };

    DrawRectangleRounded(LoginBox->box, 0.1f, 15, (CheckCollisionPointRec(mousePosition, LoginBox->box) || LoginBox->isFocused || LoginBox->length > 0) ? WHITE : LIGHTGRAY);
    
    DrawRectangleRec(IconBox_Username, LIGHTGRAY);
    DrawRectangleRoundedLinesEx(LoginBox->box, 0.1f, 1, 2.0f, BLACK);

    DrawLineEx((Vector2){IconBox_Username.x + IconBox_Username.width, IconBox_Username.y}, (Vector2){IconBox_Username.x + IconBox_Username.width, IconBox_Username.y + IconBox_Username.height}, 2.0f, BLACK);

    DrawIcon(IconBox_Username, IconUsername);

    if (CheckCollisionPointRec(mousePosition, LoginBox->box) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        LoginBox->isFocused = true;
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        LoginBox->isFocused = false;
    }

    if (LoginBox->isFocused) {
        UpdateInputBox(LoginBox);

        DrawTextEx(_Font[0], LoginBox->text, _Text, FONT_SIZE, 1, BLACK);

        float w = MeasureTextEx(_Font[0], LoginBox->text,FONT_SIZE, 1).x;
        if ((int)(GetTime()*2) % 2 == 0) {
            DrawRectangle(_Text.x + w + 2, _Text.y, 2,FONT_SIZE, BLACK);
        }
    }
    else if (LoginBox -> length > 0) {
        DrawTextEx(_Font[0], LoginBox->text, _Text,FONT_SIZE, 1, BLACK);
    }
    

}
void DrawLoginPassword(InputBox *PasswordBox, Texture2D IconPassword, Texture2D ShowPasswordIcon) {
    PasswordBox->box = (Rectangle){
        _FormX + _FormWidth * 0.55f, 
        _FormY + _FormHeight * 0.6f, 
        _FormWidth * 0.4f,
        _FormHeight * 0.06f
    };

    Rectangle IconBox_Password = {
        _FormX + _FormWidth * 0.55f,
        _FormY + _FormHeight * 0.6f,
        PasswordBox->box.height,
        PasswordBox->box.height
    };
    Rectangle ShowPasswordBox = {
        PasswordBox->box.x + PasswordBox->box.width - IconBox_Password.width,
        PasswordBox->box.y,
        IconBox_Password.width,
        IconBox_Password.height
    };

    Vector2 _Text = {
        PasswordBox->box.x + IconBox_Password.width + 10,
        PasswordBox->box.y + (PasswordBox->box.height - MeasureTextEx(_Font[0], "a", FONT_SIZE, 1).y) / 2
    };

    DrawRectangleRounded(PasswordBox->box, 0.1f, 15, (CheckCollisionPointRec(mousePosition, PasswordBox->box) || PasswordBox->isFocused || PasswordBox->length > 0) ? WHITE : LIGHTGRAY);
    
    DrawRectangleRec(IconBox_Password, LIGHTGRAY);
    DrawRectangleRoundedLinesEx(PasswordBox->box, 0.1f, 1, 2.0f, BLACK);
    
    DrawLineEx((Vector2){IconBox_Password.x + IconBox_Password.width, IconBox_Password.y}, (Vector2){IconBox_Password.x + IconBox_Password.width, IconBox_Password.y + IconBox_Password.height}, 2.0f, BLACK);
    
    DrawIcon(IconBox_Password, IconPassword);
    DrawIcon(ShowPasswordBox, ShowPasswordIcon);
    if (CheckCollisionPointRec(mousePosition, PasswordBox->box) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        PasswordBox->isFocused = true;
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        PasswordBox->isFocused = false;
    }

    if (PasswordBox->isFocused) {
        UpdateInputBox(PasswordBox);

        char maskedText[MAX_INPUT];
        float w = MeasureTextEx(_Font[0], PasswordBox->text, FONT_SIZE, 1).x;
        memset(maskedText, '*', PasswordBox->length);
        maskedText[PasswordBox->length] = '\0';

        if (CheckCollisionPointRec(mousePosition, ShowPasswordBox) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            DrawTextEx(_Font[0], PasswordBox->text, _Text, FONT_SIZE, 1, BLACK);
            w = MeasureTextEx(_Font[0], PasswordBox->text,FONT_SIZE, 1).x;
            if ((int)(GetTime()*2) % 2 == 0) {
                DrawRectangle(_Text.x + w + 2, _Text.y, 2,FONT_SIZE, BLACK);
            }
        }
        else {
            DrawTextEx(_Font[0], maskedText, _Text, FONT_SIZE, 1, BLACK);
            w = MeasureTextEx(_Font[0], maskedText, FONT_SIZE, 1).x;
            if ((int)(GetTime()*2) % 2 == 0) {
                DrawRectangle(_Text.x + w + 2, _Text.y, 2, FONT_SIZE, BLACK);
            }
        }
    }

    else if (PasswordBox -> length > 0) {
        char maskedText[MAX_INPUT];
        memset(maskedText, '*', PasswordBox->length);
        maskedText[PasswordBox->length] = '\0';

        DrawTextEx(_Font[0], (CheckCollisionPointRec(mousePosition, PasswordBox->box) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) ? PasswordBox->text : maskedText, _Text, FONT_SIZE, 1, BLACK);
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
int GetAccountData(const char *filename, AccountList accounts, int *accountCount) {
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
int SaveAccountData(const char *filename, AccountList accounts, int accountCount) {
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
int lenStringUTF8(const char *str) {
    int len = 0;
    while (*str) {
        if ((*str & 0xC0) != 0x80) { // Kiểm tra nếu không phải là byte tiếp theo của một ký tự UTF-8
            len++;
        }
        str++;
    }
    return len;
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

    // Dịch chuỗi về đầu mảng
    if (start != str) {
        memmove(str, start, end - start + 2); // +2 để bao gồm cả ký tự null
    }
}
Font SetFontUTF8(const char *_font, int _fontSize)
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

void DrawIcon(Rectangle box, Texture2D icon) {
    float scaleX = box.width / icon.width;
    float scaleY = box.height / icon.height;
    float scale = (scaleX < scaleY) ? scaleX : scaleY;

    Vector2 origin = { 0, 0 };
    Rectangle source = { 0, 0, icon.width, icon.height };
    Rectangle dest = {
        box.x + (box.width - icon.width * scale) / 2,
        box.y + (box.height - icon.height * scale) / 2,
        icon.width * scale,
        icon.height * scale
    };

    DrawTexturePro(icon, source, dest, origin, 0.0f, WHITE);
}