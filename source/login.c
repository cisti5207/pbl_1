#include "raylib.h"
#include <stdio.h>

int main() {
    InitWindow(800, 600, "ThuvienTruyen");
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    Texture2D bg = LoadTexture("img\\background_Login.jpg");

    if (bg.id == 0) {
        printf("Failed to load background image.\n");
        return 1;
    }

    // ===== SCALE về 1920x1080 (tỷ lệ giữ nguyên) =====
    float scaleX = 1920.0f / bg.width;
    float scaleY = 1080.0f / bg.height;

    // chọn scale để fill (cover)
    float scale = (scaleX > scaleY) ? scaleX : scaleY;

    float scaledW = bg.width * scale;
    float scaledH = bg.height * scale;

    while (!WindowShouldClose()) {
        int winW = GetScreenWidth();
        int winH = GetScreenHeight();

        // ===== Lấy vùng 1920x1080 từ ảnh đã scale =====
        float baseX = (scaledW - 1920) / 2.0f;
        float baseY = (scaledH - 1080) / 2.0f;

        // ===== Crop tiếp theo window =====
        float srcX = baseX + (1920 - winW) / 2.0f;
        float srcY = baseY + (1080 - winH) / 2.0f;

        if (srcX < baseX) srcX = baseX;
        if (srcY < baseY) srcY = baseY;

        float srcW = (float)winW;
        float srcH = (float)winH;

        if (srcW > 1920) srcW = 1920;
        if (srcH > 1080) srcH = 1080;

        // Map ngược lại về texture gốc
        Rectangle source = {
            srcX / scale,
            srcY / scale,
            srcW / scale,
            srcH / scale
        };

        Rectangle dest = {
            0, 0,
            (float)winW,
            (float)winH
        };

        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexturePro(bg, source, dest, (Vector2){0,0}, 0, WHITE);

        EndDrawing();
    }

    UnloadTexture(bg);
    CloseWindow();
    return 0;
}