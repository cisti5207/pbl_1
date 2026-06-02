#include "InDanhSachPM.h"
#include "TimkiemLSphieumuon.h"
#include "menu.h"
#include "libmanage.h" // Chứa hàm StringContains
#include <stddef.h>
#include <string.h>

// ============================================================
// State nội bộ cho thanh tìm kiếm đa năng và scrollbar
// ============================================================
static char s_searchFilter[256]  = {0};  // Tìm theo ngày, mã thẻ, tên truyện
static int  s_searchFilterLen   = 0;
static bool s_searchFocused     = false;
static float s_searchBSCounter  = 0.0f;

// Biến lưu trạng thái kéo thả scrollbar
static bool s_sb_dragging = false;
static float s_sb_dragStartY = 0.0f;

void InitFormInDanhSachPM(FormInDanhSachPM *form) {
    form->currentTab = VIEW_TAT_CA;
    form->scrollList = 0.0f;
    memset(s_searchFilter, 0, sizeof(s_searchFilter));
    s_searchFilterLen  = 0;
    s_searchFocused    = false;
    s_searchBSCounter  = 0.0f;
    
    // Reset state scrollbar
    s_sb_dragging    = false;
    s_sb_dragStartY  = 0.0f;
}

// Kiểm tra khớp tìm kiếm đa năng (Ngày mượn, Mã thẻ, Tên truyện)
static bool KhopTimKiemPM(PhieuMuonNode *pm, const char *filter) {
    if (!filter || filter[0] == '\0') return true;
    
    // Quét đối chiếu trên cả 3 trường dữ liệu
    if (StringContains(pm->ngaymuon, filter)) return true;
    if (StringContains(pm->mathe, filter)) return true;
    if (StringContains(pm->tentruyen, filter)) return true;
    
    return false;
}

void UpdateLogicInDanhSachPM(FormInDanhSachPM *form, PhieuMuonNode *headPM, int *currentState) {

    // --- NÚT QUAY LẠI ---
    Rectangle btnBack = { 20, 20, 130, 40 };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    if (IsKeyPressed(KEY_ESCAPE) || (isHoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
        InitFormInDanhSachPM(form);
        *currentState = APP_MENU;
        return;
    }

    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale   = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    // Header cao 128*scale, layout:
    float rowY    = 65.0f  * scale;
    float tabW    = 160.0f * scale;
    float tabH    = 44.0f  * scale;
    float searchW = 420.0f * scale; // Tăng độ rộng thanh tìm kiếm để hiển thị Placeholder dài
    float searchH = tabH;
    float gap     = 16.0f  * scale;

    float totalRowW = tabW + gap + tabW + gap + searchW;
    float rowStartX = (screenW - totalRowW) / 2.0f;

    Rectangle tabTatCa   = { rowStartX,                        rowY, tabW,    tabH };
    Rectangle tabChuaTra = { rowStartX + tabW + gap,           rowY, tabW,    tabH };
    Rectangle searchBox  = { rowStartX + tabW*2 + gap*2,       rowY, searchW, searchH };

    Vector2 mp = GetMousePosition();

    // --- Click tab ---
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mp, tabTatCa)) {
            form->currentTab = VIEW_TAT_CA;
            form->scrollList = 0;
        }
        if (CheckCollisionPointRec(mp, tabChuaTra)) {
            form->currentTab = VIEW_CHUA_TRA;
            form->scrollList = 0;
        }
        s_searchFocused = CheckCollisionPointRec(mp, searchBox);
    }

    // --- Input ô tìm kiếm đa năng có hỗ trợ Tiếng Việt (UTF-8) ---
    if (s_searchFocused) {
        int key = GetKeyPressed();
        while (key > 0) {
            if (key == KEY_BACKSPACE && s_searchFilterLen > 0) {
                do { s_searchFilterLen--; } 
                while (s_searchFilterLen > 0 && (s_searchFilter[s_searchFilterLen] & 0xC0) == 0x80);
                s_searchFilter[s_searchFilterLen] = '\0';
                form->scrollList = 0;
            }
            key = GetKeyPressed();
        }
        
        if (IsKeyDown(KEY_BACKSPACE)) {
            s_searchBSCounter += GetFrameTime();
            if (s_searchBSCounter >= 0.1f) {
                if (s_searchFilterLen > 0) {
                    do { s_searchFilterLen--; } 
                    while (s_searchFilterLen > 0 && (s_searchFilter[s_searchFilterLen] & 0xC0) == 0x80);
                    s_searchFilter[s_searchFilterLen] = '\0';
                    form->scrollList = 0;
                }
            }
        } else {
            s_searchBSCounter = 0.0f;
        }
        
        int charcode = GetCharPressed();
        while (charcode > 0) {
            if ((charcode >= 32) && (s_searchFilterLen < 250)) {
                int bytesize = 0;
                const char* utf8Char = CodepointToUTF8(charcode, &bytesize);
                if (s_searchFilterLen + bytesize < 255) {
                    for (int j = 0; j < bytesize; j++) {
                        s_searchFilter[s_searchFilterLen++] = utf8Char[j];
                    }
                    s_searchFilter[s_searchFilterLen] = '\0';
                    form->scrollList = 0;
                }
            }
            charcode = GetCharPressed();
        }
        if (IsKeyPressed(KEY_ESCAPE)) s_searchFocused = false;
    }

    // --- TÍNH TOÁN & XỬ LÝ KÉO SCROLLBAR ---
    float pmHeight = 420.0f * scale;
    int count = 0;
    PhieuMuonNode *tmp = headPM;
    while (tmp != NULL) {
        bool tabOK = (form->currentTab == VIEW_TAT_CA) ||
                     (form->currentTab == VIEW_CHUA_TRA && tmp->trangthai == 0);
        // Kiểm tra khớp từ khóa đa năng
        if (tabOK && KhopTimKiemPM(tmp, s_searchFilter)) count++;
        tmp = tmp->next;
    }

    float totalHeight = count * (pmHeight + 40.0f * scale);
    float contentH    = screenH - 128.0f * scale; // 128 là chiều cao header 
    float scrollRange = totalHeight - contentH;
    if (scrollRange < 0) scrollRange = 0;

    // Logic xử lý khi nội dung dài hơn màn hình
    if (totalHeight > contentH) {
        float sbW      = 8.0f * scale;
        float sbX      = screenW - sbW - 6.0f * scale;
        float sbTrackY = 128.0f * scale + 4.0f;
        float sbTrackH = contentH - 8.0f;

        float ratio     = contentH / totalHeight;
        float thumbH    = sbTrackH * ratio;
        if (thumbH < 24.0f * scale) thumbH = 24.0f * scale;
        float maxThumbY = sbTrackH - thumbH;
        if (maxThumbY <= 0) maxThumbY = 0.001f;

        float thumbY = sbTrackY + (-form->scrollList / scrollRange) * maxThumbY;

        // Mở rộng hitbox thêm 10px để dễ bắt chuột
        Rectangle thumbHitbox = {sbX - 10, thumbY, sbW + 20, thumbH};
        Rectangle trackHitbox = {sbX - 10, sbTrackY, sbW + 20, sbTrackH};

        // Kéo thả chuột
        if (s_sb_dragging) {
            if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                s_sb_dragging = false;
            } else {
                float newThumbY = mp.y - s_sb_dragStartY;
                if (newThumbY < sbTrackY) newThumbY = sbTrackY;
                if (newThumbY > sbTrackY + maxThumbY) newThumbY = sbTrackY + maxThumbY;
                
                form->scrollList = -(((newThumbY - sbTrackY) / maxThumbY) * scrollRange);
            }
        } else {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mp, thumbHitbox)) {
                    s_sb_dragging = true;
                    s_sb_dragStartY = mp.y - thumbY; 
                } else if (CheckCollisionPointRec(mp, trackHitbox)) {
                    // Click track mờ để nhảy trang
                    if (mp.y < thumbY) form->scrollList += contentH;
                    else form->scrollList -= contentH;
                }
            }
        }

        // Vẫn cho phép lăn chuột
        form->scrollList += GetMouseWheelMove() * 45.0f;
        
        // Giới hạn cuộn
        if (form->scrollList > 0) form->scrollList = 0;
        if (form->scrollList < -scrollRange) form->scrollList = -scrollRange;
    } else {
        form->scrollList = 0;
        s_sb_dragging = false;
    }
}

void DrawGiaoDienInDanhSachPM(FormInDanhSachPM *form, PhieuMuonNode *headPM, Font font) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale   = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    // ── Nền động ──────────────────────────────────────────────
    ClearBackground(AnimatedBackground());
    DrawBackgroundParticles();

    // ── Layout hàng tab + search ──────────────────────────────
    float rowY    = 65.0f  * scale;
    float tabW    = 160.0f * scale;
    float tabH    = 44.0f  * scale;
    float searchW = 420.0f * scale; // Đồng bộ chiều rộng
    float searchH = tabH;
    float gap     = 16.0f  * scale;
    float totalRowW = tabW + gap + tabW + gap + searchW;
    float rowStartX = (screenW - totalRowW) / 2.0f;

    Rectangle tabTatCa   = { rowStartX,                  rowY, tabW,    tabH };
    Rectangle tabChuaTra = { rowStartX + tabW + gap,     rowY, tabW,    tabH };
    Rectangle searchBox  = { rowStartX + tabW*2 + gap*2, rowY, searchW, searchH };

    // ==========================================
    // LỚP 1: DANH SÁCH CUỘN
    // ==========================================
    float pmWidth  = 800.0f * scale;
    float pmHeight = 420.0f * scale;
    float pmX      = (screenW - pmWidth) / 2.0f;
    float pmY      = 130.0f * scale + form->scrollList;

    int headerH = (int)(128.0f * scale);
    BeginScissorMode(0, headerH, (int)screenW, (int)(screenH - headerH));
    PhieuMuonNode *curr = headPM;
    int count = 0;
    while (curr != NULL) {
        bool tabOK = (form->currentTab == VIEW_TAT_CA) ||
                     (form->currentTab == VIEW_CHUA_TRA && curr->trangthai == 0);
        if (tabOK && KhopTimKiemPM(curr, s_searchFilter)) {
            count++;
            if (pmY + pmHeight > 0 && pmY < screenH) {
                DrawThePhieuMuon(curr, font, pmX, pmY, scale);
                Rectangle cardHitbox = { pmX, pmY, pmWidth, pmHeight };
                if (CheckCollisionPointRec(GetMousePosition(), cardHitbox)) {
                    DrawRectangleRounded(cardHitbox, 0.1f, 10, Fade(SKYBLUE, 0.12f));
                    DrawRectangleRoundedLines(cardHitbox, 0.1f, 10, Fade(SKYBLUE, 0.6f));
                }
            }
            pmY += pmHeight + 40.0f * scale;
        }
        curr = curr->next;
    }
    EndScissorMode();

    if (count == 0) {
        const char *msg = (s_searchFilter[0] != '\0')
            ? "Không tìm thấy kết quả phù hợp."
            : "Không có phiếu mượn nào trong danh sách!";
        Vector2 ms = MeasureTextEx(font, msg, 24*scale, 1);
        DrawTextEx(font, msg,
                   (Vector2){(screenW - ms.x)/2, screenH/2},
                   24*scale, 1, Fade(WHITE, 0.8f));
    }

    // ==========================================
    // SCROLLBAR BÊN PHẢI (HIỂN THỊ)
    // ==========================================
    {
        float contentH  = screenH - 128.0f * scale;
        float pmH2      = 420.0f * scale;
        float totalH2   = count * (pmH2 + 40.0f * scale); 

        if (totalH2 > contentH) {
            float sbW      = 8.0f * scale;
            float sbX      = screenW - sbW - 6.0f * scale;
            float sbTrackY = 128.0f * scale + 4.0f;
            float sbTrackH = contentH - 8.0f;

            // Track mờ
            DrawRectangleRounded(
                (Rectangle){sbX, sbTrackY, sbW, sbTrackH},
                0.5f, 6, Fade(WHITE, 0.12f));

            // Thumb
            float ratio       = contentH / totalH2;
            float thumbH      = sbTrackH * ratio;
            if (thumbH < 24.0f * scale) thumbH = 24.0f * scale;
            float scrollRange = totalH2 - contentH;
            float thumbTravel = sbTrackH - thumbH;
            float thumbY      = sbTrackY + (-form->scrollList / scrollRange) * thumbTravel;

            // Đổi màu khi hover hoặc kéo
            Rectangle thumbHitbox = {sbX - 10, thumbY, sbW + 20, thumbH};
            bool hoverSB = CheckCollisionPointRec(GetMousePosition(), thumbHitbox);
            
            Color thumbColor = Fade(SKYBLUE, 0.55f);
            if (s_sb_dragging) thumbColor = SKYBLUE;
            else if (hoverSB) thumbColor = Fade(SKYBLUE, 0.9f);

            DrawRectangleRounded(
                (Rectangle){sbX, thumbY, sbW, thumbH},
                0.5f, 6, thumbColor);
        }
    }

    // ==========================================
    // LỚP 2: HEADER — đặc để che card, vẽ lại particles lên trên tạo xuyên thấu
    // ==========================================
    DrawRectangle(0, 0, (int)screenW, (int)(128.0f * scale), GetColor(0x1b264fff));
    BeginScissorMode(0, 0, (int)screenW, (int)(128.0f * scale));
        DrawBackgroundParticles();
    EndScissorMode();
    DrawRectangle(0, 0, (int)screenW, (int)(128.0f * scale), Fade(GetColor(0x1b264fff), 0.5f));

    // Tiêu đề trắng
    const char *title = "DANH SÁCH PHIẾU MƯỢN THƯ VIỆN";
    Vector2 titleSz   = MeasureTextEx(font, title, 34*scale, 1);
    DrawTextEx(font, title,
               (Vector2){(screenW - titleSz.x)/2, 14*scale},
               34*scale, 1, WHITE);

    // ── Tab TẤT CẢ ──────────────────────────────────────────
    bool hoverTatCa   = CheckCollisionPointRec(GetMousePosition(), tabTatCa);
    bool activeTatCa  = (form->currentTab == VIEW_TAT_CA);
    Color clrTatCa    = activeTatCa ? SKYBLUE : (hoverTatCa ? Fade(SKYBLUE, 0.75f) : Fade(WHITE, 0.15f));
    DrawRectangleRounded(tabTatCa, 0.3f, 10, clrTatCa);
    if (activeTatCa)
        DrawRectangleRoundedLines(tabTatCa, 0.3f, 10, WHITE);
    Vector2 ttSz = MeasureTextEx(font, "TẤT CẢ", 22*scale, 1);
    DrawTextEx(font, "TẤT CẢ",
               (Vector2){tabTatCa.x + (tabW - ttSz.x)/2, tabTatCa.y + (tabH - ttSz.y)/2},
               22*scale, 1, WHITE);

    // ── Tab CHƯA TRẢ ─────────────────────────────────────────
    bool hoverChuaTra  = CheckCollisionPointRec(GetMousePosition(), tabChuaTra);
    bool activeChuaTra = (form->currentTab == VIEW_CHUA_TRA);
    Color clrChuaTra   = activeChuaTra ? SKYBLUE : (hoverChuaTra ? Fade(SKYBLUE, 0.75f) : Fade(WHITE, 0.15f));
    DrawRectangleRounded(tabChuaTra, 0.3f, 10, clrChuaTra);
    if (activeChuaTra)
        DrawRectangleRoundedLines(tabChuaTra, 0.3f, 10, WHITE);
    Vector2 ctSz = MeasureTextEx(font, "CHƯA TRẢ", 22*scale, 1);
    DrawTextEx(font, "CHƯA TRẢ",
               (Vector2){tabChuaTra.x + (tabW - ctSz.x)/2, tabChuaTra.y + (tabH - ctSz.y)/2},
               22*scale, 1, WHITE);

    // ── Ô tìm kiếm ─────────────────────────────────────
    Color searchBorder = s_searchFocused ? SKYBLUE : Fade(WHITE, 0.5f);
    DrawRectangleRounded(searchBox, 0.25f, 10, Fade(WHITE, 0.9f));
    DrawRectangleRoundedLines(searchBox, 0.25f, 10, searchBorder);

    // Icon kính lúp (text đơn giản)
    DrawTextEx(font, "🔍",
               (Vector2){searchBox.x + 8*scale, searchBox.y + (searchH - 20*scale)/2},
               20*scale, 1, GRAY);

    float fs = 18*scale;
    const char *placeholder = "Tìm theo ngày, mã thẻ, tên truyện...";
    float textOffX = 32*scale; // chừa chỗ icon

    if (s_searchFilterLen == 0 && !s_searchFocused) {
        DrawTextEx(font, placeholder,
                   (Vector2){searchBox.x + textOffX, searchBox.y + (searchH - fs)/2},
                   fs, 1, LIGHTGRAY);
    } else {
        Vector2 inputSz = MeasureTextEx(font, s_searchFilter, fs, 1);
        float availW    = searchBox.width - textOffX - 10;
        float offX      = (inputSz.x > availW) ? inputSz.x - availW : 0;
        BeginScissorMode((int)(searchBox.x + textOffX), (int)searchBox.y,
                         (int)(searchBox.width - textOffX - 4), (int)searchBox.height);
            DrawTextEx(font, s_searchFilter,
                       (Vector2){searchBox.x + textOffX - offX, searchBox.y + (searchH - fs)/2},
                       fs, 1, GetColor(0x1b264fff));
            // Con trỏ nhấp nháy
            if (s_searchFocused && (((int)(GetTime()*1.5f)) % 2 == 0)) {
                DrawRectangleV(
                    (Vector2){searchBox.x + textOffX - offX + inputSz.x + 2,
                              searchBox.y + (searchH - fs)/2 + 2},
                    (Vector2){2, fs - 4},
                    GetColor(0x1b264fff));
            }
        EndScissorMode();
    }

    // ── Nút QUAY LẠI ────────────────────────────────────────
    Rectangle btnBack   = { 20, 20, 130, 40 };
    bool isHoverBack    = CheckCollisionPointRec(GetMousePosition(), btnBack);
    DrawRectangleRounded(btnBack, 0.3f, 10,
                         isHoverBack ? SKYBLUE : Fade(WHITE, 0.15f));
    DrawRectangleRoundedLines(btnBack, 0.3f, 10, Fade(WHITE, 0.6f));
    DrawTextEx(font, "< Quay lai",
               (Vector2){btnBack.x + 15, btnBack.y + 10},
               20, 1, WHITE);
}