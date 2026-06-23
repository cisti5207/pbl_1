#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "raylib.h"
#include "ManageUser.h"
#include "libmanage.h"
#include "Timkiemthethuvien.h"
#include "menu.h"

// ==========================================
// CHUYỂN ĐỔI KHÔNG DẤU & THƯỜNG
// ==========================================
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
    int i = 0, j = 0;
    while (nguon[i] != '\0') {
        int bytesprocessed = 0;
        int codepoint = GetCodepoint(&nguon[i], &bytesprocessed);
        if (bytesprocessed <= 0) { dich[j++] = nguon[i++]; continue; }
        int unsignedlower = LayCodepointKhongDauVaThuong(codepoint);
        if (unsignedlower < 128) {
            dich[j++] = (char)unsignedlower;
        } else {
            int k;
            for (k = 0; k < bytesprocessed; k++) dich[j++] = nguon[i + k];
        }
        i += bytesprocessed;
    }
    dich[j] = '\0';
}

// ==========================================
// ĐỌC DỮ LIỆU
// ==========================================
void DocDuLieuTheBanDoc(const char *filepath, BanDoc **head, int *currenttotalusers) {
    FILE *f = fopen(filepath, "r");
    char line[512];
    int lineCount = 0;
    if (f == NULL) { printf("Khong tim thay file: %s\n", filepath); return; }
    
    while (fgets(line, sizeof(line), f)) {
        BanDoc *newNode;
        lineCount++;
        if (lineCount == 1) continue;
        newNode = (BanDoc*)malloc(sizeof(BanDoc));
        if (newNode == NULL) break;
        newNode->next = NULL;
        if (sscanf(line, " %14[^|] | %99[^|] | %19[^|] | %24[^|] | %19[^\n]",
            newNode->maThe, newNode->hoTen, newNode->sdt, newNode->cccd, newNode->hanSD) == 5) {
            int idhientai;
            trim(newNode->maThe); trim(newNode->hoTen);
            trim(newNode->sdt);   trim(newNode->cccd);  trim(newNode->hanSD);
            idhientai = atoi(newNode->maThe);
            if (currenttotalusers && idhientai > *currenttotalusers) *currenttotalusers = idhientai;
            if (*head == NULL) {
                *head = newNode;
            } else {
                BanDoc *temp = *head;
                while (temp->next) temp = temp->next;
                temp->next = newNode;
            }
        } else { free(newNode); }
    }
    fclose(f);
}

// ==========================================
// TÌM KIẾM
// ==========================================
bool KiemTraKhopTimKiem(BanDoc *the, const char *tukhoa) {
    char tennd[256], sdt[256], cccd[256], tk[256];
    if (!tukhoa || strlen(tukhoa) == 0) return true;
    ChuyenChuoiKhongDauVaThuong(the->hoTen, tennd);
    ChuyenChuoiKhongDauVaThuong(the->sdt, sdt);
    ChuyenChuoiKhongDauVaThuong(the->cccd, cccd);
    ChuyenChuoiKhongDauVaThuong(tukhoa, tk);
    if (strstr(tennd, tk)) return true;
    if (strstr(sdt, tk))   return true;
    if (strstr(cccd, tk))  return true;
    return false;
}

// ==========================================
// KHỞI TẠO & CẬP NHẬT
// ==========================================
void InitListBD(FormTimKiemThe *Form) {
    memset(Form->nhap.text, 0, 256);
    Form->nhap.letterCount    = 0;
    Form->nhap.backspaceCounter = 0;
    Form->nhap.isFocused      = true;
    Form->scroll              = 0.0f;
    Form->showmodal           = false;
    Form->scrollbar_dragging  = false;
    Form->modal_sb_dragging   = false;
}

void Updatetoado(FormTimKiemThe *Form) {
    float screenW = (float)GetScreenWidth();
    float uiScale = screenW / 1440.0f;
    if (uiScale > 1.0f) uiScale = 1.0f;
    if (uiScale < 0.6f) uiScale = 0.6f;

    float inputW = screenW * 0.50f; // Form tìm kiếm chiếm 50% màn hình
    float inputH = 55.0f * uiScale;
    float inputX, inputY;

    if (inputW < 400.0f) inputW = 400.0f;
    if (inputW > 800.0f) inputW = 800.0f;
    
    inputX = (screenW - inputW) / 2.0f;
    inputY = 155.0f * uiScale; // Đã dịch xuống 30px (từ 125 lên 155) để hết bị đè
    Form->nhap.rec = (Rectangle){ inputX, inputY, inputW, inputH };
}

void kiemtralienquantiengviet(FormTimKiemThe *Form, int *currentState) {
    Rectangle btnBack;
    bool isHoverBack;
    InputBox_BD *box;
    bool changed = false;
    float wheel;
    float uiScale = (float)GetScreenWidth() / 1440.0f;
    if (uiScale > 1.0f) uiScale = 1.0f;
    if (uiScale < 0.6f) uiScale = 0.6f;

    if (Form->showmodal) {
        wheel = GetMouseWheelMove();
        if (wheel != 0) Form->modalscroll -= wheel * 45.0f;
        if (Form->modalscroll < 0) Form->modalscroll = 0;
        return; 
    }

    float btnBackW = 130.0f * uiScale;
    float btnBackH = 44.0f * uiScale;
    btnBack = (Rectangle){ 20, 25, btnBackW, btnBackH };
    isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    
    if (IsKeyPressed(KEY_ESCAPE) || (isHoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
        InitListBD(Form);
        *currentState = APP_MENU;
        return;
    }

    box = &Form->nhap;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        box->isFocused = CheckCollisionPointRec(GetMousePosition(), box->rec);

    if (box->isFocused) {
        int key = GetKeyPressed();
        while (key > 0) {
            if (key == KEY_BACKSPACE && box->letterCount > 0) {
                do { box->letterCount--; }
                while (box->letterCount > 0 && (box->text[box->letterCount] & 0xC0) == 0x80);
                box->text[box->letterCount] = '\0';
                changed = true;
            }
            key = GetKeyPressed();
        }
        if (IsKeyDown(KEY_BACKSPACE)) {
            box->backspaceCounter += GetFrameTime();
            if (box->backspaceCounter >= 0.15f && box->letterCount > 0) { // Đổi 0.5f thành 0.15f cho nhạy
                do { box->letterCount--; }
                while (box->letterCount > 0 && (box->text[box->letterCount] & 0xC0) == 0x80);
                box->text[box->letterCount] = '\0';
                changed = true;
            }
        } else box->backspaceCounter = 0;

        {
            int charcode = GetCharPressed();
            while (charcode > 0) {
                if (charcode >= 32 && box->letterCount < 250) {
                    int bsz = 0;
                    const char *u8 = CodepointToUTF8(charcode, &bsz);
                    if (box->letterCount + bsz < 255) {
                        int j;
                        for (j = 0; j < bsz; j++) box->text[box->letterCount++] = u8[j];
                        box->text[box->letterCount] = '\0';
                        changed = true;
                    }
                }
                charcode = GetCharPressed();
            }
        }
    }

    if (changed) Form->scroll = 0;
}

// ==========================================
// CÁC DEFINE & HELPER BẢNG
// ==========================================
static Color COL_HEADER_BG = { 20, 30, 75, 240 };
static Color COL_ROW_EVEN  = { 27, 38, 79, 130 };
static Color COL_ROW_ODD   = { 18, 26, 60, 150 };
static Color COL_ROW_HOVER = { 55, 90, 175, 200 };
static Color COL_BORDER    = {  90, 130, 210,  55 };

static float colRatio[] = { 0.10f, 0.25f, 0.15f, 0.20f, 0.16f, 0.14f };
#define NUM_COLS 6
static const char *colHeaders[] = { "Mã thẻ", "Họ và tên", "Số điện thoại", "CCCD", "Hạn sử dụng", "Chi tiết" };

static float ColX(float tableX, float tableW, int col) {
    float x = tableX;
    int i;
    for (i = 0; i < col; i++) x += tableW * colRatio[i];
    return x;
}

// ==========================================
// VẼ BẢNG KẾT QUẢ KÈM PHÂN TRANG & SCROLL
// ==========================================
void DrawKetQuaTimKiem(BanDoc *head, FormTimKiemThe *Form, Font font) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    
    // Scale Responsive
    float uiScale = screenW / 1440.0f;
    if (uiScale > 1.0f) uiScale = 1.0f;
    if (uiScale < 0.6f) uiScale = 0.6f;

    // Phóng to Font size theo yêu cầu (Cộng thêm cỡ 5-6 size so với bản cũ)
    float fzHeader = 24.0f * uiScale;
    float fzRow    = 24.0f * uiScale;
    float fzBtn    = 22.0f * uiScale;
    float fzMsg    = 28.0f * uiScale;
    float fzPage   = 26.0f * uiScale;

    float listTop = Form->nhap.rec.y + Form->nhap.rec.height + 25.0f * uiScale;
    float tableW  = screenW * 0.94f;
    float tableX  = (screenW - tableW) / 2.0f; // Căn giữa
    float sbW     = 10.0f;

    int matchCount = 0;
    BanDoc *cur = head;
    static int currentPage = 1;
    int itemsPerPage = 30; // YÊU CẦU: 30 ITEM MỖI TRANG ĐỂ SCROLL
    int maxPages, startIndex, endIndex, itemsOnThisPage;
    float contentH, viewportH, maxScroll, wheel;
    float headerH = 65.0f * uiScale; // Phóng to header
    float bodyTop = listTop + headerH;
    
    // Không gian hiển thị còn lại (Chừa chỗ cho phân trang ở dưới)
    float bodyH = screenH - bodyTop - (75.0f * uiScale); 
    if (bodyH < 300.0f) bodyH = 300.0f;
    
    // YÊU CẦU: Hiển thị đúng 12 Item trong Viewport
    float rowH = bodyH / 12.0f;

    int c;

    // Đếm tổng số kết quả phù hợp
    while (cur) { if (KiemTraKhopTimKiem(cur, Form->nhap.text)) matchCount++; cur = cur->next; }

    // Tính toán số trang
    maxPages = (matchCount + itemsPerPage - 1) / itemsPerPage;
    if (currentPage > maxPages && maxPages > 0) currentPage = maxPages;
    if (currentPage < 1) currentPage = 1;

    startIndex = (currentPage - 1) * itemsPerPage;
    endIndex = startIndex + itemsPerPage;
    if (endIndex > matchCount) endIndex = matchCount;

    itemsOnThisPage = endIndex - startIndex;
    contentH  = (float)itemsOnThisPage * rowH;
    viewportH = bodyH;

    maxScroll = contentH - viewportH;
    if (maxScroll < 0) maxScroll = 0;

    // Cuộn chuột
    wheel = GetMouseWheelMove();
    if (wheel != 0 && !Form->showmodal) Form->scroll -= wheel * 45.0f;
    if (Form->scroll < 0) Form->scroll = 0;
    if (Form->scroll > maxScroll) Form->scroll = maxScroll;

    // ─── 1. VẼ HEADER BẢNG ──────────
    DrawRectangle((int)tableX, (int)listTop, (int)tableW, (int)headerH, COL_HEADER_BG);
    DrawRectangle((int)tableX, (int)(listTop + headerH - 2), (int)tableW, 2, GetColor(0x4A80FFFF));

    for (c = 0; c < NUM_COLS; c++) {
        float cx = ColX(tableX, tableW, c);
        float cw = tableW * colRatio[c];
        Vector2 ts = MeasureTextEx(font, colHeaders[c], fzHeader, 1); 
        if (c > 0) DrawRectangle((int)cx, (int)listTop, 1, (int)headerH, COL_BORDER);
        DrawTextEx(font, colHeaders[c],
                   (Vector2){ cx + (cw - ts.x) / 2.0f, listTop + (headerH - ts.y) / 2.0f },
                   fzHeader, 1, WHITE);
    }

    if (Form->nhap.letterCount == 0 && matchCount == 0) { // Đã sửa logic chỗ này để hiển thị đúng
        const char *msg = "Nhập tên, số điện thoại hoặc CCCD để tìm kiếm...";
        Vector2 ms = MeasureTextEx(font, msg, fzMsg, 1);
        DrawTextEx(font, msg, (Vector2){ (screenW - ms.x)/2, listTop + headerH + 60 },
                   fzMsg, 1, GetColor(0x7090CCAA));
        return;
    }

    // ─── 2. VẼ NỀN LƯỚI TRỐNG & DỮ LIỆU (BÊN TRONG SCISSOR) ──────────
    BeginScissorMode((int)tableX, (int)bodyTop, (int)(tableW + sbW + 6), (int)bodyH);
    {
        // A. Vẽ nền xen kẽ luôn phủ kín 12 dòng (Kể cả khi thiếu data)
        int startBgRow = (int)(Form->scroll / rowH);
        int maxVisibleRows = (int)(bodyH / rowH) + 2;
        for (int i = 0; i < maxVisibleRows; i++) {
            int rIdx = startBgRow + i;
            float y = bodyTop - Form->scroll + rIdx * rowH;
            if (y > bodyTop + bodyH) break;
            Color rowBg = (rIdx % 2 == 0) ? COL_ROW_EVEN : COL_ROW_ODD;
            DrawRectangleRec((Rectangle){tableX, y, tableW, rowH}, rowBg);
        }

        // B. Lặp dữ liệu để điền text vào
        float rowY = bodyTop - Form->scroll;
        int matchIdx = 0;
        cur = head;
        while (cur) {
            if (!KiemTraKhopTimKiem(cur, Form->nhap.text)) { cur = cur->next; continue; }

            if (matchIdx >= startIndex && matchIdx < endIndex) {
                bool rowVisible = (rowY + rowH > bodyTop) && (rowY < bodyTop + bodyH);
                if (rowVisible) {
                    Rectangle rowRect = { tableX, rowY, tableW, rowH };
                    bool isHover = !Form->showmodal && CheckCollisionPointRec(GetMousePosition(), rowRect);
                    if (GetMousePosition().y < bodyTop || GetMousePosition().y > bodyTop + bodyH) isHover = false;
                    
                    // Highlight nếu hover
                    if (isHover) DrawRectangleRec(rowRect, COL_ROW_HOVER);
                    
                    // Kẻ chân dòng
                    DrawRectangle((int)tableX, (int)(rowY + rowH - 1), (int)tableW, 1, COL_BORDER);

                    const char *cells[5] = { cur->maThe, cur->hoTen, cur->sdt, cur->cccd, cur->hanSD };
                    for (int i = 0; i < NUM_COLS - 1; i++) {
                        float cx = ColX(tableX, tableW, i);
                        float cw = tableW * colRatio[i];
                        Vector2 ts = MeasureTextEx(font, cells[i], fzRow, 1);
                        if (i > 0) DrawRectangle((int)cx, (int)rowY, 1, (int)rowH, COL_BORDER);
                        
                        // Căn giữa text trong cột thay vì căn trái
                        DrawTextEx(font, cells[i],
                                   (Vector2){ cx + (cw - ts.x) / 2.0f, rowY + (rowH - ts.y) / 2.0f },
                                   fzRow, 1, WHITE);
                    }

                    // Nút "Chi tiết"
                    {
                        float cx  = ColX(tableX, tableW, NUM_COLS - 1);
                        float cwd = tableW * colRatio[NUM_COLS - 1];
                        float btnW = cwd * 0.75f, btnH = rowH * 0.65f;
                        if(btnH > 46.0f) btnH = 46.0f; // Khống chế độ cao nút
                        float btnX = cx + (cwd - btnW) / 2.0f;
                        float btnY = rowY + (rowH - btnH) / 2.0f;
                        Rectangle btnR = { btnX, btnY, btnW, btnH };
                        bool bHov  = isHover && CheckCollisionPointRec(GetMousePosition(), btnR);
                        const char *bt = "Chi tiết";
                        Vector2 bts = MeasureTextEx(font, bt, fzBtn, 1); 

                        DrawRectangle((int)cx, (int)rowY, 1, (int)rowH, COL_BORDER);
                        DrawRectangleRounded(btnR, 0.4f, 8, bHov ? GetColor(0x4A80FFFF) : GetColor(0x2A50AACC));
                        DrawTextEx(font, bt, (Vector2){ btnX + (btnW-bts.x)/2, btnY + (btnH-bts.y)/2 }, fzBtn, 1, WHITE);

                        if (!Form->showmodal && bHov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            strcpy(Form->dachonmathe, cur->maThe);
                            Form->showmodal    = true;
                            Form->modalscroll  = 0;
                            Form->modal_sb_dragging = false;
                        }
                    }
                }
                rowY += rowH;
            }
            matchIdx++;
            cur = cur->next;
        }
    }
    EndScissorMode();

    // ─── 3. VẼ THANH CUỘN (SCROLLBAR) ──────────
    if (contentH > viewportH) {
        float trackX = tableX + tableW + 6.0f;
        float trackH = bodyH;
        Rectangle trackR = { trackX, bodyTop, sbW, trackH };
        float tRatio = viewportH / contentH;
        float thumbH, thumbY;
        Rectangle thumbHitbox;
        bool tHov;
        Color thumbColor;

        DrawRectangleRounded(trackR, 0.5f, 6, GetColor(0x1b264f80));

        if (tRatio > 1.0f) tRatio = 1.0f;
        thumbH = trackH * tRatio;
        if (thumbH < 30.0f) thumbH = 30.0f;
        
        thumbY = (maxScroll > 0) ? bodyTop + (Form->scroll / maxScroll) * (trackH - thumbH) : bodyTop;

        thumbHitbox = (Rectangle){ trackX - 10, thumbY, sbW + 20, thumbH };
        tHov = !Form->showmodal && CheckCollisionPointRec(GetMousePosition(), thumbHitbox);
        
        thumbColor = GetColor(0x4A80FF99);
        if (Form->scrollbar_dragging) thumbColor = GetColor(0x4A80FFFF);
        else if (tHov) thumbColor = GetColor(0x4A80FFCC);

        DrawRectangleRounded((Rectangle){ trackX, thumbY, sbW, thumbH }, 0.5f, 6, thumbColor);

        if (Form->scrollbar_dragging) {
            if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                Form->scrollbar_dragging = false;
            } else {
                float ny = GetMousePosition().y - Form->scrollbar_drag_startY;
                float r  = (ny - bodyTop) / (trackH - thumbH);
                if (r < 0) r = 0;
                if (r > 1) r = 1;
                Form->scroll = r * maxScroll;
            }
        } else {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !Form->showmodal) {
                if (tHov) {
                    Form->scrollbar_dragging   = true;
                    Form->scrollbar_drag_startY = GetMousePosition().y - thumbY;
                } else if (CheckCollisionPointRec(GetMousePosition(), trackR)) {
                    if (GetMousePosition().y < thumbY) Form->scroll -= viewportH;
                    else Form->scroll += viewportH;
                    if (Form->scroll < 0) Form->scroll = 0;
                    if (Form->scroll > maxScroll) Form->scroll = maxScroll;
                }
            }
        }
    } else {
        // Mờ nhẹ thanh cuộn nếu danh sách ngắn hơn 12 dòng
        DrawRectangleRounded((Rectangle){ tableX + tableW + 6.0f, bodyTop, sbW, bodyH }, 0.5f, 6, GetColor(0x4A80FF44));
    }

    // ─── 4. VẼ THANH CHUYỂN TRANG ──────────
    if (matchCount > 0) {
        float pageY = bodyTop + bodyH + (20.0f * uiScale);
        char pageText[64];
        float btnW = 50.0f * uiScale;
        float btnH = 40.0f * uiScale;
        float pageTextW;
        float centerX = screenW / 2.0f;
        Rectangle btnPrev, btnNext;
        bool hoverPrev, hoverNext;

        sprintf(pageText, "Trang %d / %d", currentPage, maxPages == 0 ? 1 : maxPages);
        pageTextW = MeasureTextEx(font, pageText, fzPage, 1).x;
        
        btnPrev = (Rectangle){ centerX - pageTextW/2.0f - btnW - (25.0f*uiScale), pageY, btnW, btnH };
        btnNext = (Rectangle){ centerX + pageTextW/2.0f + (25.0f*uiScale), pageY, btnW, btnH };
        
        hoverPrev = !Form->showmodal && CheckCollisionPointRec(GetMousePosition(), btnPrev);
        hoverNext = !Form->showmodal && CheckCollisionPointRec(GetMousePosition(), btnNext);
        
        DrawRectangleRounded(btnPrev, 0.3f, 8, hoverPrev ? GetColor(0x4A80FFFF) : GetColor(0x4A80FF88));
        DrawTextEx(font, "<", (Vector2){btnPrev.x + btnW/2 - 8, btnPrev.y + btnH/2 - fzPage/2}, fzPage, 1, WHITE);
        
        DrawTextEx(font, pageText, (Vector2){centerX - pageTextW/2.0f, pageY + btnH/2 - fzPage/2}, fzPage, 1, WHITE);
        
        DrawRectangleRounded(btnNext, 0.3f, 8, hoverNext ? GetColor(0x4A80FFFF) : GetColor(0x4A80FF88));
        DrawTextEx(font, ">", (Vector2){btnNext.x + btnW/2 - 6, btnNext.y + btnH/2 - fzPage/2}, fzPage, 1, WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !Form->showmodal) {
            if (hoverPrev && currentPage > 1) {
                currentPage--;
                Form->scroll = 0; 
            }
            if (hoverNext && currentPage < maxPages) {
                currentPage++;
                Form->scroll = 0;
            }
        }
    }

    if (matchCount == 0 && Form->nhap.letterCount > 0) {
        const char *msg = "Không tìm thấy kết quả phù hợp.";
        Vector2 ms = MeasureTextEx(font, msg, fzMsg, 1);
        DrawTextEx(font, msg, (Vector2){ (screenW-ms.x)/2, bodyTop + 60 }, fzMsg, 1, GetColor(0xFF6B6BFF));
    }
}

// ==========================================
// VẼ MODAL CHI TIẾT
// ==========================================
void DrawModalChiTiet(FormTimKiemThe *Form, BanDoc *head, Font font) {
    BanDoc *the;
    float screenW, screenH, mW, mH, mX, mY, titleBarH, padX, contentStartY, contentAreaH, sbWM, innerW;
    char modalTitle[64];
    Vector2 mtSize, xs;
    Rectangle btnX;
    bool xHov;
    float lineH, labelH, fieldH, sectionGap, totalContentH, maxMScroll;
    
    typedef struct { const char *label; const char *value; } Field;
    Field fields[5];
    int nFields = 5;
    int i;

    if (!Form->showmodal) return;

    the = head;
    while (the && strcmp(the->maThe, Form->dachonmathe) != 0) the = the->next;
    if (!the) { Form->showmodal = false; return; }

    screenW = (float)GetScreenWidth();
    screenH = (float)GetScreenHeight();
    float uiScale = screenW / 1440.0f;
    if (uiScale > 1.0f) uiScale = 1.0f;
    if (uiScale < 0.6f) uiScale = 0.6f;

    // Phóng to Font size modal
    float fzModTitle = 27.0f * uiScale;
    float fzModLbl   = 20.0f * uiScale;
    float fzModVal   = 24.0f * uiScale;
    float fzModX     = 22.0f * uiScale;

    DrawRectangle(0, 0, (int)screenW, (int)screenH, GetColor(0x00000099));

    mW = screenW * 0.52f;
    if (mW < 480.0f) mW = 480.0f;
    if (mW > 800.0f) mW = 800.0f;
    
    titleBarH = 60.0f * uiScale;
    labelH   = 28.0f * uiScale;
    fieldH   = 48.0f * uiScale;
    sectionGap = 16.0f * uiScale;
    totalContentH = (float)nFields * (labelH + fieldH + sectionGap) + 20.0f;
    
    mH = titleBarH + 20.0f + totalContentH;
    float maxM = screenH * 0.85f;
    if (mH > maxM) mH = maxM;

    mX = (screenW - mW) / 2.0f;
    mY = (screenH - mH) / 2.0f;

    DrawRectangleRounded((Rectangle){ mX, mY, mW, mH }, 0.05f, 12, GetColor(0x0d1b3eF2));
    DrawRectangleRoundedLines((Rectangle){ mX, mY, mW, mH }, 0.05f, 12, GetColor(0x4A80FFAA));

    snprintf(modalTitle, sizeof(modalTitle), "THÔNG TIN THẺ BẠN ĐỌC");
    mtSize = MeasureTextEx(font, modalTitle, fzModTitle, 1);
    
    DrawRectangleRounded((Rectangle){ mX, mY, mW, titleBarH }, 0.05f, 12, GetColor(0x1b264fFF));
    DrawRectangle((int)mX, (int)(mY + titleBarH - 2), (int)mW, 2, GetColor(0x4A80FFFF));
    DrawTextEx(font, modalTitle,
               (Vector2){ mX + (mW - mtSize.x)/2.0f, mY + (titleBarH - mtSize.y)/2.0f },
               fzModTitle, 1, WHITE);

    float btnXSize = 36.0f * uiScale;
    btnX = (Rectangle){ mX + mW - btnXSize - 12, mY + (titleBarH - btnXSize)/2.0f, btnXSize, btnXSize };
    xHov = CheckCollisionPointRec(GetMousePosition(), btnX);
    
    DrawRectangleRounded(btnX, 0.4f, 8, xHov ? GetColor(0xFF4444FF) : GetColor(0xFFFFFF22));
    xs = MeasureTextEx(font, "X", fzModX, 1);
    DrawTextEx(font, "X", (Vector2){ btnX.x + (btnX.width-xs.x)/2, btnX.y + (btnX.height-xs.y)/2 }, fzModX, 1, WHITE);
    
    if (xHov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Form->showmodal = false;
        Form->modal_sb_dragging = false;
        return;
    }

    padX    = 40.0f * uiScale;
    contentStartY = mY + titleBarH + 10.0f;
    contentAreaH  = mH - titleBarH - 20.0f; 
    sbWM = 10.0f;
    innerW = mW - padX * 2.0f - sbWM - 6.0f;

    fields[0].label = "Mã thẻ bạn đọc:"; fields[0].value = the->maThe;
    fields[1].label = "Họ và tên:";       fields[1].value = the->hoTen;
    fields[2].label = "Số điện thoại:";   fields[2].value = the->sdt;
    fields[3].label = "Số CCCD:";         fields[3].value = the->cccd;
    fields[4].label = "Hạn sử dụng:";     fields[4].value = the->hanSD;

    maxMScroll = totalContentH - contentAreaH;
    if (maxMScroll < 0) maxMScroll = 0;
    if (Form->modalscroll < 0) Form->modalscroll = 0;
    if (Form->modalscroll > maxMScroll) Form->modalscroll = maxMScroll;

    BeginScissorMode((int)(mX + 1), (int)contentStartY, (int)(mW - 2), (int)contentAreaH);
    {
        float curY = contentStartY - Form->modalscroll + 10.0f;
        for (i = 0; i < nFields; i++) {
            Rectangle fbox;
            Vector2 vs;
            DrawTextEx(font, fields[i].label, (Vector2){ mX + padX, curY }, fzModLbl, 1, GetColor(0x90B8FFFF));
            curY += labelH;

            fbox = (Rectangle){ mX + padX, curY, innerW, fieldH };
            DrawRectangleRounded(fbox, 0.2f, 8, GetColor(0xFFFFFF12));
            DrawRectangleRoundedLines(fbox, 0.2f, 8, GetColor(0x4A80FF66));
            
            vs = MeasureTextEx(font, fields[i].value, fzModVal, 1);
            DrawTextEx(font, fields[i].value, (Vector2){ fbox.x + 16.0f, fbox.y + (fieldH - vs.y)/2.0f }, fzModVal, 1, WHITE);
            curY += fieldH + sectionGap;
        }
    }
    EndScissorMode();

    if (totalContentH > contentAreaH) {
        float stX = mX + mW - sbWM - 4.0f;
        float stY = contentStartY;
        float stH = contentAreaH;
        float tR, tH, tY;
        Rectangle mThumb, mThumbHitbox;
        bool mHov;

        DrawRectangleRounded((Rectangle){ stX, stY, sbWM, stH }, 0.5f, 6, GetColor(0x1b264f80));

        tR = contentAreaH / totalContentH;
        if (tR > 1.0f) tR = 1.0f;
        tH = stH * tR;
        if (tH < 30.0f) tH = 30.0f;
        
        tY = (maxMScroll > 0) ? stY + (Form->modalscroll / maxMScroll) * (stH - tH) : stY;

        mThumb = (Rectangle){ stX, tY, sbWM, tH };
        mThumbHitbox = (Rectangle){ stX - 10, tY, sbWM + 20, tH };
        mHov = CheckCollisionPointRec(GetMousePosition(), mThumbHitbox);
        
        DrawRectangleRounded(mThumb, 0.5f, 6,
            (Form->modal_sb_dragging || mHov) ? GetColor(0x4A80FFFF) : GetColor(0x4A80FF99));

        if (Form->modal_sb_dragging) {
            if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                Form->modal_sb_dragging = false;
            } else {
                float ny = GetMousePosition().y - Form->modal_sb_dragStartY;
                float r  = (ny - stY) / (stH - tH);
                if (r < 0) r = 0;
                if (r > 1) r = 1;
                Form->modalscroll = r * maxMScroll;
            }
        } else {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && mHov) {
                Form->modal_sb_dragging   = true;
                Form->modal_sb_dragStartY = GetMousePosition().y - tY;
            }
        }
    }
}

// ==========================================
// HÀM VẼ CHÍNH
// ==========================================
void DrawTimKiemThe(FormTimKiemThe *Form, Texture2D icons[], Font font, BanDoc *head) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float uiScale = screenW / 1440.0f;
    if (uiScale > 1.0f) uiScale = 1.0f;
    if (uiScale < 0.6f) uiScale = 0.6f;

    const char *titleText = "TÌM KIẾM THẺ BẠN ĐỌC";
    float titleSize = 46.0f * uiScale; // To hơn
    float fzSearch  = 30.0f * uiScale; // Ô input to hơn
    float fzBack    = 24.0f * uiScale; // Nút quay lại to hơn

    Vector2 mT, tPos, tsz, tp;
    InputBox_BD *box;
    float toff;
    Rectangle btnBack;
    bool bkHov;

    ClearBackground(GetColor(0x080f1eFF));
    DrawBackgroundParticles();
    DrawRectangle(0, 0, (int)screenW, (int)screenH, GetColor(0x1b264f80));

    float headerAreaH = 125.0f * uiScale;
    DrawRectangleGradientEx((Rectangle){ 0, 0, screenW, headerAreaH },
        GetColor(0x1b264fCC), GetColor(0x243580CC), GetColor(0x243580CC), GetColor(0x1b264fCC));
    DrawRectangle(0, (int)headerAreaH - 2, (int)screenW, 2, GetColor(0x4A80FF70));

    mT = MeasureTextEx(font, titleText, titleSize, 1);
    tPos = (Vector2){ (screenW - mT.x)/2.0f, (headerAreaH - mT.y)/2.0f };
    DrawTextEx(font, titleText, (Vector2){ tPos.x+2, tPos.y+2 }, titleSize, 1, GetColor(0x00000060));
    DrawTextEx(font, titleText, tPos, titleSize, 1, WHITE);

    box = &Form->nhap;
    DrawRectangleRounded((Rectangle){ box->rec.x-10, box->rec.y-8, box->rec.width+20, box->rec.height+16 },
                         0.3f, 10, GetColor(0x1b264fAA));
    DrawRectangleRounded(box->rec, 0.25f, 10, GetColor(0xFFFFFF15));
    DrawRectangleRoundedLines(box->rec, 0.25f, 10,
        box->isFocused ? GetColor(0x4A80FFFF) : GetColor(0x4A80FF77));

    tsz = MeasureTextEx(font, box->text, fzSearch, 1);
    toff = (tsz.x > box->rec.width - 24) ? (tsz.x - (box->rec.width - 24)) : 0;

    BeginScissorMode((int)box->rec.x+5, (int)box->rec.y, (int)box->rec.width-10, (int)box->rec.height);
    tp = (Vector2){ box->rec.x + 16 - toff, box->rec.y + (box->rec.height - fzSearch)/2 };
    
    if (box->letterCount == 0 && !box->isFocused)
        DrawTextEx(font, "Tìm theo tên, SĐT hoặc CCCD...", (Vector2){ box->rec.x + 16, tp.y }, fzSearch, 1, GetColor(0xFFFFFF44));
    else
        DrawTextEx(font, box->text, tp, fzSearch, 1, WHITE);
        
    if (box->isFocused && !Form->showmodal && (((int)(GetTime()*1.5f))%2==0))
        DrawRectangleV((Vector2){ tp.x + tsz.x + 2, tp.y + 2 }, (Vector2){ 2, fzSearch-4 }, WHITE);
    EndScissorMode();

    DrawKetQuaTimKiem(head, Form, font);
    DrawModalChiTiet(Form, head, font);

    float btnBackW = 130.0f * uiScale;
    float btnBackH = 44.0f * uiScale;
    btnBack = (Rectangle){ 20, 25, btnBackW, btnBackH };
    bkHov = CheckCollisionPointRec(GetMousePosition(), btnBack);
    
    DrawRectangleRounded(btnBack, 0.35f, 10, bkHov ? GetColor(0xFF4444FF) : GetColor(0xFFFFFF20));
    DrawRectangleRoundedLines(btnBack, 0.35f, 10, GetColor(0xFFFFFF44));
    DrawTextEx(font, "< Quay lại", (Vector2){ btnBack.x + (btnBackW * 0.1f), btnBack.y + (btnBackH - fzBack)/2.0f }, fzBack, 1, WHITE);
}