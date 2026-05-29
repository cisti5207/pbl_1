#include <stdio.h>
#include "ManageBooks.h"
#include "raylib.h"
#include "login.h"
#include "main.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

// --- NHÚNG CÁC MODULE CỦA BRO VÀO ĐÂY ---
#include "ManageUser.h"
#include "Phieumuon.h"
#include "TimkiemLSphieumuon.h" 
#include "Timkiemthethuvien.h"
#include "Trasach.h"
#include "InDanhSachPM.h"
#include "Doanhthu.h"
#include "menu.h" 

// Gán trạng thái khởi tạo
AppState APP_STATE = LOGINAPP;

int main(){
    InitWindow(1200, 800, "Library System - PBL1");
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

    InitParticles(MainSize);

    Font fontTitle = SetFontUTF8(ArialBold, 80); 
    Font fontSub = SetFontUTF8(Roboto_Semibold, 40);

    // --- KHỞI TẠO DỮ LIỆU CỦA BRO Ở ĐÂY ---
    Font mainFont = SetFontUTF8("font/arial/ARIALBD.TTF", 108); 
    SetTextureFilter(mainFont.texture, TEXTURE_FILTER_BILINEAR);

    BanDoc *headBD = NULL; 
    int totalUsers = 0;
    PhieuMuonNode *headPM = NULL;
    
    PhieuMuonMang mapPM;
    InitPhieuMuonMang(&mapPM);

    DocDuLieuTheBanDoc("data/phieumuon/User.txt", &headBD, &totalUsers);
    DocDuLieuPhieuMuon("data/phieumuon/LSphieumuon.txt", &headPM);
    BuildMap(&mapPM, headPM); 

    DoanhThuMap mapDT;
    InitDoanhThuMap(&mapDT);
    FormDoanhThu formDT;
    InitFormDoanhThu(&formDT);

    // Nạp dữ liệu doanh thu từ file vào map
    {
        FILE *fDT = fopen("data/Doanhthu.txt", "r");
        if (fDT != NULL) {
            char lineDT[256];
            while (fgets(lineDT, sizeof(lineDT), fDT)) {
                char ngay[30];
                double tien;
                if (sscanf(lineDT, "%29[^|] | %lf", ngay, &tien) == 2) {
                    // Trim khoảng trắng cuối ngày
                    int len = strlen(ngay);
                    while (len > 0 && ngay[len-1] == ' ') ngay[--len] = '\0';
                    ThemDoanhThu(&mapDT, ngay, tien);
                }
            }
            fclose(fDT);
        }
    }

    Nhap formTaoThe; InitForm(&formTaoThe);
    FormPhieuMuon formTaoPhieu; InitPhieumuon(&formTaoPhieu);
    FormTimKiemThe formTimKiem; InitListBD(&formTimKiem);
    FormTraSach formTraSach; InitFormTraSach(&formTraSach);
    FormInDanhSachPM formInPM; InitFormInDanhSachPM(&formInPM);
    
    char maTheTempt[15] = {0}; 
    Texture2D icons[5] = { 0 }; 
    // --------------------------------------

    Account _Account;
    Texture2D userAvatar = {0};

    if (InitLogin(&_Account) == LOGIN_SUCCESS) {
        APP_STATE = HOME;
        LoadUserAvatar(_Account, &userAvatar);
    } else {
        printf("Login failed or window closed.\n");
        CloseWindow();
        return 1;
    }

    Role _role;
    if (strcmp(_Account.role, "Administrator") == 0) _role = ADMINISTRATOR;
    else _role = STAFF;

    MainContainers Containers;
    MainLoadContainers(&Containers, MainSize);
    
    while (!WindowShouldClose()){
        
        // ==========================================
        // MÀN HÌNH CHÍNH (CỦA BẠN BRO)
        // ==========================================
        if (APP_STATE == HOME) {
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
            ClearBackground(AnimatedBackground());
            DrawBackgroundParticles();
            MainDrawPanel(Containers);
            MainDrawDescribeBox(Containers.DecribeBox, _Account, userAvatar, fontTitle, fontSub);
            
            // Xử lý vẽ menu 3 NÚT TỔNG và hứng sự kiện chuyển trang
            MainFunc(Containers.ShowFuncBox, MainSize.Mouse);
            
            EndDrawing();
        } 
        // ==========================================
        // LOGIN & QUẢN LÝ SÁCH (CỦA BẠN BRO)
        // ==========================================
        else if (APP_STATE == LOGINAPP) {
            SetWindowTitle("Login");
            _Account = (Account) {0};
            if (InitLogin(&_Account) == LOGIN_SUCCESS) {
                APP_STATE = HOME;
                if (strcmp(_Account.role, "Administrator") == 0) _role = ADMINISTRATOR;
                else _role = STAFF;
                LoadUserAvatar(_Account, &userAvatar);
            } else {
                goto EXIT_APP; 
            }
        } 
        else if (APP_STATE == MANAGEBOOKS) {
            SetWindowTitle("Manage Books");
            InitManageBooks(_role);
            APP_STATE = HOME;
        } 
        // ==========================================
        // PHÂN HỆ NGHIỆP VỤ (CỦA BRO)
        // ==========================================
        else {
            BeginDrawing();
            ClearBackground(GetColor(0xFFF0F5FF)); 

            int tempState = APP_STATE; 

            switch (APP_STATE) {
                case APP_MENU: // VẼ MENU HỒNG CỦA BRO RA ĐÂY!
                    tempState = DrawAndHandleMenu(mainFont, GetScreenWidth(), GetScreenHeight());
                    break;
                case APP_TAO_THE:
                    UpdateFormPosition(&formTaoThe);
                    UpdateInputForm(&formTaoThe, &headBD, &totalUsers, maTheTempt, &tempState);
                    DrawLibraryCard(&formTaoThe, icons, maTheTempt, mainFont);
                    break;
                case APP_TAO_PHIEU:
                    UpdateVitri(&formTaoPhieu);
                    UpdateInputPM(&formTaoPhieu, &headPM, &tempState, maTheTempt);
                    DrawPM(&formTaoPhieu, icons, maTheTempt, mainFont);
                    if (formTaoPhieu.showsuccess) BuildMap(&mapPM, headPM);
                    break;
                case APP_TRA_SACH:
                    UpdateLogicTraSach(&formTraSach, headBD, headPM, &tempState);
                    DrawGiaoDienTraSach(&formTraSach, headBD, headPM, mainFont);
                    if (formTraSach.daThanhToan) {
                        BuildMap(&mapPM, headPM);
                        // Reload doanh thu vào map ngay sau khi thanh toán
                        GiaiPhongDoanhThuMap(&mapDT);
                        InitDoanhThuMap(&mapDT);
                        FILE *fDT2 = fopen("data/Doanhthu.txt", "r");
                        if (fDT2 != NULL) {
                            char lineDT2[256];
                            while (fgets(lineDT2, sizeof(lineDT2), fDT2)) {
                                char ngay[30]; double tien;
                                if (sscanf(lineDT2, "%29[^|] | %lf", ngay, &tien) == 2) {
                                    int len = strlen(ngay);
                                    while (len > 0 && ngay[len-1] == ' ') ngay[--len] = '\0';
                                    ThemDoanhThu(&mapDT, ngay, tien);
                                }
                            }
                            fclose(fDT2);
                        }
                        InitFormDoanhThu(&formDT);
                        formTraSach.daThanhToan = 0; // Reset cờ
                    }
                    break;
                case APP_XEM_DANH_SACH:
                    UpdateLogicInDanhSachPM(&formInPM, headPM, &tempState);
                    DrawGiaoDienInDanhSachPM(&formInPM, headPM, mainFont);
                    break;
                case APP_LICH_SU: 
                    Updatetoado(&formTimKiem);
                    kiemtralienquantiengviet(&formTimKiem, &tempState);
                    DrawTimKiemThe(&formTimKiem, icons, mainFont, headBD);
                    if (formTimKiem.showmodal) DrawModalLichSuPhieuMuon(&formTimKiem, &mapPM, mainFont);
                    break;
                case APP_DOANH_THU:
                    DrawDashboardDoanhThu(&mapDT, &formDT, mainFont, &tempState);
                    break;
            }

            // Gán lại trạng thái
            APP_STATE = tempState;

            EndDrawing();
        }
    }

EXIT_APP:
    if (userAvatar.id != 0) UnloadTexture(userAvatar);
    UnloadFont(fontTitle);
    UnloadFont(fontSub);
    UnloadFont(mainFont);
    
    FreeMemberList(headBD);
    GiaiPhongDoanhThuMap(&mapDT);
    PhieuMuonNode *currPM = headPM;
    while (currPM != NULL) {
        PhieuMuonNode *temp = currPM;
        currPM = currPM->next;
        free(temp);
    }

    CloseWindow(); 
    return 0;
}

void LoadUserAvatar(Account acc, Texture2D *avatar) {
    if (avatar->id != 0) {
        UnloadTexture(*avatar);
        avatar->id = 0;
    }
    char avatarPath[512];
    sprintf(avatarPath, "img/avatar/%s.jpg", acc.username);
    if (FileExists(avatarPath)) *avatar = LoadTexture(avatarPath);
    else *avatar = LoadTexture("img/avatar/non.jpg");
}

void MainDrawDescribeBox(Rectangle box, Account acc, Texture2D avatar, Font fontLarge, Font fontSmall) {
    Color darkBg = (Color){ 30, 35, 45, 255 };
    DrawRectangleRounded(box, 0.15f, 20, darkBg);
    
    BeginScissorMode((int)box.x, (int)box.y, (int)box.width, (int)box.height);
    // Đã sửa dòng dưới đây: Tách Vector2 thành 2 tham số int x, y theo đúng chuẩn của Raylib
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

static void DrawMainCard(Texture2D tex, Rectangle box, const char* title, Vector2 mouse) {
    bool isHovered = CheckCollisionPointRec(mouse, box);
    float roundness = 0.15f; 
    int segments = 16;       

    if (tex.id == 0) {
        DrawRectangleRounded(box, roundness, segments, BLACK);
    } else {
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
        DrawTexturePro(tex, srcRec, box, (Vector2){0,0}, 0.0f, WHITE);
        DrawRectangleRoundedLinesEx(box, roundness, segments, 15.0f, BRIGHTWHITE);
    }
    
    DrawRectangleRoundedLinesEx(box, roundness, segments, 2.0f, Fade(BLACK, 0.2f));
    if (isHovered) DrawRectangleRounded(box, roundness, segments, Fade(BLACK, 0.3f));
    
    int fontSize = 20;
    float WidthText = MeasureText(title, fontSize);
    Vector2 TextPos = { box.x + (box.width - WidthText) * 0.5f, box.y + box.height + 25 };
    DrawText(title, (int) TextPos.x, (int) TextPos.y, fontSize, isHovered ? TEALBLUE : BLACK);
}

// =========================================================================
// HÀM VẼ GIAO DIỆN CHỌN MENU: HIỆN CHUẨN 3 NÚT (ManageBooks, ManageUsers, Statistic)
// =========================================================================
bool MainFunc(Rectangle ShowFuncBox, Vector2 Mouse){
    static Texture2D texBook = {0}, texUser = {0}, texStatistic = {0};
    static bool isLoaded = false;
    
    if (!isLoaded) {
        texBook = LoadTexture("img/main_symbol/book.png");
        texUser = LoadTexture("img/main_symbol/user.png"); // Bro có thể kiếm icon đẹp hơn để đổi sau
        texStatistic = LoadTexture("img/main_symbol/statistic.png");
        isLoaded = true;
    }

    // HIỂN THỊ ĐÚNG 3 NÚT CHÍNH TRÊN TRANG HOME
    int numCards = 3; 
    
    const char* titles[] = {"Manage Books", "Manage Users", "Statistic"};
    Texture2D texs[] = {texBook, texUser, texStatistic};
    
    // GẮN CHỨC NĂNG TƯƠNG ỨNG CHO 3 NÚT
    // APP_MENU chính là gọi ra cái file menu.c màu hồng của bro đấy!
    int states[] = {MANAGEBOOKS, APP_MENU, APP_DOANH_THU}; 
    
    // Thuật toán tự động chia khoảng cách cho 3 nút
    float totalSpacing = ShowFuncBox.width * 0.1f; 
    float spacingPerCard = totalSpacing / (numCards + 1);
    float widthCard = (ShowFuncBox.width - totalSpacing) / numCards;

    bool clicked = false;
    for(int i = 0; i < numCards; i++) {
        Rectangle cardBox = { 
            ShowFuncBox.x + spacingPerCard * (i + 1) + widthCard * i, 
            ShowFuncBox.y + ShowFuncBox.height * 0.1f, 
            widthCard, 
            ShowFuncBox.height * 0.7f 
        };
        
        DrawMainCard(texs[i], cardBox, titles[i], Mouse);
        
        // Bắt click
        if (CheckCollisionPointRec(Mouse, cardBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            APP_STATE = states[i];
            clicked = true;
        }
    }
    return clicked;
}