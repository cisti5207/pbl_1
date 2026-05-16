#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

// Đường dẫn tương đối từ folder 'source' lùi ra ngoài để vào folder 'include'
#include "../include/ManageUser.h" 

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int main(void) {
    // Khởi tạo cửa sổ đầu tiên để Raylib nhận diện context đồ họa
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PROXIMA - QUAN LY BAN DOC");
    SetTargetFPS(60);

    // Khai báo tài nguyên (Cần khai báo sau InitWindow để đảm bảo an toàn)
    Font mainFont = GetFontDefault();
    
    // Tạo nền Gradient
    // Lưu ý: Nếu dòng này vẫn báo lỗi 'int', hãy kiểm tra file ManageUser.h có include raylib.h chưa
    Image backgroundImage = GenImageGradientV(SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 35, 35, 50, 255 }, (Color){ 20, 20, 30, 255 });
    Texture2D bgTexture = LoadTextureFromImage(backgroundImage);
    UnloadImage(backgroundImage); 

    // Quản lý dữ liệu
    struct BanDoc *memberList = NULL;    
    Nhap inputForm;               
    int totalUsers = 0;           
    char maTheMoi[15] = "";       

    // Khởi tạo trạng thái ban đầu cho form (Hàm định nghĩa trong ManageUser.c)
    InitForm(&inputForm);

    while (!WindowShouldClose()) {
        // 1. Cập nhật Logic
        UpdateFormPosition(&inputForm);
        UpdateInputForm(&inputForm, &memberList, &totalUsers, maTheMoi);

        // 2. Vẽ giao diện
        BeginDrawing();
        ClearBackground(BLACK);

        // Vẽ hình nền và lớp phủ tối
        DrawTexture(bgTexture, 0, 0, WHITE);
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.4f));

        // Tiêu đề
        DrawText("HE THONG QUAN LY THU VIEN PROXIMA", 40, 40, 30, GOLD);
        
        // Vẽ thẻ thư viện và form nhập liệu
        DrawLibraryCard(&inputForm, NULL, maTheMoi, mainFont);

        // Danh sách hiển thị bên phải
        DrawText("DANH SACH DANG KY MOI:", 750, 180, 20, LIGHTGRAY);
        struct BanDoc *current = memberList;
        int offset = 0;
        while (current != NULL && offset < 10) {
            DrawText(TextFormat("%s - %s", current->maThe, current->hoTen), 750, 220 + (offset * 35), 18, WHITE);
            current = current->next;
            offset++;
        }

        // Hiển thị thông báo khi lưu thành công
        if (inputForm.showSuccess) {
            DrawSuccessMessage(mainFont, bgTexture, inputForm.successTimer);
            if (GetTime() - inputForm.successTimer > 2.0f) {
                inputForm.showSuccess = false;
            }
        }

        EndDrawing();
    }

    // Giải phóng bộ nhớ và đóng cửa sổ
    FreeMemberList(memberList);
    UnloadTexture(bgTexture);
    CloseWindow();

    return 0;
}