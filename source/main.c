#include "ManageUser.h"
#include "raymath.h"

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(VIRTUAL_WIDTH, VIRTUAL_HEIGHT, "DUT Library System - Pure C");
    SetTargetFPS(60);

    // Tạo "Tờ giấy ảo" để giữ tỷ lệ 1200x800 cố định (Cho dù màn hình là 2.5K)
    RenderTexture2D target = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    FormNhap form;
    const char* labels[] = { "Ho va ten:", "So dien thoai:", "CCCD:", "Han su dung:" };
    for (int i = 0; i < 4; i++) {
        // Vị trí các ô nhập liệu được tính toán nằm ở nửa phải của thẻ
        form.inputs[i].rec = (Rectangle){ 780, 280.0f + (float)(i * 65), 320, 35 };
        form.inputs[i].length = 0;
        form.inputs[i].isFocused = false;
        form.inputs[i].label = labels[i];
        memset(form.inputs[i].text, 0, MAX_INPUT);
    }

    int tongSoHienTai = 0;
    char currentMaThe[MAX_INPUT];
    SinhMaTheTuDong(tongSoHienTai, currentMaThe);
    
    Rectangle cardRect = { (VIRTUAL_WIDTH - 650)/2.0f, (VIRTUAL_HEIGHT - 400)/2.0f, 650, 400 };

    while (!WindowShouldClose()) {
        // Tính toán tỷ lệ co giãn thực tế
        float scale = fminf((float)GetScreenWidth()/VIRTUAL_WIDTH, (float)GetScreenHeight()/VIRTUAL_HEIGHT);
        
        // Chuyển tọa độ chuột từ màn hình thật sang tọa độ ảo 1200x800
        Vector2 mouse = GetMousePosition();
        Vector2 virtualMouse = {
            (mouse.x - (GetScreenWidth() - (VIRTUAL_WIDTH * scale)) * 0.5f) / scale,
            (mouse.y - (GetScreenHeight() - (VIRTUAL_HEIGHT * scale)) * 0.5f) / scale
        };

        // Cập nhật logic các ô nhập
        for (int i = 0; i < 4; i++) UpdateInput(&form.inputs[i], virtualMouse);

        BeginTextureMode(target);
            ClearBackground(DARKGRAY); 
            DrawLibraryCard(cardRect, &form, currentMaThe);
        EndTextureMode();

        BeginDrawing();
            ClearBackground(BLACK); 
            
            // Vẽ Render Texture lên màn hình thật (Tự động scale cho màn 2.5K)
            DrawTexturePro(target.texture, 
                (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height },
                (Rectangle){ (GetScreenWidth() - (VIRTUAL_WIDTH * scale)) * 0.5f, 
                             (GetScreenHeight() - (VIRTUAL_HEIGHT * scale)) * 0.5f, 
                             VIRTUAL_WIDTH * scale, GetScreenHeight() }, // Fix height for 2.5K
                (Vector2){ 0, 0 }, 0.0f, WHITE);
        EndDrawing();
    }

    // Giải phóng bộ nhớ
    UnloadRenderTexture(target);
    CloseWindow();
    return 0;
}