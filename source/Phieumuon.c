#include "libmanage.h"
#include "Phieumuon.h"
#include "menu.h" // Nhúng menu vào để dùng biến APP_MENU
#include <stdio.h>
#include "raylib.h"
#include <string.h>
#include <stdlib.h>

void TimTenTruyenTheoMa(const char *matruyen, char *tentruyen_out) {
    tentruyen_out[0] = '\0';

    FILE *f = fopen("data/ManageBooks/dataTruyen.txt", "r");
    if (f == NULL) return;

    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p != '|') continue;

        char ma[64] = {0}, maphu[256] = {0}, ten[256] = {0};
        if (sscanf(p, " | %63[^|] | %255[^|] | %255[^|]", ma, maphu, ten) == 3) {
            int len = strlen(ma);
            while (len > 0 && (ma[len-1] == ' ' || ma[len-1] == '\r' || ma[len-1] == '\n')) ma[--len] = '\0';
            char *pMa = ma;
            while (*pMa == ' ') pMa++;

            if (strcmp(pMa, matruyen) == 0) {
                len = strlen(ten);
                while (len > 0 && (ten[len-1] == ' ' || ten[len-1] == '\r' || ten[len-1] == '\n')) ten[--len] = '\0';
                char *pTen = ten;
                while (*pTen == ' ') pTen++;
                strcpy(tentruyen_out, pTen);
                break;
            }
        }
    }
    fclose(f);
}

int DemSoPhieuMuon(PhieuMuonNode *head) {
    int count = 0;
    PhieuMuonNode *temp = head;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }
    return count;
}

void InitPhieumuon(FormPhieuMuon *Form) {
    memset(Form->mathe.text, 0, 512);
    memset(Form->tentruyen.text, 0, 512);
    memset(Form->ngaymuon.text, 0, 512);
    memset(Form->ngaytra.text, 0, 512);
    memset(Form->matruyen.text, 0, 512);
    Form->mathe.lettercount = 0;
    Form->tentruyen.lettercount = 0;
    Form->ngaymuon.lettercount = 0;
    Form->ngaytra.lettercount = 0;
    Form->matruyen.lettercount = 0;
    Form->mathe.backspacecounter = 0;
    Form->tentruyen.backspacecounter = 0;
    Form->ngaymuon.backspacecounter = 0;
    Form->ngaytra.backspacecounter = 0;
    Form->matruyen.backspacecounter = 0;
    Form->mathe.isfocused = true; 
    Form->tentruyen.isfocused = false;
    Form->ngaymuon.isfocused = false;
    Form->ngaytra.isfocused = false;
    Form->matruyen.isfocused = false;
    Form->showsuccess = false;
    Form->successtimer = 0;
}

void UpdateVitri(FormPhieuMuon *Form) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    Rectangle card = { (screenW - 800*scale)/2, (screenH - 550*scale)/2, 800*scale, 550*scale };
    
    float inputX = card.x + 50 * scale;
    float inputY = card.y + 125 * scale; 
    float inputW = 450 * scale;
    float inputH = 50 * scale;
    float spacing = 82 * scale;
    
    Form->mathe.rec = (Rectangle){ inputX, inputY + (0 * spacing), inputW, inputH };
    Form->matruyen.rec = (Rectangle){ inputX, inputY + (1 * spacing), inputW, inputH };
    Form->tentruyen.rec   = (Rectangle){ inputX, inputY + (2 * spacing), inputW, inputH };
    Form->ngaymuon.rec  = (Rectangle){ inputX, inputY + (3 * spacing), inputW, inputH };
    Form->ngaytra.rec = (Rectangle){ inputX, inputY + (4 * spacing), inputW, inputH };
}

void SinhMaPM(int currentcount, char *mathe){ 
    sprintf(mathe, "PM%06d", currentcount + 1); 
}

// Đổi tên biến thứ 3 thành currentState cho dễ hiểu (Trong file .h cứ để currentTotalUsers ko sao cả)
void UpdateInputPM(FormPhieuMuon *Form, PhieuMuonNode **head, int *currentState, char *mathe) {
    
    // 1. TỰ ĐỘNG SINH MÃ PM NẾU CHƯA CÓ
    if (mathe[0] == '\0') {
        int tongPhieu = DemSoPhieuMuon(*head);
        SinhMaPM(tongPhieu, mathe); 
    }

    // 2. LOGIC NÚT QUAY LẠI TỰ CHỦ
    Rectangle btnBack = { 20, 20, 130, 40 };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    if (IsKeyPressed(KEY_ESCAPE) || (isHoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
        InitPhieumuon(Form);  // Dọn dẹp form
        mathe[0] = '\0';      // Reset mã
        *currentState = APP_MENU; // Quay về Menu an toàn
        return; 
    }

    // 3. LOGIC SUCCESS
    if (Form->showsuccess) {
        Form->successtimer -= GetFrameTime();
        if (Form->successtimer <= 0) {
            Form->showsuccess = false;
            InitPhieumuon(Form);
            
            // Sinh mã tiếp theo sau khi lưu thành công
            int tongPhieu = DemSoPhieuMuon(*head);
            SinhMaPM(tongPhieu, mathe);
        }
        return; 
    }
    
    InputBox_PM *boxes[] = {&Form->mathe, &Form->matruyen,&Form->tentruyen, &Form->ngaymuon, &Form->ngaytra};
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousepoint = GetMousePosition();
        for (int i = 0; i < 5; i++) {
            if (i == 2) continue; // tentruyen bi lock
            if (CheckCollisionPointRec(mousepoint, boxes[i]->rec)) {
                for (int j = 0; j < 5; j++) boxes[j]->isfocused = false;
                boxes[i]->isfocused = true;
                break;
            }
        }
    }
    
    for (int i = 0; i < 5; i++) {
        if (i == 2) continue; // tentruyen bi lock hoan toan
        if (boxes[i]->isfocused) {
            int key = GetKeyPressed();
            while (key > 0) {
                if (key == KEY_BACKSPACE) {
                    if (boxes[i]->lettercount > 0) {
                        do { 
                            boxes[i]->lettercount--; 
                        } while (boxes[i]->lettercount > 0 && (boxes[i]->text[boxes[i]->lettercount] & 0xC0) == 0x80);
                        boxes[i]->text[boxes[i]->lettercount] = '\0';
                    }
                }
                key = GetKeyPressed();
            }
            
            if (IsKeyDown(KEY_BACKSPACE)) {
                boxes[i]->backspacecounter += GetFrameTime();
                if (boxes[i]->backspacecounter >= 0.5f) { 
                    if (boxes[i]->lettercount > 0) {
                        do {
                             boxes[i]->lettercount--; 
                            } while (boxes[i]->lettercount > 0 && (boxes[i]->text[boxes[i]->lettercount] & 0xC0) == 0x80);
                        boxes[i]->text[boxes[i]->lettercount] = '\0';
                    }
                }
            } else boxes[i]->backspacecounter = 0;
            
            int charcode = GetCharPressed();
            while (charcode > 0) {
                if ((charcode >= 32) && (boxes[i]->lettercount < 250)) {
                    int byteSize = 0;
                    const char* utf8Char = CodepointToUTF8(charcode, &byteSize);
                    if (boxes[i]->lettercount + byteSize < 255) {
                        for (int j = 0; j < byteSize; j++) {
                            boxes[i]->text[boxes[i]->lettercount] = utf8Char[j];
                            boxes[i]->lettercount++;
                        }
                        boxes[i]->text[boxes[i]->lettercount] = '\0';
                    }
                }
                charcode = GetCharPressed();
            }
            
            if (IsKeyPressed(KEY_TAB)) {
                boxes[i]->isfocused = false;
                int next = (i == 1) ? 3 : (i + 1) % 5;
                if (next == 2) next = 3;
                boxes[next]->isfocused = true;
                break;
            }
        }
    }
    
    // 3.5. TU DONG DIEN TEN TRUYEN
    {
        char tentruyen_tam[512] = {0};
        if (Form->matruyen.lettercount > 0)
            TimTenTruyenTheoMa(Form->matruyen.text, tentruyen_tam);
        strncpy(Form->tentruyen.text, tentruyen_tam, 511);
        Form->tentruyen.text[511] = '\0';
        Form->tentruyen.lettercount = (int)strlen(Form->tentruyen.text);
        Form->tentruyen.isfocused = false;
    }

    // 4. XỬ LÝ NÚT XÁC NHẬN
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    Rectangle cardright = { (screenW - 800*scale)/2 + 600*scale, (screenH - 550*scale)/2, 200*scale, 550*scale};
    Rectangle Confirm = { (screenW - 800*scale)/2 + 630*scale, cardright.y + cardright.height + 30 * scale, 140*scale, 50*scale }; 
    
    if (IsKeyPressed(KEY_ENTER) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), Confirm))) {
        if (Form->mathe.lettercount > 0 && Form->matruyen.lettercount > 0) {
            if (LuuPhieuMuonVaoFile(mathe, Form)) {
                ThemPhieuMuonVaoList(head, mathe, Form); 
                Form->showsuccess = true;
                Form->successtimer = 3.0f;
            }
        }
    }
}

// Vẽ giao diện:
void DrawPM(FormPhieuMuon *Form, Texture2D icons[], char *mathe, Font font) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    // --- VẼ NÚT QUAY LẠI TẠI ĐÂY ---
    Rectangle btnBack = { 20, 20, 130, 40 };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    Color btnColor = isHoverBack ? MAROON : WHITE;
    Color textColor = isHoverBack ? WHITE : MAROON;
    DrawRectangleRounded(btnBack, 0.3f, 10, btnColor);
    DrawRectangleRoundedLines(btnBack, 0.3f, 10, MAROON);
    DrawTextEx(font, "< Quay lai", (Vector2){btnBack.x + 15, btnBack.y + 10}, 20, 1, textColor);
    // -------------------------------

    Rectangle cardleft = { (screenW - 800*scale)/2, (screenH - 550*scale)/2, 600*scale, 550*scale};
    Rectangle cardright = { (screenW - 800*scale)/2 + 600*scale, (screenH - 550*scale)/2, 200*scale, 550*scale};
    
    DrawRectangleRec((Rectangle){cardleft.x + 6*scale, cardleft.y + 6*scale, cardleft.width, cardleft.height}, Fade(BLACK, 0.2f));
    DrawRectangleRec((Rectangle){cardright.x + 6*scale, cardright.y + 6*scale, cardright.width, cardright.height}, Fade(BLACK, 0.2f));
    DrawRectangleGradientEx(cardleft, GetColor(0xFF8BC3FF), GetColor(0xF98BC4FF), GetColor(0xFE81AAFF), GetColor(0xFFA8C5FF));
    DrawRectangleGradientEx(cardright, GetColor(0xD6EDFFFF), GetColor(0xB5F4FEFF), GetColor(0xC2FAFFFF),GetColor(0xA4FBCFFF));
    
    DrawTextEx(font, "Phiếu mượn sách ", (Vector2){cardleft.x + 136*scale, cardright.y + 36*scale}, 36*scale, 1, MAROON);
    DrawTextEx(font, TextFormat("IDPM: %s", mathe), (Vector2){cardright.x + 10*scale, cardright.y + 480*scale}, 24*scale, 1, GetColor(0x1E96FCFF));
    
    const char* labels[] = {"Mã thẻ bạn đọc:", "Mã truyện:", "Tên truyện:", "Ngày mượn:", "Ngày trả dự kiến:"};
    InputBox_PM* boxes[] = {&Form->mathe, &Form->matruyen, &Form->tentruyen, &Form->ngaymuon, &Form->ngaytra};
    
    for (int i = 0; i < 5; i++) {
        DrawTextEx(font, labels[i], (Vector2){boxes[i]->rec.x, boxes[i]->rec.y - 25*scale}, 18*scale, 1, MAROON);
        DrawRectangleRounded(boxes[i]->rec, 0.2f, 10, WHITE);
        DrawRectangleRoundedLines(boxes[i]->rec, 0.2f, 10, boxes[i]->isfocused ? MAROON : LIGHTGRAY);
        
        float fontSize = 20 * scale;
        Vector2 textSize = MeasureTextEx(font, boxes[i]->text, fontSize, 1);
        float textOffsetX = (textSize.x > (boxes[i]->rec.width - 20)) ? (textSize.x - (boxes[i]->rec.width - 20)) : 0;
        
        BeginScissorMode((int)boxes[i]->rec.x + 5, (int)boxes[i]->rec.y, (int)boxes[i]->rec.width - 10, (int)boxes[i]->rec.height);
            Vector2 textpos = { boxes[i]->rec.x + 10 - textOffsetX, boxes[i]->rec.y + (boxes[i]->rec.height - fontSize)/2 };
            DrawTextEx(font, boxes[i]->text, textpos, fontSize, 1, PINK);

            if (boxes[i]->isfocused && (((int)(GetTime() * 1.5f)) % 2 == 0)) {
                DrawRectangleV((Vector2){ textpos.x + textSize.x + 2, textpos.y + 2 }, (Vector2){ 2, fontSize - 4 }, MAROON);
            }
        EndScissorMode();
    }
    
    Rectangle Confirm = { (screenW - 800*scale)/2 + 630*scale, cardright.y + cardright.height + 30 * scale, 140*scale, 50*scale };
    bool ishover = CheckCollisionPointRec(GetMousePosition(), Confirm);
    DrawRectangleRounded(Confirm, 0.3f, 10, ishover ? MAROON : PINK);
    DrawTextEx(font, "XÁC NHẬN", (Vector2){Confirm.x + (Confirm.width - MeasureTextEx(font, "XÁC NHẬN", 20*scale, 1).x)/2, Confirm.y + (Confirm.height - 20*scale)/2}, 20*scale, 1, WHITE);

    // GỌI THÔNG BÁO THÀNH CÔNG NGAY BÊN TRONG HÀM VẼ (giống ManageUser.c)
    if (Form->showsuccess) {
        DrawSuccess(font, (Texture2D){0}, Form->successtimer);
    }
}

void DrawSuccess(Font font, Texture2D background2, float currenttimer) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

    if (background2.id != 0) {
        DrawTexturePro(background2, (Rectangle){0, 0, (float)background2.width, (float)background2.height}, (Rectangle){0, 0, screenW, screenH}, (Vector2){0, 0}, 0, WHITE);
    }
    DrawRectangle(0, 0, (int)screenW, (int)screenH, Fade(BLACK, 0.4f));
    
    Rectangle panel = { screenW/2 - 250, screenH/2 - 150, 500, 300 };
    DrawRectangleGradientEx(panel, GetColor(0xA2D6F9FF), GetColor(0x82DDF0FF), GetColor(0xF6839CFF), GetColor(0xFF85D1FF));
    DrawCircle((int)screenW/2, (int)screenH/2 - 60, 45, PINK);
    DrawTextEx(font, "V", (Vector2){screenW/2 - 15, screenH/2 - 85}, 60, 1, WHITE);

    const char* msg = "TẠO PHIẾU MƯỢN THÀNH CÔNG!";
    Vector2 msgSize = MeasureTextEx(font, msg, 32, 1);
    DrawTextEx(font, msg, (Vector2){(screenW - msgSize.x)/2, screenH/2 + 20}, 32, 1, GetColor(0x5171A5FF));
    float progress = currenttimer / 3.0f; 
    DrawRectangle((int)panel.x + 50, (int)panel.y + panel.height - 50, 400, 12, LIGHTGRAY);
    DrawRectangle((int)panel.x + 50, (int)panel.y + panel.height - 50, (int)(400 * (1.0f - progress)), 12, GetColor(0x85C7F2FF));
}

bool LuuPhieuMuonVaoFile(char *maphieu, FormPhieuMuon *Form) {
    FILE *f = fopen("data/Phieumuon/LSPhieumuon.txt", "a"); 
    if (f == NULL) f = fopen("data/Phieumuon/LSPhieumuon.txt", "a");
    
    if (f == NULL) {
        return false;
    }
    for (int i = strlen(Form->mathe.text)-1; i >= 0 && Form->mathe.text[i] == ' '; i--) Form->mathe.text[i] = '\0';
    for (int i = strlen(Form->matruyen.text)-1; i >= 0 && Form->matruyen.text[i] == ' '; i--) Form->matruyen.text[i] = '\0';
    for (int i = strlen(Form->tentruyen.text)-1; i >= 0 && Form->tentruyen.text[i] == ' '; i--) Form->tentruyen.text[i] = '\0';
    for (int i = strlen(Form->ngaymuon.text)-1; i >= 0 && Form->ngaymuon.text[i] == ' '; i--) Form->ngaymuon.text[i] = '\0';
    for (int i = strlen(Form->ngaytra.text)-1; i >= 0 && Form->ngaytra.text[i] == ' '; i--) Form->ngaytra.text[i] = '\0';

    fprintf(f, "%-*s | %-*s | %-*s | %-*s | %-*s | %-*s | %d\n",
    UTF8Width(maphieu, 12),             maphieu,
    UTF8Width(Form->mathe.text, 12),    Form->mathe.text,
    UTF8Width(Form->matruyen.text, 19), Form->matruyen.text,
    UTF8Width(Form->tentruyen.text, 50),Form->tentruyen.text,
    UTF8Width(Form->ngaymuon.text, 19), Form->ngaymuon.text,
    UTF8Width(Form->ngaytra.text, 19),  Form->ngaytra.text,
    0);
    fflush(f); 
    fclose(f);
    return true;
}
 
void ThemPhieuMuonVaoList(PhieuMuonNode **head, char *maPM, FormPhieuMuon *Form) {
    PhieuMuonNode *newNode = (PhieuMuonNode*)malloc(sizeof(PhieuMuonNode));
    if (newNode) {
        strcpy(newNode->maPM, maPM);
        strcpy(newNode->mathe, Form->mathe.text);
        strcpy(newNode->matruyen, Form->matruyen.text);
        strcpy(newNode->tentruyen, Form->tentruyen.text);
        strcpy(newNode->ngaymuon, Form->ngaymuon.text);
        strcpy(newNode->ngaytra, Form->ngaytra.text);
        newNode->trangthai = 0;
        newNode->next = *head;
        *head = newNode;
    }
}