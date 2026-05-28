#include "InDanhSachPM.h"
#include "TimkiemLSphieumuon.h" // Dùng hàm DrawThePhieuMuon
#include "menu.h"          // Bắt buộc thêm để nhận diện APP_MENU
#include <stddef.h>

void InitFormInDanhSachPM(FormInDanhSachPM *form) {
    form->currentTab = VIEW_TAT_CA;
    form->scrollList = 0.0f;
}

// Đã thêm int *currentState vào đây
void UpdateLogicInDanhSachPM(FormInDanhSachPM *form, PhieuMuonNode *headPM, int *currentState) {
    
    // --- LOGIC NÚT QUAY LẠI TỰ CHỦ ---
    Rectangle btnBack = { 20, 20, 130, 40 };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    if (IsKeyPressed(KEY_ESCAPE) || (isHoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
        InitFormInDanhSachPM(form); // Reset form sạch sẽ trước khi về Menu
        *currentState = APP_MENU;   // Ép về Menu
        return; 
    }
    // ---------------------------------

    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    // 1. Xử lý Click chuyển Tab
    float tabW = 200.0f * scale;
    float tabH = 50.0f * scale;
    float startX = (screenW - (tabW * 2 + 20*scale)) / 2.0f;
    
    Rectangle tabTatCa = { startX, 100 * scale, tabW, tabH };
    Rectangle tabChuaTra = { startX + tabW + 20*scale, 100 * scale, tabW, tabH };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), tabTatCa) && form->currentTab != VIEW_TAT_CA) {
            form->currentTab = VIEW_TAT_CA;
            form->scrollList = 0; // Reset scroll khi đổi tab
        }
        if (CheckCollisionPointRec(GetMousePosition(), tabChuaTra) && form->currentTab != VIEW_CHUA_TRA) {
            form->currentTab = VIEW_CHUA_TRA;
            form->scrollList = 0; // Reset scroll khi đổi tab
        }
    }

    // 2. Tính toán giới hạn cuộn (Scroll Clamp)
    float pmHeight = 420.0f * scale;
    int count = 0;
    PhieuMuonNode *tmp = headPM;
    while (tmp != NULL) {
        if (form->currentTab == VIEW_TAT_CA || (form->currentTab == VIEW_CHUA_TRA && tmp->trangthai == 0)) {
            count++;
        }
        tmp = tmp->next;
    }

    float totalHeight = count * (pmHeight + 40.0f * scale);
    float maxScroll = 0;
    float minScroll = screenH - (180.0f * scale) - totalHeight;
    if (minScroll > 0) minScroll = 0;

    form->scrollList += GetMouseWheelMove() * 45.0f;
    if (form->scrollList > maxScroll) form->scrollList = maxScroll;
    if (form->scrollList < minScroll) form->scrollList = minScroll;
}

void DrawGiaoDienInDanhSachPM(FormInDanhSachPM *form, PhieuMuonNode *headPM, Font font) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    ClearBackground(GetColor(0xFFF0F5FF)); // Nền hồng nhạt

    // ==========================================
    // LỚP 1: DANH SÁCH CUỘN Ở DƯỚI
    // ==========================================
    float pmWidth = 800.0f * scale; 
    float pmHeight = 420.0f * scale;
    float pmX = (screenW - pmWidth) / 2.0f;
    float pmY = 180.0f * scale + form->scrollList;

    int count = 0;
    PhieuMuonNode *curr = headPM;
    
    // Chỉ cắt phần dưới, chừa phần Header phía trên
    BeginScissorMode(0, (int)(160.0f * scale), (int)screenW, (int)screenH);
    while (curr != NULL) {
        bool thoaManDK = (form->currentTab == VIEW_TAT_CA) || (form->currentTab == VIEW_CHUA_TRA && curr->trangthai == 0);
        
        if (thoaManDK) {
            count++;
            // Chỉ vẽ những thẻ lọt vào khung hình để tối ưu FPS
            if (pmY + pmHeight > 0 && pmY < screenH) {
                DrawThePhieuMuon(curr, font, pmX, pmY, scale);
                
                // Vẽ hiệu ứng viền sáng nhẹ khi hover
                Rectangle cardHitbox = { pmX, pmY, pmWidth, pmHeight };
                if (CheckCollisionPointRec(GetMousePosition(), cardHitbox)) {
                    DrawRectangleRounded(cardHitbox, 0.1f, 10, Fade(GetColor(0xFFB6C1FF), 0.2f));
                    DrawRectangleRoundedLines(cardHitbox, 0.1f, 10, GetColor(0xFFB6C1FF));
                }
            }
            pmY += pmHeight + 40.0f * scale;
        }
        curr = curr->next;
    }
    EndScissorMode();

    if (count == 0) {
        DrawTextEx(font, "Không có phiếu mượn nào trong danh sách!", (Vector2){screenW/2 - 250*scale, screenH/2}, 24*scale, 1, GRAY);
    }

    // ==========================================
    // LỚP 2: MẶT NẠ CHE VÀ HEADER 
    // ==========================================
    DrawRectangle(0, 0, screenW, (int)(160.0f * scale), GetColor(0xFFF0F5FF)); // Che phần cuộn thừa

    // Banner Tiêu Đề
    DrawRectangleGradientV(0, 0, screenW, 80*scale, GetColor(0xFFB6C1FF), GetColor(0xFFC0CBFF));
    DrawTextEx(font, "DANH SÁCH PHIẾU MƯỢN THƯ VIỆN", (Vector2){screenW/2 - 300*scale, 20*scale}, 36*scale, 1, MAROON);

    // Vẽ Tabs Chuyển Đổi
    float tabW = 200.0f * scale;
    float tabH = 50.0f * scale;
    float startX = (screenW - (tabW * 2 + 20*scale)) / 2.0f;
    
    Rectangle tabTatCa = { startX, 100 * scale, tabW, tabH };
    Rectangle tabChuaTra = { startX + tabW + 20*scale, 100 * scale, tabW, tabH };

    // Tab Tất Cả
    bool hoverTatCa = CheckCollisionPointRec(GetMousePosition(), tabTatCa);
    Color colorTatCa = (form->currentTab == VIEW_TAT_CA) ? MAROON : (hoverTatCa ? Fade(MAROON, 0.7f) : LIGHTGRAY);
    DrawRectangleRounded(tabTatCa, 0.3f, 10, colorTatCa);
    DrawTextEx(font, "TẤT CẢ", (Vector2){tabTatCa.x + tabW/2 - MeasureTextEx(font, "TẤT CẢ", 24*scale, 1).x/2, tabTatCa.y + 13*scale}, 24*scale, 1, WHITE);

    // Tab Chưa Trả
    bool hoverChuaTra = CheckCollisionPointRec(GetMousePosition(), tabChuaTra);
    Color colorChuaTra = (form->currentTab == VIEW_CHUA_TRA) ? RED : (hoverChuaTra ? Fade(RED, 0.7f) : LIGHTGRAY);
    DrawRectangleRounded(tabChuaTra, 0.3f, 10, colorChuaTra);
    DrawTextEx(font, "CHƯA TRẢ", (Vector2){tabChuaTra.x + tabW/2 - MeasureTextEx(font, "CHƯA TRẢ", 24*scale, 1).x/2, tabChuaTra.y + 13*scale}, 24*scale, 1, WHITE);

    // --- VẼ NÚT QUAY LẠI LÊN LỚP TRÊN CÙNG ---
    Rectangle btnBack = { 20, 20, 130, 40 };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    Color btnColor = isHoverBack ? MAROON : WHITE;
    Color textColor = isHoverBack ? WHITE : MAROON;
    DrawRectangleRounded(btnBack, 0.3f, 10, btnColor);
    DrawRectangleRoundedLines(btnBack, 0.3f, 10, MAROON);
    DrawTextEx(font, "< Quay lai", (Vector2){btnBack.x + 15, btnBack.y + 10}, 20, 1, textColor);
    // ----------------------------------------
}