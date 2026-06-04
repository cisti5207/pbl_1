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
    int index;
    HashNodePM *newNode;
    
    if (pmNode == NULL) return;
    
    index = HashMaThe(pmNode->mathe);
    newNode = (HashNodePM*)malloc(sizeof(HashNodePM));
    newNode->LSphieumuon = pmNode;
    newNode->next = map->mang[index];
    map->mang[index] = newNode;
}

void BuildMap(PhieuMuonMang *map, PhieuMuonNode *head) {
    PhieuMuonNode *hientai = head;
    InitPhieuMuonMang(map); 
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
    char line[1024];
    
    if (f == NULL) {
        return;
    }

    fgets(line, sizeof(line), f);
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
    Color clrLeft, clrRight, clrText, clrIdpm, clrLabelInBox, clrBoxBorder;
    float PMLeft, PMRight, PMHeight;
    Rectangle cardleft, cardright;
    const char *chuoiinra;
    Color mautrangthai;
    Vector2 postrangthai;
    float dodaichu;
    const char *labels[5];
    const char *values[5];
    float inputX, startY, inputW, inputH, spacing;
    int i;

    if (pm == NULL) return;

    // ── Bảng màu ────────────────────────
    clrLeft        = GetColor(0x2f6690ff);
    clrRight       = GetColor(0x81c3d7ff);
    clrText        = GetColor(0xeff2f1ff); 
    clrIdpm        = WHITE;
    clrLabelInBox  = BLACK;                
    clrBoxBorder   = LIGHTGRAY;
    // ────────────────────────────────────

    PMLeft   = 600 * scale;
    PMRight  = 200 * scale;
    PMHeight = 420 * scale;

    cardleft  = (Rectangle){ x, y, PMLeft, PMHeight };
    cardright = (Rectangle){ x + PMLeft, y, PMRight, PMHeight };

    // Bóng
    DrawRectangleRec((Rectangle){cardleft.x  + 6*scale, cardleft.y  + 6*scale, cardleft.width,  cardleft.height},  Fade(BLACK, 0.2f));
    DrawRectangleRec((Rectangle){cardright.x + 6*scale, cardright.y + 6*scale, cardright.width, cardright.height}, Fade(BLACK, 0.2f));

    // Nền card
    DrawRectangleGradientEx(cardleft,  clrLeft,  clrLeft,  clrLeft,  clrLeft);
    DrawRectangleGradientEx(cardright, clrRight, clrRight, clrRight, clrRight);

    // Tiêu đề
    DrawTextEx(font, "Phiếu mượn sách",
               (Vector2){cardleft.x + 150*scale, cardleft.y + 20*scale},
               36*scale, 1, clrText);

    // IDPM
    DrawTextEx(font, TextFormat("IDPM: %s", pm->maPM),
               (Vector2){cardright.x + 16*scale, cardright.y + 340*scale},
               24*scale, 1, clrIdpm);

    // Trạng thái 
    chuoiinra   = (pm->trangthai == 1) ? "Đã trả" : "Chưa trả";
    mautrangthai = (pm->trangthai == 1) ? GREEN : RED; 
    postrangthai = (Vector2){ cardright.x + 10*scale, cardright.y + 25*scale };
    
    DrawTextEx(font, "Trạng thái:", postrangthai, 20*scale, 1, clrText);
    dodaichu = MeasureTextEx(font, "Trạng thái: ", 20*scale, 1).x;
    DrawTextEx(font, chuoiinra,
               (Vector2){postrangthai.x + dodaichu, postrangthai.y},
               20*scale, 1, mautrangthai);

    // 5 ô thông tin
    labels[0] = "Mã thẻ bạn đọc:"; labels[1] = "Mã truyện:"; labels[2] = "Tên truyện:"; labels[3] = "Ngày mượn:"; labels[4] = "Số ngày mượn:";
    values[0] = pm->mathe; values[1] = pm->matruyen; values[2] = pm->tentruyen; values[3] = pm->ngaymuon; values[4] = pm->ngaytra;

    inputX  = cardleft.x + 50  * scale;
    startY  = cardleft.y + 80  * scale;
    inputW  = 450 * scale;
    inputH  = 40  * scale;
    spacing = 65  * scale;

    for (i = 0; i < 5; i++) {
        float currentY = startY + (i * spacing);
        Rectangle textBox;
        float fontSize;
        Vector2 textPos;

        // Label 
        DrawTextEx(font, labels[i],
                   (Vector2){ inputX, currentY - 22*scale },
                   18*scale, 1, clrText);

        // Ô trắng
        textBox = (Rectangle){ inputX, currentY, inputW, inputH };
        DrawRectangleRounded(textBox, 0.2f, 10, WHITE);
        DrawRectangleRoundedLines(textBox, 0.2f, 10, clrBoxBorder);

        // Chữ trong ô 
        fontSize = 20 * scale;
        textPos  = (Vector2){ textBox.x + 10, textBox.y + (textBox.height - fontSize) / 2 };

        BeginScissorMode((int)textBox.x + 5, (int)textBox.y,
                         (int)textBox.width - 10, (int)textBox.height);
            DrawTextEx(font, values[i], textPos, fontSize, 1, clrLabelInBox);
        EndScissorMode();
    }
}

// ===============================================
// MODAL LỊCH SỬ PHIẾU MƯỢN BÊN TRONG THẺ BẠN ĐỌC
// ===============================================
void DrawModalLichSuPhieuMuon(FormTimKiemThe *Form, PhieuMuonMang *mapPM, Font font) {
    static PhieuMuonNode* selectedPhieu = NULL;
    static float s_ls_scroll = 0.0f;
    static bool s_ls_dragging = false;
    static float s_ls_dragStartY = 0.0f;

    float screenW, screenH, scale;
    float modalW, modalH, modalX, modalY;
    Rectangle modalRec;
    const char *title;
    Rectangle btnClose;
    bool hoverClose;
    HashNodePM *lsNode, *temp;
    float contentY, contentH;
    int count;

    if (!Form->showmodal) {
        selectedPhieu = NULL; 
        s_ls_scroll = 0.0f;
        s_ls_dragging = false;
        return;
    }

    screenW = (float)GetScreenWidth();
    screenH = (float)GetScreenHeight();
    scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;
    
    DrawRectangle(0, 0, (int)screenW, (int)screenH, Fade(BLACK, 0.6f));
    
    modalW = 950 * scale;
    modalH = 650 * scale;
    modalX = (screenW - modalW) / 2.0f;
    modalY = (screenH - modalH) / 2.0f;
    
    modalRec = (Rectangle){ modalX, modalY, modalW, modalH };
    DrawRectangleRounded(modalRec, 0.05f, 10, WHITE);
    DrawRectangleRoundedLines(modalRec, 0.05f, 10, GetColor(0x324376FF));
    
    title = TextFormat("LỊCH SỬ MƯỢN SÁCH - BẠN ĐỌC: %s", Form->dachonmathe);
    DrawTextEx(font, title,
               (Vector2){modalX + 30*scale, modalY + 25*scale},
               30*scale, 1, GetColor(0x274690ff));
    DrawLine((int)modalX + 30, (int)(modalY + 70*scale),
             (int)(modalX + modalW - 30), (int)(modalY + 70*scale), LIGHTGRAY);
                
    btnClose = (Rectangle){ modalX + modalW - 60*scale, modalY + 15*scale, 40*scale, 40*scale };
    hoverClose = CheckCollisionPointRec(GetMousePosition(), btnClose);
    DrawRectangleRounded(btnClose, 0.2f, 5, hoverClose ? RED : LIGHTGRAY);
    DrawTextEx(font, "X",
               (Vector2){btnClose.x + 12*scale, btnClose.y + 5*scale},
               30*scale, 1, WHITE);
    
    if (hoverClose && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Form->showmodal = false; 
        s_ls_scroll = 0.0f;
        s_ls_dragging = false;
        selectedPhieu = NULL;
        return;
    }

    lsNode = Layphantu(mapPM, Form->dachonmathe);
    
    contentY = modalY + 80 * scale;
    contentH = modalH - 100 * scale; 
    
    count = 0;
    temp = lsNode;
    while(temp != NULL) {
        if (strcmp(temp->LSphieumuon->mathe, Form->dachonmathe) == 0) count++;
        temp = temp->next;
    }
    
    if (count == 0) {
        DrawTextEx(font, "Bạn đọc này chưa có lịch sử mượn sách nào.", 
                   (Vector2){modalX + modalW/2 - 250*scale, contentY + 100*scale},
                   24*scale, 1, DARKGRAY);
    } else {
        if (selectedPhieu != NULL) {
            float detailX = modalX + (modalW - 800*scale) / 2.0f;
            float detailY = contentY + 60.0f * scale;
            Rectangle backBtn = {modalX + 20*scale, contentY + 10*scale, 130*scale, 40*scale};
            bool hoverBack = CheckCollisionPointRec(GetMousePosition(), backBtn);
            
            DrawRectangleRounded(backBtn, 0.2f, 10, hoverBack ? Fade(SKYBLUE, 0.5f) : Fade(LIGHTGRAY, 0.3f));
            DrawTextEx(font, "< Quay lại", (Vector2){backBtn.x + 15*scale, backBtn.y + 10*scale}, 20*scale, 1, DARKGRAY);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hoverBack) {
                selectedPhieu = NULL; 
            }

            DrawThePhieuMuon(selectedPhieu, font, detailX, detailY, scale);

        } else {
            float itemHeight = 100.0f * scale; 
            float spacing = 15.0f * scale; 
            float totalheight = count * (itemHeight + spacing) + 20.0f * scale;
            
            if (totalheight > contentH) {
                float maxScroll = totalheight - contentH;
                float wheel = GetMouseWheelMove();
                float trackX, trackY, trackW, trackH;
                Rectangle trackRec;
                float thumbH, maxThumbY, currentThumbY;
                Rectangle thumbRec, thumbHitbox;
                Vector2 mousePos;
                
                if (wheel != 0) {
                    s_ls_scroll -= wheel * 45.0f;
                }
                
                if (s_ls_scroll < 0) s_ls_scroll = 0;
                if (s_ls_scroll > maxScroll) s_ls_scroll = maxScroll;

                trackX = modalX + modalW - 20 * scale;
                trackY = contentY;
                trackW = 10 * scale;
                trackH = contentH;
                trackRec = (Rectangle){trackX, trackY, trackW, trackH};

                thumbH = trackH * (contentH / totalheight);
                if (thumbH < 40.0f * scale) thumbH = 40.0f * scale; 
                
                maxThumbY = trackH - thumbH;
                if (maxThumbY <= 0) maxThumbY = 0.001f; 
                
                currentThumbY = trackY + (s_ls_scroll / maxScroll) * maxThumbY;
                thumbRec = (Rectangle){trackX, currentThumbY, trackW, thumbH};
                thumbHitbox = (Rectangle){thumbRec.x - 10, thumbRec.y, thumbRec.width + 20, thumbRec.height};

                mousePos = GetMousePosition();

                if (s_ls_dragging) {
                    if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                        s_ls_dragging = false; 
                    } else {
                        float newThumbY = mousePos.y - s_ls_dragStartY;
                        float scrollFraction;
                        if (newThumbY < trackY) newThumbY = trackY;
                        if (newThumbY > trackY + maxThumbY) newThumbY = trackY + maxThumbY;
                        scrollFraction = (newThumbY - trackY) / maxThumbY;
                        s_ls_scroll = scrollFraction * maxScroll; 
                    }
                } else {
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if (CheckCollisionPointRec(mousePos, thumbHitbox)) {
                            s_ls_dragging = true;
                            s_ls_dragStartY = mousePos.y - thumbRec.y; 
                        } else if (CheckCollisionPointRec(mousePos, trackRec)) {
                            if (mousePos.y < thumbRec.y) s_ls_scroll -= contentH;
                            else s_ls_scroll += contentH;
                            if (s_ls_scroll < 0) s_ls_scroll = 0;
                            if (s_ls_scroll > maxScroll) s_ls_scroll = maxScroll;
                        }
                    }
                }
            } else {
                s_ls_scroll = 0;
                s_ls_dragging = false;
            }

            BeginScissorMode((int)modalX, (int)contentY, (int)modalW, (int)contentH);
            {
                float startX = modalX + 30 * scale; 
                float listW = modalW - 80 * scale;
                float startY = contentY + 10.0f * scale - s_ls_scroll; 
                HashNodePM *tempNode = lsNode;
                
                while (tempNode != NULL) {
                    if (strcmp(tempNode->LSphieumuon->mathe, Form->dachonmathe) == 0) {
                        Rectangle itemRec;
                        bool hoverItem;
                        bool inScissor;
                        char line1[512], line2[512];
                        Color statusColor;
                        const char* statusText;
                        
                        itemRec = (Rectangle){startX, startY, listW, itemHeight};
                        hoverItem = !s_ls_dragging && CheckCollisionPointRec(GetMousePosition(), itemRec);
                        inScissor = GetMousePosition().y > contentY && GetMousePosition().y < (contentY + contentH);

                        DrawRectangleRounded(itemRec, 0.1f, 10, hoverItem ? Fade(SKYBLUE, 0.2f) : Fade(LIGHTGRAY, 0.2f));
                        DrawRectangleLinesEx(itemRec, 1, hoverItem ? GetColor(0x274690ff) : LIGHTGRAY);

                        sprintf(line1, "Tên truyện: %s (Mã: %s)", tempNode->LSphieumuon->tentruyen, tempNode->LSphieumuon->matruyen);
                        sprintf(line2, "Ngày mượn: %s   |   Số ngày mượn: %s", tempNode->LSphieumuon->ngaymuon, tempNode->LSphieumuon->ngaytra);

                        DrawTextEx(font, line1, (Vector2){startX + 20*scale, startY + 15*scale}, 26*scale, 1, GetColor(0x274690ff));
                        DrawTextEx(font, line2, (Vector2){startX + 20*scale, startY + 55*scale}, 22*scale, 1, DARKGRAY);

                        statusColor = (tempNode->LSphieumuon->trangthai == 1) ? GREEN : RED;
                        statusText = (tempNode->LSphieumuon->trangthai == 1) ? "Đã trả" : "Chưa trả";
                        DrawTextEx(font, statusText, (Vector2){startX + listW - 250*scale, startY + 42*scale}, 22*scale, 1, statusColor);

                        DrawTextEx(font, "Chi tiết >", (Vector2){startX + listW - 120*scale, startY + 42*scale}, 22*scale, 1, BLUE);

                        if (hoverItem && inScissor && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            selectedPhieu = tempNode->LSphieumuon; 
                        }

                        startY += itemHeight + spacing;
                    }
                    tempNode = tempNode->next;
                }
            }
            EndScissorMode();

            if (totalheight > contentH) {
                float trackX = modalX + modalW - 20 * scale;
                float trackY = contentY;
                float trackW = 10 * scale;
                float trackH = contentH;
                float thumbH, maxThumbY, maxScroll, thumbY;
                Rectangle trackRec, thumbRec, thumbHitbox;
                Color thumbColor;
                
                thumbH = trackH * (contentH / totalheight);
                if (thumbH < 40.0f * scale) thumbH = 40.0f * scale;
                
                maxThumbY = trackH - thumbH;
                if (maxThumbY <= 0) maxThumbY = 0.001f;
                
                maxScroll = totalheight - contentH;
                thumbY = trackY + (s_ls_scroll / maxScroll) * maxThumbY;
                
                trackRec = (Rectangle){trackX, trackY, trackW, trackH};
                thumbRec = (Rectangle){trackX, thumbY, trackW, thumbH};
                thumbHitbox = (Rectangle){thumbRec.x - 10, thumbRec.y, thumbRec.width + 20, thumbRec.height};

                DrawRectangleRounded(trackRec, 1.0f, 10, Fade(LIGHTGRAY, 0.3f));
                
                thumbColor = Fade(GRAY, 0.7f);
                if (s_ls_dragging) thumbColor = DARKGRAY;
                else if (CheckCollisionPointRec(GetMousePosition(), thumbHitbox)) thumbColor = GRAY;

                DrawRectangleRounded(thumbRec, 1.0f, 10, thumbColor);
            }
        }
    }
}