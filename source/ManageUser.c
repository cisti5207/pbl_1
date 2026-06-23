#include "ManageUser.h"
#include "menu.h" 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libmanage.h>
#include <time.h>

// ============================================================
// Cờ lỗi hạn sử dụng (static nội bộ)
// ============================================================
static bool s_hanSDLoi    = false;  // true = ngày không hợp lệ hoặc là quá khứ
static bool s_daFillHanSD = false;  // true = đã fill ngày mặc định 1 lần rồi, không fill lại

// ============================================================
// Tiện ích kiểm tra ngày
// ============================================================

// Kiểm tra định dạng dd/mm/yyyy và ngày tồn tại
static bool KiemTraNgayHopLe_BD(const char *ngay) {
    if (!ngay || ngay[0] == '\0') return false;
    int d = 0, m = 0, y = 0;
    if (sscanf(ngay, "%d/%d/%d", &d, &m, &y) != 3) return false;
    if (y < 1000 || y > 9999) return false;
    if (m < 1 || m > 12) return false;
    int ngayToiDa[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) ngayToiDa[2] = 29;
    if (d < 1 || d > ngayToiDa[m]) return false;
    char ef[12], es1[12], es2[12], es3[12];
    snprintf(ef,  sizeof(ef),  "%02d/%02d/%04d", d, m, y);
    snprintf(es1, sizeof(es1), "%d/%02d/%04d",   d, m, y);
    snprintf(es2, sizeof(es2), "%02d/%d/%04d",   d, m, y);
    snprintf(es3, sizeof(es3), "%d/%d/%04d",     d, m, y);
    return (strcmp(ngay, ef)  == 0 || strcmp(ngay, es1) == 0 ||
            strcmp(ngay, es2) == 0 || strcmp(ngay, es3) == 0);
}

// Hạn sử dụng phải là tương lai (> hôm nay)
// Trả về true nếu ngày > ngày hệ thống
static bool NgayLaTuongLai(const char *ngay) {
    if (!ngay || ngay[0] == '\0') return false;
    int d = 0, m = 0, y = 0;
    if (sscanf(ngay, "%d/%d/%d", &d, &m, &y) != 3) return false;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    int todayY = t->tm_year + 1900;
    int todayM = t->tm_mon + 1;
    int todayD = t->tm_mday;

    if (y > todayY) return true;
    if (y < todayY) return false;
    if (m > todayM) return true;
    if (m < todayM) return false;
    return d > todayD;   // phải SAU hôm nay (không bao gồm hôm nay)
}

// ============================================================
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
    form->errorHoTen[0] = '\0';
    form->errorSdt[0]   = '\0';
    form->errorCccd[0]  = '\0';

    // Reset cờ lỗi hạn sử dụng
    s_hanSDLoi    = false;
    s_daFillHanSD = false;  // cho phép fill lại khi mở form mới
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

// ============================================================
// Hàm tiện ích xóa 1 ký tự (hỗ trợ UTF-8) và reset lỗi
// ============================================================
static void DeleteOneChar(InputBox_BD *box, int fieldIndex, Nhap *form) {
    if (box->letterCount <= 0) return;
    // Với ô ASCII-only (SDT, CCCD, hạn SD) thì xóa thẳng 1 byte
    // Với ô UTF-8 (họ tên) thì lùi qua continuation bytes
    do { box->letterCount--; }
    while (box->letterCount > 0 &&
           (box->text[box->letterCount] & 0xC0) == 0x80);
    box->text[box->letterCount] = '\0';

    if (fieldIndex == 0) form->errorHoTen[0] = '\0';
    if (fieldIndex == 1) form->errorSdt[0]   = '\0';
    if (fieldIndex == 2) form->errorCccd[0]  = '\0';
    if (fieldIndex == 3) s_hanSDLoi = false;
}

void UpdateInputForm(Nhap *form, BanDoc **head, int *currentTotalUsers, char *maThe, int *currentState) {

    // 1. Tự động sinh ID khi vừa mở form
    if (maThe[0] == '\0') {
        LayMaTheTiepTheo(*head, maThe);
    }

    // Fill ngày hệ thống +1 năm 1 lần duy nhất khi mở form
    // Sau đó người dùng có thể xóa/sửa thoải mái, không bị fill lại
    if (!s_daFillHanSD) {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        t->tm_year += 1;
        mktime(t);
        snprintf(form->hanSD.text, 256, "%02d/%02d/%04d",
                 t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
        form->hanSD.letterCount = (int)strlen(form->hanSD.text);
        s_hanSDLoi    = false;
        s_daFillHanSD = true;
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

    // 3. Logic Success
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

        // --- Backspace nhấn 1 lần ---
        int key = GetKeyPressed();
        while (key > 0) {
            if (key == KEY_BACKSPACE) {
                DeleteOneChar(boxes[i], i, form);
                // Reset counter để delay trước khi repeat
                boxes[i]->backspaceCounter = 0;
            }
            key = GetKeyPressed();
        }

        // --- Backspace giữ phím: delay 0.5s rồi repeat mỗi 0.05s ---
        if (IsKeyDown(KEY_BACKSPACE)) {
            boxes[i]->backspaceCounter += GetFrameTime();
            if (boxes[i]->backspaceCounter >= 0.5f) {
                DeleteOneChar(boxes[i], i, form);
                // Lùi lại 0.05s để tạo repeat rate ~20 ký tự/giây
                boxes[i]->backspaceCounter -= 0.05f;
            }
        } else {
            boxes[i]->backspaceCounter = 0;
        }

        // --- Nhận ký tự ---
        int charCode = GetCharPressed();
        while (charCode > 0) {

            if (i == 1 || i == 2) {
                // SDT (i=1) và CCCD (i=2): chỉ nhận chữ số 0-9
                int maxDigits = (i == 1) ? 10 : 12;
                if (charCode >= '0' && charCode <= '9') {
                    if (boxes[i]->letterCount < maxDigits) {
                        boxes[i]->text[boxes[i]->letterCount] = (char)charCode;
                        boxes[i]->letterCount++;
                        boxes[i]->text[boxes[i]->letterCount] = '\0';
                        if (i == 1) form->errorSdt[0]  = '\0';
                        if (i == 2) form->errorCccd[0] = '\0';
                    }
                }

            } else if (i == 3) {
                // Ô hạn sử dụng: chỉ nhận số và '/', giới hạn 10 ký tự (dd/mm/yyyy)
                if ((charCode >= '0' && charCode <= '9' || charCode == '/') &&
                    boxes[i]->letterCount < 10) {
                    boxes[i]->text[boxes[i]->letterCount++] = (char)charCode;
                    boxes[i]->text[boxes[i]->letterCount]   = '\0';

                    // Chỉ báo lỗi khi đã gõ đủ 10 ký tự (dd/mm/yyyy)
                    // Tránh báo lỗi khi đang gõ dở
                    if (boxes[i]->letterCount == 10) {
                        if (KiemTraNgayHopLe_BD(boxes[i]->text) && !NgayLaTuongLai(boxes[i]->text))
                            s_hanSDLoi = true;
                        else
                            s_hanSDLoi = false;
                    } else {
                        s_hanSDLoi = false;  // Đang gõ dở, chưa báo lỗi
                    }
                }

            } else {
                // Ô Họ tên (i=0)
                bool isDigit = (charCode >= '0' && charCode <= '9');
                if (i == 0 && isDigit) {
                    strcpy(form->errorHoTen, "Họ và tên không được chứa số!");
                } else if ((charCode >= 32) && (boxes[i]->letterCount < 250)) {
                    int byteSize = 0;
                    const char* utf8Char = CodepointToUTF8(charCode, &byteSize);
                    if (boxes[i]->letterCount + byteSize < 255) {
                        for (int j = 0; j < byteSize; j++) {
                            boxes[i]->text[boxes[i]->letterCount] = utf8Char[j];
                            boxes[i]->letterCount++;
                        }
                        boxes[i]->text[boxes[i]->letterCount] = '\0';
                        if (i == 0) form->errorHoTen[0] = '\0';
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
        form->errorHoTen[0] = '\0';
        form->errorSdt[0]   = '\0';
        form->errorCccd[0]  = '\0';
        bool valid = true;

        // Bắt buộc có Họ tên
        if (form->hoTen.letterCount == 0) {
            valid = false;
        }

        // Validate SDT: đúng 10 chữ số
        if (form->sdt.letterCount != 10) {
            strcpy(form->errorSdt, "Số điện thoại phải có đúng 10 số!");
            valid = false;
        }

        // Validate CCCD: đúng 12 chữ số
        if (form->cccd.letterCount != 12) {
            strcpy(form->errorCccd, "Căn cước công dân phải có đúng 12 số!");
            valid = false;
        }

        // Validate hạn sử dụng: phải hợp lệ và là tương lai
        bool hanSDHopLe = KiemTraNgayHopLe_BD(form->hanSD.text) &&
                          NgayLaTuongLai(form->hanSD.text);
        if (!hanSDHopLe) {
            s_hanSDLoi = true;
            valid = false;
        }

        if (valid) {
            if (LuuThanhVienVaoFile(maThe, form)) {
                ThemBanDocVaoList(head, maThe, form);
                form->showSuccess  = true;
                form->successTimer = 3.0f;
            }
        }
    }
}

// ============================================================
// DrawLibraryCard
// ============================================================
void DrawLibraryCard(Nhap *form, Texture2D icons[], char *maThe, Font font) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    Color clrCard        = GetColor(0x003559ff);
    Color clrCardBorder  = GetColor(0x8ecae6ff);
    Color clrAccent      = GetColor(0xb8e4f9ff);
    Color clrText        = GetColor(0xe8f7ffff);
    Color clrLabel       = GetColor(0xcceeffff);
    Color clrError       = GetColor(0xff6b8aff);
    Color clrBtnNormal   = GetColor(0x4fc3f7ff);
    Color clrBtnHover    = GetColor(0x0288d1ff);
    Color clrFocusBorder = GetColor(0x81d4faff);

    ClearBackground(AnimatedBackground());
    DrawBackgroundParticles();

    // ── Nút Quay Lại ────────────────────────────────────────
    Rectangle btnBack = { 20, 20, 130, 40 };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    DrawRectangleRounded(btnBack, 0.3f, 10, isHoverBack ? clrBtnHover : Fade(clrAccent, 0.25f));
    DrawRectangleRoundedLines(btnBack, 0.3f, 10, clrCardBorder);
    DrawTextEx(font, "< Quay lại", (Vector2){btnBack.x + 15, btnBack.y + 10}, 20, 1, clrText);

    Rectangle card = { (screenW - 650*scale)/2, (screenH - 420*scale)/2, 650*scale, 420*scale };

    // ── Thẻ ─────────────────────────────────────────────────
    DrawRectangleRounded(
        (Rectangle){card.x + 6*scale, card.y + 8*scale, card.width, card.height},
        0.1f, 10, Fade(BLACK, 0.25f));
    DrawRectangleRounded(card, 0.1f, 10, clrCard);
    DrawRectangleRounded(
        (Rectangle){card.x, card.y, card.width, 70*scale},
        0.1f, 10, Fade(BLACK, 0.01f));
    DrawRectangleRoundedLines(card, 0.1f, 10, clrCardBorder);

    // ── Avatar ──────────────────────────────────────────────
    Rectangle avatarBox = { card.x + 45*scale, card.y + 80*scale, 180*scale, 230*scale };
    DrawRectangleRounded(avatarBox, 0.05f, 5, Fade(WHITE, 0.15f));
    DrawRectangleRoundedLines(avatarBox, 0.05f, 5, clrAccent);
    DrawCircle(avatarBox.x + avatarBox.width/2, avatarBox.y + 80*scale,
               40*scale, Fade(clrAccent, 0.5f));
    DrawEllipse(avatarBox.x + avatarBox.width/2, avatarBox.y + 180*scale,
                60*scale, 45*scale, Fade(clrAccent, 0.5f));

    // ── Tiêu đề ─────────────────────────────────────────────
    DrawTextEx(font, "HoanHoang_DUT library",
               (Vector2){card.x + 260*scale, card.y + 18*scale}, 28*scale, 1, clrText);

    // ── ID: to hơn, căn giữa avatar box, ngang với ô Hạn sử dụng ──
    char idStr[32];
    snprintf(idStr, sizeof(idStr), "ID: %s", maThe);
    float idFontSize = 26*scale;
    Vector2 idSz = MeasureTextEx(font, idStr, idFontSize, 1);
    float idX = avatarBox.x + (avatarBox.width - idSz.x) / 2.0f;
    // Căn ngang với ô Hạn sử dụng (ô thứ 4, index 3)
    float inputY  = card.y + 95*scale;
    float spacing = 78*scale;
    float boxH    = 42*scale;
    float idY = inputY + (3 * spacing) + (boxH - idSz.y) / 2.0f;
    DrawTextEx(font, idStr, (Vector2){idX, idY}, idFontSize, 1, clrAccent);

    // ── Các ô nhập ──────────────────────────────────────────
    const char*   labels[] = { "Họ và tên:", "Số điện thoại:", "Căn cước công dân:", "Hạn sử dụng:" };
    InputBox_BD*  boxes[]  = { &form->hoTen, &form->sdt, &form->cccd, &form->hanSD };

    for (int i = 0; i < 4; i++) {
        DrawTextEx(font, labels[i],
                   (Vector2){boxes[i]->rec.x, boxes[i]->rec.y - 22*scale},
                   16*scale, 1, clrLabel);

        // Viền đỏ nếu lỗi (bao gồm cả hạn SD)
        bool hasError = (i == 0 && form->errorHoTen[0] != '\0') ||
                        (i == 1 && form->errorSdt[0]   != '\0') ||
                        (i == 2 && form->errorCccd[0]  != '\0') ||
                        (i == 3 && s_hanSDLoi);
        Color borderColor = hasError            ? clrError       :
                            boxes[i]->isFocused ? clrFocusBorder : Fade(WHITE, 0.3f);

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

            if (boxes[i]->isFocused && (((int)(GetTime() * 1.5f)) % 2 == 0)) {
                DrawRectangleV(
                    (Vector2){ textPos.x + textSize.x + 2, textPos.y + 2 },
                    (Vector2){ 2, fontSize - 4 },
                    GetColor(0x2196f3ff)
                );
            }
        EndScissorMode();

        // Thông báo lỗi dưới ô
        const char *errMsg = NULL;
        if (i == 0 && form->errorHoTen[0] != '\0') errMsg = form->errorHoTen;
        if (i == 1 && form->errorSdt[0]   != '\0') errMsg = form->errorSdt;
        if (i == 2 && form->errorCccd[0]  != '\0') errMsg = form->errorCccd;
        if (i == 3 && s_hanSDLoi) {
            // Phân biệt: sai format vs ngày quá khứ
            errMsg = KiemTraNgayHopLe_BD(form->hanSD.text)
                     ? "! Chỉ được nhập ngày tương lai"
                     : "! Hạn sử dụng không hợp lệ (dd/mm/yyyy)";
        }

        if (errMsg) {
            DrawTextEx(font, errMsg,
                       (Vector2){ boxes[i]->rec.x, boxes[i]->rec.y + boxes[i]->rec.height + 3*scale },
                       13*scale, 1, clrError);
        }
    }

    // ── Nút XÁC NHẬN ────────────────────────────────────────
    Rectangle btnConfirm = {
        (screenW - 190*scale) / 2,
        card.y + card.height + 28 * scale,
        190*scale, 50*scale
    };
    bool isHover = CheckCollisionPointRec(GetMousePosition(), btnConfirm);
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

    // Banner thành công
    if (form->showSuccess) {
        DrawSuccessMessage(font, (Texture2D){0}, form->successTimer);
    }
}

// ============================================================
// DrawSuccessMessage
// ============================================================
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

    Rectangle panel = { screenW/2 - 250, screenH/2 - 150, 500, 300 };
    DrawRectangleRounded(panel, 0.12f, 10, GetColor(0x1e3a4aff));
    DrawRectangleRoundedLines(panel, 0.12f, 10, GetColor(0x8ecae6ff));

    DrawCircle((int)screenW/2, (int)screenH/2 - 60, 45, GetColor(0x4fc3f7ff));
    Vector2 vSz = MeasureTextEx(font, "V", 58, 1);
    DrawTextEx(font, "V", (Vector2){screenW/2 - vSz.x/2, screenH/2 - 60 - vSz.y/2}, 58, 1, WHITE);

    const char* msg = "TẠO THẺ THÀNH CÔNG!";
    Vector2 msgSize = MeasureTextEx(font, msg, 30, 1);
    DrawTextEx(font, msg,
               (Vector2){(screenW - msgSize.x)/2, screenH/2 + 18},
               30, 1, GetColor(0xe8f7ffff));

    float progress = currentTimer / 3.0f;
    DrawRectangleRounded(
        (Rectangle){panel.x + 50, panel.y + panel.height - 46, 400, 10},
        0.5f, 6, Fade(WHITE, 0.15f));
    DrawRectangleRounded(
        (Rectangle){panel.x + 50, panel.y + panel.height - 46, 400 * (1.0f - progress), 10},
        0.5f, 6, GetColor(0x4fc3f7ff));
}

// ============================================================
// Lưu & Thêm node
// ============================================================
bool LuuThanhVienVaoFile(char *maThe, Nhap *form) {
    FILE *f = fopen("data/Phieumuon/User.txt", "a");
    if (f == NULL) return false;

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
        newNode->next = NULL;
        if (*head == NULL) {
            *head = newNode;
        } else {
            BanDoc *temp = *head;
            while (temp->next != NULL) temp = temp->next;
            temp->next = newNode;
        }
    }
}

void SinhMaTheTuDong(int currentCount, char *maThe) {
    sprintf(maThe, "%08d", currentCount + 1);
}

void FreeMemberList(BanDoc *head) {
    while (head) { BanDoc *t = head; head = head->next; free(t); }
}

// ============================================================
// DrawTheBanDoc_TimKiem — hiển thị thẻ khi tìm kiếm
// ============================================================
void DrawTheBanDoc_TimKiem(BanDoc *the, Font font, float toa_do_x, float toa_do_y) {
    if (the == NULL) return;

    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    Color clrCard       = GetColor(0x5ba8d0ff);
    Color clrCardBorder = GetColor(0x8ecae6ff);
    Color clrAccent     = GetColor(0xb8e4f9ff);
    Color clrText       = GetColor(0xe8f7ffff);
    Color clrLabel      = GetColor(0xcceeffff);

    Rectangle card = { toa_do_x, toa_do_y, 650*scale, 420*scale };

    DrawRectangleRounded(
        (Rectangle){card.x + 6*scale, card.y + 8*scale, card.width, card.height},
        0.1f, 10, Fade(BLACK, 0.25f));
    DrawRectangleRounded(card, 0.1f, 10, clrCard);
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

    // Tiêu đề
    DrawTextEx(font, "HoanHoang_DUT library",
               (Vector2){card.x + 260*scale, card.y + 18*scale}, 28*scale, 1, clrText);

    // ── ID: to hơn, căn giữa avatar box, ngang với ô Hạn sử dụng ──
    char idStr[32];
    snprintf(idStr, sizeof(idStr), "ID: %s", the->maThe);
    float idFontSize = 26*scale;
    Vector2 idSz = MeasureTextEx(font, idStr, idFontSize, 1);
    float idX = avatarBox.x + (avatarBox.width - idSz.x) / 2.0f;
    // Căn ngang với ô Hạn sử dụng (ô thứ 4, index 3)
    float inputY  = card.y + 95*scale;
    float spacing = 80.0f*scale;   // DrawTheBanDoc dùng spacing = 80
    float boxH    = 42*scale;
    float idY = inputY + (3 * spacing) + (boxH - idSz.y) / 2.0f;
    DrawTextEx(font, idStr, (Vector2){idX, idY}, idFontSize, 1, clrAccent);

    // Các field
    const char* labels[] = { "Họ và tên:", "Số điện thoại:", "Căn cước công dân:", "Hạn sử dụng:" };
    const char* values[] = { the->hoTen, the->sdt, the->cccd, the->hanSD };

    float textX   = card.x + 270 * scale;
    float startY  = card.y + 95  * scale;
    float boxW    = 340 * scale;

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