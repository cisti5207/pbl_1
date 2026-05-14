#include "raylib.h"
#include "ManageUser.h"
#include <stdlib.h>

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(1100, 750, "Library System - Pink Style Pro");
    SetTargetFPS(60);

    int codepoints[1500];
    int count = 0;
    for (int i = 32; i < 127; i++) codepoints[count++] = i;         // ASCII
    for (int i = 0xA0; i < 0x0100; i++) codepoints[count++] = i;     // Latin-1 (á, à...)
    for (int i = 0x0100; i < 0x0250; i++) codepoints[count++] = i;   // Latin Ext A&B (ư, ơ, đ, ê...)
    for (int i = 0x1E00; i < 0x1EFF; i++) codepoints[count++] = i;   // Latin Ext Additional (ọ, ữ...)

    Font mainFont = LoadFontEx("font/arial/ARIAL.TTF", 96, codepoints, count); 
    SetTextureFilter(mainFont.texture, TEXTURE_FILTER_BILINEAR);

    Texture2D background = LoadTexture("img/nen.jpg");
    Texture2D icons[4] = {0};

    char maThe[15];
    int currentTotalUsers = 0; 
    SinhMaTheTuDong(currentTotalUsers, maThe); 

    Nhap myForm;
    InitForm(&myForm);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F11)) ToggleFullscreen();

        UpdateFormPosition(&myForm); 
        UpdateInputForm(&myForm);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (background.id != 0) {
            DrawTexturePro(background, 
                (Rectangle){0, 0, (float)background.width, (float)background.height},
                (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
                (Vector2){0, 0}, 0, WHITE);
        }
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.15f));
        
        DrawLibraryCard(&myForm, icons, maThe, mainFont);

        EndDrawing();
    }

    UnloadTexture(background);
    UnloadFont(mainFont);
    CloseWindow();

    return 0;
}