#include "ManageUser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void DeleteLastChar_BD(InputBox_BD *input) {
    if (input->length > 0) {
        input->length--;
        while (input->length > 0 && (input->text[input->length] & 0xC0) == 0x80) {
            input->length--;
        }
        input->text[input->length] = '\0';
    }
}

void InitForm(Nhap *form) {
    memset(form->inputHoTen.text, 0, MAX_INPUT); form->inputHoTen.length = 0; form->inputHoTen.isFocused = false;
    memset(form->inputSDT.text, 0, MAX_INPUT);   form->inputSDT.length = 0;   form->inputSDT.isFocused = false;
    memset(form->inputCCCD.text, 0, MAX_INPUT);  form->inputCCCD.length = 0;  form->inputCCCD.isFocused = false;
    memset(form->inputHanSD.text, 0, MAX_INPUT); form->inputHanSD.length = 0; form->inputHanSD.isFocused = false;
}

void SinhMaTheTuDong(int tongSo, char *maTheMoi) {
    sprintf(maTheMoi, "%08d", tongSo + 1);
}

void DrawLibraryCard(Nhap *form, Texture2D fieldIcons[], char *maThe, Font font) {
    int cardW = 600, cardH = 400;
    int cardX = (GetScreenWidth() - cardW) / 2;
    int cardY = (GetScreenHeight() - cardH) / 2;
    Rectangle cardRec = { (float)cardX, (float)cardY, (float)cardW, (float)cardH };

    // 1. Vẽ bóng đổ
    for (int i = 1; i <= 8; i++) 
        DrawRectangleRounded((Rectangle){cardRec.x + i, cardRec.y + i, (float)cardW, (float)cardH}, 0.1f, 15, Fade(BLACK, 0.03f));

    DrawRectangleRounded(cardRec, 0.1f, 15, WHITE); 

    // 2. Vẽ nền nửa trái (Pastel Pink)
    BeginScissorMode(cardX, cardY, cardW / 2, cardH);
    DrawRectangleGradientV(cardX, cardY, cardW / 2, cardH, (Color){255, 192, 203, 255}, (Color){255, 105, 180, 255});
    EndScissorMode();

    // 3. Khung ảnh Placeholder
    Rectangle frameRec = { cardX + (300 - 150)/2, cardY + (400 - 200)/2, 150, 200 };
    DrawRectangleRoundedLinesEx((Rectangle){frameRec.x - 5, frameRec.y - 5, 160, 210}, 0.05f, 5, 3.0f, WHITE);
    DrawRectangleRec(frameRec, Fade(WHITE, 0.3f));
    DrawCircle(frameRec.x + 75, frameRec.y + 70, 30, Fade(WHITE, 0.6f));
    DrawEllipse(frameRec.x + 75, frameRec.y + 160, 50, 40, Fade(WHITE, 0.6f));

    // 4. Nội dung bên phải
    DrawTextEx(font, "HoanHoang_DUT", (Vector2){ (float)cardX + 355, (float)cardY + 45 }, 26, 2, DARKPURPLE);
    DrawLineEx((Vector2){cardX + 330, cardY + 80}, (Vector2){cardX + 570, cardY + 80}, 2, Fade(DARKPURPLE, 0.3f));
    DrawTextEx(font, TextFormat("ID: %s", maThe), (Vector2){ cardX + 20, cardY + 370 }, 16, 1, WHITE);

    InputBox_BD *boxes[] = { &form->inputHoTen, &form->inputSDT, &form->inputCCCD, &form->inputHanSD };
    const char *labels[] = { "Họ và Tên", "Số điện thoại", "Số CCCD", "Hạn sử dụng" };
    int startY = cardY + 110;

    for (int i = 0; i < 4; i++) {
        boxes[i]->rec = (Rectangle){ (float)cardX + 360, (float)(startY + i * 65), 210, 40 };
        
        if (fieldIcons[i].id != 0) {
            float iconSize = 25.0f;
            float scale = iconSize / fieldIcons[i].width;
            DrawTextureEx(fieldIcons[i], (Vector2){ boxes[i]->rec.x - 35, boxes[i]->rec.y + 7 }, 0, scale, DARKGRAY);
        }

        DrawRectangleRoundedLinesEx(boxes[i]->rec, 0.2f, 10, boxes[i]->isFocused ? 2.0f : 1.0f, boxes[i]->isFocused ? BLUE : LIGHTGRAY);
        
        if (strlen(boxes[i]->text) > 0) {
            DrawTextEx(font, boxes[i]->text, (Vector2){ boxes[i]->rec.x + 10, boxes[i]->rec.y + 10 }, 18, 1, BLACK);
        } else {
            DrawTextEx(font, labels[i], (Vector2){ boxes[i]->rec.x + 10, boxes[i]->rec.y + 10 }, 16, 1, GRAY);
        }

        if (boxes[i]->isFocused && (int)(GetTime() * 2) % 2 == 0) {
            float textW = MeasureTextEx(font, boxes[i]->text, 18, 1).x;
            DrawRectangle((int)(boxes[i]->rec.x + 12 + textW), (int)(boxes[i]->rec.y + 8), 2, 24, BLUE);
        }
    }
}

void UpdateInputForm(Nhap *form) {
    InputBox_BD *boxes[] = { &form->inputHoTen, &form->inputSDT, &form->inputCCCD, &form->inputHanSD };
    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < 4; i++) {
        if (CheckCollisionPointRec(mouse, boxes[i]->rec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            for (int j = 0; j < 4; j++) boxes[j]->isFocused = false;
            boxes[i]->isFocused = true;
        }
        
        if (boxes[i]->isFocused) {
            int key = GetKeyPressed();
            while (key > 0) {
                if (key == KEY_BACKSPACE) DeleteLastChar_BD(boxes[i]);
                key = GetKeyPressed();
            }
            int ch = GetCharPressed();
            while (ch > 0) {
                int size = 0;
                const char *utf8 = CodepointToUTF8(ch, &size);
                if (boxes[i]->length + size < MAX_INPUT) {
                    memcpy(&boxes[i]->text[boxes[i]->length], utf8, size);
                    boxes[i]->length += size;
                    boxes[i]->text[boxes[i]->length] = '\0';
                }
                ch = GetCharPressed();
            }
        }
    }
}