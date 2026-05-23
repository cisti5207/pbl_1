#include "libmanage.h" // Include header chứa các hàm dùng chung
#include "raylib.h"
#include "Phieumuon.h"
#include <stdio.h>
#include <stdlib.h>

// ============================================================================
// KHU VỰC CẤU HÌNH HỆ THỐNG - ÔNG THAY ĐỔI ĐƯỜNG DẪN FONT TẠI ĐÂY NHA BRO!
// ============================================================================
// Sử dụng đường dẫn tương đối để mang sang máy khác vẫn chạy bình thường nhé bro!
#define FONT_PATH "font/arial/ARIALBD.TTF"
#define MAIN_FONT_SIZE 128

#define SCREEN_WIDTH 1100
#define SCREEN_HEIGHT 750

// KHAI BÁO NGUYÊN MẪU (PROTOTYPE)
// Chỉ khai báo dòng này để main.c biết hàm tồn tại, KHÔNG định nghĩa lại thân hàm {} ở đây
// vì thân hàm đã nằm trọn vẹn bên trong file libmanage.c của ông rồi!
Font SetFontUTF8(const char *_font, int _fontSize);

int main(void) {
    // Cho phép người dùng thu phóng / kéo giãn cửa sổ làm việc tự do
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hệ Thống Quản Lý Thư Viện - PBL1");
    SetTargetFPS(60);

    // Áp dụng đường dẫn và kích thước Font đã định nghĩa ở phần cấu hình đầu file
    Font mainFont = SetFontUTF8(FONT_PATH, MAIN_FONT_SIZE); 
    
    Texture2D dummyIcons[5] = { 0 }; 
    Texture2D dummyBg = { 0 };

    FormPhieuMuon formPM;
    InitPhieumuon(&formPM);

    PhieuMuonNode *headList = NULL; 
    int totalSlips = 0;             
    char maPMHienTai[15] = "";      

    SinhMaPM(totalSlips, maPMHienTai);

    while (!WindowShouldClose()) {
        UpdateVitri(&formPM);
        UpdateInputPM(&formPM, &headList, &totalSlips, maPMHienTai);

        float screenW = (float)GetScreenWidth();
        float screenH = (float)GetScreenHeight();
        float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
        if (scale < 0.5f) scale = 0.5f;

        float cardrightY = (screenH - 550 * scale) / 2;
        Rectangle confirmButton = { 
            (screenW - 800 * scale) / 2 + 630 * scale, 
            cardrightY + 550 * scale + 30 * scale, 
            140 * scale, 
            50 * scale 
        };

        if (!formPM.showsuccess) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointRec(mousePos, confirmButton)) {
                    bool saveSuccess = LuuPhieuMuonVaoFile(maPMHienTai, &formPM);
                    if (saveSuccess) {
                        ThemPhieuMuonVaoList(&headList, maPMHienTai, &formPM);
                        formPM.showsuccess = true;
                        formPM.successtimer = 3.0f;
                    } else {
                        printf("Lỗi: Không thể ghi dữ liệu phiếu mượn vào file txt!\n");
                    }
                }
            }
        }

        BeginDrawing();
            ClearBackground(GetColor(0xF4F4F9FF)); 

            // Vẽ Thẻ Phiếu mượn sử dụng Font chữ tiếng Việt đã được tối ưu hóa bằng mảng codepoints
            DrawPM(&formPM, dummyIcons, maPMHienTai, mainFont);

            if (formPM.showsuccess) {
                DrawSuccess(mainFont, dummyBg, formPM.successtimer);
            }
        EndDrawing();
    }

    // Giải phóng bộ nhớ RAM cho Danh sách liên kết đơn trước khi đóng ứng dụng
    PhieuMuonNode *current = headList;
    while (current != NULL) {
        PhieuMuonNode *temp = current;
        current = current->next;
        free(temp);
    }

    // Giải phóng Font chữ
    UnloadFont(mainFont);

    CloseWindow();
    return 0;
}