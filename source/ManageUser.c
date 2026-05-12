#include "ManageUser.h"

// Màu sắc định nghĩa riêng cho giao diện thẻ
static Color PASTEL_BLUE = (Color){ 185, 224, 247, 255 }; 
static Color DARK_BLUE_TEXT = (Color){ 20, 60, 100, 255 };

// Hàm vẽ viền bo góc có độ dày tùy chỉnh (Tự chế để thay thế hàm thiếu tham số của Raylib)
void DrawRectangleRoundedLinesEx(Rectangle rec, float roundness, int segments, float thickness, Color color) {
    for (int i = 0; i < (int)thickness; i++) {
        Rectangle thickRec = { rec.x + (float)i, rec.y + (float)i, rec.width - (float)i*2, rec.height - (float)i*2 };
        DrawRectangleRoundedLines(thickRec, roundness, segments, color);
    }
}

void UpdateInput(InputBox_BD *box, Vector2 mousePos) {
    // Kiểm tra Click chuột để Focus vào ô nhập
    if (CheckCollisionPointRec(mousePos, box->rec)) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) box->isFocused = true;
    } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        box->isFocused = false;
    }

    if (box->isFocused) {
        int key = GetCharPressed();
        while (key > 0) {
            // Nhận ký tự ASCII (Có thể nâng cấp lên UTF-8 sau này với font của bro)
            if ((key >= 32) && (key <= 125) && (box->length < MAX_INPUT - 1)) {
                box->text[box->length] = (char)key;
                box->text[box->length + 1] = '\0';
                box->length++;
            }
            key = GetCharPressed();
        }
        // Xóa ký tự bằng phím Backspace
        if (IsKeyPressed(KEY_BACKSPACE) && box->length > 0) {
            box->text[--box->length] = '\0';
        }
    }
}

void SinhMaTheTuDong(int tongSo, char *maTheMoi) {
    sprintf(maTheMoi, "%08d", tongSo + 1);
}

void DrawLibraryCard(Rectangle cardRect, FormNhap *form, const char* maTheHienTai) {
    // 1. Vẽ nền trắng của toàn bộ thẻ
    DrawRectangleRounded(cardRect, 0.05f, 20, RAYWHITE);
    DrawRectangleRoundedLinesEx(cardRect, 0.05f, 20, 3, LIGHTGRAY);

    // 2. Vẽ nửa bên trái (Màu xanh Pastel) - Dùng ScissorMode để không lem ra ngoài góc bo
    float splitX = cardRect.x + cardRect.width * 0.35f;
    Rectangle leftPart = { cardRect.x, cardRect.y, cardRect.width * 0.35f, cardRect.height };
    
    BeginScissorMode((int)leftPart.x, (int)leftPart.y, (int)leftPart.width, (int)leftPart.height);
        DrawRectangleRounded(cardRect, 0.05f, 20, PASTEL_BLUE);
    EndScissorMode();

    // 3. Khu vực ảnh thẻ (120x160)
    Rectangle photoRect = { leftPart.x + (leftPart.width - 120)/2, leftPart.y + 80, 120, 160 };
    DrawRectangleRec(photoRect, WHITE);
    DrawRectangleLinesEx(photoRect, 1, GRAY);
    DrawText("ANH 3x4", (int)photoRect.x + 35, (int)photoRect.y + 70, 15, DARKGRAY);

    // 4. Mã thẻ (Góc trái dưới)
    DrawText("ID Card:", (int)leftPart.x + 20, (int)leftPart.y + (int)cardRect.height - 50, 15, DARK_BLUE_TEXT);
    DrawText(maTheHienTai, (int)leftPart.x + 20, (int)leftPart.y + (int)cardRect.height - 35, 20, DARK_BLUE_TEXT);

    // 5. Tiêu đề Thư viện bên phải
    DrawText("HoanHoang_DUT Library", (int)splitX + 50, (int)cardRect.y + 25, 22, DARKBLUE);
    DrawLine((int)splitX + 40, (int)cardRect.y + 55, (int)cardRect.x + (int)cardRect.width - 40, (int)cardRect.y + 55, LIGHTGRAY);

    // 6. Vẽ các ô nhập liệu
    for (int i = 0; i < 4; i++) {
        InputBox_BD *b = &form->inputs[i];
        // Vẽ nhãn (Label) phía trên ô nhập
        DrawText(b->label, (int)b->rec.x, (int)b->rec.y - 18, 15, DARKGRAY);
        // Vẽ ô nhập nền xanh nhạt
        DrawRectangleRounded(b->rec, 0.3f, 10, PASTEL_BLUE); 
        
        // Vẽ viền xanh đậm khi đang chọn (Focus)
        if (b->isFocused) DrawRectangleRoundedLinesEx(b->rec, 0.3f, 10, 2, BLUE);
        // Vẽ nội dung chữ đã nhập
        DrawText(b->text, (int)b->rec.x + 10, (int)b->rec.y + 10, 18, BLACK);
    }
}