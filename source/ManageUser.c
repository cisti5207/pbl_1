#include "ManageUser.h"
#include "menu.h" 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libmanage.h>
#include <time.h>

void InitForm(Nhap *form) {
    memset(form->hoTen.text, 0, 256);
    memset(form->sdt.text,   0, 256);
    memset(form->cccd.text,  0, 256);
    memset(form->hanSD.text, 0, 256);

    form->hoTen.letterCount = 0;
    form->sdt.letterCount   = 0;
    form->cccd.letterCount  = 0;
    form->hanSD.letterCount = 0;

    form->hoTen.backspaceCounter = 0;
    form->sdt.backspaceCounter   = 0;
    form->cccd.backspaceCounter  = 0;
    form->hanSD.backspaceCounter = 0;

    form->hoTen.isFocused = true;
    form->sdt.isFocused   = false;
    form->cccd.isFocused  = false;
    form->hanSD.isFocused = false;

    form->showSuccess  = false;
    form->successTimer = 0;

    // Reset thông báo lỗi
    form->errorSdt[0]  = '\0';
    form->errorCccd[0] = '\0';
}

void UpdateFormPosition(Nhap *form) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    Rectangle card = { (screenW - 650*scale)/2, (screenH - 420*scale)/2, 650*scale, 420*scale };

    float inputX  = card.x + 270 * scale;
    float inputY  = card.y + 95  * scale;
    float inputW  = 340 * scale;
    float inputH  = 42  * scale;
    float spacing = 78  * scale;

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
if (form->hanSD.letterCount == 0) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    t->tm_year += 1;   // +1 năm
    mktime(t);         // normalize (xử lý 29/02 leap year, v.v.)
    snprintf(form->hanSD.text, 256, "%02d/%02d/%04d",
             t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
    form->hanSD.letterCount = (int)strlen(form->hanSD.text);
}
    // 2. Logic nút Quay Lại
    Rectangle btnBack = { 20, 20, 130, 40 };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    if (IsKeyPressed(KEY_ESCAPE) || (isHoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
        InitForm(form);
        maThe[0] = '\0';
        *currentState = APP_MENU;
        return;
    }

    // 3. Logic Success (đang hiện banner → đợi hết timer rồi reset)
    if (form->showSuccess) {
        form->successTimer -= GetFrameTime();
        if (form->successTimer <= 0) {
            form->showSuccess = false;
            InitForm(form);
            (*currentTotalUsers)++;
            LayMaTheTiepTheo(*head, maThe);
        }
        return;
    }

    InputBox_BD* boxes[] = { &form->hoTen, &form->sdt, &form->cccd, &form->hanSD };

    // 4. Đổi focus khi click chuột
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

    // 5. Xử lý nhập liệu từng ô
    for (int i = 0; i < 4; i++) {
        if (!boxes[i]->isFocused) continue;

        // --- Backspace (nhấn 1 lần) ---
        int key = GetKeyPressed();
        while (key > 0) {
            if (key == KEY_BACKSPACE) {
                if (boxes[i]->letterCount > 0) {
                    // Lùi 1 codepoint UTF-8
                    do { boxes[i]->letterCount--; }
                    while (boxes[i]->letterCount > 0 &&
                           (boxes[i]->text[boxes[i]->letterCount] & 0xC0) == 0x80);
                    boxes[i]->text[boxes[i]->letterCount] = '\0';

                    // Xoá lỗi khi user chỉnh lại
                    if (i == 1) form->errorSdt[0]  = '\0';
                    if (i == 2) form->errorCccd[0] = '\0';
                }
            }
            key = GetKeyPressed();
        }

        // --- Backspace (giữ phím) ---
        if (IsKeyDown(KEY_BACKSPACE)) {
            boxes[i]->backspaceCounter += GetFrameTime();
            if (boxes[i]->backspaceCounter >= 0.5f) {
                if (boxes[i]->letterCount > 0) {
                    do { boxes[i]->letterCount--; }
                    while (boxes[i]->letterCount > 0 &&
                           (boxes[i]->text[boxes[i]->letterCount] & 0xC0) == 0x80);
                    boxes[i]->text[boxes[i]->letterCount] = '\0';

                    if (i == 1) form->errorSdt[0]  = '\0';
                    if (i == 2) form->errorCccd[0] = '\0';
                }
            }
        } else {
            boxes[i]->backspaceCounter = 0;
        }

        // --- Nhận ký tự ---
        int charCode = GetCharPressed();
        while (charCode > 0) {

            if (i == 1 || i == 2) {
                // ---- Ô SDT (i=1) và CCCD (i=2): chỉ nhận chữ số 0-9 ----
                int maxDigits = (i == 1) ? 10 : 12;

                if (charCode >= '0' && charCode <= '9') {
                    // Còn chỗ thì ghi, đủ rồi thì bỏ qua (hard-cap)
                    if (boxes[i]->letterCount < maxDigits) {
                        boxes[i]->text[boxes[i]->letterCount] = (char)charCode;
                        boxes[i]->letterCount++;
                        boxes[i]->text[boxes[i]->letterCount] = '\0';

                        // Xoá lỗi khi user gõ lại
                        if (i == 1) form->errorSdt[0]  = '\0';
                        if (i == 2) form->errorCccd[0] = '\0';
                    }
                    // Nếu đã đủ số → bỏ qua, không báo lỗi ngay lúc gõ
                }
                // Ký tự không phải số → bỏ qua hoàn toàn (chặn tại đây)

            } else {
                // ---- Ô thường (Họ tên, Hạn SD): giữ nguyên logic cũ ----
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
            }

            charCode = GetCharPressed();
        }

        // --- Tab để chuyển ô ---
        if (IsKeyPressed(KEY_TAB)) {
            boxes[i]->isFocused = false;
            boxes[(i + 1) % 4]->isFocused = true;
            break;
        }
    }

    // 6. Validate & Xác nhận
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale   = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    Rectangle btnConfirm = {
        (screenW - 180*scale) / 2,
        ((screenH - 420*scale)/2) + 420*scale + 30*scale,
        180*scale, 50*scale
    };

    bool clickConfirm = IsKeyPressed(KEY_ENTER) ||
                        (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                         CheckCollisionPointRec(GetMousePosition(), btnConfirm));

    if (clickConfirm) {
        // Reset lỗi cũ trước
        form->errorSdt[0]  = '\0';
        form->errorCccd[0] = '\0';
        bool valid = true;

        // Bắt buộc có Họ tên
        if (form->hoTen.letterCount == 0) {
            valid = false;
        }

        // Validate SDT: bắt buộc đúng 10 chữ số (nếu đã nhập)
        if (form->sdt.letterCount >= 0 && form->sdt.letterCount != 10) {
            strcpy(form->errorSdt, "Số điện thoại phải có đúng 10 số!");
            valid = false;
        }

        // Validate CCCD: bắt buộc đúng 12 chữ số (nếu đã nhập)
        if (form->cccd.letterCount >= 0 && form->cccd.letterCount != 12) {
            strcpy(form->errorCccd, "Căn cước công dân phải có đúng 12 số!");
            valid = false;
        }

        // Chỉ lưu khi tất cả hợp lệ
        if (valid) {
            if (LuuThanhVienVaoFile(maThe, form)) {
                ThemBanDocVaoList(head, maThe, form);
                form->showSuccess  = true;
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

    // ── Màu chủ đạo (pastel sáng) ──────────────────────────────────────────
    // Nền thẻ: xanh sky pastel hơi đậm
    Color clrCard       = GetColor(0x003559ff);
    Color clrCardBorder = GetColor(0x8ecae6ff);
    Color clrAccent     = GetColor(0xb8e4f9ff); 
    // Chữ pastel sáng
    Color clrText       = GetColor(0xe8f7ffff); 
    Color clrLabel      = GetColor(0xcceeffff); 
    Color clrInput      = GetColor(0x1a3a4aff); 
    Color clrCursor     = GetColor(0x2196f3ff); 
    Color clrError      = GetColor(0xff6b8aff); 
    Color clrBtnNormal  = GetColor(0x4fc3f7ff); 
    Color clrBtnHover   = GetColor(0x0288d1ff); 
    Color clrFocusBorder= GetColor(0x81d4faff); 
    // ────────────────────────────────────────────────────────────────────────

    // ── Nền động (particle) ─────────────────────────────────────────────────
    ClearBackground(AnimatedBackground());
    DrawBackgroundParticles();
    // ────────────────────────────────────────────────────────────────────────

    // ── Nút Quay Lại ────────────────────────────────────────────────────────
    Rectangle btnBack = { 20, 20, 130, 40 };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    Color btnBackColor  = isHoverBack ? clrBtnHover  : Fade(clrAccent, 0.25f);
    Color btnBackText   = isHoverBack ? clrText      : clrText;
    DrawRectangleRounded(btnBack, 0.3f, 10, btnBackColor);
    DrawRectangleRoundedLines(btnBack, 0.3f, 10, clrCardBorder);
    DrawTextEx(font, "< Quay lại", (Vector2){btnBack.x + 15, btnBack.y + 10}, 20, 1, btnBackText);
    // ────────────────────────────────────────────────────────────────────────

    Rectangle card = { (screenW - 650*scale)/2, (screenH - 420*scale)/2, 650*scale, 420*scale };

    // ── Thẻ ─────────────────────────────────────────────────────────────────
    // Bóng mềm
    DrawRectangleRounded(
        (Rectangle){card.x + 6*scale, card.y + 8*scale, card.width, card.height},
        0.1f, 10, Fade(BLACK, 0.25f));
    // Nền thẻ
    DrawRectangleRounded(card, 0.1f, 10, clrCard);
    // Dải tiêu đề đậm hơn (top bar)
    Rectangle topBar = { card.x, card.y, card.width, 70*scale };
    DrawRectangleRounded(topBar, 0.1f, 10, Fade(BLACK, 0.01f));
    // Viền thẻ
    DrawRectangleRoundedLines(card, 0.1f, 10, clrCardBorder);
    // ────────────────────────────────────────────────────────────────────────

    // ── Avatar ──────────────────────────────────────────────────────────────
    Rectangle avatarBox = { card.x + 45*scale, card.y + 80*scale, 180*scale, 230*scale };
    DrawRectangleRounded(avatarBox, 0.05f, 5, Fade(WHITE, 0.15f));
    DrawRectangleRoundedLines(avatarBox, 0.05f, 5, clrAccent);
    DrawCircle(avatarBox.x + avatarBox.width/2, avatarBox.y + 80*scale,
               40*scale, Fade(clrAccent, 0.5f));
    DrawEllipse(avatarBox.x + avatarBox.width/2, avatarBox.y + 180*scale,
                60*scale, 45*scale, Fade(clrAccent, 0.5f));
    // ────────────────────────────────────────────────────────────────────────

    // ── Tiêu đề & ID ────────────────────────────────────────────────────────
    DrawTextEx(font, "HoanHoang_DUT library",
               (Vector2){card.x + 260*scale, card.y + 18*scale}, 28*scale, 1, clrText);
    // Đường kẻ ngang dưới tiêu đề
    DrawTextEx(font, TextFormat("ID: %s", maThe),
               (Vector2){card.x + 50*scale, card.y + 345*scale}, 22*scale, 1, clrAccent);
    // ────────────────────────────────────────────────────────────────────────

    // ── Các ô nhập ──────────────────────────────────────────────────────────
    const char*   labels[] = { "Họ và tên:", "Số điện thoại:", "Căn cước công dân:", "Hạn sử dụng:" };
    InputBox_BD*  boxes[]  = { &form->hoTen, &form->sdt, &form->cccd, &form->hanSD };

    for (int i = 0; i < 4; i++) {
        // Label
        DrawTextEx(font, labels[i],
                   (Vector2){boxes[i]->rec.x, boxes[i]->rec.y - 22*scale},
                   16*scale, 1, clrLabel);

        // Viền ô: đỏ hồng nếu lỗi, xanh nếu focus, mờ nếu bình thường
        bool hasError = (i == 1 && form->errorSdt[0]  != '\0') ||
                        (i == 2 && form->errorCccd[0] != '\0');
        Color borderColor = hasError          ? clrError      :
                            boxes[i]->isFocused ? clrFocusBorder : Fade(WHITE, 0.3f);

        // Nền ô: trắng mờ
        DrawRectangleRounded(boxes[i]->rec, 0.2f, 10, Fade(WHITE, 0.18f));
        DrawRectangleRoundedLines(boxes[i]->rec, 0.2f, 10, borderColor);

        // Text trong ô
        float fontSize = 20 * scale;
        Vector2 textSize = MeasureTextEx(font, boxes[i]->text, fontSize, 1);
        float textOffsetX = (textSize.x > (boxes[i]->rec.width - 20))
                            ? (textSize.x - (boxes[i]->rec.width - 20)) : 0;

        BeginScissorMode((int)boxes[i]->rec.x + 5, (int)boxes[i]->rec.y,
                         (int)boxes[i]->rec.width - 10, (int)boxes[i]->rec.height);
            Vector2 textPos = {
                boxes[i]->rec.x + 10 - textOffsetX,
                boxes[i]->rec.y + (boxes[i]->rec.height - fontSize) / 2
            };
            DrawTextEx(font, boxes[i]->text, textPos, fontSize, 1, clrText);

            // Con trỏ nhấp nháy
            if (boxes[i]->isFocused && (((int)(GetTime() * 1.5f)) % 2 == 0)) {
                DrawRectangleV(
                    (Vector2){ textPos.x + textSize.x + 2, textPos.y + 2 },
                    (Vector2){ 2, fontSize - 4 },
                    clrCursor
                );
            }
        EndScissorMode();

        // Thông báo lỗi dưới ô
        if (i == 1 && form->errorSdt[0] != '\0') {
            DrawTextEx(font, form->errorSdt,
                       (Vector2){ boxes[i]->rec.x, boxes[i]->rec.y + boxes[i]->rec.height + 3*scale },
                       13*scale, 1, clrError);
        }
        if (i == 2 && form->errorCccd[0] != '\0') {
            DrawTextEx(font, form->errorCccd,
                       (Vector2){ boxes[i]->rec.x, boxes[i]->rec.y + boxes[i]->rec.height + 3*scale },
                       13*scale, 1, clrError);
        }
    }
    // ────────────────────────────────────────────────────────────────────────

    // ── Nút XÁC NHẬN ────────────────────────────────────────────────────────
    Rectangle btnConfirm = {
        (screenW - 190*scale) / 2,
        card.y + card.height + 28 * scale,
        190*scale, 50*scale
    };
    bool isHover = CheckCollisionPointRec(GetMousePosition(), btnConfirm);
    // Bóng nút
    DrawRectangleRounded(
        (Rectangle){btnConfirm.x + 3, btnConfirm.y + 4, btnConfirm.width, btnConfirm.height},
        0.4f, 10, Fade(BLACK, 0.2f));
    DrawRectangleRounded(btnConfirm, 0.4f, 10, isHover ? clrBtnHover : clrBtnNormal);
    DrawTextEx(font, "XÁC NHẬN",
               (Vector2){
                   btnConfirm.x + (btnConfirm.width  - MeasureTextEx(font, "XÁC NHẬN", 20*scale, 1).x) / 2,
                   btnConfirm.y + (btnConfirm.height - 20*scale) / 2
               },
               20*scale, 1, WHITE);
    // ────────────────────────────────────────────────────────────────────────

    // Banner thành công
    if (form->showSuccess) {
        DrawSuccessMessage(font, (Texture2D){0}, form->successTimer);
    }
}

void DrawSuccessMessage(Font font, Texture2D background2, float currentTimer) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

    if (background2.id != 0) {
        DrawTexturePro(background2,
                       (Rectangle){0, 0, (float)background2.width, (float)background2.height},
                       (Rectangle){0, 0, screenW, screenH},
                       (Vector2){0, 0}, 0, WHITE);
    }
    DrawRectangle(0, 0, (int)screenW, (int)screenH, Fade(BLACK, 0.55f));

    // Panel
    Rectangle panel = { screenW/2 - 250, screenH/2 - 150, 500, 300 };
    DrawRectangleRounded(panel, 0.12f, 10, GetColor(0x1e3a4aff));        // nền xanh đậm
    DrawRectangleRoundedLines(panel, 0.12f, 10, GetColor(0x8ecae6ff));   // viền sky pastel

    // Icon check
    DrawCircle((int)screenW/2, (int)screenH/2 - 60, 45, GetColor(0x4fc3f7ff)); // xanh sky sáng
    DrawTextEx(font, "✓", (Vector2){screenW/2 - 18, screenH/2 - 88}, 62, 1, WHITE);

    // Thông báo
    const char* msg = "TẠO THẺ THÀNH CÔNG!";
    Vector2 msgSize = MeasureTextEx(font, msg, 30, 1);
    DrawTextEx(font, msg,
               (Vector2){(screenW - msgSize.x)/2, screenH/2 + 18},
               30, 1, GetColor(0xe8f7ffff));   // trắng xanh pastel

    // Progress bar
    float progress = currentTimer / 3.0f;
    DrawRectangleRounded(
        (Rectangle){panel.x + 50, panel.y + panel.height - 46, 400, 10},
        0.5f, 6, Fade(WHITE, 0.15f));
    DrawRectangleRounded(
        (Rectangle){panel.x + 50, panel.y + panel.height - 46, 400 * (1.0f - progress), 10},
        0.5f, 6, GetColor(0x4fc3f7ff));
}

bool LuuThanhVienVaoFile(char *maThe, Nhap *form) {
    FILE *f = fopen("data/Phieumuon/User.txt", "a");
    if (f == NULL) return false;

    // Trim trailing spaces
    for (int i = strlen(form->hoTen.text)-1; i >= 0 && form->hoTen.text[i] == ' '; i--) form->hoTen.text[i] = '\0';
    for (int i = strlen(form->sdt.text)-1;   i >= 0 && form->sdt.text[i]   == ' '; i--) form->sdt.text[i]   = '\0';
    for (int i = strlen(form->cccd.text)-1;  i >= 0 && form->cccd.text[i]  == ' '; i--) form->cccd.text[i]  = '\0';
    for (int i = strlen(form->hanSD.text)-1; i >= 0 && form->hanSD.text[i] == ' '; i--) form->hanSD.text[i] = '\0';

    fprintf(f, "%-*s | %-*s | %-*s | %-*s | %-*s\n",
        UTF8Width(maThe,             12), maThe,
        UTF8Width(form->hoTen.text,  35), form->hoTen.text,
        UTF8Width(form->sdt.text,    15), form->sdt.text,
        UTF8Width(form->cccd.text,   15), form->cccd.text,
        UTF8Width(form->hanSD.text,  12), form->hanSD.text);

    fclose(f);
    return true;
}

void ThemBanDocVaoList(BanDoc **head, char *maThe, Nhap *form) {
    BanDoc *newNode = (BanDoc*)malloc(sizeof(BanDoc));
    if (newNode) {
        strcpy(newNode->maThe, maThe);
        strcpy(newNode->hoTen, form->hoTen.text);
        strcpy(newNode->sdt,   form->sdt.text);
        strcpy(newNode->cccd,  form->cccd.text);
        strcpy(newNode->hanSD, form->hanSD.text);
        newNode->next = *head;
        *head = newNode;
    }
}

void SinhMaTheTuDong(int currentCount, char *maThe) {
    sprintf(maThe, "%08d", currentCount + 1);
}

void FreeMemberList(BanDoc *head) {
    while (head) { BanDoc *t = head; head = head->next; free(t); }
}

void DrawTheBanDoc_TimKiem(BanDoc *the, Font font, float toa_do_x, float toa_do_y) {
    if (the == NULL) return;

    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    // ── Bảng màu đồng bộ với DrawLibraryCard ───────────────────────────────
    Color clrCard       = GetColor(0x5ba8d0ff);
    Color clrCardBorder = GetColor(0x8ecae6ff);
    Color clrAccent     = GetColor(0xb8e4f9ff);
    Color clrText       = GetColor(0xe8f7ffff);
    Color clrLabel      = GetColor(0xcceeffff);
    // ────────────────────────────────────────────────────────────────────────

    Rectangle card = { toa_do_x, toa_do_y, 650*scale, 420*scale };

    // Bóng & nền thẻ
    DrawRectangleRounded(
        (Rectangle){card.x + 6*scale, card.y + 8*scale, card.width, card.height},
        0.1f, 10, Fade(BLACK, 0.25f));
    DrawRectangleRounded(card, 0.1f, 10, clrCard);
    // Dải top bar
    DrawRectangleRounded(
        (Rectangle){card.x, card.y, card.width, 70*scale},
        0.1f, 10, Fade(BLACK, 0.01f));
    DrawRectangleRoundedLines(card, 0.1f, 10, clrCardBorder);

    // Avatar
    Rectangle avatarBox = { card.x + 45*scale, card.y + 80*scale, 180*scale, 230*scale };
    DrawRectangleRounded(avatarBox, 0.05f, 5, Fade(WHITE, 0.15f));
    DrawRectangleRoundedLines(avatarBox, 0.05f, 5, clrAccent);
    DrawCircle(avatarBox.x + avatarBox.width/2, avatarBox.y + 80*scale,
               40*scale, Fade(clrAccent, 0.5f));
    DrawEllipse(avatarBox.x + avatarBox.width/2, avatarBox.y + 180*scale,
                60*scale, 45*scale, Fade(clrAccent, 0.5f));

    // Tiêu đề & ID
    DrawTextEx(font, "HoanHoang_DUT library",
               (Vector2){card.x + 260*scale, card.y + 18*scale}, 28*scale, 1, clrText);
    DrawTextEx(font, TextFormat("ID: %s", the->maThe),
               (Vector2){card.x + 50*scale, card.y + 345*scale}, 22*scale, 1, clrAccent);

    const char* labels[] = { "Họ và tên:", "Số điện thoại:", "Căn cước công dân:", "Hạn sử dụng:" };
    const char* values[] = { the->hoTen, the->sdt, the->cccd, the->hanSD };

    float textX   = card.x + 270 * scale;
    float startY  = card.y + 95  * scale;
    float boxW    = 340 * scale;
    float boxH    = 42  * scale;
    float spacing = 80.0f * scale;

    for (int i = 0; i < 4; i++) {
        float currentY = startY + (i * spacing);

        DrawTextEx(font, labels[i],
                   (Vector2){textX, currentY - 22*scale}, 16*scale, 1, clrLabel);

        Rectangle textBox = { textX, currentY, boxW, boxH };
        DrawRectangleRounded(textBox, 0.2f, 10, Fade(WHITE, 0.18f));
        DrawRectangleRoundedLines(textBox, 0.2f, 10, Fade(WHITE, 0.3f));

        BeginScissorMode((int)textBox.x + 5, (int)textBox.y,
                         (int)textBox.width - 10, (int)textBox.height);
            float fontSize = 20 * scale;
            Vector2 textPos = { textBox.x + 10, textBox.y + (textBox.height - fontSize)/2 };
            DrawTextEx(font, values[i], textPos, fontSize, 1, clrText);
        EndScissorMode();
    }
}

void LayMaTheTiepTheo(BanDoc *head, char *maThe) {
    int maxID = 0;
    BanDoc *temp = head;
    while (temp != NULL) {
        int currentID = atoi(temp->maThe);
        if (currentID > maxID) maxID = currentID;
        temp = temp->next;
    }
    sprintf(maThe, "%08d", maxID + 1);
}