#include "menu.h"
#include <math.h> 

// Hàm vẽ và xử lý Menu VIP, co giãn mượt mà mọi tỉ lệ
AppState DrawAndHandleMenu(Font font, float screenW, float screenH) {
    // 1. VẼ NỀN GRADIENT SANG TRỌNG
    DrawRectangleGradientV(0, 0, screenW, screenH, GetColor(0xFFF0F5FF), GetColor(0xFFE4EBFF)); // Hồng pastel chuyển sắc
    
    // 2. TÍNH TOÁN SCALE LINH HOẠT TỔNG THỂ
    float baseW = 1100.0f;
    float baseH = 750.0f;
    float scale = (screenW / baseW < screenH / baseH) ? screenW / baseW : screenH / baseH;
    if (scale < 0.5f) scale = 0.5f; // Chặn không cho thu nhỏ quá mức lố

    // 3. VẼ HEADER & TIÊU ĐỀ (Kèm hiệu ứng nhịp thở Pulsing)
    float headerH = 140 * scale;
    DrawRectangleGradientV(0, 0, screenW, headerH, GetColor(0xFFB6C1FF), GetColor(0xFFC0CBFF));
    
    // Tạo hiệu ứng thở cho kích thước chữ
    float timeObj = GetTime();
    float pulse = sinf(timeObj * 3.0f) * 2.0f; // Biên độ nhịp thở 2px
    float titleSize = (46 + pulse) * scale;
    
    const char *title = "HỆ THỐNG QUẢN LÝ THƯ VIỆN";
    Vector2 titleMeasure = MeasureTextEx(font, title, titleSize, 1);
    DrawTextEx(font, title, (Vector2){(screenW - titleMeasure.x)/2, (headerH - titleMeasure.y)/2}, titleSize, 1, MAROON);
    
    // 4. TÍNH TOÁN LƯỚI NÚT BẤM (CĂN GIỮA TUYỆT ĐỐI)
    float btnW = 380 * scale;
    float btnH = 85 * scale;
    float gapX = 60 * scale;
    float gapY = 40 * scale;
    
    // Tính tổng chiều rộng và chiều cao của cụm nút
    float gridW = btnW * 2 + gapX;
    float gridH = btnH * 3 + gapY * 2;
    
    // Tọa độ bắt đầu để ép toàn bộ khối grid vào giữa phần màn hình còn trống
    float startX = (screenW - gridW) / 2.0f;
    float startY = headerH + (screenH - headerH - gridH) / 2.0f;
    
    // Mảng gốc lưu vị trí 5 nút
    Rectangle baseBtns[5] = {
        { startX, startY, btnW, btnH },                                   // 1. Trái trên
        { startX + btnW + gapX, startY, btnW, btnH },                     // 2. Phải trên
        { startX, startY + btnH + gapY, btnW, btnH },                     // 3. Trái giữa
        { startX + btnW + gapX, startY + btnH + gapY, btnW, btnH },       // 4. Phải giữa
        { startX + (gridW - btnW)/2.0f, startY + (btnH + gapY)*2, btnW, btnH } // 5. Căn giữa đáy
    };
    
    const char *labels[5] = {
        "1. Tạo Thẻ Thư Viện", "2. Tạo Phiếu Mượn", 
        "3. Xem Danh Sách PM", "4. Lịch Sử Phiếu Mượn", "5. Xử Lý Trả Sách"
    };
    
    AppState nextState = APP_MENU;
    
    // 5. VẼ CÁC NÚT BẤM (Kèm hiệu ứng Nổi 3D và Hover)
    for (int i = 0; i < 5; i++) {
        Rectangle currentBtn = baseBtns[i];
        bool isHover = CheckCollisionPointRec(GetMousePosition(), currentBtn);
        
        // HIỆU ỨNG HOVER LIFT (Nổi bổng nút lên)
        float liftOffset = isHover ? (6.0f * scale) : 0.0f;
        currentBtn.y -= liftOffset;
        
        // VẼ BÓNG ĐỔ (Đổ dài hơn khi hover)
        float shadowOffset = isHover ? (12.0f * scale) : (6.0f * scale);
        Rectangle shadowRec = { currentBtn.x + shadowOffset/2, currentBtn.y + shadowOffset, currentBtn.width, currentBtn.height };
        DrawRectangleRounded(shadowRec, 0.2f, 10, Fade(BLACK, isHover ? 0.15f : 0.2f)); // Bóng nhạt đi chút khi nổi lên
        
        // VẼ NỀN NÚT
        Color btnColor = isHover ? GetColor(0xFF8BC3FF) : WHITE; // Hover đổi sang xanh dương nhạt cực sang
        Color lineColor = isHover ? MAROON : GetColor(0xFFB6C1FF);
        
        DrawRectangleRounded(currentBtn, 0.2f, 10, btnColor);
        DrawRectangleRoundedLines(currentBtn, 0.2f, 10, lineColor);
        
        // VẼ TEXT (Căn giữa hoàn hảo trong nút)
        float fontSize = (isHover ? 26 : 24) * scale; // Chữ hơi to ra một xíu khi hover
        Vector2 textSize = MeasureTextEx(font, labels[i], fontSize, 1);
        Vector2 textPos = {
            currentBtn.x + (currentBtn.width - textSize.x) / 2.0f,
            currentBtn.y + (currentBtn.height - textSize.y) / 2.0f
        };
        DrawTextEx(font, labels[i], textPos, fontSize, 1, isHover ? WHITE : MAROON);
        
        // Bắt sự kiện Click
        if (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (i == 0) nextState = APP_TAO_THE;
            if (i == 1) nextState = APP_TAO_PHIEU;
            if (i == 2) nextState = APP_XEM_DANH_SACH;
            if (i == 3) nextState = APP_LICH_SU;
            if (i == 4) nextState = APP_TRA_SACH;
        }
    }
    
    // Trang trí thêm: 2 dải màu nhấn nhá ở góc
    DrawCircle(0, screenH, 150 * scale, Fade(PINK, 0.2f));
    DrawCircle(screenW, screenH, 100 * scale, Fade(GetColor(0xFFB6C1FF), 0.3f));

    return nextState;
}