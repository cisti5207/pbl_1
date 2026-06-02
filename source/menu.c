#include "menu.h"
#include "libmanage.h"
#include <math.h> 

// Hàm vẽ và xử lý Menu VIP, co giãn mượt mà mọi tỉ lệ
AppState DrawAndHandleMenu(Font font, float screenW, float screenH) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        return HOME;
    }

    // 1. NỀN ĐỘNG (Dark navy + hạt particles từ libmanage)
    ClearBackground(AnimatedBackground());
    DrawBackgroundParticles();
    // Lớp phủ navy đồng bộ với bước 1 & 2 của các màn hình khác
    DrawRectangle(0, 0, (int)screenW, (int)screenH, GetColor(0x1b264f80));

    // 2. TÍNH TOÁN SCALE LINH HOẠT TỔNG THỂ
    float baseW = 1100.0f;
    float baseH = 750.0f;
    float scale = (screenW / baseW < screenH / baseH) ? screenW / baseW : screenH / baseH;
    if (scale < 0.5f) scale = 0.5f;

    // 3. VẼ HEADER (0x2b3a67ff) & TIÊU ĐỀ TRẮNG (Kèm hiệu ứng nhịp thở Pulsing)
    float headerH = 140 * scale;
    DrawRectangleGradientEx(
        (Rectangle){ 0, 0, screenW, headerH },
        GetColor(0x2b3a67ff), GetColor(0x2b3a67ff),
        GetColor(0x2b3a67ff), GetColor(0x2b3a67ff)
    );
    // Đường kẻ sáng phía dưới header
    DrawRectangle(0, (int)(headerH - 2), (int)screenW, 2, GetColor(0x4A80FF70));

    // Hiệu ứng thở cho kích thước chữ
    float timeObj = GetTime();
    float pulse = sinf(timeObj * 3.0f) * 2.0f;
    float titleSize = (46 + pulse) * scale;

    const char *title = "HỆ THỐNG QUẢN LÝ THƯ VIỆN";
    Vector2 titleMeasure = MeasureTextEx(font, title, titleSize, 1);
    DrawTextEx(font, title, (Vector2){(screenW - titleMeasure.x)/2, (headerH - titleMeasure.y)/2}, titleSize, 1, WHITE);

    // 4. TÍNH TOÁN LƯỚI NÚT BẤM (CĂN GIỮA TUYỆT ĐỐI)
    float btnW = 380 * scale;
    float btnH = 85 * scale;
    float gapX = 60 * scale;
    float gapY = 40 * scale;

    float gridW = btnW * 2 + gapX;
    float gridH = btnH * 3 + gapY * 2;

    float startX = (screenW - gridW) / 2.0f;
    float startY = headerH + (screenH - headerH - gridH) / 2.0f;

    Rectangle baseBtns[5] = {
        { startX,                          startY,                    btnW, btnH },
        { startX + btnW + gapX,            startY,                    btnW, btnH },
        { startX,                          startY + btnH + gapY,      btnW, btnH },
        { startX + btnW + gapX,            startY + btnH + gapY,      btnW, btnH },
        { startX + (gridW - btnW)/2.0f,    startY + (btnH + gapY)*2,  btnW, btnH }
    };

    const char *labels[5] = {
        "1. Tạo Thẻ Thư Viện", "2. Tạo Phiếu Mượn",
        "3. Xem Danh Sách PM", "4. Lịch Sử Phiếu Mượn", "5. Xử Lý Trả Sách"
    };

    AppState nextState = APP_MENU;

    // 5. VẼ CÁC NÚT BẤM
    for (int i = 0; i < 5; i++) {
        Rectangle currentBtn = baseBtns[i];
        bool isHover = CheckCollisionPointRec(GetMousePosition(), currentBtn);

        // Hiệu ứng hover lift
        float liftOffset = isHover ? (6.0f * scale) : 0.0f;
        currentBtn.y -= liftOffset;

        // Bóng đổ
        float shadowOffset = isHover ? (12.0f * scale) : (6.0f * scale);
        Rectangle shadowRec = { currentBtn.x + shadowOffset/2, currentBtn.y + shadowOffset, currentBtn.width, currentBtn.height };
        DrawRectangleRounded(shadowRec, 0.2f, 10, Fade(BLACK, isHover ? 0.30f : 0.25f));

        // Màu nút: sáng hơn 0x2b3a67 một bậc → 0x3D5498, hover sáng thêm → 0x4A6ABFFF
        Color btnColor  = isHover ? GetColor(0x4A6ABFFF) : GetColor(0x3D5498FF);
        Color lineColor = isHover ? GetColor(0x7AA4FFFF) : GetColor(0x4A80FF70);

        DrawRectangleRounded(currentBtn, 0.2f, 10, btnColor);
        DrawRectangleRoundedLines(currentBtn, 0.2f, 10, lineColor);

        // Chữ trắng, hover to hơn 1 chút
        float fontSize = (isHover ? 26 : 24) * scale;
        Vector2 textSize = MeasureTextEx(font, labels[i], fontSize, 1);
        Vector2 textPos = {
            currentBtn.x + (currentBtn.width  - textSize.x) / 2.0f,
            currentBtn.y + (currentBtn.height - textSize.y) / 2.0f
        };
        DrawTextEx(font, labels[i], textPos, fontSize, 1, WHITE);

        if (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (i == 0) nextState = APP_TAO_THE;
            if (i == 1) nextState = APP_TAO_PHIEU;
            if (i == 2) nextState = APP_XEM_DANH_SACH;
            if (i == 3) nextState = APP_LICH_SU;
            if (i == 4) nextState = APP_TRA_SACH;
        }
    }

    // Nút về HOME — nền trong suốt + chữ trắng đồng bộ nền tối
    Rectangle btnBack = { 20, 20, 140, 44 };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    DrawRectangleRounded(btnBack, 0.3f, 10, isHoverBack ? RED : GetColor(0xFFFFFF20));
    DrawRectangleRoundedLines(btnBack, 0.3f, 10, GetColor(0xFFFFFF44));
    DrawTextEx(font, "< Quay lại", (Vector2){btnBack.x + 12, btnBack.y + 12}, 20, 1, WHITE);
    if (isHoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return HOME;
    }

    return nextState;
}