#ifndef MANAGEUSER_H
#define MANAGEUSER_H

#include "raylib.h"

typedef struct {
    Rectangle rec;
    char text[256];
    int letterCount;
    bool isFocused;
} InputBox_BD;

typedef struct {
    InputBox_BD hoTen;
    InputBox_BD sdt;
    InputBox_BD cccd;
    InputBox_BD hanSD;
} Nhap;

void InitForm(Nhap *form);
void UpdateFormPosition(Nhap *form); 
void UpdateInputForm(Nhap *form);
void DrawLibraryCard(Nhap *form, Texture2D icons[], char *maThe, Font font);
void SinhMaTheTuDong(int currentCount, char *maThe);

#endif