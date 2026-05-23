#include "libmanage.h"
#include "Phieumuon.h"
#include <stdio.h>
#include "raylib.h"
#include <string.h>
#include <stdlib.h>
// Hàm để nhắc xem cái dấu nhấy nháy để viết đang nằm trong ô nhập liệu nào.
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
// Hàm này giúp di chuyển như kiểu kéo thả màn hình mà ko bị bể thẻ hay phiếu bên trong.
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
//Hàm sinh mã phiếu mượn 6 số
void SinhMaPM(int currentcount, char *mathe){ 
    sprintf(mathe, "PM%06d", currentcount + 1); 
}
void UpdateInputPM(FormPhieuMuon *Form, PhieuMuonNode**head, int *currentTotalUsers, char *mathe) {
    // Hàm này giúp người dùng sau khi đã xác nhận thì sẽ ko thể thao tác gây ảnh hưởng tới quá trình xác nhận
        if (Form->showsuccess) {
        Form->successtimer -= GetFrameTime();
        if (Form->successtimer <= 0) {
            Form->showsuccess = false;
            InitPhieumuon(Form);
            (*currentTotalUsers)++;
            SinhMaPM(*currentTotalUsers, mathe);
        }
        return; 
    }
    InputBox_PM *boxes[] = {&Form->mathe, &Form->matruyen,&Form->tentruyen, &Form->ngaymuon, &Form->ngaytra};
    // Hàm kiểm tra vị trí xem cái nhấp nháy có đang ở trong ô không
     if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousepoint = GetMousePosition();
        for (int i = 0; i < 5; i++) {
            if (CheckCollisionPointRec(mousepoint, boxes[i]->rec)) {
                for (int j = 0; j < 5; j++) boxes[j]->isfocused = false;
                boxes[i]->isfocused = true;
                break;
            }
        }
    }
    //Hàm tránh đè chữ khi gõ tiếng việt (do kí tự rác nó bị thừa trong hàng đợi)
    for (int i = 0; i < 5; i++) {
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
            //Hàm xóa đè backspace
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
                boxes[(i + 1) % 5]->isfocused = true;
                break;
            }
        }
    }
        }
        // Vẽ giao diện:
    void DrawPM( FormPhieuMuon *Form, Texture2D icons[], char *mathe, Font font) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    Rectangle cardleft = { (screenW - 800*scale)/2, (screenH - 550*scale)/2, 600*scale, 550*scale};
    Rectangle cardright = { (screenW - 800*scale)/2 + 600*scale, (screenH - 550*scale)/2, 200*scale, 550*scale};
        // Đổ bóng và pha màu cho nền của phiếu
        DrawRectangleRec((Rectangle){cardleft.x + 6*scale, cardleft.y + 6*scale, cardleft.width, cardleft.height}, Fade(BLACK, 0.2f));
        DrawRectangleRec((Rectangle){cardright.x + 6*scale, cardright.y + 6*scale, cardright.width, cardright.height}, Fade(BLACK, 0.2f));
        DrawRectangleGradientEx(cardleft, GetColor(0xFF8BC3FF), GetColor(0xF98BC4FF), GetColor(0xFE81AAFF), GetColor(0xFFA8C5FF));
        DrawRectangleGradientEx(cardright, GetColor(0xD6EDFFFF), GetColor(0xB5F4FEFF), GetColor(0xC2FAFFFF),GetColor(0xA4FBCFFF));
        //Vẽ tiêu đề cho phiếu mượn và IDPM
        DrawTextEx(font, "Phiếu mượn sách ", (Vector2){cardleft.x + 136*scale, cardright.y + 36*scale}, 36*scale, 1, MAROON);
        DrawTextEx(font, TextFormat("IDPM: %s", mathe), (Vector2){cardright.x + 10*scale, cardright.y + 480*scale}, 24*scale, 1, GetColor(0x1E96FCFF));
        
    const char* labels[] = {"Mã thẻ bạn đọc:", "Mã truyện:", "Tên truyện:", "Ngày mượn:", "Ngày trả thực tế:"};
    InputBox_PM* boxes[] = {&Form->mathe, &Form->matruyen, &Form->tentruyen, &Form->ngaymuon, &Form->ngaytra};
    // Thao tác UI và logic ô nhập liệu.
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

    fprintf(f, "%-12s | %-12s | %-6s | %-50s | %-12s | %-12s\n", maphieu,Form->mathe.text, Form->matruyen.text, Form->tentruyen.text, Form->ngaymuon.text, Form->ngaytra.text);
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
        newNode->next = *head;
        *head = newNode;
    }
}



     


