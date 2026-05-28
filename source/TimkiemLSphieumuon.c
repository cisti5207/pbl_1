#include "TimkiemLSphieumuon.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Timkiemthethuvien.h"
#include "libmanage.h"

void InitPhieuMuonMang(PhieuMuonMang *map) {
    for (int i = 0; i < PM_HASH_SIZE; i++) {
        map->mang[i] = NULL;
    }
}

int HashMaThe(const char *mathe) {
    int id = atoi(mathe);
    return id % PM_HASH_SIZE;
}

void InsertPhieuMuonMang(PhieuMuonMang *map, PhieuMuonNode *pmNode) {
    if (pmNode == NULL) return;
    int index = HashMaThe(pmNode->mathe);
    HashNodePM *newNode = (HashNodePM*)malloc(sizeof(HashNodePM));
    newNode->LSphieumuon = pmNode;
    newNode->next = map->mang[index];
    map->mang[index] = newNode;
}

void BuildMap(PhieuMuonMang *map, PhieuMuonNode *head) {
    InitPhieuMuonMang(map); 
    PhieuMuonNode *hientai = head;
    while (hientai != NULL) {
        InsertPhieuMuonMang(map, hientai);
        hientai = hientai->next;
    }
}

HashNodePM* Layphantu(PhieuMuonMang *map, const char *mathe) {
    int index = HashMaThe(mathe);
    return map->mang[index]; 
}

void DocDuLieuPhieuMuon(const char *filepath, PhieuMuonNode **head) {
    FILE *f = fopen(filepath, "r");
    if (f == NULL) {
        printf("DEBUG: Khong tim thay file LS phieu muon tai: %s\n", filepath);
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        PhieuMuonNode *newNode = (PhieuMuonNode*)malloc(sizeof(PhieuMuonNode));
        if (newNode == NULL) break;

        if (sscanf(line, " %14[^|] | %14[^|] | %19[^|] | %255[^|] | %19[^|] | %19[^|] | %d\n", 
                   newNode->maPM, newNode->mathe, newNode->matruyen, 
                   newNode->tentruyen, newNode->ngaymuon, newNode->ngaytra, &newNode->trangthai) == 7) {
            
            trim(newNode->maPM);
            trim(newNode->mathe);
            trim(newNode->matruyen);
            trim(newNode->tentruyen);
            trim(newNode->ngaymuon);
            trim(newNode->ngaytra);

            newNode->next = *head;
            *head = newNode;
        } else {
            free(newNode); 
        }
    }
    fclose(f);
}

void DrawThePhieuMuon(PhieuMuonNode *pm, Font font, float x, float y, float scale) {
    if (pm == NULL) return;

    float PMLeft = 600 * scale;
    float PMRight = 200 * scale;
    float PMHeight = 420 * scale;
    
    Rectangle cardleft = { x, y, PMLeft, PMHeight };
    Rectangle cardright = { x + PMLeft, y, PMRight, PMHeight };

    DrawRectangleRec((Rectangle){cardleft.x + 6*scale, cardleft.y + 6*scale, cardleft.width, cardleft.height}, Fade(BLACK, 0.2f));
    DrawRectangleRec((Rectangle){cardright.x + 6*scale, cardright.y + 6*scale, cardright.width, cardright.height}, Fade(BLACK, 0.2f));

    DrawRectangleGradientEx(cardleft, GetColor(0xFF8BC3FF), GetColor(0xF98BC4FF), GetColor(0xFE81AAFF), GetColor(0xFFA8C5FF));
    DrawRectangleGradientEx(cardright, GetColor(0xD6EDFFFF), GetColor(0xB5F4FEFF), GetColor(0xC2FAFFFF), GetColor(0xA4FBCFFF));

    DrawTextEx(font, "Phiếu mượn sách ", (Vector2){cardleft.x + 150*scale, cardleft.y + 20*scale}, 36*scale, 1, MAROON);
    DrawTextEx(font, TextFormat("IDPM:%s", pm->maPM), (Vector2){cardright.x + 16*scale, cardright.y + 340*scale}, 24*scale, 1, GetColor(0x1E96FCFF));
    const char *chuoiinra = (pm->trangthai == 1) ? "Đã trả" : "Chưa trả";
    Color mautrangthai = (pm->trangthai == 1) ? SUCCESSGREEN : ERRORRED; 

        // Đặt ở góc trên cùng bên phải
    Vector2 postrangthai = {cardright.x + 10*scale, cardright.y + 25*scale};
    DrawTextEx(font, "Trạng thái:", postrangthai, 20*scale, 1, DARKGRAY);
    float dodaichu = MeasureTextEx(font, "Trạng thái: ", 20*scale, 1).x;
    DrawTextEx(font, chuoiinra, (Vector2){postrangthai.x + dodaichu, postrangthai.y}, 20*scale, 1, mautrangthai);
    const char* labels[] = {"Mã thẻ bạn đọc:", "Mã truyện:", "Tên truyện:", "Ngày mượn:", "Ngày trả:"};
    const char* values[] = {pm->mathe, pm->matruyen, pm->tentruyen, pm->ngaymuon, pm->ngaytra};
    
    float inputX = cardleft.x + 50 * scale;
    float startY = cardleft.y + 80 * scale;
    float inputW = 450 * scale;
    float inputH = 40 * scale; 
    float spacing = 65 * scale; 

    for (int i = 0; i < 5; i++) {
        float currentY = startY + (i * spacing);
        DrawTextEx(font, labels[i], (Vector2){inputX, currentY - 22*scale}, 18*scale, 1, MAROON);
        
        Rectangle textBox = { inputX, currentY, inputW, inputH };
        DrawRectangleRounded(textBox, 0.2f, 10, WHITE);
        DrawRectangleRoundedLines(textBox, 0.2f, 10, LIGHTGRAY);
        
            float fontSize = 20 * scale;
            Vector2 textPos = { textBox.x + 10, textBox.y + (textBox.height - fontSize)/2 };
            DrawTextEx(font, values[i], textPos, fontSize, 1, PINK);
    }
}

void DrawModalLichSuPhieuMuon(FormTimKiemThe *Form, PhieuMuonMang *mapPM, Font font) {
    if (!Form->showmodal) return;

    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;
    
    DrawRectangle(0, 0, (int)screenW, (int)screenH, Fade(BLACK, 0.6f));
    
    float modalW = 950 * scale;
    float modalH = 650 * scale;
    float modalX = (screenW - modalW) / 2.0f;
    float modalY = (screenH - modalH) / 2.0f;
    
    Rectangle modalRec = { modalX, modalY, modalW, modalH };
    DrawRectangleRounded(modalRec, 0.05f, 10, WHITE);
    DrawRectangleRoundedLines(modalRec, 0.05f, 10, GetColor(0x324376FF));
    
    const char *title = TextFormat("LỊCH SỬ MƯỢN SÁCH - BẠN ĐỌC: %s", Form->dachonmathe);
    DrawTextEx(font, title, (Vector2){modalX + 30*scale, modalY + 25*scale}, 30*scale, 1, MAROON);
    DrawLine((int)modalX + 30, (int)(modalY + 70*scale), (int)(modalX + modalW - 30), (int)(modalY + 70*scale), LIGHTGRAY);
               
    Rectangle btnClose = { modalX + modalW - 60*scale, modalY + 15*scale, 40*scale, 40*scale };
    bool hoverClose = CheckCollisionPointRec(GetMousePosition(), btnClose);
    DrawRectangleRounded(btnClose, 0.2f, 5, hoverClose ? RED : LIGHTGRAY);
    DrawTextEx(font, "X", (Vector2){btnClose.x + 12*scale, btnClose.y + 5*scale}, 30*scale, 1, WHITE);
    
    if (hoverClose && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Form->showmodal = false; 
    }

    HashNodePM *lsNode = Layphantu(mapPM, Form->dachonmathe);
    
    float contentY = modalY + 80 * scale;
    float contentH = modalH - 100 * scale; 
    
    // TỐI ƯU HÓA: Lọc chính xác các phiếu mượn thuộc về mã thẻ này (bỏ qua rác do trùng hash)
    int count = 0;
    HashNodePM *temp = lsNode;
    while(temp != NULL) {
        if (strcmp(temp->LSphieumuon->mathe, Form->dachonmathe) == 0) count++;
        temp = temp->next;
    }
    
    if (count == 0) {
        DrawTextEx(font, "Bạn đọc này chưa có lịch sử mượn sách nào.", 
                   (Vector2){modalX + modalW/2 - 250*scale, contentY + 100*scale}, 24*scale, 1, DARKGRAY);
    } else {
        float cardspacing = 450.0f * scale; 
        float totalheight = count * cardspacing;
        
        float wheel = GetMouseWheelMove();
        Form->modalscroll += wheel * 45.0f;
        
        if (totalheight > contentH) {
            if (Form->modalscroll > 0) Form->modalscroll = 0;
            if (Form->modalscroll < contentH - totalheight) Form->modalscroll = contentH - totalheight;
        } else {
            Form->modalscroll = 0;
        }

        BeginScissorMode((int)modalX, (int)contentY, (int)modalW, (int)contentH);
            float startX = modalX + (modalW - 800*scale) / 2.0f; 
            float startY = contentY + 20.0f * scale + Form->modalscroll;
            
            while (lsNode != NULL) {
                // Chỉ vẽ nếu mã thẻ trùng khớp
                if (strcmp(lsNode->LSphieumuon->mathe, Form->dachonmathe) == 0) {
                    if (startY > contentY - cardspacing && startY < contentY + contentH) {
                        DrawThePhieuMuon(lsNode->LSphieumuon, font, startX, startY, scale);
                    }
                    startY += cardspacing;
                }
                lsNode = lsNode->next;
            }
        EndScissorMode();
    }
}