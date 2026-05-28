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
}