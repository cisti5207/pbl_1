<<<<<<< HEAD
#include "raylib.h"
#include "libmanage.h"
#include "menu.h" 
#include "ManageUser.h"
#include "Phieumuon.h"
#include "TimkiemLSphieumuon.h" 
#include "Timkiemthethuvien.h"
#include "Trasach.h"
#include "InDanhSachPM.h"
#include <stdlib.h>

int main(void) {
    // ========================================================
    // 1. KHỞI TẠO CỬA SỔ & HỆ THỐNG RAYLIB
    // ========================================================
    const int screenWidth = 1100;
    const int screenHeight = 750;
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "HỆ THỐNG QUẢN LÝ THƯ VIỆN - PBL 1");
    SetExitKey(0); // Chặn phím ESC tắt app đột ngột
    SetTargetFPS(60);

    // Đã cập nhật đúng đường dẫn font của bro
    Font mainFont = SetFontUTF8("font/arial/ARIALBD.TTF", 108); 
    SetTextureFilter(mainFont.texture, TEXTURE_FILTER_BILINEAR);
    
    // ========================================================
    // 2. KHỞI TẠO DỮ LIỆU & BIẾN TOÀN CỤC
    // ========================================================
    AppState currentState = APP_MENU;

    BanDoc *headBD = NULL; 
    int totalUsers = 0;
    PhieuMuonNode *headPM = NULL;
    
    PhieuMuonMang mapPM;
    InitPhieuMuonMang(&mapPM);

    // Đã cập nhật đúng đường dẫn file của bro
    DocDuLieuTheBanDoc("data/phieumuon/User.txt", &headBD, &totalUsers);
    DocDuLieuPhieuMuon("data/phieumuon/LSPhieumuon.txt", &headPM);
    BuildMap(&mapPM, headPM); 

    // Khởi tạo các Form
    Nhap formTaoThe; InitForm(&formTaoThe);
    FormPhieuMuon formTaoPhieu; InitPhieumuon(&formTaoPhieu);
    FormTimKiemThe formTimKiem; InitListBD(&formTimKiem);
    FormTraSach formTraSach; InitFormTraSach(&formTraSach);
    FormInDanhSachPM formInPM; InitFormInDanhSachPM(&formInPM);
    
    char maTheTempt[15] = {0}; 
    Texture2D icons[5] = { 0 }; 

    // ========================================================
    // 3. VÒNG LẶP GAME CHÍNH (MAIN LOOP)
    // ========================================================
    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(GetColor(0xFFF0F5FF)); 

        switch (currentState) {
            case APP_MENU:
                currentState = DrawAndHandleMenu(mainFont, (float)GetScreenWidth(), (float)GetScreenHeight());
                break;

            case APP_TAO_THE:
                UpdateFormPosition(&formTaoThe);
                UpdateInputForm(&formTaoThe, &headBD, &totalUsers, maTheTempt, (int*)&currentState);
                DrawLibraryCard(&formTaoThe, icons, maTheTempt, mainFont);
                break;

            case APP_TAO_PHIEU:
                UpdateVitri(&formTaoPhieu);
                UpdateInputPM(&formTaoPhieu, &headPM, (int*)&currentState, maTheTempt);
                DrawPM(&formTaoPhieu, icons, maTheTempt, mainFont);
                if (formTaoPhieu.showsuccess) {
                    BuildMap(&mapPM, headPM);
                }
                break;

            case APP_TRA_SACH:
                UpdateLogicTraSach(&formTraSach, headBD, headPM, (int*)&currentState);
                DrawGiaoDienTraSach(&formTraSach, headBD, headPM, mainFont);
                if (formTraSach.state == TRA_B1_TIMKEM && formTraSach.tongTien > 0) {
                    BuildMap(&mapPM, headPM);
                    formTraSach.tongTien = 0; 
                }
                break;

            case APP_XEM_DANH_SACH:
                UpdateLogicInDanhSachPM(&formInPM, headPM, (int*)&currentState);
                DrawGiaoDienInDanhSachPM(&formInPM, headPM, mainFont);
                break;

            case APP_LICH_SU: 
                Updatetoado(&formTimKiem);
                kiemtralienquantiengviet(&formTimKiem, (int*)&currentState);
                DrawTimKiemThe(&formTimKiem, icons, mainFont, headBD);
                if (formTimKiem.showmodal) DrawModalLichSuPhieuMuon(&formTimKiem, &mapPM, mainFont);
                break;
        }

        EndDrawing();
    }

    // ========================================================
    // 4. DỌN DẸP BỘ NHỚ & ĐÓNG CHƯƠNG TRÌNH
    // ========================================================
    UnloadFont(mainFont);
    
    FreeMemberList(headBD);
    
    PhieuMuonNode *currPM = headPM;
    while (currPM != NULL) {
        PhieuMuonNode *temp = currPM;
        currPM = currPM->next;
        free(temp);
    }
    
    CloseWindow();
    return 0;
=======
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
        (Vector2) { GetMonitorWidth(0), GetMonitorHeight(0) },
        (Vector2) { GetScreenWidth(), GetScreenHeight() },
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
            (Vector2) { GetScreenWidth(), GetScreenHeight() },
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
    // HIỆU ỨNG CẦU VỒNG LƯU CHUYỂN QUA LẠI
    // ----------------------------------------------------
    float funcRoundness = FindRoundness(rounded, Containers.FuncBox.width, Containers.FuncBox.height);
    float w = Containers.FuncBox.width;
    float h = Containers.FuncBox.height;
    
    float minDim = (w < h) ? w : h;
    float R = funcRoundness * (minDim / 2.0f); 
    
    float timeOffset = sin(GetTime() * 1.5f) * 180.0f; 
    
    for (int i = 0; i < (int)w; i++) {
        float hue = fmod(timeOffset + ((float)i / w) * 360.0f + 360.0f, 360.0f);
        Color c = ColorFromHSV(hue, 0.85f, 0.9f); 
        
        float dx = 0;
        if (i < R) dx = R - i; 
        else if (i > w - R) dx = i - (w - R);
        
        float dy = 0;
        if (dx > 0 && R > 0) {
            float val = R * R - dx * dx;
            if (val > 0) dy = R - sqrt(val);
            else dy = R;
        }
        DrawRectangle((int)(Containers.FuncBox.x + i), (int)(Containers.FuncBox.y + dy), 1, (int)(h - 2 * dy), c);
    }
    
    DrawRectangleRoundedLinesEx(Containers.FuncBox, funcRoundness, 10, 2.0f, Fade(LIGHTGRAY, 0.5f));
    DrawRectangleRounded(Containers.ShowFuncBox, FindRoundness(rounded, Containers.ShowFuncBox.width, Containers.ShowFuncBox.height), 10, BRIGHTWHITE);
}

// -------------------------------------------------------------------------
// HÀM VẼ THẺ ẢNH (BẢN CẬP NHẬT - CÓ BO GÓC)
// -------------------------------------------------------------------------
static void DrawMainCard(Texture2D tex, Rectangle box, const char* title, Vector2 mouse) {
    bool isHovered = CheckCollisionPointRec(mouse, box);
    
    // Cài đặt thông số bo góc
    float roundness = 0.15f; // Tỉ lệ bo góc (0.0 đến 1.0)
    int segments = 16;       // Độ mịn của đường cong

    // 1. Nếu không có ảnh thì vẽ khối nền đen bo góc
    if (tex.id == 0) {
        DrawRectangleRounded(box, roundness, segments, BLACK);
    } else {
        // 2. THUẬT TOÁN CENTER CROP CHUẨN (Giữ nguyên tỷ lệ, cắt rìa, không làm bóp méo ảnh)
        float boxRatio = box.width / box.height;
        float texRatio = (float)tex.width / (float)tex.height;
        
        Rectangle srcRec;
        if (texRatio > boxRatio) {
            float newWidth = tex.height * boxRatio;
            srcRec = (Rectangle){ (tex.width - newWidth) / 2.0f, 0.0f, newWidth, (float)tex.height };
        } else {
            float newHeight = tex.width / boxRatio;
            srcRec = (Rectangle){ 0.0f, (tex.height - newHeight) / 2.0f, (float)tex.width, newHeight };
        }
        
        // Vẽ ảnh vuông vức
        DrawTexturePro(tex, srcRec, box, (Vector2){0,0}, 0.0f, WHITE);

        // CHE GÓC NHỌN CỦA ẢNH (nếu ảnh không phải png trong suốt)
        // Vẽ viền ngoài cực dày trùng màu nền (BRIGHTWHITE) đè lên 4 góc của thẻ ảnh.
        // Nếu ảnh của bạn là PNG trong suốt chuẩn, bạn có thể comment lại dòng này.
        DrawRectangleRoundedLinesEx(box, roundness, segments, 15.0f, BRIGHTWHITE);
    }
    
    // 3. Phủ một viền ngoài mỏng tạo điểm nhấn
    DrawRectangleRoundedLinesEx(box, roundness, segments, 2.0f, Fade(BLACK, 0.2f));
    
    // 4. Hiệu ứng Hover làm tối toàn bộ khối hình chữ nhật
    if (isHovered) {
        DrawRectangleRounded(box, roundness, segments, Fade(BLACK, 0.3f));
    }

    // 5. Vẽ Text căn giữa ở dưới Thẻ
    int fontSize = 20;
    float WidthText = MeasureText(title, fontSize);
    Vector2 TextPos = { box.x + (box.width - WidthText) * 0.5f, box.y + box.height + 40 };
    DrawText(title, (int) TextPos.x, (int) TextPos.y, fontSize, isHovered ? TEALBLUE : BLACK);
}

bool MainFunc(Rectangle ShowFuncBox, Vector2 Mouse){
    // Nạp ảnh tĩnh (Tải 1 lần duy nhất chống giật FPS)
    static Texture2D texBook = {0}, texUser = {0}, texBorrow = {0}, texStatistic = {0};
    static bool isLoaded = false;
    
    if (!isLoaded) {
        texBook = LoadTexture("img/main_symbol/book.png");
        texUser = LoadTexture("img/main_symbol/user.png");
        texBorrow = LoadTexture("img/main_symbol/borrow.png");
        texStatistic = LoadTexture("img/main_symbol/statistic.png");
        isLoaded = true;
    }

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

    // Gọi hàm vẽ Thẻ Ảnh đã bo góc
    DrawMainCard(texBook, ManageBooksBox, "Manage Books", Mouse);
    DrawMainCard(texUser, ManageUserBox, "Manage User", Mouse);
    DrawMainCard(texBorrow, ManageBorrowBox, "Manage Borrow", Mouse);
    DrawMainCard(texStatistic, StatisticBox, "Statistic", Mouse);

    // Xử lý sự kiện click
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
>>>>>>> 674ec34198b8d64dfa4271f76cec6b3461d3aa40
}