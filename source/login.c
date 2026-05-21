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
        (Vector2) {
            GetMonitorWidth(0),
            GetMonitorHeight(0)
        },
        (Vector2) {
            GetScreenWidth(),
            GetScreenHeight()
        },
        (Vector2) {
            GetScreenWidth() / GetMonitorWidth(0),
            GetScreenHeight() /GetMonitorHeight(0)
        },
        GetMousePosition()
    );

    Rectangle Form = {
        LoginSize.Screen.x * 0.1f,
        LoginSize.Screen.y * 0.1f,
        LoginSize.Screen.x * 0.8f,
        LoginSize.Screen.y * 0.8f
    };
    
    AccountList _accounts = CreateHeaderNode();
    if (_accounts == NULL) {
        printf("Failed to create header node for accounts.\n");
        return FAILED;
    }

    int _accountCount = 0;
    printf("Loading account data...\n");
    if (LoginGetAccountData("data\\accounts.txt", _accounts, &_accountCount) == FAILED) {
        printf("Failed to load account data.\n");
        return FAILED;
    }

    Texture2D Img_Bg = LoadTexture(LOGIN_BACKGROUND_IMG);
    Texture2D Img_FormLogin = LoadTexture(FORM_LOGIN_IMG);

    Font _Font[2];

    _Font[0] = SetFontUTF8(ArialBold, 100);
    _Font[1] = SetFontUTF8(Roboto_Semibold, 100);

    InputBox _LoginBox = { 0 }; 
        Texture2D _IconUsername = LoadTexture(ICON_USERNAME);

    InputBox _PasswordBox = { 0 }; 
        Texture2D _IconPassword = LoadTexture(ICON_PASSWORD);
        Texture2D _ShowPasswordIcon = LoadTexture(SHOW_PASSWORD);

    while (!WindowShouldClose() && LOGIN_RESULT != LOGIN_SUCCESS) {
        if (IsWindowResized()){
            LoadSize(
                &LoginSize,
                (Vector2) {0},
                (Vector2) {
                    GetScreenWidth(),
                    GetScreenHeight()
                },
                (Vector2) {
                    GetScreenWidth() / GetMonitorWidth(0),
                    GetScreenHeight() /GetMonitorHeight(0)
                },
                (Vector2) {0}
            );

            Form = (Rectangle) {
                LoginSize.Screen.x * 0.1f,
                LoginSize.Screen.y * 0.1f,
                LoginSize.Screen.x * 0.8f,
                LoginSize.Screen.y * 0.8f
            };
        }

        LoginSize.Mouse = GetMousePosition();

        BeginDrawing();
        ClearBackground(BLACK);
        
        LoginDrawBackground(LoginSize, Img_Bg);
        LoginDrawForm(Form, Img_FormLogin, _Font);
        LoginDrawLoginForm(
            &_LoginBox, 
            &_PasswordBox, 
            _accounts, 
            _accountCount, 
            _IconUsername, 
            _IconPassword, 
            _ShowPasswordIcon,
            _Font,
            Form,
            LoginSize.Mouse,
            &LOGIN_RESULT
        );

        EndDrawing();
    }

    LoginSaveAccountData("data\\accounts.txt", _accounts, _accountCount);

    for (int i = 0; i < 2; i++)
        UnloadFont(_Font[i]);

    UnloadTexture(Img_Bg);
    UnloadTexture(Img_FormLogin);
    UnloadTexture(_IconUsername);
    UnloadTexture(_IconPassword);
    UnloadTexture(_ShowPasswordIcon);


    Get_account_return(
        _accounts, 
        account_return, 
        _LoginBox.text
    );

    return LOGIN_SUCCESS;
}


void LoginDrawBackground(Size LoginSize, Texture2D Img_Bg){
    float scaleX = LoginSize.Monitor.x / Img_Bg.width;
    float scaleY = LoginSize.Monitor.y / Img_Bg.height;
    float scale = (scaleX > scaleY)? scaleX : scaleY;

    Rectangle source = {
        0,
        0,
        Img_Bg.width,
        Img_Bg.height
    };

    Rectangle dest = {
        (LoginSize.Screen.x - Img_Bg.width * scale) / 2,
        (LoginSize.Screen.y - Img_Bg.height * scale) / 2,
        Img_Bg.width * scale,
        Img_Bg.height * scale
    };

    Vector2 origin = { 
        0, 
        0 
    };
    
    DrawTexturePro(
        Img_Bg, 
        source, 
        dest, 
        origin, 
        0.0f, 
        WHITE
    );

    DrawRectangleGradientV(
        0, 
        0, 
        LoginSize.Screen.x, 
        LoginSize.Screen.y, 
        Fade(WHITE, 0.0f), 
        Fade(BLACK, 0.5f)
    );
}
void LoginDrawForm(Rectangle Form, Texture2D Img_FormLogin, Font *_Font){    
    Rectangle FormRect = { 
        Form.x, 
        Form.y, 
        Form.width, 
        Form.height 
    };

    Rectangle FormRectLoginRight = { 
        Form.x + Form.width / 2, 
        Form.y, 
        Form.width / 2, 
        Form.height 
    };

    Rectangle FormRectLoginLeft = { 
        Form.x, 
        Form.y, 
        Form.width / 2, 
        Form.height
    };

    Rectangle BoxWelcomeText = {
        Form.x + (Form.width / 2) * 0.1f,
        Form.y + Form.height * 0.3f,
        Form.width / 2 * 0.8f,
        Form.height * 0.5f,
    };

    DrawRectangleRec(
        FormRect, 
        WHITE
    );

    float _ScaleTextureY = FormRectLoginRight.height / Img_FormLogin.height;
    float _ScaleTextureX = FormRectLoginRight.width / Img_FormLogin.width;

    float _ScaleTexture = (_ScaleTextureX > _ScaleTextureY) ? _ScaleTextureX : _ScaleTextureY;

    Rectangle source = {
        Img_FormLogin.width * 0.2f,
        Img_FormLogin.height - Form.height / _ScaleTexture,
        FormRectLoginRight.width / _ScaleTexture,
        FormRectLoginRight.height / _ScaleTexture
    };

    Rectangle dest = {
        Form.x,
        Form.y,
        source.width * _ScaleTexture,
        source.height * _ScaleTexture
    };

    Vector2 origin = { 0, 0 };

    DrawTexturePro(
        Img_FormLogin, 
        source, 
        dest, 
        origin, 
        0.0f, 
        WHITE
    );
    
    Vector2 _TextWidth = MeasureTextEx(
        _Font[0], 
        WELCOME_TEXT, 
        Form.height * 0.06f, 
        2
    );
    
    int TextPerRect = _TextWidth.x / (Form.width / 2);
    int FontSize = Form.height * 0.06f * 0.7f / TextPerRect;

    _TextWidth = MeasureTextEx(
        _Font[0], 
        WELCOME_TEXT, 
        FontSize, 
        2
    );

    Vector2 _Text = {
        Form.x + (Form.width / 2 - _TextWidth.x) * 0.5f,
        Form.y + Form.height * 0.1f
    };

    Vector2 _ShadowText = {
        _Text.x - 1,
        _Text.y
    };

    DrawRectangleGradientV(
        FormRectLoginLeft.x, 
        FormRectLoginLeft.y, 
        FormRectLoginLeft.width, 
        FormRectLoginLeft.height, 
        Fade(PINK, 0.2f),
        Fade(BLACK, 0.4f)
    );

    DrawRectangleRounded(
        BoxWelcomeText, 
        0.1f, 
        100, 
        Fade(WHITE, 0.5f)
    );

    DrawTextEx(
        _Font[0], 
        WELCOME_TEXT, 
        _ShadowText, 
        FontSize + 2, 
        1, 
        YELLOW
    );

    DrawTextEx(
        _Font[0], 
        WELCOME_TEXT, 
        _Text, 
        FontSize, 
        2, 
        Fade(BLACK, 0.8f)
    );
}
void LoginDrawLoginForm(InputBox *_loginBox, InputBox *_passwordBox, AccountList accounts, int accountCount, Texture2D _IconUsername, Texture2D _IconPassword, Texture2D _ShowPasswordIcon, Font *_Font, Rectangle Form, Vector2 mouse, LoginResult *LOGIN_RESULT) {
    Vector2 _TextWidth = MeasureTextEx(
        _Font[0], 
        LOGIN_TEXT, 
        Form.height * 0.1f, 
        2
    );

    Vector2 _Text = {
        Form.x + Form.width/2 + (Form.width / 2 - _TextWidth.x) * 0.5f,
        Form.y + Form.height * 0.2f
    };

    DrawTextEx(
        _Font[0], 
        LOGIN_TEXT, 
        _Text, 
        Form.height * 0.1f, 
        2, 
        BLACK
    );

    LoginDrawLoginUsername(_loginBox, _IconUsername, Form, mouse, _Font);
    LoginDrawLoginPassword(_passwordBox, _IconPassword, _ShowPasswordIcon, Form, _Font, mouse);

    Vector2 _TextWidthButton = MeasureTextEx(
        _Font[1], 
        LoginButtonText, 
        Form.height * 0.04f, 
        2
    );

    Rectangle buttonRect = {
        Form.x + (Form.width / 2 - _TextWidthButton.x) * 0.5f + Form.width * 0.5f - 20,
        Form.y + Form.height * 0.8f - 10,
        _TextWidthButton.x + 40,
        _TextWidthButton.y + 20
    };

    DrawRectangleRounded(
        buttonRect, 
        0.1f, 
        15, 
        (CheckCollisionPointRec(mouse, buttonRect)) ? Fade(WHITE, 0.5f) : LIGHTGRAY
    );

    DrawRectangleRoundedLinesEx(
        buttonRect, 
        0.1f, 
        15, 
        2.0f, 
        BLACK
    );

    Vector2 _TextButton = {
        buttonRect.x + 20,
        buttonRect.y + 10
    };

    DrawTextEx(
        _Font[1], 
        LoginButtonText, 
        _TextButton, 
        Form.height * 0.04f, 
        2, 
        BLACK
    );

    Vector2 _TextWidthLoginResult;
    Vector2 _TextLoginResult;
    
    switch(*LOGIN_RESULT)
    {
        case LOGIN_IDLE: 
            break;
        case LOGIN_SUCCESS: 
            _TextWidthLoginResult = MeasureTextEx(
                _Font[1], 
                LoginSuccessText, 
                Form.height * 0.02f, 
                2
            );
            
            _TextLoginResult = (Vector2){
                Form.x + (Form.width / 2 - _TextWidthLoginResult.x) * 0.5f + Form.width * 0.5f,
                Form.y + Form.height * 0.75f
            };

            DrawTextEx(
                _Font[1], 
                LoginSuccessText, 
                _TextLoginResult, 
                Form.height * 0.02f, 
                2, 
                GREEN
            );

            break;
        case LOGIN_EMPTY: 
            _TextWidthLoginResult = MeasureTextEx(
                _Font[1], 
                LoginEmptyText, 
                Form.height * 0.02f, 
                2
            );

            _TextLoginResult = (Vector2){
                Form.x + (Form.width / 2 - _TextWidthLoginResult.x) * 0.5f + Form.width * 0.5f,
                Form.y + Form.height * 0.75f
            };

            DrawTextEx(
                _Font[1], 
                LoginEmptyText, 
                _TextLoginResult, 
                Form.height * 0.02f, 
                2, 
                ORANGE
            );

            break;
        case LOGIN_FAILED: 
            _TextWidthLoginResult = MeasureTextEx(
                _Font[1], 
                LoginFailedText, 
                Form.height * 0.02f, 
                2
            );
            
            _TextLoginResult = (Vector2){
                Form.x + (Form.width / 2 - _TextWidthLoginResult.x) * 0.5f + Form.width * 0.5f,
                Form.y + Form.height * 0.75f
            };

            DrawTextEx(
                _Font[1], 
                LoginFailedText, 
                _TextLoginResult, 
                Form.height * 0.02f, 
                2, 
                RED
            );

            break;
    }

    if (CheckCollisionPointRec(mouse, buttonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (_loginBox -> length > 0 && _passwordBox -> length > 0) {
            *LOGIN_RESULT = CheckLogin(accounts, _loginBox->text, _passwordBox->text);
        }
        else {
            *LOGIN_RESULT = LOGIN_EMPTY;
        }
    }
}
void LoginDrawLoginUsername(InputBox *_LoginBox, Texture2D _IconUsername, Rectangle Form, Vector2 mouse, Font *_Font) {
    _LoginBox->box = (Rectangle){
        Form.x + Form.width * 0.55f, 
        Form.y + Form.height * 0.5f, 
        Form.width * 0.4f,
        Form.height * 0.06f
    };

    Rectangle IconBox_Username = {
        Form.x + Form.width * 0.55f,
        Form.y + Form.height * 0.5f,
        _LoginBox->box.height,
        _LoginBox->box.height
    };

    DrawRectangleRounded(
        _LoginBox->box, 
        0.1f, 
        15, 
        (CheckCollisionPointRec(mouse, _LoginBox->box) || _LoginBox->isFocused || _LoginBox->length > 0) ? WHITE : LIGHTGRAY
    );
    
    DrawRectangleRec(
        IconBox_Username, 
        LIGHTGRAY
    );

    DrawRectangleRoundedLinesEx(
        _LoginBox->box, 
        0.1f, 
        1, 
        2.0f, 
        BLACK
    );

    DrawLineEx(
        (Vector2) {
            IconBox_Username.x + IconBox_Username.width, 
            IconBox_Username.y
        },
        (Vector2) {
            IconBox_Username.x + IconBox_Username.width, 
            IconBox_Username.y + IconBox_Username.height
        }, 
        2.0f, 
        BLACK
    );

    DrawIcon(
        IconBox_Username, 
        _IconUsername
    );

    if (CheckCollisionPointRec(mouse, _LoginBox->box) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        _LoginBox->isFocused = true;
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        _LoginBox->isFocused = false;
    }

    Rectangle _Text = {
        _LoginBox->box.x + IconBox_Username.width + 5,
        _LoginBox->box.y,
        _LoginBox->box.width - IconBox_Username.width - 10,
        _LoginBox->box.height
    };

    BeginScissorMode(
        _Text.x,
        _Text.y,
        _Text.width,
        _Text.height
    );

    if (_LoginBox->isFocused) {
        UpdateInputBox(_LoginBox);

        float w = MeasureTextEx (
            _Font[0],
            _LoginBox->text,
            20,
            1
        ).x;

        float dx = (w - _Text.width + 5) >= 0 ? (w - _Text.width + 5) : 0;
        Vector2 pos = {
            _Text.x - dx,
            _Text.y + (_Text.height - 20) / 2
        };

        DrawTextEx(
            _Font[0], 
            _LoginBox->text, 
            pos,
            20, 
            1, 
            BLACK
        );

        if ((int)(GetTime()*2) % 2 == 0) {
            DrawRectangle(
                pos.x + w + 1, 
                pos.y, 
                2,
                20, 
                BLACK
            );
        }
    }
    else if (_LoginBox -> length > 0) {
        float w = MeasureTextEx (
            _Font[0],
            _LoginBox->text,
            20,
            1
        ).x;

        float dx = (w - _Text.width - 5) >= 0 ? (w - _Text.width - 5) : 0;
        Vector2 pos = {
            _Text.x - dx,
            _Text.y + (_Text.height - 20) / 2
        };

        DrawTextEx(
            _Font[0], 
            _LoginBox->text, 
            pos,
            20, 
            1, 
            BLACK
        );
    }
    EndScissorMode();
}
void LoginDrawLoginPassword(InputBox *_PasswordBox, Texture2D _IconPassword, Texture2D _ShowPasswordIcon, Rectangle Form, Font *_Font, Vector2 mouse) {
    _PasswordBox->box = (Rectangle){
        Form.x + Form.width * 0.55f, 
        Form.y + Form.height * 0.6f, 
        Form.width * 0.4f,
        Form.height * 0.06f
    };

    Rectangle IconBox_Password = {
        Form.x + Form.width * 0.55f,
        Form.y + Form.height * 0.6f,
        _PasswordBox->box.height,
        _PasswordBox->box.height
    };
    Rectangle Show_PasswordBox = {
        _PasswordBox->box.x + _PasswordBox->box.width - IconBox_Password.width,
        _PasswordBox->box.y,
        IconBox_Password.width,
        IconBox_Password.height
    };

    DrawRectangleRounded(
        _PasswordBox->box, 
        0.1f, 
        15, 
        (CheckCollisionPointRec(mouse, _PasswordBox->box) || _PasswordBox->isFocused || _PasswordBox->length > 0) ? WHITE : LIGHTGRAY
    );
    
    DrawRectangleRec(
        IconBox_Password, 
        LIGHTGRAY
    );
    
    DrawRectangleRoundedLinesEx(
        _PasswordBox->box, 
        0.1f, 
        1, 
        2.0f, 
        BLACK
    );
    
    DrawLineEx(
        (Vector2) {
            IconBox_Password.x + IconBox_Password.width, 
            IconBox_Password.y
        }, 
        (Vector2) {
            IconBox_Password.x + IconBox_Password.width, 
            IconBox_Password.y + IconBox_Password.height
        }, 
        2.0f, 
        BLACK
    );
    
    DrawIcon(
        IconBox_Password, 
        _IconPassword
    );
    
    DrawIcon(
        Show_PasswordBox, 
        _ShowPasswordIcon
    );
    if (CheckCollisionPointRec(mouse, _PasswordBox->box) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        _PasswordBox->isFocused = true;
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        _PasswordBox->isFocused = false;
    }

    Rectangle _Text = {
        _PasswordBox->box.x + IconBox_Password.width + 5,
        _PasswordBox->box.y,
        _PasswordBox->box.width - (IconBox_Password.width + Show_PasswordBox.width + 10),
        _PasswordBox->box.height
    };

    BeginScissorMode(
        _Text.x,
        _Text.y,
        _Text.width,
        _Text.height
    );

    if (_PasswordBox->isFocused) {
        UpdateInputBox(_PasswordBox);

        if (CheckCollisionPointRec(mouse, Show_PasswordBox) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            float w = MeasureTextEx(
                _Font[0], 
                _PasswordBox->text, 
                20, 
                1
            ).x;

            float dx = (w - (_Text.width - 5)) > 0 ? (w - (_Text.width - 5)) : 0;
            Vector2 pos = {
                _Text.x - dx,
                _Text.y + (_Text.height - 20) * 0.5f
            };

            DrawTextEx(
                _Font[0],
                _PasswordBox->text,
                pos,
                20,
                1,
                BLACK
            );

            if ((int)(GetTime() * 2) % 2 == 0){
                DrawRectangle(
                    pos.x + w + 2,
                    pos.y,
                    2,
                    20,
                    BLACK           
                );
            }
        }
        else {
            char maskedText[MAX_INPUT];
        
            memset(
                maskedText, 
                '*', 
                _PasswordBox->length
            );

            maskedText[_PasswordBox->length] = '\0';

            float w = MeasureTextEx(
                _Font[0], 
                maskedText, 
                20, 
                1
            ).x;

            float dx = (w - (_Text.width - 5)) > 0 ? (w - (_Text.width - 5)) : 0;
            Vector2 pos = {
                _Text.x - dx,
                _Text.y + (_Text.height - 20) * 0.5f
            };

            DrawTextEx(
                _Font[0],
                maskedText,
                pos,
                20,
                1,
                BLACK
            );

            if ((int)(GetTime() * 2) % 2 == 0){
                DrawRectangle(
                    pos.x + w + 2,
                    pos.y,
                    2,
                    20,
                    BLACK           
                );
            }
        }
    }

    else if (_PasswordBox -> length > 0) {
        if (CheckCollisionPointRec(mouse, Show_PasswordBox) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            float w = MeasureTextEx(
                _Font[0], 
                _PasswordBox->text, 
                20, 
                1
            ).x;

            float dx = (w - (_Text.width - 5)) > 0 ? (w - (_Text.width - 5)) : 0;
            Vector2 pos = {
                _Text.x - dx,
                _Text.y + (_Text.height - 20) * 0.5f
            };

            DrawTextEx(
                _Font[0],
                _PasswordBox->text,
                pos,
                20,
                1,
                BLACK
            );
        }
        else {
            char maskedText[MAX_INPUT];
        
            memset(
                maskedText, 
                '*', 
                _PasswordBox->length
            );

            maskedText[_PasswordBox->length] = '\0';

            float w = MeasureTextEx(
                _Font[0], 
                maskedText, 
                20, 
                1
            ).x;

            float dx = (w - (_Text.width - 5)) > 0 ? (w - (_Text.width - 5)) : 0;
            Vector2 pos = {
                _Text.x - dx,
                _Text.y + (_Text.height - 20) * 0.5f
            };

            DrawTextEx(
                _Font[0],
                maskedText,
                pos,
                20,
                1,
                BLACK
            );
        }
    }
    EndScissorMode();
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
int LoginGetAccountData(const char *filename, AccountList accounts, int *accountCount) {
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
int LoginSaveAccountData(const char *filename, AccountList accounts, int accountCount) {
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

CheckResult Get_account_return(AccountList _account, Account *account, const char *username) {
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