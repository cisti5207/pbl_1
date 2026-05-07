#include <stdio.h>
#include "raylib.h"
#include "login.h"
#include <string.h>
#include "ManageUser.h"
Font font[6];

// Khai báo lại font toàn cục nếu ông dùng mảng font[6] như code cũ
Font font[6];

int main() {
    // 1. Khởi tạo cửa sổ
    const int screenWidth = 1200;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "PBL1 - Test Library Card UI");
    SetTargetFPS(60);

    // 2. Load Font (Thay đường dẫn cho đúng với máy ông nhé)
    // Tui dùng font[0] làm font chính để hiển thị tiếng Việt
    font[0] = LoadFontEx("font/arial/arial.ttf", 32, NULL, 0); 

    // 3. Khởi tạo dữ liệu Nhập và Bạn Đọc
    Nhap formBD;
    InitForm_BD(&formBD); // Hàm này tui viết ở trên để set tọa độ ô nhập

    // Tạo một biến BanDoc tạm để chứa dữ liệu hiển thị lên thẻ
    BanDoc hienThiBD;
    strcpy(hienThiBD.maThe, "20240001");

    while (!WindowShouldClose()) {
        // --- 4. UPDATE: Xử lý nhập liệu ---
        // Ông di chuột vào vùng thông tin bên phải cái thẻ để nhập nhé
        XuLyNhap(&formBD.inputHoTen);
        XuLyNhap(&formBD.inputCCCD);
        XuLyNhap(&formBD.inputSDT);
        XuLyNhap(&formBD.inputHanSD);

        // Đồng bộ dữ liệu từ Form sang biến hiển thị
        strcpy(hienThiBD.hoTen, formBD.inputHoTen.text);
        strcpy(hienThiBD.cccd, formBD.inputCCCD.text);
        strcpy(hienThiBD.sdt, formBD.inputSDT.text);
        strcpy(hienThiBD.hanSD, formBD.inputHanSD.text);

        // --- 5. DRAW: Vẽ giao diện ---
        BeginDrawing();
            ClearBackground(RAYWHITE); // Nền trắng phía dưới cùng

            // Vẽ một cái background giả lập cho đỡ trống (ví dụ danh sách bạn đọc)
            DrawText("DANH SACH BAN DOC (BACKGROUND)", 50, 50, 20, LIGHTGRAY);
            for(int i=0; i<10; i++) DrawLine(50, 80 + i*30, 400, 80 + i*30, LIGHTGRAY);

            // GỌI HÀM VẼ THẺ SIÊU CẤP (Nó sẽ tự vẽ lớp phủ mờ đen luôn)
            VeTheThuVien(font[0], &hienThiBD, &formBD);

            // Vẽ hướng dẫn cho người dùng
            DrawText("Bam chuot vao cac dong ben phai de nhap lieu", 400, 720, 20, DARKGRAY);
            DrawText("ESC: Thoat | Enter: Luu du lieu", 400, 750, 18, GRAY);

        EndDrawing();
    }

    // 6. Giải phóng bộ nhớ
    UnloadFont(font[0]);
    CloseWindow();

    return 0;
}