#include <stdio.h>
#include "ManageBooks.h"
#include "raylib.h"
#include "login.h"
#include "main.h"
#include <string.h>
#include <math.h>

AppState APP_STATE = LOGINAPP;

int main(){
    InitWindow(1200, 800, "Login");
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(1000, 600);

    Size MainSize;
    LoadSize(
        &MainSize,
        (Vector2) { 
            GetMonitorWidth(0), 
            GetMonitorHeight(0) 
        },
        (Vector2) { 
            GetScreenWidth(), 
            GetScreenHeight() 
        },
        (Vector2) { 
            (float)GetScreenWidth() / GetMonitorWidth(0), 
            (float)GetScreenHeight() / GetMonitorHeight(0) 
        },
        GetMousePosition()
    );

    // Khởi tạo hạt động 1 lần duy nhất từ thư viện libmanage
    InitParticles(MainSize);

    // Load font để hiển thị Tên Tiếng Việt không bị lỗi
    Font fontTitle = SetFontUTF8(ArialBold, 80); 
    Font fontSub = SetFontUTF8(Roboto_Semibold, 40);

    Account _Account;
    Texture2D userAvatar = {0};

    if (InitLogin(&_Account) == LOGIN_SUCCESS) {
        APP_STATE = HOME;
        LoadUserAvatar(_Account, &userAvatar); // Nạp avatar khi đăng nhập thành công
    } else {
        printf("Login failed or window closed.\n");
        CloseWindow();
        return 1;
    }

    Role _role;
    if (strcmp(_Account.role, "Administrator") == 0)
        _role = ADMINISTRATOR;
    else 
        _role = STAFF;

    MainContainers Containers;
    MainLoadContainers(&Containers, MainSize);
    
    while (!WindowShouldClose()){
        // Cập nhật vị trí hạt động từ thư viện
        UpdateParticlesPosition(MainSize);
        
        MainSize.Mouse = GetMousePosition();
        LoadSize(
            &MainSize,
            (Vector2) {0},
            (Vector2) { 
                GetScreenWidth(), 
                GetScreenHeight() 
            },
            (Vector2) { 
                (float)GetScreenWidth() / GetMonitorWidth(0), 
                (float)GetScreenHeight() / GetMonitorHeight(0) 
            },
            (Vector2) {0}
        );
    
        MainLoadContainers(&Containers, MainSize);

        BeginDrawing();
        // Gọi nền động từ thư viện
        ClearBackground(AnimatedBackground());
        DrawBackgroundParticles();

        MainDrawPanel(Containers);
        
        // Vẽ DecribeBox Xịn Xò đè lên khu vực đã chia
        MainDrawDescribeBox(Containers.DecribeBox, _Account, userAvatar, fontTitle, fontSub);

        MainFunc(Containers.ShowFuncBox, MainSize.Mouse);
        
        EndDrawing();

        switch(APP_STATE){
            case HOME:
                SetWindowTitle("Home");
                break;
            
            case LOGINAPP:
                SetWindowTitle("Login");
                
                _Account = (Account) {0};
                if (InitLogin(&_Account) == LOGIN_SUCCESS) {
                    APP_STATE = HOME;
                    if (strcmp(_Account.role, "Administrator") == 0)
                        _role = ADMINISTRATOR;
                    else 
                        _role = STAFF;
                    
                    // Nạp lại avatar mỗi khi đăng nhập một tài khoản khác
                    LoadUserAvatar(_Account, &userAvatar);
                } else {
                    printf("Login failed or window closed.\n");
                    goto EXIT_APP; 
                }

                printf("| %s | %s | %s | %s | %s | %s |\n",
                    _Account.username, _Account.realName, _Account.password,
                    _Account.cccd, _Account.dateOfBirth, _Account.role
                );
                break;

            case MANAGEBOOKS:
                SetWindowTitle("Manage Books");
                InitManageBooks(_role);
                APP_STATE = HOME;
                break;

            case MANAGEUSER:
                SetWindowTitle("Manage User");
                APP_STATE = HOME; 
                break;

            case MANAGEBORROWING:
                SetWindowTitle("Manage Borrow");
                APP_STATE = HOME; 
                break;
                
            case STATISTIC: 
                SetWindowTitle("Statistic");
                APP_STATE = HOME; 
                break;
        }
    }

EXIT_APP:
    printf("Exiting application...\n");
    // Giải phóng bộ nhớ ảnh và font trước khi thoát
    if (userAvatar.id != 0) UnloadTexture(userAvatar);
    UnloadFont(fontTitle);
    UnloadFont(fontSub);
    CloseWindow(); 
    return 0;
}

// ---------------------------------------------------------
// HÀM XỬ LÝ LOAD AVATAR ĐỘNG
// ---------------------------------------------------------
void LoadUserAvatar(Account acc, Texture2D *avatar) {
    if (avatar->id != 0) {
        UnloadTexture(*avatar);
        avatar->id = 0;
    }
    
    char avatarPath[512];
    sprintf(avatarPath, "img/avatar/%s.jpg", acc.username);
    
    if (FileExists(avatarPath)) {
        *avatar = LoadTexture(avatarPath);
    } else {
        *avatar = LoadTexture("img/avatar/non.jpg");
    }
}

// ---------------------------------------------------------
// HÀM VẼ GIAO DIỆN DESCRIBE BOX
// ---------------------------------------------------------
void MainDrawDescribeBox(Rectangle box, Account acc, Texture2D avatar, Font fontLarge, Font fontSmall) {
    Color darkBg = (Color){ 30, 35, 45, 255 };
    DrawRectangleRounded(box, 0.15f, 20, darkBg);
    
    BeginScissorMode((int)box.x, (int)box.y, (int)box.width, (int)box.height);
    DrawCircleGradient((int)(box.x + box.height/2), (int)(box.y + box.height/2), box.height * 1.5f, Fade(TEALBLUE, 0.2f), BLANK);
    EndScissorMode();
    
    DrawRectangleRoundedLinesEx(box, 0.15f, 20, 2.0f, Fade(LIGHTGRAY, 0.3f));

    float paddingY = box.height * 0.15f;
    float avatarSize = box.height - 2 * paddingY;
    Rectangle avatarDest = { box.x + 30.0f, box.y + paddingY, avatarSize, avatarSize };
    
    if (avatar.id != 0) {
        Rectangle source = { 0, 0, (float)avatar.width, (float)avatar.height };
        DrawTexturePro(avatar, source, avatarDest, (Vector2){0,0}, 0.0f, WHITE);
        DrawRectangleLinesEx(avatarDest, 3.0f, GOLDACCENT);
    } else {
        DrawRectangleRec(avatarDest, DARKGRAY);
        DrawText("No Img", (int)avatarDest.x + 10, (int)avatarDest.y + (int)avatarSize/2 - 10, 20, LIGHTGRAY);
    }

    float textX = avatarDest.x + avatarSize + 40.0f;
    float totalTextHeight = 45 + 10 + 30; 
    float startY = box.y + (box.height - totalTextHeight) / 2.0f;
    
    DrawTextEx(fontLarge, acc.realName, (Vector2){ textX, startY }, 45, 1, WHITE);
    
    char dobText[512];
    sprintf(dobText, "Ngày sinh: %s", acc.dateOfBirth);
    DrawTextEx(fontSmall, dobText, (Vector2){ textX, startY + 55 }, 25, 1, LIGHTGRAY);

    float roleFontSize = 25.0f;
    float roleW = MeasureTextEx(fontSmall, acc.role, roleFontSize, 1).x + 40.0f;
    float roleH = 40.0f;
    float logoutH = 30.0f;
    float spacing = 10.0f;
    float totalRightBlockH = roleH + spacing + logoutH; 
    
    float blockStartY = box.y + (box.height - totalRightBlockH) / 2.0f;

    Rectangle roleBox = {
        box.x + box.width - roleW - 30.0f,
        blockStartY,
        roleW,
        roleH
    };
    
    Color roleBgColor = (strcmp(acc.role, "Administrator") == 0) ? MAROON : STEELBLUE;
    DrawRectangleRounded(roleBox, 0.5f, 15, roleBgColor);
    
    Vector2 roleTextPos = {
        roleBox.x + (roleBox.width - MeasureTextEx(fontSmall, acc.role, roleFontSize, 1).x) / 2.0f,
        roleBox.y + (roleBox.height - roleFontSize) / 2.0f
    };
    DrawTextEx(fontSmall, acc.role, roleTextPos, roleFontSize, 1, WHITE);

    float logoutW = roleW * 0.8f;
    Rectangle logoutBox = {
        roleBox.x + (roleBox.width - logoutW) / 2.0f, 
        roleBox.y + roleBox.height + spacing,
        logoutW,
        logoutH
    };

    Vector2 mousePos = GetMousePosition();
    bool isLogoutHovered = CheckCollisionPointRec(mousePos, logoutBox);
    Color logoutBgColor = isLogoutHovered ? RED : DARKGRAY;
    
    DrawRectangleRounded(logoutBox, 0.5f, 10, logoutBgColor);
    
    float logoutFontSize = 18.0f;
    float logoutTextW = MeasureTextEx(fontSmall, "Logout", logoutFontSize, 1).x;
    Vector2 logoutTextPos = {
        logoutBox.x + (logoutBox.width - logoutTextW) / 2.0f,
        logoutBox.y + (logoutBox.height - logoutFontSize) / 2.0f
    };
    DrawTextEx(fontSmall, "Logout", logoutTextPos, logoutFontSize, 1, WHITE);

    if (isLogoutHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        APP_STATE = LOGINAPP;
    }
}

void MainLoadContainers(MainContainers *Containers, Size MainSize){
    Containers->PanelBox = (Rectangle) {
        MainSize.Screen.x * 0.15f * pow(0.9, (double) MainSize.Scale.x),
        MainSize.Screen.y * 0.1f * pow(0.9, (double) MainSize.Scale.y),
        MainSize.Screen.x - 2 * (MainSize.Screen.x * 0.15f * pow(0.9, (double) MainSize.Scale.x)),
        MainSize.Screen.y - (MainSize.Screen.y * 0.1f * pow(0.9, (double) MainSize.Scale.y))
    };

    Containers->DecribeBox = (Rectangle) {
        Containers->PanelBox.x,
        Containers->PanelBox.y,
        Containers->PanelBox.width,
        Containers->PanelBox.height * 0.2f
    };

    Containers->FuncBox = (Rectangle) {
        Containers->PanelBox.x,
        Containers->DecribeBox.y + Containers->DecribeBox.height + Containers->PanelBox.height * 0.02f,
        Containers->PanelBox.width,
        Containers->PanelBox.height * 0.05f
    };

    Containers->ShowFuncBox = (Rectangle) {
        Containers->PanelBox.x,
        Containers->FuncBox.y + Containers->FuncBox.height + Containers->PanelBox.height * 0.02f,
        Containers->PanelBox.width,
        (Containers->PanelBox.height + Containers->PanelBox.y - (Containers->FuncBox.y + Containers->FuncBox.height + Containers->PanelBox.height * 0.02f)) + Containers->PanelBox.width * 0.01f
    };
}

void MainDrawPanel(MainContainers Containers){
    float rounded = Containers.PanelBox.width * 0.01f;
    
    DrawRectangleRounded(Containers.PanelBox, FindRoundness(rounded, Containers.PanelBox.width, Containers.PanelBox.height), 10, Fade(SOFTWHITE, 0.1f));
    
    // ----------------------------------------------------
    // HIỆU ỨNG CẦU VỒNG BO TRÒN - LƯU CHUYỂN QUA LẠI
    // ----------------------------------------------------
    float funcRoundness = FindRoundness(rounded, Containers.FuncBox.width, Containers.FuncBox.height);
    float w = Containers.FuncBox.width;
    float h = Containers.FuncBox.height;
    
    // Tính bán kính bo góc (R) quy đổi sang pixel
    float minDim = (w < h) ? w : h;
    float R = funcRoundness * (minDim / 2.0f); 
    
    // Dùng hàm sin() kết hợp GetTime() để tạo độ dời (offset), làm màu sắc trôi qua trôi lại (ping-pong)
    float timeOffset = sin(GetTime() * 1.5f) * 180.0f; 
    
    for (int i = 0; i < (int)w; i++) {
        // Trải dài hệ màu HSV (360 độ) theo chiều ngang và cộng thêm độ dời thời gian
        float hue = fmod(timeOffset + ((float)i / w) * 360.0f + 360.0f, 360.0f);
        Color c = ColorFromHSV(hue, 0.85f, 0.9f); // Giảm nhẹ độ bão hòa (0.85) để màu nhìn dịu mắt hơn
        
        float dx = 0;
        // Kiểm tra xem nét vẽ hiện tại có nằm trong khu vực 2 góc bo tròn không
        if (i < R) dx = R - i; 
        else if (i > w - R) dx = i - (w - R);
        
        float dy = 0;
        if (dx > 0 && R > 0) {
            // Áp dụng phương trình đường tròn: dx^2 + (R - dy)^2 = R^2 
            float val = R * R - dx * dx;
            if (val > 0) dy = R - sqrt(val);
            else dy = R;
        }
        
        // Vẽ từng thanh dọc nhỏ ghép lại thành dải gradient, tự động thụt vào (dy) để tạo hình bo tròn
        DrawRectangle((int)(Containers.FuncBox.x + i), (int)(Containers.FuncBox.y + dy), 1, (int)(h - 2 * dy), c);
    }
    
    // Vẽ thêm một lớp viền mỏng ngoài cùng để che khuyết điểm răng cưa (nếu có) và làm khối nổi lên
    DrawRectangleRoundedLinesEx(Containers.FuncBox, funcRoundness, 10, 2.0f, Fade(LIGHTGRAY, 0.5f));
    // ----------------------------------------------------

    DrawRectangleRounded(Containers.ShowFuncBox, FindRoundness(rounded, Containers.ShowFuncBox.width, Containers.ShowFuncBox.height), 10, BRIGHTWHITE);
}

bool MainFunc(Rectangle ShowFuncBox, Vector2 Mouse){
    int i = 0;
    float ratioDistance = 0.1f;
    float widthScissor = ShowFuncBox.width - ShowFuncBox.width/4 * ratioDistance;
    float widthCard = widthScissor / 4;
    
    Rectangle ManageBooksBox = { 
        ShowFuncBox.x + widthCard * ratioDistance + (float) i++ * widthCard, 
        ShowFuncBox.y + ShowFuncBox.height * 0.1f, 
        widthCard * (1.0f - ratioDistance), 
        ShowFuncBox.height * 0.7f 
    };
    
    Rectangle ManageUserBox = { 
        ShowFuncBox.x + widthCard * ratioDistance + (float) i++ * widthCard, 
        ShowFuncBox.y + ShowFuncBox.height * 0.1f, 
        widthCard * (1.0f - ratioDistance), 
        ShowFuncBox.height * 0.7f 
    };
    
    Rectangle ManageBorrowBox = { 
        ShowFuncBox.x + widthCard * ratioDistance + (float) i++ * widthCard, 
        ShowFuncBox.y + ShowFuncBox.height * 0.1f, 
        widthCard * (1.0f - ratioDistance), 
        ShowFuncBox.height * 0.7f 
    };
    
    Rectangle StatisticBox = { 
        ShowFuncBox.x + widthCard * ratioDistance + (float) i++ * widthCard, 
        ShowFuncBox.y + ShowFuncBox.height * 0.1f, 
        widthCard * (1.0f - ratioDistance), 
        ShowFuncBox.height * 0.7f 
    };

    float roundness = widthCard * 0.1f;
    float WidthText;

    DrawRectangleRounded(ManageBooksBox, FindRoundness(roundness, ManageBooksBox.width, ManageBooksBox.height), 10, BLACK);
    WidthText = MeasureText("Manage Books", 20);
    Vector2 ManageBooksText = { 
        ManageBooksBox.x + (ManageBooksBox.width - WidthText) * 0.5f, 
        ManageBooksBox.y + ManageBooksBox.height + 50 
    };
    DrawText("Manage Books", (int) ManageBooksText.x, (int) ManageBooksText.y, 20, BLACK);
    
    DrawRectangleRounded(ManageUserBox, FindRoundness(roundness, ManageUserBox.width, ManageUserBox.height), 10, BLACK);
    WidthText = MeasureText("Manage User", 20);
    Vector2 ManageUserText = { 
        ManageUserBox.x + (ManageUserBox.width - WidthText) * 0.5f, 
        ManageUserBox.y + ManageUserBox.height + 50 
    };
    DrawText("Manage User", (int) ManageUserText.x, (int) ManageUserText.y, 20, BLACK);

    DrawRectangleRounded(ManageBorrowBox, FindRoundness(roundness, ManageBorrowBox.width, ManageBorrowBox.height), 10, BLACK);
    WidthText = MeasureText("Manage Borrow", 20);
    Vector2 ManageBorrowText = { 
        ManageBorrowBox.x + (ManageBorrowBox.width - WidthText) * 0.5f, 
        ManageBorrowBox.y + ManageBorrowBox.height + 50 
    };
    DrawText("Manage Borrow", (int) ManageBorrowText.x, (int) ManageBorrowText.y, 20, BLACK);

    DrawRectangleRounded(StatisticBox, FindRoundness(roundness, StatisticBox.width, StatisticBox.height), 10, BLACK);
    WidthText = MeasureText("Statistic", 20);
    Vector2 StatisticText = { 
        StatisticBox.x + (StatisticBox.width - WidthText) * 0.5f, 
        StatisticBox.y + StatisticBox.height + 50 
    };
    DrawText("Statistic", (int) StatisticText.x, (int) StatisticText.y, 20, BLACK);

    if (CheckCollisionPointRec(Mouse, ManageBooksBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        APP_STATE = MANAGEBOOKS; return 1;
    }
    if (CheckCollisionPointRec(Mouse, ManageUserBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        APP_STATE = MANAGEUSER; return 1;
    }
    if (CheckCollisionPointRec(Mouse, ManageBorrowBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        APP_STATE = MANAGEBORROWING; return 1;
    }
    if (CheckCollisionPointRec(Mouse, StatisticBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        APP_STATE = STATISTIC; return 1;
    }

    return 0;
}