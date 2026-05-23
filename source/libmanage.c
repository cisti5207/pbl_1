#include "libmanage.h"
#include "raylib.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void LoadSize(Size *FormSize, Vector2 monitor, Vector2 screen, Vector2 scale, Vector2 mouse){
    if (monitor.x || monitor.y)
        FormSize->Monitor = monitor;

    if (mouse.x || mouse.y)
        FormSize->Mouse = mouse;

    if (scale.x || scale.y)
        FormSize->Scale = scale;
    
    if (screen.x || screen.y)
        FormSize->Screen = screen;
}

int lenStringUTF8(const char *str){
    int len = 0;
    while (*str) {
        if ((*str & 0xC0) != 0x80) { // Kiểm tra nếu không phải là byte tiếp theo của một ký tự UTF-8
            len++;
        }
        str++;
    }
    return len;
}
void trim(char *str){
    // Xóa khoảng trắng ở đầu
    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    // Xóa khoảng trắng ở cuối
    char *end = str + strlen(str) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0';

    // Dịch chuỗi về đầu mảng
    if (start != str) {
        memmove(str, start, end - start + 2); // +2 để bao gồm cả ký tự null
    }
}
Font SetFontUTF8(const char *_font, int _fontSize){
    int codepoints[] = {
    // Basic Latin (ký tự thường)
    32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
    48,49,50,51,52,53,54,55,56,57,
    58,59,60,61,62,63,64,
    65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,
    81,82,83,84,85,86,87,88,89,90,
    91,92,93,94,95,96,
    97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,
    113,114,115,116,117,118,119,120,121,122,
    123,124,125,126,

    // Vietnamese Latin bổ sung
    192,193,194,195,200,201,202,204,205,210,211,212,213,
    217,218,221,
    224,225,226,227,232,233,234,236,237,242,243,244,245,
    249,250,253,
    258,259,272,273,296,297,416,417,431,432,

    // Vietnamese đầy đủ dấu (quan trọng nhất)
    7840,7841,7842,7843,7844,7845,7846,7847,7848,7849,
    7850,7851,7852,7853,7854,7855,7856,7857,7858,7859,
    7860,7861,7862,7863,7864,7865,7866,7867,7868,7869,
    7870,7871,7872,7873,7874,7875,7876,7877,7878,7879,
    7880,7881,7882,7883,7884,7885,7886,7887,7888,7889,
    7890,7891,7892,7893,7894,7895,7896,7897,7898,7899,
    7900,7901,7902,7903,7904,7905,7906,7907,7908,7909,
    7910,7911,7912,7913,7914,7915,7916,7917,7918,7919,
    7920,7921,7922,7923,7924,7925,7926,7927,7928,7929
    };

    int codepointCount = sizeof(codepoints) / sizeof(codepoints[0]);

    Font finalFont = LoadFontEx(_font, _fontSize, codepoints, codepointCount);

    return finalFont;
}

void DeleteLastChar(InputBox *input){

    if (input->length > 0) {

        input->length--;

        // UTF-8 continuation byte
        while (
            input->length > 0 &&
            (input->text[input->length] & 0xC0) == 0x80
        ) {
            input->length--;
        }

        input->text[input->length] = '\0';
    }
}
void DeleteLastWord(InputBox *input){

    if (input->length <= 0) return;

    // Xóa khoảng trắng cuối
    while (
        input->length > 0 &&
        input->text[input->length - 1] == ' '
    ) {
        input->length--;
    }

    // Xóa nguyên từ
    while (
        input->length > 0 &&
        input->text[input->length - 1] != ' '
    ) {

        input->length--;

        // UTF-8 continuation byte
        while (
            input->length > 0 &&
            (input->text[input->length] & 0xC0) == 0x80
        ) {
            input->length--;
        }
    }

    input->text[input->length] = '\0';
}
void PasteClipboard(InputBox *input){

    const char *clip = GetClipboardText();

    if (!clip) return;

    int len = strlen(clip);

    if (input->length + len >= MAX_INPUT) {
        len = MAX_INPUT - input->length - 1;
    }

    if (len > 0) {

        memcpy(
            &input->text[input->length],
            clip,
            len
        );

        input->length += len;

        input->text[input->length] = '\0';
    }
}
void UpdateInputBox(InputBox *input){

    bool ctrl =
        IsKeyDown(KEY_LEFT_CONTROL) ||
        IsKeyDown(KEY_RIGHT_CONTROL);

    // ===== CTRL + V =====
    if (ctrl && IsKeyPressed(KEY_V)) {
        PasteClipboard(input);
    }

    // ===== CTRL + BACKSPACE =====
    if (ctrl && IsKeyPressed(KEY_BACKSPACE)) {
        DeleteLastWord(input);
    }
    else {

        // ===== BACKSPACE =====
        int keycode = GetKeyPressed();

        while (keycode > 0) {

            if (keycode == KEY_BACKSPACE) {
                DeleteLastChar(input);
            }

            keycode = GetKeyPressed();
        }

        if (IsKeyPressedRepeat(KEY_BACKSPACE)) {
            DeleteLastChar(input);
        }
    }

    // ===== INPUT UTF-8 =====
    int charKey = GetCharPressed();

    while (charKey > 0) {

        int size = 0;

        const char *utf8 =
            CodepointToUTF8(charKey, &size);

        if (input->length + size < MAX_INPUT) {

            memcpy(
                &input->text[input->length],
                utf8,
                size
            );

            input->length += size;

            input->text[input->length] = '\0';
        }

        charKey = GetCharPressed();
    }
}
int UTF8Width(const char *s, int width){
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

    return len / min;
}

int TinhSoNgay(const char *ngaymuon, const char *ngaytrathucte) {
    struct tm tmStart = {0}, tmEnd = {0};
    sscanf(ngaymuon, "%d/%d/%d", &tmStart.tm_mday, &tmStart.tm_mon, &tmStart.tm_year);
    sscanf(ngaytrathucte, "%d/%d/%d", &tmEnd.tm_mday, &tmEnd.tm_mon, &tmEnd.tm_year);
    tmStart.tm_year -= 1900; tmStart.tm_mon -= 1;
    tmEnd.tm_year -= 1900; tmEnd.tm_mon -= 1;
    double diff = difftime(mktime(&tmEnd), mktime(&tmStart));
    return (int)(diff / 86400);
}

