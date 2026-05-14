#include "raylib.h"
#include "ManageUser.h"
#include <stdio.h>

int main() {
    // 1. Khởi tạo cửa sổ
    InitWindow(1100, 750, "Library System - Professional Debug Mode");
    SetTargetFPS(60);

    // DEBUG: In ra đường dẫn làm việc hiện tại
    printf("--- KHOI DONG HE THONG ---\n");

    // 2. Load Font - FIX DUONG DAN THEO ANH BRO GUI
    // Trong hinh: font -> arial -> ARIAL.TTF
    Font mainFont = LoadFontEx("font/arial/ARIAL.TTF", 32, NULL, 0); 
    if (mainFont.texture.id == 0) {
        printf("[LOI] Khong tim thay font tai: font/arial/ARIAL.TTF\n");
        printf("Vui long kiem tra file ARIAL.TTF co nam trong folder 'font/arial/' khong.\n");
        // Tra ve gia tri mac dinh de khong crash
        mainFont = GetFontDefault();
    } else {
        printf("[OK] Da nạ p Font thanh cong.\n");
    }
    
    // 3. Load Anh Nen
    Texture2D background = LoadTexture("img/nen.jpg");
    if (background.id == 0) printf("[CANH BAO] Khong tim thay img/nen.jpg\n");

    // 4. Load Icons
    Texture2D icons[4];
    icons[0] = LoadTexture("img/avatar.jpg");   
    icons[1] = LoadTexture("img/phone.jpg");    
    icons[2] = LoadTexture("img/identity.png"); 
    icons[3] = LoadTexture("img/calendar.jpg"); 

    for(int i=0; i<4; i++) {
        if(icons[i].id == 0) printf("[CANH BAO] Thieu icon thu %d\n", i);
    }

    // 5. Khoi tao du lieu
    char maThe[12];
    SinhMaTheTuDong(100, maThe); 
    Nhap myForm;
    InitForm(&myForm);

    printf("--- VAO VONG LAP CHINH ---\n");

    // 6. Vòng lặp chính
    while (!WindowShouldClose()) {
        UpdateInputForm(&myForm);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Vẽ ảnh nền
        if (background.id != 0) {
            DrawTexturePro(background, 
                (Rectangle){ 0, 0, (float)background.width, (float)background.height },
                (Rectangle){ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
                (Vector2){ 0, 0 }, 0.0f, WHITE);
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.25f));
        }

        // Vẽ thẻ thư viện
        DrawLibraryCard(&myForm, icons, maThe, mainFont);
        
        EndDrawing();
    }

    // 7. Giải phóng bộ nhớ
    UnloadTexture(background);
    if (mainFont.texture.id != GetFontDefault().texture.id) UnloadFont(mainFont);
    for(int i=0; i<4; i++) UnloadTexture(icons[i]);
    CloseWindow();

    return 0;
}