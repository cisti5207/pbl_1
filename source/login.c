#include "raylib.h"
#include <stdio.h>
#include "login.h"
#include <string.h>

Font font[5];
int main() {
    InitWindow(1200, 800, "Login");
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(1000, 600);

    font[0] = SetFontUTF8("font/arial/arial.ttf", 30);
    font[1] = SetFontUTF8("font/cooper/COOPBL.ttf", 100);
    font[2] = SetFontUTF8("font/times/times.ttf", 50);
    font[3] = SetFontUTF8("font/impact/impact.ttf", 30);
    font[4] = SetFontUTF8("font/roboto/Roboto-Regular.ttf", 30);

    Texture2D ImgForFormLogin = LoadTexture("img/image1.png");
    Texture2D ImgForBackground = LoadTexture("img/image2.png");

    int _ScreenWidth = GetScreenWidth();
    int _ScreenHeight = GetScreenHeight();

    int _MonitorWidth = GetMonitorWidth(0);
    int _MonitorHeight = GetMonitorHeight(0);

    if (font[0].texture.id == 0 || font[1].texture.id == 0) {
        printf("Failed to load font!\n");
        return -1;
    }
    
    while (!WindowShouldClose()) {
        if (IsWindowResized()) {
            _ScreenWidth = GetScreenWidth();
            _ScreenHeight = GetScreenHeight();
        }

        BeginDrawing();
        ClearBackground(BLACK);
        
        _DrawLoginBackground(ImgForBackground, _ScreenWidth, _ScreenHeight, _MonitorWidth, _MonitorHeight);
        _DrawLoginForm(ImgForFormLogin, _ScreenWidth, _ScreenHeight);

        DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadFont(font[0]);
    UnloadFont(font[1]);
    CloseWindow();
    return 0;
}

Font SetFontUTF8(char *_font, int _fontSize)
{
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

void _DrawLoginBackground(Texture2D img, int _ScreenWidth, int _ScreenHeight, int _MonitorWidth, int _MonitorHeight) {
    float _ScaleX = (float)_MonitorWidth / img.width;
    float _ScaleY = (float)_MonitorHeight / img.height;

    float _Scale = (_ScaleX > _ScaleY) ? _ScaleX : _ScaleY;

    int _ImgWidth = img.width * _Scale;
    int _ImgHeight = img.height * _Scale;

    int _ImgX = (_ScreenWidth - _ImgWidth) * 0.5f;
    int _ImgY = (_ScreenHeight - _ImgHeight) * 0.5;

    DrawTextureEx(img, (Vector2){_ImgX, _ImgY}, 0.0f, _Scale, WHITE);
    DrawRectangleGradientV(0, 0, _ScreenWidth, _ScreenHeight, Fade(WHITE, 0.2f), Fade(BLACK, 0.2f));
}

void _DrawLoginForm(Texture2D img, int _ScreenWidth, int _ScreenHeight) {
    int _FormWidth = _ScreenWidth * 0.7f;
    int _FormHeight = _ScreenHeight * 0.7f;

    int _FormX = (_ScreenWidth - _FormWidth) * 0.5f;
    int _FormY = (_ScreenHeight - _FormHeight) * 0.5f;

    DrawRectangleRounded((Rectangle){_FormX + 2, _FormY + 2, _FormWidth + 2, _FormHeight + 2}, 0.05f, 10, Fade(DARKBLUE, 0.8f));
    DrawRectangleRounded((Rectangle){_FormX, _FormY, _FormWidth, _FormHeight}, 0.05f, 10, WHITE);
    DrawRectangleRoundedLinesEx((Rectangle){_FormX, _FormY, _FormWidth, _FormHeight}, 0.05f, 10, 2.0f, Fade(DARKBLUE, 0.8f));

    DrawRectangleRounded((Rectangle){_FormX, _FormY, _FormWidth / 2, _FormHeight}, 0.06f, 10, DARKBLUE);

    DrawNameLibText             (_FormX, _FormY, _FormWidth, _FormHeight, _FormHeight * 0.06f);
    DrawWelcomeText             (_FormX, _FormY, _FormWidth, _FormHeight, _FormHeight * 0.06f);
    DrawLoginText               (_FormX, _FormY, _FormWidth, _FormHeight, _FormHeight * 0.1f);
    DrawLoginUsernameText       (_FormX, _FormY, _FormWidth, _FormHeight, _FormHeight * 0.02f);
    DrawLoginPasswordText       (_FormX, _FormY, _FormWidth, _FormHeight, _FormHeight * 0.02f);
    DrawRegisterText            (_FormX, _FormY, _FormWidth, _FormHeight, _FormHeight * 0.01f);
    DrawForgotPasswordText      (_FormX, _FormY, _FormWidth, _FormHeight, _FormHeight * 0.04f);
    DrawLoginButtonText         (_FormX, _FormY, _FormWidth, _FormHeight, _FormHeight * 0.04f);

}

void DrawNameLibText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize) {
    int _TextWidth = MeasureText(NameLibText, _FontSize);
    int _TextX = _FormX + (_FormWidth / 2 - _TextWidth) * 0.5f + _FormWidth * 0.5f;
    int _TextY = _FormY + _FormHeight * 0.1f;
    DrawTextEx(font[1], NameLibText, (Vector2){_TextX, _TextY}, _FontSize, 2, BLACK);
}

void DrawWelcomeText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize) {
    Vector2 _TextWidth = MeasureTextEx(font[1], WelcomeText, _FontSize, 2);
    int _TextX = _FormX + (_FormWidth / 2 - _TextWidth.x) * 0.5f;
    int _TextY = _FormY + _FormHeight * 0.1f;
    DrawTextEx(font[1], WelcomeText, (Vector2){_TextX, _TextY}, _FontSize, 2, WHITE);
    DrawLine(_TextX, _TextY + _FontSize + 2, _TextX + _TextWidth.x, _TextY + _FontSize + 2, WHITE);
}

void DrawLoginText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize) {
    int _TextWidth = MeasureText(LoginText, _FontSize);
    int _TextX = _FormX + _FormWidth/2 + (_FormWidth / 2 - _TextWidth) * 0.5f;
    int _TextY = _FormY + _FormHeight * 0.2f;
    DrawTextEx(font[1], LoginText, (Vector2){_TextX, _TextY}, _FontSize, 2, BLACK);
}

void DrawLoginUsernameText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize) {
    int _TextX = _FormX + _FormWidth * 0.55f;
    int _TextY = _FormY + _FormHeight * 0.4f;
    DrawRectangleRoundedLinesEx((Rectangle){_TextX - 10, _TextY - 10, _FormWidth * 0.4f, _FontSize + 20}, 0.1f, 10, 2.0f, LIGHTGRAY);
    DrawRectangleRounded((Rectangle){_TextX - 10, _TextY - 10, _FormWidth * 0.4f, _FontSize + 20}, 0.1f, 10, BEIGE);
    DrawTextEx(font[2], LoginUsernameText, (Vector2){_TextX, _TextY}, _FontSize, 2, BLACK);
}

void DrawLoginPasswordText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize) {
    int _TextX = _FormX + _FormWidth * 0.55f;
    int _TextY = _FormY + _FormHeight * 0.5f;
    DrawRectangleRoundedLinesEx((Rectangle){_TextX - 10, _TextY - 10, _FormWidth * 0.4f, _FontSize + 20}, 0.1f, 15, 2.0f, LIGHTGRAY);
    DrawRectangleRounded((Rectangle){_TextX - 10, _TextY - 10, _FormWidth * 0.4f, _FontSize + 20}, 0.1f, 10, BEIGE);
    DrawTextEx(font[2], LoginPasswordText, (Vector2){_TextX, _TextY}, _FontSize, 2, BLACK);
}

void DrawRegisterText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize) {
    int _TextWidth = MeasureText(RegisterText, _FontSize);
    int _TextX = _FormX + _FormWidth * 0.55f;
    int _TextY = _FormY + _FormHeight * 0.5f;
    DrawTextEx(font[0], RegisterText, (Vector2){_TextX, _TextY}, _FontSize, 2, BLACK);
    DrawLine(_TextX, _TextY + _FontSize + 2, _TextX + _TextWidth, _TextY + _FontSize + 2, BLACK);
}

void DrawForgotPasswordText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize) {
    int _TextWidth = MeasureText(ForgotPasswordText, _FontSize);
    int _TextX = _FormX + _FormWidth * 0.55f;
    int _TextY = _FormY + _FormHeight * 0.6f;
    DrawTextEx(font[0], ForgotPasswordText, (Vector2){_TextX, _TextY}, _FontSize, 2, BLACK);
    DrawLine(_TextX, _TextY + _FontSize + 2, _TextX + _TextWidth, _TextY + _FontSize + 2, BLACK);
}

void DrawLoginButtonText(int _FormX, int _FormY, int _FormWidth, int _FormHeight, int _FontSize) {
    int _TextWidth = MeasureText(LoginButtonText, _FontSize);
    int _TextX = _FormX + _FormWidth/2 + (_FormWidth / 2 - _TextWidth) * 0.5f;
    int _TextY = _FormY + _FormHeight * 0.7f;
    DrawTextEx(font[0], LoginButtonText, (Vector2){_TextX, _TextY}, _FontSize, 2, BLACK);
}