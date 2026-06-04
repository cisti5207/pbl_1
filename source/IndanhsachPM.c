#include "InDanhSachPM.h"
#include "TimkiemLSphieumuon.h"
#include "menu.h"
#include "libmanage.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>

// ============================================================
// State nội bộ
// ============================================================
static char  s_searchFilter[256] = {0};
static int   s_searchFilterLen   = 0;
static bool  s_searchFocused     = false;
static float s_searchBSCounter   = 0.0f;

static bool  s_sb_dragging       = false;
static float s_sb_dragStartY     = 0.0f;

// Modal chi tiết
static bool  s_showModal         = false;
static char  s_modalMaPM[16]     = {0};
static float s_modalScroll       = 0.0f;
static bool  s_modal_sb_drag     = false;

// Phân trang
static int   s_currentPage       = 1;
static float s_tableScroll       = 0.0f;

// ============================================================
// Màu bảng
// ============================================================
static Color PM_COL_HEADER_BG = { 20,  30,  75, 240 };
static Color PM_COL_ROW_EVEN  = { 27,  38,  79, 130 };
static Color PM_COL_ROW_ODD   = { 18,  26,  60, 150 };
static Color PM_COL_ROW_HOVER = { 55,  90, 175, 200 };
static Color PM_COL_BORDER    = { 90, 130, 210,  55 };

#define PM_HEADER_H 60.0f
#define PM_ITEMS_PER_PAGE 30
#define PM_NUM_COLS 8

// Tỉ lệ cột (tổng = 1.0f): maPM | maThe | tenTruyen | maTruyen | ngayMuon | soNgay | trangThai | chiTiet
static float s_colRatio[PM_NUM_COLS] = { 0.10f, 0.10f, 0.24f, 0.12f, 0.11f, 0.09f, 0.12f, 0.12f };
static const char *s_colHeaders[PM_NUM_COLS] = {
    "Mã PM", "Mã thẻ", "Tên truyện", "Mã truyện",
    "Ngày mượn", "Số ngày mượn", "Trạng thái", "Chi tiết"
};

static float PM_ColX(float tableX, float tableW, int col) {
    float x = tableX;
    for (int i = 0; i < col; i++) x += tableW * s_colRatio[i];
    return x;
}

// ============================================================
// Kiểm tra khớp tìm kiếm
// ============================================================
static bool KhopTimKiemPM(PhieuMuonNode *pm, const char *filter) {
    if (!filter || filter[0] == '\0') return true;
    if (StringContains(pm->maPM,      filter)) return true;
    if (StringContains(pm->mathe,     filter)) return true;
    if (StringContains(pm->tentruyen, filter)) return true;
    if (StringContains(pm->matruyen,  filter)) return true;
    if (StringContains(pm->ngaymuon,  filter)) return true;
    return false;
}

// ============================================================
// Init
// ============================================================
void InitFormInDanhSachPM(FormInDanhSachPM *form) {
    form->currentTab     = VIEW_TAT_CA;
    form->scrollList     = 0.0f;
    memset(s_searchFilter, 0, sizeof(s_searchFilter));
    s_searchFilterLen    = 0;
    s_searchFocused      = false;
    s_searchBSCounter    = 0.0f;
    s_sb_dragging        = false;
    s_sb_dragStartY      = 0.0f;
    s_showModal          = false;
    s_modalMaPM[0]       = '\0';
    s_modalScroll        = 0.0f;
    s_modal_sb_drag      = false;
    s_currentPage        = 1;
    s_tableScroll        = 0.0f;
}

// ============================================================
// Update logic
// ============================================================
void UpdateLogicInDanhSachPM(FormInDanhSachPM *form, PhieuMuonNode *headPM, int *currentState) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

    // Hệ số Scale cho phép co dãn UI khi thu nhỏ cửa sổ
    float uiScale = screenW / 1440.0f;
    if (uiScale > 1.0f) uiScale = 1.0f;
    if (uiScale < 0.55f) uiScale = 0.55f;

    // --- Nút quay lại ---
    float btnBackW = 130.0f * uiScale;
    float btnBackH = 44.0f * uiScale;
    Rectangle btnBack = { 14, 28, btnBackW, btnBackH };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    if (IsKeyPressed(KEY_ESCAPE) || (isHoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
        InitFormInDanhSachPM(form);
        *currentState = APP_MENU;
        return;
    }

    // --- Scroll modal nếu đang mở ---
    if (s_showModal) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0) s_modalScroll -= wheel * 45.0f;
        if (s_modalScroll < 0) s_modalScroll = 0;
        return;
    }

    // --- Layout header ---
    float rowTopY = 70.0f;
    float tabW    = 180.0f * uiScale;
    float tabH    = 50.0f * uiScale;
    float searchW = 440.0f * uiScale;
    float gap     = 16.0f * uiScale;
    float totalRowW = tabW + gap + tabW + gap + searchW;
    float rowStartX = (screenW - totalRowW) / 2.0f;

    Rectangle tabTatCa   = { rowStartX,                  rowTopY, tabW,    tabH };
    Rectangle tabChuaTra = { rowStartX + tabW + gap,     rowTopY, tabW,    tabH };
    Rectangle searchBox  = { rowStartX + tabW*2 + gap*2, rowTopY, searchW, tabH };

    bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    Vector2 mp = GetMousePosition();

    // --- Click tab ---
    if (mousePressed) {
        if (CheckCollisionPointRec(mp, tabTatCa)) {
            form->currentTab = VIEW_TAT_CA;
            s_currentPage = 1; s_tableScroll = 0;
        }
        if (CheckCollisionPointRec(mp, tabChuaTra)) {
            form->currentTab = VIEW_CHUA_TRA;
            s_currentPage = 1; s_tableScroll = 0;
        }
        s_searchFocused = CheckCollisionPointRec(mp, searchBox);
    }

    // --- Input tìm kiếm ---
    if (s_searchFocused) {
        int key = GetKeyPressed();
        while (key > 0) {
            if (key == KEY_BACKSPACE && s_searchFilterLen > 0) {
                do { s_searchFilterLen--; }
                while (s_searchFilterLen > 0 && (s_searchFilter[s_searchFilterLen] & 0xC0) == 0x80);
                s_searchFilter[s_searchFilterLen] = '\0';
                s_currentPage = 1; s_tableScroll = 0;
            }
            key = GetKeyPressed();
        }
        if (IsKeyDown(KEY_BACKSPACE)) {
            s_searchBSCounter += GetFrameTime();
            if (s_searchBSCounter >= 0.1f && s_searchFilterLen > 0) {
                do { s_searchFilterLen--; }
                while (s_searchFilterLen > 0 && (s_searchFilter[s_searchFilterLen] & 0xC0) == 0x80);
                s_searchFilter[s_searchFilterLen] = '\0';
                s_currentPage = 1; s_tableScroll = 0;
            }
        } else s_searchBSCounter = 0.0f;

        int ch = GetCharPressed();
        while (ch > 0) {
            if (ch >= 32 && s_searchFilterLen < 250) {
                int bsz = 0;
                const char *u8 = CodepointToUTF8(ch, &bsz);
                if (s_searchFilterLen + bsz < 255) {
                    for (int j = 0; j < bsz; j++) s_searchFilter[s_searchFilterLen++] = u8[j];
                    s_searchFilter[s_searchFilterLen] = '\0';
                    s_currentPage = 1; s_tableScroll = 0;
                }
            }
            ch = GetCharPressed();
        }
        if (IsKeyPressed(KEY_ESCAPE)) s_searchFocused = false;
    }

    // --- Đếm kết quả để tính scroll ---
    float listTop  = 136.0f; 
    float tableX   = screenW * 0.015f;
    float tableW   = screenW * 0.955f;
    float bodyTop  = listTop + PM_HEADER_H;
    
    // Khung bảng kéo dài đến đáy (chừa 60px cho nút chuyển trang)
    float bodyH    = screenH - bodyTop - 60.0f; 
    if (bodyH < 240.0f) bodyH = 240.0f; 

    // Chia đều 12 dòng
    float rowH = bodyH / 12.0f;

    int matchCount = 0;
    PhieuMuonNode *tmp = headPM;
    while (tmp) {
        bool tabOK = (form->currentTab == VIEW_TAT_CA) ||
                     (form->currentTab == VIEW_CHUA_TRA && tmp->trangthai == 0);
        if (tabOK && KhopTimKiemPM(tmp, s_searchFilter)) matchCount++;
        tmp = tmp->next;
    }

    int maxPages  = (matchCount + PM_ITEMS_PER_PAGE - 1) / PM_ITEMS_PER_PAGE;
    if (maxPages < 1) maxPages = 1;
    if (s_currentPage > maxPages) s_currentPage = maxPages;
    if (s_currentPage < 1)        s_currentPage = 1;

    int itemsOnPage = matchCount - (s_currentPage - 1) * PM_ITEMS_PER_PAGE;
    if (itemsOnPage > PM_ITEMS_PER_PAGE) itemsOnPage = PM_ITEMS_PER_PAGE;
    if (itemsOnPage < 0) itemsOnPage = 0;

    float contentH  = (float)itemsOnPage * rowH;
    float maxScroll = contentH - bodyH;
    if (maxScroll < 0) maxScroll = 0;

    // Scroll chuột
    float wheel = GetMouseWheelMove();
    if (wheel != 0) s_tableScroll -= wheel * 45.0f;
    if (s_tableScroll < 0) s_tableScroll = 0;
    if (s_tableScroll > maxScroll) s_tableScroll = maxScroll;

    // --- Scrollbar kéo thả ---
    float sbW     = 10.0f;
    float trackX  = tableX + tableW + 4.0f;

    if (contentH > bodyH) {
        float trackH   = bodyH;
        float tRatio   = bodyH / contentH;
        if (tRatio > 1.0f) tRatio = 1.0f;
        float thumbH   = trackH * tRatio;
        if (thumbH < 30.0f) thumbH = 30.0f;
        float thumbY   = (maxScroll > 0) ? bodyTop + (s_tableScroll / maxScroll) * (trackH - thumbH) : bodyTop;

        Rectangle thumbHB = { trackX - 10, thumbY, sbW + 20, thumbH };
        Rectangle trackR  = { trackX, bodyTop, sbW, trackH };

        if (s_sb_dragging) {
            if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                s_sb_dragging = false;
            } else {
                float ny = mp.y - s_sb_dragStartY;
                float r  = (ny - bodyTop) / (trackH - thumbH);
                if (r < 0) r = 0; 
                if (r > 1) r = 1;
                s_tableScroll = r * maxScroll;
            }
        } else if (mousePressed) {
            if (CheckCollisionPointRec(mp, thumbHB)) {
                s_sb_dragging   = true;
                s_sb_dragStartY = mp.y - thumbY;
            } else if (CheckCollisionPointRec(mp, trackR)) {
                if (mp.y < thumbY) s_tableScroll -= bodyH;
                else               s_tableScroll += bodyH;
                if (s_tableScroll < 0) s_tableScroll = 0;
                if (s_tableScroll > maxScroll) s_tableScroll = maxScroll;
            }
        }
    }

    // --- Phân trang ---
    if (mousePressed) {
        float pageY   = bodyTop + bodyH + 15.0f;
        float centerX = tableX + tableW / 2.0f;
        char pageText[32];
        snprintf(pageText, sizeof(pageText), "Trang %d / %d", s_currentPage, maxPages);
        
        float fzPage = 23.0f * uiScale; 
        float ptW    = MeasureText(pageText, (int)fzPage);
        float btnW   = 50.0f * uiScale, btnH = 40.0f * uiScale;
        Rectangle btnPrev = { centerX - ptW/2.0f - btnW - 25.0f, pageY, btnW, btnH };
        Rectangle btnNext = { centerX + ptW/2.0f + 25.0f,        pageY, btnW, btnH };
        if (CheckCollisionPointRec(mp, btnPrev) && s_currentPage > 1) {
            s_currentPage--; s_tableScroll = 0;
        }
        if (CheckCollisionPointRec(mp, btnNext) && s_currentPage < maxPages) {
            s_currentPage++; s_tableScroll = 0;
        }
    }
}

// ============================================================
// Vẽ Modal Chi Tiết phiếu mượn
// ============================================================
static void DrawModalPM(PhieuMuonNode *headPM, Font font) {
    if (!s_showModal) return;

    PhieuMuonNode *pm = headPM;
    while (pm && strcmp(pm->maPM, s_modalMaPM) != 0) pm = pm->next;
    if (!pm) { s_showModal = false; return; }

    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

    float uiScale = screenW / 1440.0f;
    if (uiScale > 1.0f) uiScale = 1.0f;
    if (uiScale < 0.6f) uiScale = 0.6f;

    DrawRectangle(0, 0, (int)screenW, (int)screenH, GetColor(0x00000099));

    float titleBarH  = 64.0f * uiScale;
    float padX       = 36.0f * uiScale;
    float labelH     = 28.0f * uiScale;
    float fieldH     = 48.0f * uiScale;
    float sectionGap = 12.0f * uiScale;
    int   nFields    = 7;
    float totalContentH = (float)nFields * (labelH + fieldH + sectionGap) + 20.0f;

    float mW = screenW * 0.52f;
    if (mW < 460.0f) mW = 460.0f;
    if (mW > 860.0f) mW = 860.0f;
    
    float mH = titleBarH + 18.0f + totalContentH;
    float maxModalH = screenH * 0.90f;
    if (mH > maxModalH) mH = maxModalH;
    float mX = (screenW - mW) / 2.0f;
    float mY = (screenH - mH) / 2.0f;

    // Nền modal
    DrawRectangleRounded((Rectangle){ mX, mY, mW, mH }, 0.05f, 12, GetColor(0x0d1b3eF2));
    DrawRectangleRoundedLines((Rectangle){ mX, mY, mW, mH }, 0.05f, 12, GetColor(0x4A80FFAA));

    // Title bar — vẽ bằng DrawRectangle thường để không đè lên góc bo của modal
    // Phần bo tròn 2 góc trên đã có từ DrawRectangleRounded của nền modal bên trên
    float cornerR = mW * 0.05f * 0.5f;
    // Vẽ phần thân title bar (bỏ qua vùng góc bo trên)
    DrawRectangle((int)(mX + cornerR), (int)mY, (int)(mW - cornerR * 2), (int)titleBarH, GetColor(0x1b264fFF));
    // Che 2 bên góc trên (hình chữ nhật đứng sát góc)
    DrawRectangle((int)mX, (int)(mY + cornerR), (int)cornerR, (int)(titleBarH - cornerR), GetColor(0x1b264fFF));
    DrawRectangle((int)(mX + mW - cornerR), (int)(mY + cornerR), (int)(cornerR + 1), (int)(titleBarH - cornerR), GetColor(0x1b264fFF));
    // Đường kẻ dưới title bar
    DrawRectangle((int)mX, (int)(mY + titleBarH - 2), (int)mW, 2, GetColor(0x4A80FFFF));

    float fzModTitle = 27.0f * uiScale;
    float fzModX     = 21.0f * uiScale;
    float fzModLbl   = 19.0f * uiScale;
    float fzModVal   = 23.0f * uiScale;

    const char *modalTitle = "CHI TIẾT PHIẾU MƯỢN";
    Vector2 mtSz = MeasureTextEx(font, modalTitle, fzModTitle, 1);
    DrawTextEx(font, modalTitle,
               (Vector2){ mX + (mW - mtSz.x)/2.0f, mY + (titleBarH - mtSz.y)/2.0f },
               fzModTitle, 1, WHITE);

    // Nút X
    float btnXSize = 32.0f * uiScale;
    Rectangle btnX = { mX + mW - btnXSize - 10, mY + (titleBarH - btnXSize)/2.0f, btnXSize, btnXSize };
    bool xHov = CheckCollisionPointRec(GetMousePosition(), btnX);
    DrawRectangleRounded(btnX, 0.4f, 8, xHov ? GetColor(0xFF4444FF) : GetColor(0xFFFFFF22));
    Vector2 xSz = MeasureTextEx(font, "X", fzModX, 1);
    DrawTextEx(font, "X", (Vector2){ btnX.x + (btnX.width - xSz.x)/2, btnX.y + (btnX.height - xSz.y)/2 }, fzModX, 1, WHITE);
    if (xHov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        s_showModal = false; s_modal_sb_drag = false; return;
    }

    float contentStartY = mY + titleBarH + 10.0f;
    float contentAreaH  = mH - titleBarH - 18.0f;
    float innerW        = mW - padX * 2.0f - 12.0f;

    typedef struct { const char *label; const char *value; } Field;
    Field fields[7];
    const char *trangThaiStr = (pm->trangthai == 1) ? "Đã trả" : "Chưa trả";
    fields[0].label = "Mã phiếu mượn:";  fields[0].value = pm->maPM;
    fields[1].label = "Mã thẻ bạn đọc:"; fields[1].value = pm->mathe;
    fields[2].label = "Tên truyện:";     fields[2].value = pm->tentruyen;
    fields[3].label = "Mã truyện:";      fields[3].value = pm->matruyen;
    fields[4].label = "Ngày mượn:";      fields[4].value = pm->ngaymuon;
    fields[5].label = "Số ngày mượn:";   fields[5].value = pm->ngaytra;
    fields[6].label = "Trạng thái:";     fields[6].value = trangThaiStr;

    s_modalScroll = 0; 

    BeginScissorMode((int)(mX + 1), (int)contentStartY, (int)(mW - 2), (int)contentAreaH);
    {
        float curY = contentStartY + 10.0f;
        for (int i = 0; i < nFields; i++) {
            DrawTextEx(font, fields[i].label, (Vector2){ mX + padX, curY }, fzModLbl, 1, GetColor(0x90B8FFFF));
            curY += labelH;

            Rectangle fbox = { mX + padX, curY, innerW, fieldH };
            DrawRectangleRounded(fbox, 0.2f, 8, GetColor(0xFFFFFF12));
            DrawRectangleRoundedLines(fbox, 0.2f, 8, GetColor(0x4A80FF66));

            Color valColor = WHITE;
            if (i == 6) valColor = (pm->trangthai == 1) ? GREEN : GetColor(0xFF6B6BFF);

            Vector2 vs = MeasureTextEx(font, fields[i].value, fzModVal, 1);
            DrawTextEx(font, fields[i].value,
                       (Vector2){ fbox.x + 12.0f, fbox.y + (fieldH - vs.y)/2.0f },
                       fzModVal, 1, valColor);
            curY += fieldH + sectionGap;
        }
    }
    EndScissorMode();
}

// ============================================================
// Vẽ giao diện chính
// ============================================================
void DrawGiaoDienInDanhSachPM(FormInDanhSachPM *form, PhieuMuonNode *headPM, Font font) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

    // Scale động dựa trên chiều rộng màn hình (Responsive)
    float uiScale = screenW / 1440.0f;
    if (uiScale > 1.0f) uiScale = 1.0f;
    if (uiScale < 0.55f) uiScale = 0.55f;

    // Giảm 3 size font cơ bản và áp dụng Scale 
    float fzTitle  = 37.0f * uiScale;
    float fzTab    = 23.0f * uiScale;
    float fzSearch = 21.0f * uiScale;
    float fzBack   = 19.0f * uiScale;
    float fzHeader = 22.0f * uiScale;
    float fzRow    = 22.0f * uiScale;
    float fzBtn    = 19.0f * uiScale;
    float fzPage   = 23.0f * uiScale;
    float fzNoRes  = 29.0f * uiScale;

    // --- Nền ---
    ClearBackground(GetColor(0x080f1eFF));
    DrawBackgroundParticles();
    DrawRectangle(0, 0, (int)screenW, (int)screenH, GetColor(0x1b264f80));

    // --- Layout ---
    float listTop  = 136.0f;  
    float tableX   = screenW * 0.015f;
    float tableW   = screenW * 0.955f;
    float sbW      = 10.0f;
    float bodyTop  = listTop + PM_HEADER_H;
    
    // Khung bảng kéo dài đến đáy (chừa 60px cho nút chuyển trang)
    float bodyH    = screenH - bodyTop - 60.0f;
    if (bodyH < 240.0f) bodyH = 240.0f;

    // Tính toán chiều cao mỗi dòng động để luôn chứa 12 dòng
    float rowH = bodyH / 12.0f;

    // --- Đếm & phân trang ---
    int matchCount = 0;
    PhieuMuonNode *cur = headPM;
    while (cur) {
        bool tabOK = (form->currentTab == VIEW_TAT_CA) ||
                     (form->currentTab == VIEW_CHUA_TRA && cur->trangthai == 0);
        if (tabOK && KhopTimKiemPM(cur, s_searchFilter)) matchCount++;
        cur = cur->next;
    }

    int maxPages   = (matchCount + PM_ITEMS_PER_PAGE - 1) / PM_ITEMS_PER_PAGE;
    if (maxPages < 1) maxPages = 1;
    int startIdx   = (s_currentPage - 1) * PM_ITEMS_PER_PAGE;
    int endIdx     = startIdx + PM_ITEMS_PER_PAGE;
    if (endIdx > matchCount) endIdx = matchCount;
    int itemsOnPage = endIdx - startIdx;

    float contentH  = (float)itemsOnPage * rowH;
    float maxScroll = contentH - bodyH;
    if (maxScroll < 0) maxScroll = 0;

    // ─── Vẽ Body bảng ──────────
    {
        float rowY   = bodyTop - s_tableScroll;
        int matchIdx = 0;
        cur = headPM;
        while (cur) {
            bool tabOK = (form->currentTab == VIEW_TAT_CA) ||
                         (form->currentTab == VIEW_CHUA_TRA && cur->trangthai == 0);
            if (!(tabOK && KhopTimKiemPM(cur, s_searchFilter))) { cur = cur->next; continue; }

            if (matchIdx >= startIdx && matchIdx < endIdx) {
                // Kiểm tra dòng hiển thị
                bool rowVisible = (rowY + rowH > bodyTop) && (rowY < bodyTop + bodyH);
                if (rowVisible) {
                    Rectangle rowRect = { tableX, rowY, tableW, rowH };
                    Color rowBg = (matchIdx % 2 == 0) ? PM_COL_ROW_EVEN : PM_COL_ROW_ODD;
                    bool isHover = !s_showModal &&
                                   CheckCollisionPointRec(GetMousePosition(), rowRect) &&
                                   GetMousePosition().y >= bodyTop &&
                                   GetMousePosition().y <= bodyTop + bodyH;
                    if (isHover) rowBg = PM_COL_ROW_HOVER;

                    float clipY  = (rowY < bodyTop) ? bodyTop : rowY;
                    float clipH  = rowY + rowH - clipY;
                    if (clipY + clipH > bodyTop + bodyH) clipH = bodyTop + bodyH - clipY;
                    BeginScissorMode((int)tableX, (int)clipY, (int)(tableW + sbW + 8), (int)clipH);

                    DrawRectangleRec(rowRect, rowBg);
                    DrawRectangle((int)tableX, (int)(rowY + rowH - 1), (int)tableW, 1, PM_COL_BORDER);

                    const char *vals[7] = {
                        cur->maPM, cur->mathe, cur->tentruyen,
                        cur->matruyen, cur->ngaymuon, cur->ngaytra, ""
                    };
                    for (int c = 0; c < PM_NUM_COLS - 1; c++) {
                        float cx = PM_ColX(tableX, tableW, c);
                        float cw = tableW * s_colRatio[c];
                        if (c > 0) DrawRectangle((int)cx, (int)rowY, 1, (int)rowH, PM_COL_BORDER);

                        if (c == 6) {
                            const char *st  = (cur->trangthai == 1) ? "Đã trả" : "Chưa trả";
                            Color stColor   = (cur->trangthai == 1) ? GetColor(0x00DD88FF) : GetColor(0xFF6B6BFF);
                            Vector2 stSz    = MeasureTextEx(font, st, fzRow, 1);
                            DrawTextEx(font, st,
                                       (Vector2){ cx + (cw - stSz.x)/2.0f, rowY + (rowH - stSz.y)/2.0f },
                                       fzRow, 1, stColor);
                        } else {
                            Vector2 ts = MeasureTextEx(font, vals[c], fzRow, 1);
                            // CĂN GIỮA TEXT TẤT CẢ CÁC CỘT
                            DrawTextEx(font, vals[c],
                                       (Vector2){ cx + (cw - ts.x)/2.0f, rowY + (rowH - ts.y)/2.0f },
                                       fzRow, 1, WHITE);
                        }
                    }

                    // Nút chi tiết
                    {
                        float cx  = PM_ColX(tableX, tableW, PM_NUM_COLS - 1);
                        float cw  = tableW * s_colRatio[PM_NUM_COLS - 1];
                        
                        float btnW = cw * 0.80f;
                        float btnH = rowH * 0.60f;
                        if (btnH > 42.0f) btnH = 42.0f; // Scale mượt
                        
                        float btnX = cx + (cw - btnW) / 2.0f;
                        float btnY = rowY + (rowH - btnH) / 2.0f;
                        Rectangle btnR = { btnX, btnY, btnW, btnH };
                        bool bHov = isHover && CheckCollisionPointRec(GetMousePosition(), btnR);

                        DrawRectangle((int)cx, (int)rowY, 1, (int)rowH, PM_COL_BORDER);
                        DrawRectangleRounded(btnR, 0.4f, 8,
                            bHov ? GetColor(0x4A80FFFF) : GetColor(0x2A50AACC));

                        const char *bt = "Chi tiết";
                        Vector2 bts = MeasureTextEx(font, bt, fzBtn, 1);
                        DrawTextEx(font, bt,
                                   (Vector2){ btnX + (btnW - bts.x)/2, btnY + (btnH - bts.y)/2 },
                                   fzBtn, 1, WHITE);

                        if (!s_showModal && bHov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            strncpy(s_modalMaPM, cur->maPM, 15);
                            s_showModal      = true;
                            s_modalScroll    = 0;
                            s_modal_sb_drag  = false;
                        }
                    }
                    EndScissorMode();
                }
                rowY += rowH;
            }
            matchIdx++;
            cur = cur->next;
        }
    }

    // ─── Scrollbar ────────────────────────────────────────────
    if (contentH > bodyH) {
        float trackX = tableX + tableW + 4.0f;
        float trackH = bodyH;
        Rectangle trackR = { trackX, bodyTop, sbW, trackH };

        float tRatio = bodyH / contentH;
        if (tRatio > 1.0f) tRatio = 1.0f;
        float thumbH = trackH * tRatio;
        if (thumbH < 30.0f) thumbH = 30.0f;
        float thumbY = (maxScroll > 0) ? bodyTop + (s_tableScroll / maxScroll) * (trackH - thumbH) : bodyTop;

        DrawRectangleRounded(trackR, 0.5f, 6, GetColor(0x1b264f80));

        Rectangle thumbHB = { trackX - 10, thumbY, sbW + 20, thumbH };
        bool tHov = !s_showModal && CheckCollisionPointRec(GetMousePosition(), thumbHB);
        Color thumbColor = GetColor(0x4A80FF99);
        if (s_sb_dragging) thumbColor = GetColor(0x4A80FFFF);
        else if (tHov)     thumbColor = GetColor(0x4A80FFCC);

        DrawRectangleRounded((Rectangle){ trackX, thumbY, sbW, thumbH }, 0.5f, 6, thumbColor);
    }

    // ─── Phân trang ───────────────────────────────────────────
    if (matchCount > 0) {
        float pageY   = bodyTop + bodyH + 15.0f;
        float centerX = tableX + tableW / 2.0f;
        char pageText[32];
        snprintf(pageText, sizeof(pageText), "Trang %d / %d", s_currentPage, maxPages);
        float ptW  = MeasureTextEx(font, pageText, fzPage, 1).x;
        float btnW = 50.0f * uiScale, btnH = 40.0f * uiScale;
        Rectangle btnPrev = { centerX - ptW/2.0f - btnW - (25.0f * uiScale), pageY, btnW, btnH };
        Rectangle btnNext = { centerX + ptW/2.0f + (25.0f * uiScale),        pageY, btnW, btnH };

        bool hPrev = !s_showModal && CheckCollisionPointRec(GetMousePosition(), btnPrev);
        bool hNext = !s_showModal && CheckCollisionPointRec(GetMousePosition(), btnNext);

        DrawRectangleRounded(btnPrev, 0.3f, 8, hPrev ? GetColor(0x4A80FFFF) : GetColor(0x4A80FF88));
        DrawTextEx(font, "<", (Vector2){ btnPrev.x + (btnW/2) - 8, btnPrev.y + (btnH/2) - (fzPage/2) }, fzPage, 1, WHITE);

        DrawTextEx(font, pageText, (Vector2){ centerX - ptW/2.0f, pageY + (btnH/2) - (fzPage/2) }, fzPage, 1, WHITE);

        DrawRectangleRounded(btnNext, 0.3f, 8, hNext ? GetColor(0x4A80FFFF) : GetColor(0x4A80FF88));
        DrawTextEx(font, ">", (Vector2){ btnNext.x + (btnW/2) - 6, btnNext.y + (btnH/2) - (fzPage/2) }, fzPage, 1, WHITE);
    }

    if (matchCount == 0) {
        const char *msg = (s_searchFilter[0] != '\0')
            ? "Không tìm thấy kết quả phù hợp."
            : "Không có phiếu mượn nào trong danh sách!";
        Vector2 ms = MeasureTextEx(font, msg, fzNoRes, 1);
        DrawTextEx(font, msg, (Vector2){ (screenW - ms.x)/2, bodyTop + 40 }, fzNoRes, 1, GetColor(0xFF6B6BFF));
    }

    // ─── Header UI ────────────────────────────────────────────
    float uiHeaderH = 132.0f;
    DrawRectangleGradientEx((Rectangle){ 0, 0, screenW, uiHeaderH },
        GetColor(0x1b264fCC), GetColor(0x243580CC), GetColor(0x243580CC), GetColor(0x1b264fCC));
    DrawRectangle(0, (int)(uiHeaderH - 2), (int)screenW, 2, GetColor(0x4A80FF70));
    BeginScissorMode(0, 0, (int)screenW, (int)uiHeaderH);
        DrawBackgroundParticles();
    EndScissorMode();
    DrawRectangle(0, 0, (int)screenW, (int)uiHeaderH, GetColor(0x1b264f60));

    const char *title = "DANH SÁCH PHIẾU MƯỢN THƯ VIỆN";
    Vector2 titleSz   = MeasureTextEx(font, title, fzTitle, 1);
    float titleY = 16.0f;
    DrawTextEx(font, title,
               (Vector2){ (screenW - titleSz.x)/2.0f + 1, titleY + 1 },
               fzTitle, 1, GetColor(0x00000060));
    DrawTextEx(font, title,
               (Vector2){ (screenW - titleSz.x)/2.0f, titleY },
               fzTitle, 1, WHITE);

    // ─── Tab TẤT CẢ & CHƯA TRẢ ───────────────────────────────
    float rowTopY = 70.0f;
    float tabW    = 180.0f * uiScale;
    float tabH    = 50.0f * uiScale;
    float searchW = 440.0f * uiScale;
    float gap     = 16.0f * uiScale;
    float totalRowW = tabW + gap + tabW + gap + searchW;
    float rowStartX = (screenW - totalRowW) / 2.0f;

    Rectangle tabTatCa   = { rowStartX,                  rowTopY, tabW,    tabH };
    Rectangle tabChuaTra = { rowStartX + tabW + gap,     rowTopY, tabW,    tabH };
    Rectangle searchBox  = { rowStartX + tabW*2 + gap*2, rowTopY, searchW, tabH };

    bool hoverTatCa   = CheckCollisionPointRec(GetMousePosition(), tabTatCa);
    bool activeTatCa  = (form->currentTab == VIEW_TAT_CA);
    DrawRectangleRounded(tabTatCa, 0.3f, 10,
        activeTatCa ? GetColor(0x4A80FFFF) : (hoverTatCa ? GetColor(0x4A80FFCC) : GetColor(0xFFFFFF18)));
    if (activeTatCa) DrawRectangleRoundedLines(tabTatCa, 0.3f, 10, WHITE);
    Vector2 ttSz = MeasureTextEx(font, "TẤT CẢ", fzTab, 1);
    DrawTextEx(font, "TẤT CẢ",
               (Vector2){ tabTatCa.x + (tabW - ttSz.x)/2, tabTatCa.y + (tabH - ttSz.y)/2 },
               fzTab, 1, WHITE);

    bool hoverChuaTra  = CheckCollisionPointRec(GetMousePosition(), tabChuaTra);
    bool activeChuaTra = (form->currentTab == VIEW_CHUA_TRA);
    DrawRectangleRounded(tabChuaTra, 0.3f, 10,
        activeChuaTra ? GetColor(0x4A80FFFF) : (hoverChuaTra ? GetColor(0x4A80FFCC) : GetColor(0xFFFFFF18)));
    if (activeChuaTra) DrawRectangleRoundedLines(tabChuaTra, 0.3f, 10, WHITE);
    Vector2 ctSz = MeasureTextEx(font, "CHƯA TRẢ", fzTab, 1);
    DrawTextEx(font, "CHƯA TRẢ",
               (Vector2){ tabChuaTra.x + (tabW - ctSz.x)/2, tabChuaTra.y + (tabH - ctSz.y)/2 },
               fzTab, 1, WHITE);

    // ─── Ô tìm kiếm ──────────────────────────────────────────
    DrawRectangleRounded(searchBox, 0.25f, 10, GetColor(0xFFFFFF15));
    DrawRectangleRoundedLines(searchBox, 0.25f, 10,
        s_searchFocused ? GetColor(0x4A80FFFF) : GetColor(0x4A80FF77));

    DrawTextEx(font, "🔍",
               (Vector2){ searchBox.x + 12, searchBox.y + (tabH - fzSearch)/2 },
               fzSearch, 1, GetColor(0xFFFFFF88));

    float textOffX = 42.0f * uiScale;
    if (s_searchFilterLen == 0 && !s_searchFocused) {
        DrawTextEx(font, "Tìm theo mã PM, mã thẻ, tên truyện...",
                   (Vector2){ searchBox.x + textOffX, searchBox.y + (tabH - fzSearch)/2 },
                   fzSearch, 1, GetColor(0xFFFFFF44));
    } else {
        Vector2 inputSz = MeasureTextEx(font, s_searchFilter, fzSearch, 1);
        float availW    = searchBox.width - textOffX - 10;
        float offX      = (inputSz.x > availW) ? inputSz.x - availW : 0;
        BeginScissorMode((int)(searchBox.x + textOffX), (int)searchBox.y,
                         (int)(searchBox.width - textOffX - 4), (int)searchBox.height);
        DrawTextEx(font, s_searchFilter,
                   (Vector2){ searchBox.x + textOffX - offX, searchBox.y + (tabH - fzSearch)/2 },
                   fzSearch, 1, WHITE);
        if (s_searchFocused && (((int)(GetTime()*1.5f)) % 2 == 0)) {
            DrawRectangleV(
                (Vector2){ searchBox.x + textOffX - offX + inputSz.x + 2,
                           searchBox.y + (tabH - fzSearch)/2 + 2 },
                (Vector2){ 2, fzSearch - 4 }, WHITE);
        }
        EndScissorMode();
    }

    // ─── Nút Quay lại ─────────────────────────────────────────
    float btnBackW = 130.0f * uiScale;
    float btnBackH = 44.0f * uiScale;
    Rectangle btnBack = { 14, 28, btnBackW, btnBackH };
    bool bkHov = CheckCollisionPointRec(GetMousePosition(), btnBack);
    DrawRectangleRounded(btnBack, 0.35f, 10, bkHov ? GetColor(0xFF4444FF) : GetColor(0xFFFFFF20));
    DrawRectangleRoundedLines(btnBack, 0.35f, 10, GetColor(0xFFFFFF44));
    DrawTextEx(font, "< Quay lại", (Vector2){ btnBack.x + (btnBackW * 0.1f), btnBack.y + (btnBackH - fzBack)/2.0f }, fzBack, 1, WHITE);

    // ─── Vẽ Header bảng ───────
    DrawRectangle((int)tableX, (int)listTop, (int)tableW, (int)PM_HEADER_H, PM_COL_HEADER_BG);
    DrawRectangle((int)tableX, (int)(listTop + PM_HEADER_H - 2), (int)tableW, 2, GetColor(0x4A80FFFF));
    for (int c = 0; c < PM_NUM_COLS; c++) {
        float cx = PM_ColX(tableX, tableW, c);
        float cw = tableW * s_colRatio[c];
        if (c > 0) DrawRectangle((int)cx, (int)listTop, 1, (int)PM_HEADER_H, PM_COL_BORDER);
        Vector2 ts = MeasureTextEx(font, s_colHeaders[c], fzHeader, 1);
        DrawTextEx(font, s_colHeaders[c],
                   (Vector2){ cx + (cw - ts.x)/2.0f, listTop + (PM_HEADER_H - ts.y)/2.0f },
                   fzHeader, 1, WHITE);
    }

    // ─── Modal ─────────────────────────────────
    DrawModalPM(headPM, font);
}