#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "ManageUser.h"
#include "Timkiemthethuvien.h"
#include "libmanage.h"

int main(void) {
    // 1. Định cấu hình kích thước màn hình
    const int screenWidth = 1100;
    const int screenHeight = 750;
    InitWindow(screenWidth, screenHeight, "PBL1 - Quản Lý Thư Viện - Tìm Kiếm Thẻ Bạn Đọc");
    SetTargetFPS(60);

    // 2. Thiết lập đường dẫn tuyệt đối đến file User.txt trên máy của ông
    // Dùng gạch chéo xuôi (/) là chuẩn nhất trong C, Windows vẫn đọc mượt mà không lo lỗi escape character!
    const char *filePath = "D:/Code file/pbl/pbl_1/data/Phieumuon/User.txt";

    // 3. Khởi tạo danh sách liên kết Bạn Đọc và đọc dữ liệu từ file
    BanDoc *head = NULL;
    int currentTotalUsers = 0;
    
    // Đọc dữ liệu từ file User.txt vào danh sách liên kết 'head'
    // Hàm này đã bao gồm tính năng tự động kiểm tra xem có mở được file không
    DocDuLieuTheBanDoc(filePath, &head, &currentTotalUsers);

    // 4. Khởi tạo Form Tìm Kiếm
    FormTimKiemThe formTimKiem;
    InitListBD(&formTimKiem);

    // Tải font Arial từ đường dẫn của ông
    // Ghi chú: Nhớ kiểm tra lại xem tên file có dấu chấm không nhé (ví dụ: ARIAL.ttf hoặc ARIAL.TTF)
    Font defaultFont = LoadFont("font/arial/ARIAL.TTF");

    // 5. Vòng lặp vẽ màn hình chính
    while (!WindowShouldClose()) {
        // Cập nhật vị trí hiển thị và xử lý nhập liệu tiếng Việt / cuộn chuột
        Updatetoado(&formTimKiem);
        kiemtralienquantiengviet(&formTimKiem);

        BeginDrawing();
            ClearBackground(GetColor(0xf5f7faff)); // Màu nền xám nhạt hiện đại

            // Gọi hàm vẽ giao diện tìm kiếm và danh sách kết quả (chỉ hiện khi gõ chữ)
            DrawTimKiemThe(&formTimKiem, NULL, defaultFont, head);

        EndDrawing();
    }

    // 6. Dọn dẹp bộ nhớ của danh sách liên kết và font trước khi đóng ứng dụng
    FreeMemberList(head);
    UnloadFont(defaultFont); // Giải phóng bộ nhớ font
    CloseWindow();

    return 0;
}