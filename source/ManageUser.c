#include "ManageUser.h"
#include <stdio.h>
#include <string.h>

void InitForm(Nhap *form) {
    memset(form, 0, sizeof(Nhap));
}

void SinhMaTheTuDong(int currentCount, char *maThe) {
    sprintf(maThe, "%08d", currentCount + 1);
}

void UpdateFormPosition(Nhap *form) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    float cardW = 650 * scale;
    float cardH = 420 * scale;
    float cardX = (screenW - cardW) / 2.0f;
    float cardY = (screenH - cardH) / 2.0f;
    
    float inputX = cardX + 270 * scale;
    float inputY = cardY + 110 * scale;
    float inputW = 340 * scale;
    float inputH = 45 * scale;
    float spacing = 70 * scale;

    form->hoTen.rec = (Rectangle){ inputX, inputY + (0 * spacing), inputW, inputH };
    form->sdt.rec   = (Rectangle){ inputX, inputY + (1 * spacing), inputW, inputH };
    form->cccd.rec  = (Rectangle){ inputX, inputY + (2 * spacing), inputW, inputH };
    form->hanSD.rec = (Rectangle){ inputX, inputY + (3 * spacing), inputW, inputH };
}

void DeleteLastCharUTF8(char* text, int* count) {
    if (*count > 0) {
        do {
            (*count)--;
        } while (*count > 0 && (text[*count] & 0xC0) == 0x80);
        text[*count] = '\0';
    }
}

void UpdateInputForm(Nhap *form) {
    InputBox_BD* boxes[] = {&form->hoTen, &form->sdt, &form->cccd, &form->hanSD};
    Vector2 mousePos = GetMousePosition();

    for (int i = 0; i < 4; i++) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            boxes[i]->isFocused = CheckCollisionPointRec(mousePos, boxes[i]->rec);
        }

        if (boxes[i]->isFocused) {
            SetMouseCursor(MOUSE_CURSOR_IBEAM);
            
            // 1. Xử lý Backspace trong vòng lặp key (theo code bạn bro)
            int key = GetKeyPressed();
            while (key > 0) {
                if (key == KEY_BACKSPACE) {
                    DeleteLastCharUTF8(boxes[i]->text, &boxes[i]->letterCount);
                }
                key = GetKeyPressed(); 
            }

            // 2. Lấy ký tự Unicode
            int codepoint = GetCharPressed();
            while (codepoint > 0) {
                if (boxes[i]->letterCount < 250) {
                    int utf8Len = 0;
                    const char* utf8Char = CodepointToUTF8(codepoint, &utf8Len);
                    for (int j = 0; j < utf8Len; j++) {
                        boxes[i]->text[boxes[i]->letterCount] = utf8Char[j];
                        boxes[i]->letterCount++;
                    }
                    boxes[i]->text[boxes[i]->letterCount] = '\0';
                }
                codepoint = GetCharPressed();
            }
        }
    }
}

void DrawLibraryCard(Nhap *form, Texture2D icons[], char *maThe, Font font) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    Rectangle card = { (screenW - 650*scale)/2, (screenH - 420*scale)/2, 650*scale, 420*scale };
    
    DrawRectangleRounded((Rectangle){card.x+6*scale, card.y+6*scale, card.width, card.height}, 0.1f, 10, Fade(BLACK, 0.2f));
    DrawRectangleRounded(card, 0.1f, 10, RAYWHITE);
    DrawRectangleGradientV(card.x, card.y, card.width, card.height, Fade(PINK, 0.4f), WHITE);

    // Avatar Bóng Hồng
    Rectangle imgBox = { card.x + 40*scale, card.y + 80*scale, 180*scale, 240*scale };
    DrawRectangleRoundedLines(imgBox, 0.05f, 10, PINK); 
    DrawCircle(imgBox.x + imgBox.width/2, imgBox.y + imgBox.height/2 - 35*scale, 45*scale, Fade(PINK, 0.7f)); 
    DrawEllipse(imgBox.x + imgBox.width/2, imgBox.y + imgBox.height/2 + 65*scale, 65*scale, 55*scale, Fade(PINK, 0.7f)); 

    DrawTextEx(font, "HuuHoang_DUT library", (Vector2){card.x + 270*scale, card.y + 40*scale}, 38*scale, 1, MAROON);
    DrawTextEx(font, TextFormat("ID: %s", maThe), (Vector2){card.x + 40*scale, card.y + 340*scale}, 24*scale, 1, PINK);

    const char* labels[] = {"Họ tên:", "SĐT:", "CCCD:", "Hạn SD:"};
    InputBox_BD* boxes[] = {&form->hoTen, &form->sdt, &form->cccd, &form->hanSD};

    for (int i = 0; i < 4; i++) {
        DrawTextEx(font, labels[i], (Vector2){boxes[i]->rec.x, boxes[i]->rec.y - 22*scale}, 18*scale, 1, MAROON);
        
        DrawRectangleRounded(boxes[i]->rec, 0.15f, 10, WHITE);
        Color borderColor = boxes[i]->isFocused ? PINK : LIGHTGRAY;
        DrawRectangleRoundedLines(boxes[i]->rec, 0.15f, 10, borderColor); 

        // --- LOGIC CUỘN CHỮ THÔNG MINH ---
        float fontSize = 22*scale;
        float padding = 15*scale;
        float maxTextWidth = boxes[i]->rec.width - (padding * 2);
        Vector2 textSize = MeasureTextEx(font, boxes[i]->text, fontSize, 1);
        
        // Tính toán độ lệch (Offset)
        float textOffsetX = 0;
        if (textSize.x > maxTextWidth) {
            textOffsetX = textSize.x - maxTextWidth; 
        }

        // Bật Scissor Mode: Chỉ vẽ bên trong ô nhập (trừ đi padding)
        BeginScissorMode((int)(boxes[i]->rec.x + padding), (int)boxes[i]->rec.y, (int)maxTextWidth, (int)boxes[i]->rec.height);
            
            Vector2 textPos = { boxes[i]->rec.x + padding - textOffsetX, boxes[i]->rec.y + 10*scale };
            DrawTextEx(font, boxes[i]->text, textPos, fontSize, 1, PINK);

            // Con trỏ nháy (Cũng phải chạy theo chữ)
            if (boxes[i]->isFocused && (int)(GetTime() * 2) % 2 == 0) {
                DrawRectangle((int)(textPos.x + textSize.x + 2), (int)(textPos.y + 2), 2, (int)(18*scale), PINK);
            }

        EndScissorMode(); // Tắt Scissor Mode
    }
}