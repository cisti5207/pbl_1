// login.c - Refactored version
// Bỏ WelcomeText, form login full width, ảnh ở nửa trên

#include "raylib.h"
#include "login.h"
#include "libmanage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int InitLogin(Account *account_return)
{
    LoginResult LOGIN_RESULT = LOGIN_IDLE;

    Size LoginSize;
    LoadSize(
        &LoginSize,
        (Vector2){GetMonitorWidth(0), GetMonitorHeight(0)},
        (Vector2){GetScreenWidth(), GetScreenHeight()},
        (Vector2){(float)GetScreenWidth() / GetMonitorWidth(0), (float)GetScreenHeight() / GetMonitorHeight(0)},
        GetMousePosition());

    Rectangle Form = {
        LoginSize.Screen.x * 0.1f,
        LoginSize.Screen.y * 0.1f,
        LoginSize.Screen.x * 0.8f,
        LoginSize.Screen.y * 0.8f};

    AccountList _accounts = CreateHeaderNode();
    if (_accounts == NULL)
    {
        printf("Failed to create header node for accounts.\n");
        return FAILED;
    }

    int _accountCount = 0;
    printf("Loading account data...\n");
    if (LoginGetAccountData("data\\accounts.txt", _accounts, &_accountCount) == FAILED)
    {
        printf("Failed to load account data.\n");
        return FAILED;
    }

    Texture2D Img_FormLogin = LoadTexture(FORM_LOGIN_IMG);

    Font _Font[2];
    _Font[0] = SetFontUTF8(ArialBold, 100);
    _Font[1] = SetFontUTF8(Roboto_Semibold, 100);

    InputBox _LoginBox = {0};
    Texture2D _IconUsername = LoadTexture(ICON_USERNAME);

    InputBox _PasswordBox = {0};
    Texture2D _IconPassword = LoadTexture(ICON_PASSWORD);

    while (!WindowShouldClose() && LOGIN_RESULT != LOGIN_SUCCESS)
    {
        UpdateParticlesPosition(LoginSize);

        if (IsWindowResized())
        {
            LoadSize(
                &LoginSize,
                (Vector2){0},
                (Vector2){GetScreenWidth(), GetScreenHeight()},
                (Vector2){(float)GetScreenWidth() / GetMonitorWidth(0), (float)GetScreenHeight() / GetMonitorHeight(0)},
                (Vector2){0});

            Form = (Rectangle){
                LoginSize.Screen.x * 0.1f, LoginSize.Screen.y * 0.1f,
                LoginSize.Screen.x * 0.8f, LoginSize.Screen.y * 0.8f};
        }

        LoginSize.Mouse = GetMousePosition();

        BeginDrawing();
        ClearBackground(AnimatedBackground());
        DrawBackgroundParticles();

        LoginDrawForm(Form, Img_FormLogin, _Font);
        LoginDrawLoginForm(
            &_LoginBox, &_PasswordBox, _accounts, _accountCount,
            _IconUsername, _IconPassword,
            _Font, Form, LoginSize.Mouse, &LOGIN_RESULT);

        EndDrawing();
    }

    LoginSaveAccountData("data\\accounts.txt", _accounts, _accountCount);

    for (int i = 0; i < 2; i++)
        UnloadFont(_Font[i]);
    UnloadTexture(Img_FormLogin);
    UnloadTexture(_IconUsername);
    UnloadTexture(_IconPassword);

    Get_account_return(_accounts, account_return, _LoginBox.text);

    Node *current_node = _accounts->next;
    while (current_node != NULL)
    {
        Node *temp = current_node;
        current_node = current_node->next;
        free(temp->account);
        free(temp);
    }
    free(_accounts);

    if (LOGIN_RESULT == LOGIN_SUCCESS)
        return LOGIN_SUCCESS;
    return FAILED;
}

// ============================================================
//  LoginDrawForm: Bỏ cột welcome, ảnh chiếm nửa trên form
// ============================================================
void LoginDrawForm(Rectangle Form, Texture2D Img_FormLogin, Font *_Font)
{
    Color BG_LIGHT = (Color){ 220, 238, 248, 255 }; 

    // Vùng ảnh (48% trên)
    Rectangle ImgArea = {Form.x, Form.y, Form.width, Form.height * 0.48f};

    // Bước 1: Nền form (không bo góc, không viền — clean)
    DrawRectangleRec(
        (Rectangle){Form.x, Form.y, Form.width, Form.height},
        BG_LIGHT);

    // Bước 2: Vẽ ảnh login
    if (Img_FormLogin.id > 0)
    {
        float scaleX = ImgArea.width / (float)Img_FormLogin.width;
        float scaleY = ImgArea.height / (float)Img_FormLogin.height;
        float scale = (scaleX > scaleY) ? scaleX : scaleY;

        float srcW = ImgArea.width / scale;
        float srcH = ImgArea.height / scale;
        float srcX = (Img_FormLogin.width - srcW) * 0.5f;
        float srcY = (Img_FormLogin.height - srcH) * 0.5f;

        Rectangle src = {srcX, srcY, srcW, srcH};
        Rectangle dest = {ImgArea.x, ImgArea.y, ImgArea.width, ImgArea.height};

        BeginScissorMode((int)Form.x, (int)Form.y, (int)Form.width, (int)(Form.height * 0.48f));
        DrawTexturePro(Img_FormLogin, src, dest, (Vector2){0, 0}, 0.0f, DARKBLUE);
        EndScissorMode();
    }

    // Không fade, không viền ngoài
}

// ============================================================
//  LoginDrawLoginForm: input fields căn giữa toàn form
// ============================================================
void LoginDrawLoginForm(
    InputBox *_loginBox, InputBox *_passwordBox,
    AccountList accounts, int accountCount,
    Texture2D _IconUsername, Texture2D _IconPassword,
    Font *_Font, Rectangle Form, Vector2 mouse, LoginResult *LOGIN_RESULT)
{
    // Tông xanh chủ đạo
    Color ACCENT_COLOR = (Color){200, 225, 255, 255}; // nút: trắng xanh nhạt

    // --- Tiêu đề "USER LOGIN" ---
    float titleFontSize = Form.height * 0.055f;
    Vector2 titleSize = MeasureTextEx(_Font[0], LOGIN_TEXT, titleFontSize, 2);
    Vector2 titlePos = {
        Form.x + (Form.width - titleSize.x) * 0.5f,
        Form.y + Form.height * 0.53f};
    DrawTextEx(_Font[0], LOGIN_TEXT, titlePos, titleFontSize, 2, DARKBLUE);

    // --- Divider dưới tiêu đề ---
    float divY = titlePos.y + titleSize.y + Form.height * 0.015f;
    float divW = Form.width * 0.3f;
    DrawLineEx(
        (Vector2){Form.x + (Form.width - divW) * 0.5f, divY},
        (Vector2){Form.x + (Form.width + divW) * 0.5f, divY},
        1.5f, Fade(DARKBLUE, 0.5f));

    // --- Input boxes căn giữa ---
    LoginDrawLoginUsername(_loginBox, _IconUsername, Form, mouse, _Font);
    LoginDrawLoginPassword(_passwordBox, _IconPassword, Form, _Font, mouse);

    // --- Nút LOGIN ---
    float btnFontSize = Form.height * 0.038f;
    Vector2 btnTextSize = MeasureTextEx(_Font[1], LoginButtonText, btnFontSize, 2);
    float btnW = btnTextSize.x + Form.width * 0.08f;
    float btnH = btnTextSize.y + Form.height * 0.025f;
    Rectangle buttonRect = {
        Form.x + (Form.width - btnW) * 0.5f,
        Form.y + Form.height * 0.83f,
        btnW, btnH};

    bool btnHover = CheckCollisionPointRec(mouse, buttonRect);
    DrawRectangleRec(buttonRect, btnHover ? (Color){255, 255, 255, 230} : ACCENT_COLOR);
    DrawRectangleLinesEx(buttonRect, 1.0f, (Color){180, 210, 255, 200});

    Vector2 btnTextPos = {
        buttonRect.x + (buttonRect.width - btnTextSize.x) * 0.5f,
        buttonRect.y + (buttonRect.height - btnTextSize.y) * 0.5f};
    DrawTextEx(_Font[1], LoginButtonText, btnTextPos, btnFontSize, 2,
               (Color){10, 30, 80, 255}); // chữ xanh đậm trên nền trắng xanh

    // --- Thông báo kết quả ---
    float msgFontSize = Form.height * 0.025f;
    Vector2 msgSize;
    Vector2 msgPos;

    switch (*LOGIN_RESULT)
    {
    case LOGIN_IDLE:
        break;

    case LOGIN_SUCCESS:
        msgSize = MeasureTextEx(_Font[1], LoginSuccessText, msgFontSize, 2);
        msgPos = (Vector2){
            Form.x + (Form.width - msgSize.x) * 0.5f,
            buttonRect.y - msgSize.y - Form.height * 0.01f};
        DrawTextEx(_Font[1], LoginSuccessText, msgPos, msgFontSize, 2, GREEN);
        break;

    case LOGIN_EMPTY:
        msgSize = MeasureTextEx(_Font[1], LoginEmptyText, msgFontSize, 2);
        msgPos = (Vector2){
            Form.x + (Form.width - msgSize.x) * 0.5f,
            buttonRect.y - msgSize.y - Form.height * 0.01f};
        DrawTextEx(_Font[1], LoginEmptyText, msgPos, msgFontSize, 2, ORANGE);
        break;

    case LOGIN_FAILED:
        msgSize = MeasureTextEx(_Font[1], LoginFailedText, msgFontSize, 2);
        msgPos = (Vector2){
            Form.x + (Form.width - msgSize.x) * 0.5f,
            buttonRect.y - msgSize.y - Form.height * 0.01f};
        DrawTextEx(_Font[1], LoginFailedText, msgPos, msgFontSize, 2, RED);
        break;
    }

    // --- Xử lý click button ---
    if (btnHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (_loginBox->length > 0 && _passwordBox->length > 0)
            *LOGIN_RESULT = CheckLogin(accounts, _loginBox->text, _passwordBox->text);
        else
            *LOGIN_RESULT = LOGIN_EMPTY;
    }
}

// ============================================================
//  LoginDrawLoginUsername: căn giữa form, style tối
// ============================================================
void LoginDrawLoginUsername(InputBox *_LoginBox, Texture2D _IconUsername, Rectangle Form, Vector2 mouse, Font *_Font)
{
    float boxW = Form.width * 0.6f;
    float boxH = Form.height * 0.065f;
    float boxX = Form.x + (Form.width - boxW) * 0.5f;
    float boxY = Form.y + Form.height * 0.62f;

    _LoginBox->box = (Rectangle){boxX, boxY, boxW, boxH};

    // Tông xanh navy — KHÔNG đổi trắng khi focus
    Color BOX_NORMAL = (Color){15, 30, 65, 255};     // nền thường   #0f1e41
    Color BOX_FOCUS = (Color){20, 40, 85, 255};      // nền khi focus #142855  (vẫn xanh tối)
    Color BORDER_IDLE = (Color){40, 80, 140, 180};   // viền xanh mờ
    Color BORDER_FOCUS = (Color){70, 160, 255, 230}; // viền xanh sáng khi focus
    Color TEXT_HINT = (Color){80, 120, 180, 200};    // placeholder xanh nhạt
    Color TEXT_COLOR = (Color){180, 215, 255, 255};  // chữ xanh trắng

    bool focused = _LoginBox->isFocused;
    bool hovered = CheckCollisionPointRec(mouse, _LoginBox->box);

    // Nền box — kích thước CỐ ĐỊNH, không dùng Rounded để tránh méo
    DrawRectangleRec(_LoginBox->box, focused ? BOX_FOCUS : (hovered ? BOX_FOCUS : BOX_NORMAL));
    // Viền sắc nét 1px — DrawRectangleLinesEx giữ đúng kích thước
    DrawRectangleLinesEx(_LoginBox->box, 1.0f, focused ? BORDER_FOCUS : BORDER_IDLE);

    // Icon
    Rectangle IconBox = {boxX, boxY, boxH, boxH};
    DrawIcon(IconBox, _IconUsername);

    // Divider dọc sau icon
    DrawLineEx(
        (Vector2){boxX + boxH, boxY + boxH * 0.15f},
        (Vector2){boxX + boxH, boxY + boxH * 0.85f},
        1.0f, BORDER_IDLE);

    // Focus / click
    if (CheckCollisionPointRec(mouse, _LoginBox->box) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        _LoginBox->isFocused = true;
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        _LoginBox->isFocused = false;

    // Vùng text (scissor chính xác)
    Rectangle _Text = {boxX + boxH + 5, boxY, boxW - boxH - 10, boxH};
    float fontSize = boxH * 0.45f;

    BeginScissorMode((int)_Text.x, (int)_Text.y, (int)_Text.width, (int)_Text.height);
    if (_LoginBox->isFocused)
    {
        UpdateInputBox(_LoginBox);
        float w = MeasureTextEx(_Font[0], _LoginBox->text, fontSize, 1).x;
        float dx = (w - _Text.width + 5) >= 0.0f ? (w - _Text.width + 5) : 0.0f;
        Vector2 pos = {_Text.x - dx, boxY + (_Text.height - fontSize) * 0.5f};
        DrawTextEx(_Font[0], _LoginBox->text, pos, fontSize, 1, TEXT_COLOR);
        if ((int)(GetTime() * 2) % 2 == 0)
            DrawRectangle((int)(pos.x + w + 1), (int)pos.y, 2, (int)fontSize, BORDER_FOCUS);
    }
    else if (_LoginBox->length > 0)
    {
        float w = MeasureTextEx(_Font[0], _LoginBox->text, fontSize, 1).x;
        float dx = (w - _Text.width - 5) >= 0.0f ? (w - _Text.width - 5) : 0.0f;
        Vector2 pos = {_Text.x - dx, boxY + (_Text.height - fontSize) * 0.5f};
        DrawTextEx(_Font[0], _LoginBox->text, pos, fontSize, 1, TEXT_COLOR);
    }
    else
    {
        Vector2 pos = {_Text.x + 4, boxY + (_Text.height - fontSize) * 0.5f};
        DrawTextEx(_Font[0], "USERNAME", pos, fontSize, 1, TEXT_HINT);
    }
    EndScissorMode();
}

// ============================================================
//  LoginDrawLoginPassword: căn giữa form, style tối
// ============================================================
void LoginDrawLoginPassword(InputBox *_PasswordBox, Texture2D _IconPassword, Rectangle Form, Font *_Font, Vector2 mouse)
{
    float boxW = Form.width * 0.6f;
    float boxH = Form.height * 0.065f;
    float boxX = Form.x + (Form.width - boxW) * 0.5f;
    float boxY = Form.y + Form.height * 0.72f;

    _PasswordBox->box = (Rectangle){boxX, boxY, boxW, boxH};

    Color BOX_NORMAL = (Color){15, 30, 65, 255};
    Color BOX_FOCUS = (Color){20, 40, 85, 255};
    Color BORDER_IDLE = (Color){40, 80, 140, 180};
    Color BORDER_FOCUS = (Color){70, 160, 255, 230};
    Color TEXT_HINT = (Color){80, 120, 180, 200};
    Color TEXT_COLOR = (Color){180, 215, 255, 255};

    bool focused = _PasswordBox->isFocused;
    bool hovered = CheckCollisionPointRec(mouse, _PasswordBox->box);

    DrawRectangleRec(_PasswordBox->box, focused ? BOX_FOCUS : (hovered ? BOX_FOCUS : BOX_NORMAL));
    DrawRectangleLinesEx(_PasswordBox->box, 1.0f, focused ? BORDER_FOCUS : BORDER_IDLE);

    // Chỉ icon password bên trái, không có nút show
    Rectangle IconBox = {boxX, boxY, boxH, boxH};
    DrawIcon(IconBox, _IconPassword);

    DrawLineEx(
        (Vector2){boxX + boxH, boxY + boxH * 0.15f},
        (Vector2){boxX + boxH, boxY + boxH * 0.85f},
        1.0f, BORDER_IDLE);

    if (CheckCollisionPointRec(mouse, _PasswordBox->box) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        _PasswordBox->isFocused = true;
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        _PasswordBox->isFocused = false;

    // Text region mở rộng hết chiều ngang (không cần trừ ShowPassBox)
    Rectangle _Text = {boxX + boxH + 5, boxY, boxW - boxH - 10, boxH};
    float fontSize = boxH * 0.45f;

    BeginScissorMode((int)_Text.x, (int)_Text.y, (int)_Text.width, (int)_Text.height);
    if (_PasswordBox->isFocused)
    {
        UpdateInputBox(_PasswordBox);
        char maskedText[MAX_INPUT + 1];
        memset(maskedText, '*', _PasswordBox->length);
        maskedText[_PasswordBox->length] = '\0';
        float w = MeasureTextEx(_Font[0], maskedText, fontSize, 1).x;
        float dx = (w - (_Text.width - 5)) > 0.0f ? (w - (_Text.width - 5)) : 0.0f;
        Vector2 pos = {_Text.x - dx, boxY + (_Text.height - fontSize) * 0.5f};
        DrawTextEx(_Font[0], maskedText, pos, fontSize, 1, TEXT_COLOR);
        if ((int)(GetTime() * 2) % 2 == 0)
            DrawRectangle((int)(pos.x + w + 2), (int)pos.y, 2, (int)fontSize, BORDER_FOCUS);
    }
    else if (_PasswordBox->length > 0)
    {
        char maskedText[MAX_INPUT + 1];
        memset(maskedText, '*', _PasswordBox->length);
        maskedText[_PasswordBox->length] = '\0';
        float w = MeasureTextEx(_Font[0], maskedText, fontSize, 1).x;
        float dx = (w - (_Text.width - 5)) > 0.0f ? (w - (_Text.width - 5)) : 0.0f;
        Vector2 pos = {_Text.x - dx, boxY + (_Text.height - fontSize) * 0.5f};
        DrawTextEx(_Font[0], maskedText, pos, fontSize, 1, TEXT_COLOR);
    }
    else
    {
        Vector2 pos = {_Text.x + 4, boxY + (_Text.height - fontSize) * 0.5f};
        DrawTextEx(_Font[0], "PASSWORD", pos, fontSize, 1, TEXT_HINT);
    }
    EndScissorMode();
}

// ============================================================
//  Các hàm còn lại giữ nguyên
// ============================================================
LoginResult CheckLogin(AccountList accounts, const char *username, const char *password)
{
    Node *current = accounts->next;
    while (current != NULL)
    {
        if (strcmp(current->account->username, username) == 0)
        {
            if (strcmp(current->account->password, password) == 0)
                return LOGIN_SUCCESS;
            else
                return LOGIN_FAILED;
        }
        current = current->next;
    }
    return LOGIN_FAILED;
}

int InsertAccount(AccountList pL, Account *e, AccountPosition p)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL)
        return FALSE;
    if (p == NULL)
        p = pL;
    newNode->account = e;
    newNode->next = p->next;
    newNode->prev = p;
    if (p->next != NULL)
        p->next->prev = newNode;
    p->next = newNode;
    return TRUE;
}

AccountList CreateHeaderNode()
{
    Node *header = (Node *)malloc(sizeof(Node));
    if (header == NULL)
        return NULL;
    header->next = NULL;
    header->prev = NULL;
    return header;
}

int LoginGetAccountData(const char *filename, AccountList accounts, int *accountCount)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Failed to open account data file.\n");
        return FAILED;
    }

    int count = 0;
    int expectedCount;
    fscanf(file, "Số lượng: %d\n", &expectedCount);

    while (!feof(file))
    {
        Account *tempAccount = (Account *)malloc(sizeof(Account));
        if (tempAccount == NULL)
        {
            fclose(file);
            return FAILED;
        }

        if (fscanf(file, " | %[^|]| %[^|]| %[^|]| %[^|]| %[^|]| %[^|]|",
                   tempAccount->username, tempAccount->password, tempAccount->realName,
                   tempAccount->dateOfBirth, tempAccount->cccd, tempAccount->role) == 6)
        {
            trim(tempAccount->username);
            trim(tempAccount->password);
            trim(tempAccount->realName);
            trim(tempAccount->dateOfBirth);
            trim(tempAccount->cccd);
            trim(tempAccount->role);

            if (InsertAccount(accounts, tempAccount, NULL) == FALSE)
            {
                free(tempAccount);
                fclose(file);
                return FAILED;
            }
            count++;
        }
        else
        {
            free(tempAccount);
            break;
        }
    }

    fclose(file);
    *accountCount = count;
    return (count == expectedCount) ? TRUE : FAILED;
}

int LoginSaveAccountData(const char *filename, AccountList accounts, int accountCount)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
        return FAILED;

    fprintf(file, "Số lượng: %d\n", accountCount);
    Node *current = accounts;
    while (current->next != NULL)
        current = current->next;

    while (current != accounts)
    {
        Account *acc = current->account;
        fprintf(file, "| %s%*s | %s%*s | %s%*s | %s%*s | %s%*s | %s%*s |\n",
                acc->username, LEN_SHOW_USERNAME - lenStringUTF8(acc->username), "",
                acc->password, LEN_SHOW_PASSWORD - lenStringUTF8(acc->password), "",
                acc->realName, LEN_SHOW_REALNAME - lenStringUTF8(acc->realName), "",
                acc->cccd, LEN_SHOW_CCCD - lenStringUTF8(acc->cccd), "",
                acc->dateOfBirth, LEN_SHOW_DOB - lenStringUTF8(acc->dateOfBirth), "",
                acc->role, LEN_SHOW_ROLE - lenStringUTF8(acc->role), "");
        current = current->prev;
    }

    fclose(file);
    return TRUE;
}

CheckResult Get_account_return(AccountList _account, Account *account, const char *username)
{
    Account A;
    _account = _account->next;
    while (_account != NULL)
    {
        A = *(_account->account);
        if (strcmp(username, A.username) == 0)
        {
            *account = A;
            return TRUE;
        }
        _account = _account->next;
    }
    return FAILED;
}