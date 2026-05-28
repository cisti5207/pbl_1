#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "raylib.h"  
#include "ManageUser.h" 
#include "libmanage.h" 
#include "Timkiemthethuvien.h"
#include "menu.h" // Nhúng menu vào để lấy APP_MENU

int LayCodepointKhongDauVaThuong(int codepoint) {
    if (codepoint >= 'A' && codepoint <= 'Z') return codepoint + 32;
    switch (codepoint) {
        case 0x00C0: case 0x00C1: case 0x00C2: case 0x00C3:
        case 0x00E0: case 0x00E1: case 0x00E2: case 0x00E3:
        case 0x1EA0: case 0x1EA1: case 0x1EA2: case 0x1EA3:
        case 0x1EA4: case 0x1EA5: case 0x1EA6: case 0x1EA7:
        case 0x1EA8: case 0x1EA9: case 0x1EAA: case 0x1EAB:
        case 0x1EAC: case 0x1EAD:
        case 0x0102: case 0x0103:
        case 0x1EAE: case 0x1EAF: case 0x1EB0: case 0x1EB1:
        case 0x1EB2: case 0x1EB3: case 0x1EB4: case 0x1EB5:
        case 0x1EB6: case 0x1EB7:
            return 'a';
        case 0x00C8: case 0x00C9: case 0x00CA:
        case 0x00E8: case 0x00E9: case 0x00EA:
        case 0x1EB8: case 0x1EB9:
        case 0x1EBA: case 0x1EBB: case 0x1EBC: case 0x1EBD:
        case 0x1EBE: case 0x1EBF: case 0x1EC0: case 0x1EC1:
        case 0x1EC2: case 0x1EC3: case 0x1EC4: case 0x1EC5:
        case 0x1EC6: case 0x1EC7:
            return 'e';
        case 0x00CC: case 0x00CD: case 0x0128:
        case 0x00EC: case 0x00ED: case 0x0129:
        case 0x1EC8: case 0x1EC9: case 0x1ECA: case 0x1ECB:
            return 'i';
        case 0x00D2: case 0x00D3: case 0x00D4: case 0x00D5:
        case 0x00F2: case 0x00F3: case 0x00F4: case 0x00F5:
        case 0x01A0: case 0x01A1:
        case 0x1ECC: case 0x1ECD: case 0x1ECE: case 0x1ECF:
        case 0x1ED0: case 0x1ED1: case 0x1ED2: case 0x1ED3:
        case 0x1ED4: case 0x1ED5: case 0x1ED6: case 0x1ED7:
        case 0x1ED8: case 0x1ED9: case 0x1EDA: case 0x1EDB:
        case 0x1EDC: case 0x1EDD: case 0x1EDE: case 0x1EDF:
        case 0x1EE0: case 0x1EE1: case 0x1EE2: case 0x1EE3:
            return 'o';
        case 0x00D9: case 0x00DA: case 0x0168:
        case 0x00F9: case 0x00FA: case 0x0169:
        case 0x01AF: case 0x01B0:
        case 0x1EE4: case 0x1EE5: case 0x1EE6: case 0x1EE7:
        case 0x1EE8: case 0x1EE9: case 0x1EEA: case 0x1EEB:
        case 0x1EEC: case 0x1EED: case 0x1EEE: case 0x1EEF:
        case 0x1EF0: case 0x1EF1:
            return 'u';
        case 0x00DD: case 0x00FD:
        case 0x1EF2: case 0x1EF3: case 0x1EF4: case 0x1EF5:
        case 0x1EF6: case 0x1EF7: case 0x1EF8: case 0x1EF9:
            return 'y';
        case 0x0110: case 0x0111:
            return 'd';
        default:
            return codepoint;
    }
}

void ChuyenChuoiKhongDauVaThuong(const char *nguon, char *dich) {
    int i = 0;
    int j = 0;
    while (nguon[i] != '\0') {
        int bytesprocessed = 0;
        int codepoint = GetCodepoint(&nguon[i], &bytesprocessed);
        
        if (bytesprocessed <= 0) {
            dich[j++] = nguon[i++];
            continue;
        }
        
        int unsignedlower = LayCodepointKhongDauVaThuong(codepoint);
        
        if (unsignedlower < 128) {
            dich[j++] = (char)unsignedlower;
        } else {
            for (int k = 0; k < bytesprocessed; k++) {
                dich[j++] = nguon[i + k];
            }
        }
        i += bytesprocessed;
    }
    dich[j] = '\0';
}

void DocDuLieuTheBanDoc(const char *filepath, BanDoc **head, int *currenttotalusers) {
    FILE *f = fopen(filepath, "r");
    if (f == NULL) {
        printf("Khong tim thay file du lieu the tai: %s\n", filepath);
        return; 
    }
    char line[512];
    int lineCount = 0; 
    while (fgets(line, sizeof(line), f)) {
        lineCount++;
        if (lineCount == 1) continue; 
        BanDoc *newNode = (BanDoc*)malloc(sizeof(BanDoc));
        if (newNode == NULL) break;
        newNode->next = NULL;
        
        if (sscanf(line, " %14[^|] | %99[^|] | %19[^|] | %24[^|] | %19[^\n]", newNode->maThe, newNode->hoTen, newNode->sdt, newNode->cccd, newNode->hanSD) == 5)  {
            trim(newNode->maThe);
            trim(newNode->hoTen); 
            trim(newNode->sdt);
            trim(newNode->cccd);
            trim(newNode->hanSD);  
            int idhientai = atoi(newNode->maThe);
            if (currenttotalusers != NULL && idhientai > *currenttotalusers) {
                *currenttotalusers = idhientai;
            }
            if (*head == NULL) {
                *head = newNode;
            } else {
                BanDoc *temp = *head;
                while (temp->next != NULL) {
                    temp = temp->next;
                }
                temp->next = newNode;
            }
        } else {
            free(newNode); 
        }
    }
    fclose(f);
}

bool KiemTraKhopTimKiem(BanDoc *the, const char *tukhoa) {
    if (tukhoa == NULL || strlen(tukhoa) == 0) {
        return true; 
    }
    char tenkhongdau[256];
    char sdt[256];
    char cccd[256];
    char tukhoakhongdau[256];
    ChuyenChuoiKhongDauVaThuong(the->hoTen, tenkhongdau);
    ChuyenChuoiKhongDauVaThuong(the->sdt, sdt);
    ChuyenChuoiKhongDauVaThuong(the->cccd, cccd);
    ChuyenChuoiKhongDauVaThuong(tukhoa, tukhoakhongdau);
    if (strstr(tenkhongdau, tukhoakhongdau) != NULL) return true;
    if (strstr(sdt, tukhoakhongdau) != NULL) return true;
    if (strstr(cccd, tukhoakhongdau) != NULL) return true;
    return false; 
}

void InitListBD(FormTimKiemThe *Form) {
    memset(Form->nhap.text, 0, 256);
    Form->nhap.letterCount = 0;
    Form->nhap.backspaceCounter = 0;
    Form->nhap.isFocused = true; 
    Form->scroll = 0.0f;
    Form->showmodal = false; // Nhớ reset modal
}

void Updatetoado(FormTimKiemThe *Form) {
    float screenW = (float)GetScreenWidth();
    float inputW = screenW * 0.4f; 
    if(inputW < 400.0f) inputW = 400.0f; 
    if(inputW > 800.0f) inputW = 800.0f; 
    
    float inputH = 50.0f;
    float inputX = (screenW - inputW) / 2.0f; 
    float inputY = 150.0f; 
    
    Form->nhap.rec = (Rectangle){inputX, inputY, inputW, inputH};
}

// ĐÃ THÊM XỬ LÝ NÚT QUAY LẠI VÀO ĐÂY
void kiemtralienquantiengviet(FormTimKiemThe *Form, int *currentState){
    
    // --- LOGIC NÚT QUAY LẠI ---
    Rectangle btnBack = { 20, 20, 130, 40 };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    
    if (IsKeyPressed(KEY_ESCAPE) || (isHoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
        if (Form->showmodal) {
            Form->showmodal = false; // Bấm ESC hoặc nút Back khi có modal thì chỉ tắt Modal
        } else {
            InitListBD(Form);         // Dọn dẹp ô tìm kiếm
            *currentState = APP_MENU; // Quay về Menu
        }
        return; 
    }
    // --------------------------

    InputBox_BD *box[] = {&Form->nhap};
    bool changed = false; 
    
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousepoint = GetMousePosition();
        if(CheckCollisionPointRec(mousepoint, box[0]->rec)) {
            box[0]->isFocused = true;
        } 
        else {
            box[0]->isFocused = false;
        }
    }

    if(box[0]->isFocused && !Form->showmodal) { // Chỉ cho gõ khi KHÔNG mở modal
        int key = GetKeyPressed();
        while(key>0) {
            if(key == KEY_BACKSPACE) {
                if(box[0]->letterCount > 0) {
                    do {
                        box[0]->letterCount--;
                    } while(box[0]->letterCount > 0 && (box[0]->text[box[0]->letterCount] & 0xC0) == 0x80);
                    box[0]->text[box[0]->letterCount] = '\0';
                    changed = true;
                }
            }  
            key = GetKeyPressed();
        }

        if(IsKeyDown(KEY_BACKSPACE)) {
            box[0]->backspaceCounter += GetFrameTime();
            if(box[0]->backspaceCounter >=0.5f) {
                if(box[0]->letterCount >0) {
                    do {
                        box[0]->letterCount--;
                    } while (box[0]->letterCount > 0 && (box[0]->text[box[0]->letterCount] & 0xC0) == 0x80);
                    box[0]->text[box[0]->letterCount] = '\0';
                    changed = true;
                }
            }
        } else box[0]->backspaceCounter = 0;

        int charcode = GetCharPressed();
        while(charcode>0) {
            if ((charcode >= 32) && (box[0]->letterCount < 250)) {
                int bytesize = 0;
                const char* utf8Char = CodepointToUTF8(charcode, &bytesize);
                if(box[0]->letterCount + bytesize < 255) {
                    for(int j = 0; j<bytesize; j++) {
                        box[0]->text[box[0]->letterCount] = utf8Char[j];
                        box[0]->letterCount++;
                    }
                    box[0]->text[box[0]->letterCount] = '\0';
                    changed = true;
                }
            }
            charcode = GetCharPressed();
        }
    }
    
    if (changed) {
        Form->scroll = 0;
    }
    float wheel = GetMouseWheelMove();
    if (wheel != 0 && !Form->showmodal) {
        Form->scroll += wheel * 45.0f; 
    }
}

void DrawKetQuaTimKiem(BanDoc *head, FormTimKiemThe *Form, Font font) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    float toa_do_y_bat_dau = Form->nhap.rec.y + Form->nhap.rec.height + 40.0f; 

    if (Form->nhap.letterCount == 0) {
        const char *msg = "Vui lòng nhập tên, số điện thoại hoặc CCCD để tìm kiếm...";
        Vector2 msgSize = MeasureTextEx(font, msg, 24 * scale, 1);
        DrawTextEx(font, msg, (Vector2){(screenW - msgSize.x)/2, toa_do_y_bat_dau + 30}, 24 * scale, 1, GRAY);
        Form->scroll = 0; 
        return;
    }

    float cardWidth = 650.0f * scale; 
    float startX = (screenW - cardWidth) / 2.0f; 
    float startY = toa_do_y_bat_dau + 20.0f + Form->scroll; 
    float cardspacing = 480.0f * scale; 
    
    int matchcount = 0;
    
    BeginScissorMode(0, (int)toa_do_y_bat_dau, (int)screenW, (int)(screenH - toa_do_y_bat_dau));
    BanDoc *curr = head;
    while (curr != NULL) {
        if (KiemTraKhopTimKiem(curr, Form->nhap.text)) {
            
            Rectangle cardHitbox = { startX, startY, 650.0f * scale, 420.0f * scale };
            
            if (!Form->showmodal && CheckCollisionPointRec(GetMousePosition(), cardHitbox)) {
                DrawRectangleRoundedLines(cardHitbox, 0.1f, 10, ORANGE); 
                
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    strcpy(Form->dachonmathe, curr->maThe); 
                    Form->showmodal = true;                
                    Form->modalscroll = 0;                  
                    printf("DEBUG: Da click the co ma: %s\n", curr->maThe);
                }
            }

            DrawTheBanDoc_TimKiem(curr, font, startX, startY);
            
            startY += cardspacing; 
            matchcount++;
        }
        curr = curr->next;
    }
    EndScissorMode();
}

void DrawTimKiemThe(FormTimKiemThe *Form, Texture2D icons[], Font font, BanDoc *head) {
    float screenW = (float)GetScreenWidth();
    float toa_do_y_bat_dau = Form->nhap.rec.y + Form->nhap.rec.height + 40.0f; 

    DrawKetQuaTimKiem(head, Form, font);
    DrawRectangle(0, 0, (int)screenW, (int)toa_do_y_bat_dau, GetColor(0xf5f7faff));

    Rectangle daimau = {0, 0, screenW, 100};
    DrawRectangleGradientEx(daimau, GetColor(0x70E4EFFF), GetColor(0xC4F5FCFF), GetColor(0xC5DCA0FF), GetColor(0xCDE7BEFF));
    
    // VẼ TIÊU ĐỀ
    const char* titleText = "TÌM KIẾM THẺ BẠN ĐỌC";
    float titleSize = 42.0f;
    Vector2 measureTitle = MeasureTextEx(font, titleText, titleSize, 1);
    Vector2 titlePos = { (screenW - measureTitle.x) / 2.0f, (100 - measureTitle.y) / 2.0f }; 
    DrawTextEx(font, titleText, titlePos, titleSize, 1, GetColor(0xF68E5FFF));
    
    // VẼ Ô SEARCH
    InputBox_BD* box = &Form->nhap;
    DrawRectangleRounded(box->rec, 0.2f, 10, WHITE);
    DrawRectangleRoundedLines(box->rec, 0.2f, 10, GetColor(0x324376FF));
    
    float fontsize = 30.0f;
    Vector2 textsize = MeasureTextEx(font, box->text, fontsize, 1);
    float textoffset = (textsize.x > (box->rec.width - 20)) ? (textsize.x - (box->rec.width - 20)) : 0;
    BeginScissorMode((int)box->rec.x + 5, (int)box->rec.y, (int)box->rec.width - 10, (int)box->rec.height);
        Vector2 textpos = { box->rec.x + 10 - textoffset, box->rec.y + (box->rec.height - fontsize)/2 };
        DrawTextEx(font, box->text, textpos, fontsize, 1, GetColor(0x586BA4FF));
        if (box->isFocused && !Form->showmodal && (((int)(GetTime() * 1.5f)) % 2 == 0)) {
            DrawRectangleV((Vector2){ textpos.x + textsize.x + 2, textpos.y + 2 }, (Vector2){ 2, fontsize - 4 }, GetColor(0x324376FF));
        }
    EndScissorMode();

    // --- VẼ NÚT QUAY LẠI TỰ CHỦ TRÊN LỚP NÀY ---
    Rectangle btnBack = { 20, 20, 130, 40 };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    Color btnColor = isHoverBack ? MAROON : WHITE;
    Color textColor = isHoverBack ? WHITE : MAROON;
    DrawRectangleRounded(btnBack, 0.3f, 10, btnColor);
    DrawRectangleRoundedLines(btnBack, 0.3f, 10, MAROON);
    DrawTextEx(font, "< Quay lai", (Vector2){btnBack.x + 15, btnBack.y + 10}, 20, 1, textColor);
    // ------------------------------------------
}