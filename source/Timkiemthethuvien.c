#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "raylib.h"  
#include "ManageUser.h" 
#include "libmanage.h" 
#include "Timkiemthethuvien.h"
int LayCodepointKhongDauVaThuong(int codepoint) {
    if (codepoint >= 'A' && codepoint <= 'Z') return codepoint + 32;
    switch (codepoint) {
        // Nhóm chữ A 
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
            
        // Nhóm chữ E 
        case 0x00C8: case 0x00C9: case 0x00CA:
        case 0x00E8: case 0x00E9: case 0x00EA:
        case 0x1EB8: case 0x1EB9:
        case 0x1EBA: case 0x1EBB: case 0x1EBC: case 0x1EBD:
        case 0x1EBE: case 0x1EBF: case 0x1EC0: case 0x1EC1:
        case 0x1EC2: case 0x1EC3: case 0x1EC4: case 0x1EC5:
        case 0x1EC6: case 0x1EC7:
            return 'e';
            
        // Nhóm chữ I 
        case 0x00CC: case 0x00CD: case 0x0128:
        case 0x00EC: case 0x00ED: case 0x0129:
        case 0x1EC8: case 0x1EC9: case 0x1ECA: case 0x1ECB:
            return 'i';
            
        // Nhóm chữ O 
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
            
        // Nhóm chữ U 
        case 0x00D9: case 0x00DA: case 0x0168:
        case 0x00F9: case 0x00FA: case 0x0169:
        case 0x01AF: case 0x01B0:
        case 0x1EE4: case 0x1EE5: case 0x1EE6: case 0x1EE7:
        case 0x1EE8: case 0x1EE9: case 0x1EEA: case 0x1EEB:
        case 0x1EEC: case 0x1EED: case 0x1EEE: case 0x1EEF:
        case 0x1EF0: case 0x1EF1:
            return 'u';
            
        // Nhóm chữ Y 
        case 0x00DD: case 0x00FD:
        case 0x1EF2: case 0x1EF3: case 0x1EF4: case 0x1EF5:
        case 0x1EF6: case 0x1EF7: case 0x1EF8: case 0x1EF9:
            return 'y';
            
        // Nhóm chữ D
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
        printf("Không tìm thấy file dữ liệu thẻ tại: %s\n", filepath);
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
        if (sscanf(line, " %[^|] | %[^|] | %[^|] | %[^|] | %[^\n]", newNode->maThe, newNode->hoTen, newNode->sdt, newNode->cccd, newNode->hanSD) == 5) {
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
}
void Updatetoado(FormTimKiemThe *Form) {

    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;
    Rectangle list = { (screenW - 900*scale)/2, (screenH - 600*scale)/2, 900*scale, 600*scale };
    float inputX = list.x + 270*scale;
    float inputY = list.y + 120*scale;
    float inputH = 50*scale;
    float inputW = 360*scale;
    Form->nhap.rec = (Rectangle){inputX, inputY, inputW, inputH};
}

void kiemtralienquantiengviet(FormTimKiemThe *Form){
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

    if(box[0]->isFocused) {
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
    if (wheel != 0) {
        Form->scroll += wheel * 45.0f; // Tăng nhẹ tốc độ cuộn cho mượt
    }
}
void DrawKetQuaTimKiem(BanDoc *head, FormTimKiemThe *Form, Font font) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    float toa_do_y_bat_dau = Form->nhap.rec.y + Form->nhap.rec.height + 20 * scale; 

    // Kiểm tra nếu chưa nhập ký tự nào
    if (Form->nhap.letterCount == 0) {
        const char *msg = "Vui lòng nhập tên, số điện thoại hoặc CCCD để tìm kiếm...";
        Vector2 msgSize = MeasureTextEx(font, msg, 24*scale, 1);
        DrawTextEx(font, msg, (Vector2){(screenW - msgSize.x)/2, toa_do_y_bat_dau + 60*scale}, 24*scale, 1, GRAY);
        Form->scroll = 0; 
        return;
    }

    float startX = (screenW - 650*scale) / 2;
    float startY = toa_do_y_bat_dau + 40*scale + Form->scroll; 
    int matchcount = 0;
    // Sử dụng Scissor Mode của Raylib làm mặt nạ giới hạn khung hiển thị thẻ khi cuộn
    BeginScissorMode(0, (int)toa_do_y_bat_dau, (int)screenW, (int)(screenH - toa_do_y_bat_dau));
    BanDoc *curr = head;
    while (curr != NULL) {
        if (KiemTraKhopTimKiem(curr, Form->nhap.text)) {
            DrawTheBanDoc_TimKiem(curr, font, startX, startY);
            startY += 500 * scale; // Tăng khoảng cách 500px tránh dính thẻ chồng lên nhau
            matchcount++;
        }
        curr = curr->next;
    }

    // Nếu không tìm thấy ai trùng khớp
    if (matchcount == 0) {
        const char *msg = "Không tìm thấy kết quả nào phù hợp!";
        Vector2 msgSize = MeasureTextEx(font, msg, 24*scale, 1);
        DrawTextEx(font, msg, (Vector2){(screenW - msgSize.x)/2, toa_do_y_bat_dau + 60*scale}, 24*scale, 1, MAROON);
    }

    EndScissorMode(); 

    // Giới hạn cuộn chuột để không bị lăn ra ngoài ds
    float totalheight = matchcount * 500 * scale; 
    float visibleheight = screenH - toa_do_y_bat_dau;
    float minscroll = visibleheight - totalheight - 80*scale;

    if (minscroll > 0) minscroll = 0; 
    if (Form->scroll > 0) Form->scroll = 0; 
    if (Form->scroll < minscroll) Form->scroll = minscroll; 
}
// Vẽ giao diện của list thẻ bạn đọc
void DrawTimKiemThe(FormTimKiemThe *Form, Texture2D icons[], Font font, BanDoc *head) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;
    Rectangle timkiem = {(screenW - 950*scale)/2, (screenH-670*scale), 950*scale, 670*scale};
    Rectangle daimau = {(screenW - 950*scale)/2, 0, 950*scale, 80*scale};
    // Pha màu dạng gradient cho chữ nhật ở trên và tiêu đề.
    DrawRectangleRec(timkiem, WHITE);
    DrawRectangleGradientEx(daimau, GetColor(0x70E4EFFF), GetColor(0xC4F5FCFF), GetColor(0xC5DCA0FF), GetColor(0xCDE7BEFF));
    DrawTextEx(font, "TÌM KIẾM THẺ BẠN ĐỌC", (Vector2){daimau.x + 400*scale, daimau.y + 20*scale}, 42*scale, 1, GetColor(0xF68E5FFF) );
    InputBox_BD* box[] = {&Form->nhap};
    DrawRectangleRounded(box[0]->rec, 0.2f, 10, WHITE);
    DrawRectangleRoundedLines(box[0]->rec, 0.2f, 10, GetColor(0x324376FF));
    float fontsize = 30*scale;
    Vector2 textsize = MeasureTextEx(font, box[0]->text, fontsize, 1);
    float textoffset = (textsize.x > (box[0]->rec.width - 20)) ? (textsize.x - (box[0]->rec.width - 20)) : 0;
        BeginScissorMode((int)box[0]->rec.x + 5, (int)box[0]->rec.y, (int)box[0]->rec.width - 10, (int)box[0]->rec.height);
            Vector2 textpos = { box[0]->rec.x + 10 - textoffset, box[0]->rec.y + (box[0]->rec.height - fontsize)/2 };
            DrawTextEx(font, box[0]->text, textpos, fontsize, 1, GetColor(0x586BA4FF));

            if (box[0]->isFocused && (((int)(GetTime() * 1.5f)) % 2 == 0)) {
                DrawRectangleV((Vector2){ textpos.x + textsize.x + 2, textpos.y + 2 }, (Vector2){ 2, fontsize - 4 }, GetColor(0x324376FF));
            }
        EndScissorMode();
        DrawKetQuaTimKiem(head, Form, font);
} 