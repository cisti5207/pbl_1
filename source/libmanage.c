#include "libmanage.h"
#include "raylib.h"
#include <ctype.h>
#include <string.h>
<<<<<<< HEAD
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
=======
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
// Khai báo mảng hạt toàn cục dùng chung cho cả ứng dụng
Particle AppParticles[MAX_PARTICLE];

// ==========================================
// CÁC HÀM XỬ LÝ NỀN ĐỘNG (PARTICLES)
// ==========================================

void InitParticles(Size size) {
    for (int i = 0; i < MAX_PARTICLE; i++) {
        AppParticles[i].Position = (Vector2) {
            (float)(GetRandomValue(0, size.Monitor.x)),
            (float)(GetRandomValue(0, size.Monitor.y))
        };
        AppParticles[i].Velocity = (Vector2) {
            (GetRandomValue(-50, 50) / 100.0f - 0.5f) * 0.3f,
            -0.2f - GetRandomValue(-50, 50) / 500.0f
        };
        AppParticles[i].radius = 1.0f + GetRandomValue(0, 4);
        AppParticles[i].alpha = 0.1f + GetRandomValue(0, 100) / 200.0f;
    }
}

void UpdateParticlesPosition(Size size) {
    for (int i = 0; i < MAX_PARTICLE; i++) {
        AppParticles[i].Position.x += AppParticles[i].Velocity.x;
        AppParticles[i].Position.y += AppParticles[i].Velocity.y;

        if (AppParticles[i].Position.x < -10) 
            AppParticles[i].Position.x = size.Monitor.x + 10;
        if (AppParticles[i].Position.x > size.Monitor.x + 10)
            AppParticles[i].Position.x = -10;

        if (AppParticles[i].Position.y < -10) {
            AppParticles[i].Position.y = size.Monitor.y + 10;
            AppParticles[i].Position.x = (float)(GetRandomValue(0, size.Monitor.x));
        }
    }
}

void DrawBackgroundParticles(void) {
    for (int i = 0; i < MAX_PARTICLE; i++) {
        DrawCircleV(AppParticles[i].Position, AppParticles[i].radius, Fade(SKYBLUE, AppParticles[i].alpha));
    }
    
    for (int i = 0; i < MAX_PARTICLE; i++) {
        for (int j = i + 1; j < MAX_PARTICLE; j++) {
            float dx = AppParticles[i].Position.x - AppParticles[j].Position.x;
            float dy = AppParticles[i].Position.y - AppParticles[j].Position.y;
            float distance = sqrtf(dx * dx + dy * dy);

            if (distance < 120.0f) {
                float alpha = 1.0f - (distance / 120.0f);
                float thickness = alpha * 1.5f;
                DrawLineEx(AppParticles[i].Position, AppParticles[j].Position, thickness, Fade(WHITESMOKE, alpha * 0.15f));
            }
        }
    }
}

Color AnimatedBackground(void) {
    float t = GetTime();
    int r = 10 + (int)(sin(t * 0.4f) * 10.0f);
    int g = 20 + (int)(sin(t * 0.7f) * 15.0f);
    int b = 35 + (int)(sin(t * 0.3f) * 15.0f);
    return (Color){ r, g, b, 255 };
}


// ==========================================
// CÁC HÀM TIỆN ÍCH UI & CHUỖI
// ==========================================
>>>>>>> 381e2b85a73eb32f5adb00643f2275eeaf34bf6f

void LoadSize(Size *FormSize, Vector2 monitor, Vector2 screen, Vector2 scale, Vector2 mouse){
    if (monitor.x || monitor.y) FormSize->Monitor = monitor;
    if (mouse.x || mouse.y) FormSize->Mouse = mouse;
    if (scale.x || scale.y) FormSize->Scale = scale;
    if (screen.x || screen.y) FormSize->Screen = screen;
}

int lenStringUTF8(const char *str){
    if (!str) return 0;
    int len = 0;
    while (*str) {
        if ((*str & 0xC0) != 0x80) len++;
        str++;
    }
    return len;
}

void trim(char *str){
    if (!str || *str == '\0') return;
    char *start = str;
    while (*start && isspace((unsigned char)*start)) start++;

    if (*start == '\0') {
        *str = '\0';
        return;
    }
    char *end = str + strlen(str) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    
    *(end + 1) = '\0';
    if (start != str) memmove(str, start, end - start + 2); 
}

Font SetFontUTF8(const char *_font, int _fontSize){
    int codepoints[] = {
        32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,
        58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,
        81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,
        97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,
        113,114,115,116,117,118,119,120,121,122,123,124,125,126,
        192,193,194,195,200,201,202,204,205,210,211,212,213,217,218,221,
        224,225,226,227,232,233,234,236,237,242,243,244,245,249,250,253,
        258,259,272,273,296,297,416,417,431,432,
        7840,7841,7842,7843,7844,7845,7846,7847,7848,7849,7850,7851,7852,7853,7854,7855,7856,7857,7858,7859,
        7860,7861,7862,7863,7864,7865,7866,7867,7868,7869,7870,7871,7872,7873,7874,7875,7876,7877,7878,7879,
        7880,7881,7882,7883,7884,7885,7886,7887,7888,7889,7890,7891,7892,7893,7894,7895,7896,7897,7898,7899,
        7900,7901,7902,7903,7904,7905,7906,7907,7908,7909,7910,7911,7912,7913,7914,7915,7916,7917,7918,7919,
        7920,7921,7922,7923,7924,7925,7926,7927,7928,7929
    };
    int codepointCount = sizeof(codepoints) / sizeof(codepoints[0]);
    return LoadFontEx(_font, _fontSize, codepoints, codepointCount);
}

void DeleteLastChar(InputBox *input){
    if (input->length > 0) {
        input->length--;
        while (input->length > 0 && (input->text[input->length] & 0xC0) == 0x80) input->length--;
        input->text[input->length] = '\0';
    }
}

void DeleteLastWord(InputBox *input){
    if (input->length <= 0) return;
    while (input->length > 0 && input->text[input->length - 1] == ' ') input->length--;
    while (input->length > 0 && input->text[input->length - 1] != ' ') {
        input->length--;
        while (input->length > 0 && (input->text[input->length] & 0xC0) == 0x80) input->length--;
    }
    input->text[input->length] = '\0';
}

void PasteClipboard(InputBox *input){
    const char *clip = GetClipboardText();
    if (!clip) return;
    int len = strlen(clip);
    if (input->length + len >= MAX_INPUT) len = MAX_INPUT - input->length - 1;
    if (len > 0) {
        memcpy(&input->text[input->length], clip, len);
        input->length += len;
        input->text[input->length] = '\0';
    }
}

void UpdateInputBox(InputBox *input){
    bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
    if (ctrl && IsKeyPressed(KEY_V)) PasteClipboard(input);
    if (ctrl && IsKeyPressed(KEY_BACKSPACE)) {
        DeleteLastWord(input);
    } else {
        int keycode = GetKeyPressed();
        while (keycode > 0) {
            if (keycode == KEY_BACKSPACE) DeleteLastChar(input);
            keycode = GetKeyPressed();
        }
        if (IsKeyPressedRepeat(KEY_BACKSPACE)) DeleteLastChar(input);
    }
    int charKey = GetCharPressed();
    while (charKey > 0) {
        int size = 0;
        const char *utf8 = CodepointToUTF8(charKey, &size);
        if (input->length + size < MAX_INPUT) {
            memcpy(&input->text[input->length], utf8, size);
            input->length += size;
            input->text[input->length] = '\0';
        }
        charKey = GetCharPressed();
    }
}

int UTF8Width(const char *s, int width){
    if (!s) return width;
    return width + ((int)strlen(s) - lenStringUTF8(s));
}

void DrawIcon(Rectangle box, Texture2D icon){
    float scaleX = box.width / icon.width;
    float scaleY = box.height / icon.height;
    float scale = (scaleX < scaleY) ? scaleX : scaleY;

    Vector2 origin = { 0, 0 };
    Rectangle source = { 0, 0, icon.width, icon.height };
    Rectangle dest = {
        box.x + (box.width - icon.width * scale) / 2,
        box.y + (box.height - icon.height * scale) / 2,
        icon.width * scale,
        icon.height * scale
    };
    DrawTexturePro(icon, source, dest, origin, 0.0f, WHITE);
}

float FindRoundness(float len, float width, float height){
    float min = (width > height) ? height : width;
    if (min <= 0.0f) return 0.0f; 
    return len / (min / 2.0f);
}

<<<<<<< HEAD
int TinhSoNgay(const char *ngaymuon, const char *ngaytrathucte) {
    struct tm tmStart = {0}, tmEnd = {0};
    sscanf(ngaymuon, "%d/%d/%d", &tmStart.tm_mday, &tmStart.tm_mon, &tmStart.tm_year);
    sscanf(ngaytrathucte, "%d/%d/%d", &tmEnd.tm_mday, &tmEnd.tm_mon, &tmEnd.tm_year);
    tmStart.tm_year -= 1900; tmStart.tm_mon -= 1;
    tmEnd.tm_year -= 1900; tmEnd.tm_mon -= 1;
    double diff = difftime(mktime(&tmEnd), mktime(&tmStart));
    return (int)(diff / 86400);
}

=======
int FindFontSize(float Width, Font font, int space, const char *str){
    if (!str || str[0] == '\0') return FONT_SIZE;
    float TextWidth = MeasureTextEx(font, str, FONT_SIZE, space).x;
    if (TextWidth == 0.0f) return FONT_SIZE; 
    return (Width / TextWidth) * FONT_SIZE;
}

void FormatPriceToVND(int price, char *buffer) {
    if (price == 0) {
        strcpy(buffer, "0 VND");
        return;
    }
    
    char temp[32];
    sprintf(temp, "%d", price); // Chuyển số thành chuỗi cơ bản
    int len = strlen(temp);
    
    int j = 0;
    for (int i = 0; i < len; i++) {
        buffer[j++] = temp[i];
        // Đặt dấu chấm sau mỗi 3 chữ số tính từ bên phải sang
        if ((len - i - 1) > 0 && (len - i - 1) % 3 == 0) {
            buffer[j++] = '.';
        }
    }
    buffer[j] = '\0';
    strcat(buffer, " VND"); // Nối chữ VND vào đuôi
}

void DrawTextAutoWrap(Font font, const char *text, Rectangle bounds, float fontSize, float spacing, Color color) {
    float currentX = bounds.x;
    float currentY = bounds.y;
    float lineHeight = fontSize * 1.2f; // Hệ số dãn dòng (bạn có thể chỉnh lên 1.5f nếu muốn thoáng hơn)

    const char *ptr = text;
    char wordBuffer[512]; // Bộ đệm chứa từng từ (tối đa 512 bytes/từ)

    // Lấy kích thước thực tế của 1 dấu cách (đã bao gồm cả spacing)
    float spaceWidth = MeasureTextEx(font, " ", fontSize, spacing).x;

    while (*ptr != '\0') {
        // 1. Xử lý ký tự xuống dòng chủ động (\n)
        if (*ptr == '\n') {
            currentX = bounds.x;
            currentY += lineHeight;
            ptr++;
            continue;
        }

        // 2. Xử lý dấu cách
        if (*ptr == ' ') {
            // Không cộng thêm khoảng trắng nếu đang đứng ở mép trái (đầu dòng mới)
            if (currentX > bounds.x) {
                currentX += spaceWidth;
            }
            ptr++;
            continue;
        }

        // 3. Tách lấy một từ (cụm ký tự không chứa dấu cách hoặc \n)
        // Trong C, các byte của UTF-8 không bao giờ bị nhầm với dấu cách (0x20) hay \n (0x0A)
        int i = 0;
        while (ptr[i] != ' ' && ptr[i] != '\n' && ptr[i] != '\0' && i < sizeof(wordBuffer) - 1) {
            wordBuffer[i] = ptr[i];
            i++;
        }
        wordBuffer[i] = '\0'; // Chốt chuỗi cho từ hiện tại

        // 4. Đo chiều rộng của từ vừa tách
        Vector2 wordSize = MeasureTextEx(font, wordBuffer, fontSize, spacing);

        // 5. Kiểm tra nếu vẽ từ này sẽ bị tràn ra khỏi lề phải của Rectangle
        if (currentX + wordSize.x > bounds.x + bounds.width) {
            currentX = bounds.x;    // Về lại lề trái
            currentY += lineHeight; // Kéo tọa độ Y xuống dòng dưới
        }

        // 6. Kiểm tra nếu text tràn ra khỏi lề dưới của Rectangle hitbox
        if (currentY + wordSize.y > bounds.y + bounds.height) {
            break; // Hết chỗ để vẽ, lập tức dừng vòng lặp (cắt phần text thừa)
        }

        // 7. Vẽ từ lên màn hình
        Vector2 position = { currentX, currentY };
        DrawTextEx(font, wordBuffer, position, fontSize, spacing, color);

        // Cập nhật vị trí X cho từ tiếp theo
        currentX += wordSize.x;
        ptr += i; // Dịch con trỏ chuỗi qua từ vừa xử lý xong
    }
}

bool StringContains(const char *haystack, const char *needle) {
    if (!haystack || !needle) return false;
    if (needle[0] == '\0') return true;

    int lenH = strlen(haystack);
    int lenN = strlen(needle);

    for (int i = 0; i <= lenH - lenN; i++) {
        int j;
        for (j = 0; j < lenN; j++) {
            // Đưa về chữ thường để so sánh (hoạt động tốt với chuẩn ASCII/Tiếng Anh không dấu)
            char c1 = tolower((unsigned char)haystack[i + j]);
            char c2 = tolower((unsigned char)needle[j]);
            if (c1 != c2) break; // Lệch 1 ký tự là bỏ qua ngay
        }
        if (j == lenN) return true; // Khớp toàn bộ needle
    }
    return false;
}
>>>>>>> 381e2b85a73eb32f5adb00643f2275eeaf34bf6f
