#include "ManageUser.h"
#include "menu.h" 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void InitForm(Nhap *form) {
    memset(form->hoTen.text, 0, 256);
    memset(form->sdt.text, 0, 256);
    memset(form->cccd.text, 0, 256);
    memset(form->hanSD.text, 0, 256);
    form->hoTen.letterCount = 0;
    form->sdt.letterCount = 0;
    form->cccd.letterCount = 0;
    form->hanSD.letterCount = 0;
    
    form->hoTen.backspaceCounter = 0;
    form->sdt.backspaceCounter = 0;
    form->cccd.backspaceCounter = 0;
    form->hanSD.backspaceCounter = 0;
    
    form->hoTen.isFocused = true; 
    form->sdt.isFocused = false;
    form->cccd.isFocused = false;
    form->hanSD.isFocused = false;
    
    form->showSuccess = false;
    form->successTimer = 0;
}

void UpdateFormPosition(Nhap *form) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    Rectangle card = { (screenW - 650*scale)/2, (screenH - 420*scale)/2, 650*scale, 420*scale };
    
    float inputX = card.x + 270 * scale;
    float inputY = card.y + 95 * scale; 
    float inputW = 340 * scale;
    float inputH = 42 * scale;
    float spacing = 78 * scale;

    form->hoTen.rec = (Rectangle){ inputX, inputY + (0 * spacing), inputW, inputH };
    form->sdt.rec   = (Rectangle){ inputX, inputY + (1 * spacing), inputW, inputH };
    form->cccd.rec  = (Rectangle){ inputX, inputY + (2 * spacing), inputW, inputH };
    form->hanSD.rec = (Rectangle){ inputX, inputY + (3 * spacing), inputW, inputH };
}

void UpdateInputForm(Nhap *form, BanDoc **head, int *currentTotalUsers, char *maThe, int *currentState) {
    // 1. Tự động sinh ID khi vừa mở form
    if (maThe[0] == '\0') {
        LayMaTheTiepTheo(*head, maThe);
    }

    // 2. Logic Nút Quay Lại "Tự Chủ"
    Rectangle btnBack = { 20, 20, 130, 40 };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    if (IsKeyPressed(KEY_ESCAPE) || (isHoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
        InitForm(form);
        maThe[0] = '\0';
        *currentState = APP_MENU; // Ép về Menu an toàn
        return;
    }

    // 3. Logic Success
    if (form->showSuccess) {
        form->successTimer -= GetFrameTime();
        if (form->successTimer <= 0) {
            form->showSuccess = false;
            InitForm(form);
            (*currentTotalUsers)++;
            LayMaTheTiepTheo(*head, maThe); // Tiếp tục sinh mã mới cho thẻ tiếp theo
        }
        return; 
    }

    InputBox_BD* boxes[] = {&form->hoTen, &form->sdt, &form->cccd, &form->hanSD};

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePoint = GetMousePosition();
        for (int i = 0; i < 4; i++) {
            if (CheckCollisionPointRec(mousePoint, boxes[i]->rec)) {
                for (int j = 0; j < 4; j++) boxes[j]->isFocused = false;
                boxes[i]->isFocused = true;
                break;
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        if (boxes[i]->isFocused) {
            int key = GetKeyPressed();
            while (key > 0) {
                if (key == KEY_BACKSPACE) {
                    if (boxes[i]->letterCount > 0) {
                        do { boxes[i]->letterCount--; } while (boxes[i]->letterCount > 0 && (boxes[i]->text[boxes[i]->letterCount] & 0xC0) == 0x80);
                        boxes[i]->text[boxes[i]->letterCount] = '\0';
                    }
                }
                key = GetKeyPressed();
            }
            if (IsKeyDown(KEY_BACKSPACE)) {
                boxes[i]->backspaceCounter += GetFrameTime();
                if (boxes[i]->backspaceCounter >= 0.5f) { 
                    if (boxes[i]->letterCount > 0) {
                        do { boxes[i]->letterCount--; } while (boxes[i]->letterCount > 0 && (boxes[i]->text[boxes[i]->letterCount] & 0xC0) == 0x80);
                        boxes[i]->text[boxes[i]->letterCount] = '\0';
                    }
                }
            } else boxes[i]->backspaceCounter = 0;

            // Nhận ký tự Tiếng Việt UTF-8
            int charCode = GetCharPressed();
            while (charCode > 0) {
                if ((charCode >= 32) && (boxes[i]->letterCount < 250)) {
                    int byteSize = 0;
                    const char* utf8Char = CodepointToUTF8(charCode, &byteSize);
                    if (boxes[i]->letterCount + byteSize < 255) {
                        for (int j = 0; j < byteSize; j++) {
                            boxes[i]->text[boxes[i]->letterCount] = utf8Char[j];
                            boxes[i]->letterCount++;
                        }
                        boxes[i]->text[boxes[i]->letterCount] = '\0';
                    }
                }
                charCode = GetCharPressed();
            }
            
            if (IsKeyPressed(KEY_TAB)) {
                boxes[i]->isFocused = false;
                boxes[(i + 1) % 4]->isFocused = true;
                break;
            }
        }
    }

    // Logic xác nhận
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    Rectangle btnConfirm = { (screenW - 180*scale) / 2, ((screenH - 420*scale)/2) + 420*scale + 30*scale, 180*scale, 50*scale };

    if (IsKeyPressed(KEY_ENTER) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), btnConfirm))) {
        if (form->hoTen.letterCount > 0) {
            if (LuuThanhVienVaoFile(maThe, form)) {
                ThemBanDocVaoList(head, maThe, form);
                form->showSuccess = true;    
                form->successTimer = 3.0f;   
            }
        }
    }
}

void DrawLibraryCard(Nhap *form, Texture2D icons[], char *maThe, Font font) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    // --- VẼ NÚT QUAY LẠI TỰ CHỦ Ở ĐÂY ---
    Rectangle btnBack = { 20, 20, 130, 40 };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    Color btnColor = isHoverBack ? MAROON : WHITE;
    Color textColor = isHoverBack ? WHITE : MAROON;
    DrawRectangleRounded(btnBack, 0.3f, 10, btnColor);
    DrawRectangleRoundedLines(btnBack, 0.3f, 10, MAROON);
    DrawTextEx(font, "< Quay lai", (Vector2){btnBack.x + 15, btnBack.y + 10}, 20, 1, textColor);
    // ------------------------------------

    Rectangle card = { (screenW - 650*scale)/2, (screenH - 420*scale)/2, 650*scale, 420*scale };
    
    // Đổ bóng & Nền thẻ
    DrawRectangleRounded((Rectangle){card.x + 6*scale, card.y + 6*scale, card.width, card.height}, 0.1f, 10, Fade(BLACK, 0.2f));
    DrawRectangleRounded(card, 0.1f, 10, GetColor(0xffd1dcff)); 
    DrawRectangleRoundedLines(card, 0.1f, 10, MAROON);

    // Vẽ Avatar Silhouette
    Rectangle avatarBox = { card.x + 45*scale, card.y + 80*scale, 180*scale, 230*scale };
    DrawRectangleRounded(avatarBox, 0.05f, 5, WHITE);
    DrawRectangleRoundedLines(avatarBox, 0.05f, 5, MAROON); 
    DrawCircle(avatarBox.x + avatarBox.width/2, avatarBox.y + 80*scale, 40*scale, Fade(MAROON, 0.3f));
    DrawEllipse(avatarBox.x + avatarBox.width/2, avatarBox.y + 180*scale, 60*scale, 45*scale, Fade(MAROON, 0.3f));

    DrawTextEx(font, "HoanHoang_DUT library", (Vector2){card.x + 260*scale, card.y + 32*scale}, 36*scale, 1, MAROON);
    DrawTextEx(font, TextFormat("ID: %s", maThe), (Vector2){card.x + 45*scale, card.y + 340*scale}, 24*scale, 1, MAROON); 

    const char* labels[] = {"Họ và tên:", "Số điện thoại:", "Căn cước công dân:", "Hạn sử dụng:"};
    InputBox_BD* boxes[] = {&form->hoTen, &form->sdt, &form->cccd, &form->hanSD};

    for (int i = 0; i < 4; i++) {
        DrawTextEx(font, labels[i], (Vector2){boxes[i]->rec.x, boxes[i]->rec.y - 22*scale}, 16*scale, 1, MAROON);
        DrawRectangleRounded(boxes[i]->rec, 0.2f, 10, WHITE);
        DrawRectangleRoundedLines(boxes[i]->rec, 0.2f, 10, boxes[i]->isFocused ? MAROON : LIGHTGRAY);
        
        float fontSize = 20 * scale;
        Vector2 textSize = MeasureTextEx(font, boxes[i]->text, fontSize, 1);
        float textOffsetX = (textSize.x > (boxes[i]->rec.width - 20)) ? (textSize.x - (boxes[i]->rec.width - 20)) : 0;

        BeginScissorMode((int)boxes[i]->rec.x + 5, (int)boxes[i]->rec.y, (int)boxes[i]->rec.width - 10, (int)boxes[i]->rec.height);
            Vector2 textPos = { boxes[i]->rec.x + 10 - textOffsetX, boxes[i]->rec.y + (boxes[i]->rec.height - fontSize)/2 };
            DrawTextEx(font, boxes[i]->text, textPos, fontSize, 1, PINK);

            if (boxes[i]->isFocused && (((int)(GetTime() * 1.5f)) % 2 == 0)) {
                DrawRectangleV((Vector2){ textPos.x + textSize.x + 2, textPos.y + 2 }, (Vector2){ 2, fontSize - 4 }, MAROON);
            }
        EndScissorMode();
    }

    // Nút xác nhận
    Rectangle btnConfirm = { (screenW - 180*scale) / 2, card.y + card.height + 30 * scale, 180*scale, 50*scale };
    bool isHover = CheckCollisionPointRec(GetMousePosition(), btnConfirm);
    DrawRectangleRounded(btnConfirm, 0.3f, 10, isHover ? MAROON : PINK);
    DrawTextEx(font, "XÁC NHẬN", (Vector2){btnConfirm.x + (btnConfirm.width - MeasureTextEx(font, "XÁC NHẬN", 20*scale, 1).x)/2, btnConfirm.y + (btnConfirm.height - 20*scale)/2}, 20*scale, 1, WHITE);

    // Gom vẽ SuccessMessage vào chung đây luôn cho main.c nhàn rỗi
    if (form->showSuccess) {
        DrawSuccessMessage(font, (Texture2D){0}, form->successTimer);
    }
}

void DrawSuccessMessage(Font font, Texture2D background2, float currentTimer) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

    if (background2.id != 0) {
        DrawTexturePro(background2, (Rectangle){0, 0, (float)background2.width, (float)background2.height}, 
                       (Rectangle){0, 0, screenW, screenH}, (Vector2){0, 0}, 0, WHITE);
    }
    DrawRectangle(0, 0, (int)screenW, (int)screenH, Fade(BLACK, 0.5f));
    
    Rectangle panel = { screenW/2 - 250, screenH/2 - 150, 500, 300 };
    DrawRectangleRounded(panel, 0.1f, 10, WHITE);
    DrawRectangleRoundedLines(panel, 0.1f, 10, PINK);

    DrawCircle((int)screenW/2, (int)screenH/2 - 60, 45, PINK);
    DrawTextEx(font, "V", (Vector2){screenW/2 - 15, screenH/2 - 85}, 60, 1, WHITE);

    const char* msg = "TẠO THẺ THÀNH CÔNG!";
    Vector2 msgSize = MeasureTextEx(font, msg, 32, 1);
    DrawTextEx(font, msg, (Vector2){(screenW - msgSize.x)/2, screenH/2 + 20}, 32, 1, MAROON);

    float progress = currentTimer / 3.0f; 
    DrawRectangle((int)panel.x + 50, (int)panel.y + panel.height - 50, 400, 12, LIGHTGRAY);
    DrawRectangle((int)panel.x + 50, (int)panel.y + panel.height - 50, (int)(400 * (1.0f - progress)), 12, PINK);
}

bool LuuThanhVienVaoFile(char *maThe, Nhap *form) {
    FILE *f = fopen("data/Phieumuon/User.txt", "a"); 
    if (f == NULL) return false;

    // Xóa khoảng trắng thừa để file gọn gàng
    for (int i = strlen(form->hoTen.text)-1; i >= 0 && form->hoTen.text[i] == ' '; i--) form->hoTen.text[i] = '\0';
    for (int i = strlen(form->sdt.text)-1; i >= 0 && form->sdt.text[i] == ' '; i--) form->sdt.text[i] = '\0';
    for (int i = strlen(form->cccd.text)-1; i >= 0 && form->cccd.text[i] == ' '; i--) form->cccd.text[i] = '\0';
    for (int i = strlen(form->hanSD.text)-1; i >= 0 && form->hanSD.text[i] == ' '; i--) form->hanSD.text[i] = '\0';

    fprintf(f, "%-12s | %-25s | %-12s | %-15s | %-12s\n", 
            maThe, form->hoTen.text, form->sdt.text, form->cccd.text, form->hanSD.text);
    
    fclose(f);
    return true;
}

void ThemBanDocVaoList(BanDoc **head, char *maThe, Nhap *form) {
    BanDoc *newNode = (BanDoc*)malloc(sizeof(BanDoc));
    if (newNode) {
        strcpy(newNode->maThe, maThe);
        strcpy(newNode->hoTen, form->hoTen.text);
        strcpy(newNode->sdt, form->sdt.text);
        strcpy(newNode->cccd, form->cccd.text);
        strcpy(newNode->hanSD, form->hanSD.text);
        newNode->next = *head;
        *head = newNode;
    }
}   

void SinhMaTheTuDong(int currentCount, char *maThe) { sprintf(maThe, "%08d", currentCount + 1); }
void FreeMemberList(BanDoc *head) { while (head) { BanDoc *t = head; head = head->next; free(t); } }

void DrawTheBanDoc_TimKiem(BanDoc *the, Font font, float toa_do_x, float toa_do_y) {
    if (the == NULL) return;

    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    Rectangle card = { toa_do_x, toa_do_y, 650*scale, 420*scale };
    
    DrawRectangleRounded((Rectangle){card.x + 6*scale, card.y + 6*scale, card.width, card.height}, 0.1f, 10, Fade(BLACK, 0.2f));
    DrawRectangleRounded(card, 0.1f, 10, GetColor(0xffd1dcff)); 
    DrawRectangleRoundedLines(card, 0.1f, 10, MAROON);

    Rectangle avatarBox = { card.x + 45*scale, card.y + 80*scale, 180*scale, 230*scale };
    DrawRectangleRounded(avatarBox, 0.05f, 5, WHITE);
    DrawRectangleRoundedLines(avatarBox, 0.05f, 5, MAROON); 
    DrawCircle(avatarBox.x + avatarBox.width/2, avatarBox.y + 80*scale, 40*scale, Fade(MAROON, 0.3f));
    DrawEllipse(avatarBox.x + avatarBox.width/2, avatarBox.y + 180*scale, 60*scale, 45*scale, Fade(MAROON, 0.3f));

    DrawTextEx(font, "HoanHoang_DUT library", (Vector2){card.x + 260*scale, card.y + 32*scale}, 36*scale, 1, MAROON);
    DrawTextEx(font, TextFormat("ID: %s", the->maThe), (Vector2){card.x + 45*scale, card.y + 340*scale}, 24*scale, 1, MAROON); 

    const char* labels[] = {"Họ và tên:", "Số điện thoại:", "Căn cước công dân:", "Hạn sử dụng:"};
    const char* values[] = {the->hoTen, the->sdt, the->cccd, the->hanSD};
    
    float textX = card.x + 270 * scale;
    float startY = card.y + 95 * scale;
    float boxWidth = 340 * scale;
    float boxHeight = 42 * scale;
    float spacing = 80.0f * scale; 

    // Đã fix sạch sẽ code vẽ hộp bị lặp ở đây!
    for (int i = 0; i <4; i++) {
        float currentY = startY + (i * spacing);
        
        // Vẽ Label
        DrawTextEx(font, labels[i], (Vector2){textX, currentY - 22*scale}, 16*scale, 1, MAROON);
        
        // Vẽ khung trắng
        Rectangle textBox = { textX, currentY, boxWidth, boxHeight };
        DrawRectangleRounded(textBox, 0.2f, 10, WHITE);
        DrawRectangleRoundedLines(textBox, 0.2f, 10, LIGHTGRAY);
        
        // Vẽ Text
        BeginScissorMode((int)textBox.x + 5, (int)textBox.y, (int)textBox.width - 10, (int)textBox.height);
            float fontSize = 20 * scale;
            Vector2 textPos = { textBox.x + 10, textBox.y + (textBox.height - fontSize)/2 };
            DrawTextEx(font, values[i], textPos, fontSize, 1, PINK);
        EndScissorMode();
    }
}

void LayMaTheTiepTheo(BanDoc *head, char *maThe) {
    int maxID = 0;
    BanDoc *temp = head;
    
    while (temp != NULL) {
        int currentID = atoi(temp->maThe); 
        if (currentID > maxID) {
            maxID = currentID;
        }
        temp = temp->next;
    }
    
    sprintf(maThe, "%08d", maxID + 1);
}