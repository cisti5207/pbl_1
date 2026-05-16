#include "raylib.h"
#include "manage_user.h"
#include <stdio.h>

// Định nghĩa kích thước màn hình
const int screenWidth = 1280;
const int screenHeight = 720;

int main(void) {
    InitWindow(screenWidth, screenHeight, "PROXIMA - HE THONG QUAN LY BAN DOC");
    SetTargetFPS(60);

    // Load font mặc định của hệ thống hoặc font custom nếu có
    Font mainFont = GetFontDefault();
    
    // Giả lập mảng icon (Trong thực tế bro sẽ load từ file .png)
    // Texture2D icons[5]; 
    // icons[0] = LoadTexture("assets/user_icon.png");
    
    // Tạo texture nền đơn giản nếu không có file ảnh
    Image backgroundImage = GenImageGradientV(screenWidth, screenHeight, (Color){ 30, 30, 45, 255 }, (Color){ 15, 15, 25, 255 });
    Texture2D bgTexture = LoadTextureFromImage(backgroundImage);
    UnloadImage(backgroundImage);

    BanDoc *memberList = NULL;    // Danh sách liên kết quản lý bạn đọc
    Nhap inputForm;               // Cấu trúc chứa dữ liệu form nhập
    int totalUsers = 0;           // Biến đếm tổng số bạn đọc để sinh mã
    char maTheMoi[15] = "";       // Bộ nhớ đệm chứa mã thẻ vừa sinh

    // Khởi tạo trạng thái ban đầu cho form
    InitForm(&inputForm);

    while (!WindowShouldClose()) {
        
        // Cập nhật vị trí form dựa trên kích thước màn hình (Responsive)
        UpdateFormPosition(&inputForm);

        // Xử lý logic nhập liệu và lưu trữ
        UpdateInputForm(&inputForm, &memberList, &totalUsers, maTheMoi);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Vẽ hình nền
        DrawTexture(bgTexture, 0, 0, WHITE);
        
        // Vẽ hiệu ứng lớp phủ tối để nổi bật Form
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.2f));

        // Vẽ tiêu đề hệ thống
        DrawText("HE THONG QUAN LY THU VIEN PROXIMA", 40, 40, 30, GOLD);
        DrawRectangle(40, 80, 500, 2, SKYBLUE);
        
        // Vẽ Form đăng ký thành viên
        // Lưu ý: Tôi truyền mảng icon trống vì chưa có file thực tế
        DrawLibraryCard(&inputForm, NULL, maTheMoi, mainFont);

        // Vẽ danh sách bạn đọc đã đăng ký (Demo nhanh)
        DrawText("DANH SACH MOI DANG KY:", 750, 180, 20, LIGHTGRAY);
        BanDoc *current = memberList;
        int offset = 0;
        while (current != NULL && offset < 10) { // Chỉ hiện 10 người gần nhất
            DrawText(TextFormat("%s - %s", current->maThe, current->hoTen), 750, 220 + (offset * 30), 18, RAYWHITE);
            current = current->next;
            offset++;
        }

        // Nếu đăng ký thành công thì hiện popup
        if (inputForm.showSuccess) {
            DrawSuccessMessage(mainFont, bgTexture, inputForm.successTimer);
            
            // Tự động tắt thông báo sau 3 giây
            if (GetTime() - inputForm.successTimer > 3.0f) {
                inputForm.showSuccess = false;
            }
        }

        EndDrawing();
    }

    // Giải phóng bộ nhớ danh sách liên kết
    FreeMemberList(memberList);
    
    // Giải phóng texture
    UnloadTexture(bgTexture);
    
    CloseWindow();

    return 0;
}