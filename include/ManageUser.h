#ifndef MANAGEUSER_H
#define MANAGEUSER_H

#include "raylib.h"

typedef struct BanDoc {
    char maThe[15];
    char hoTen[100];
    char sdt[20];
    char cccd[25];
    char hanSD[20];
    struct BanDoc* next;
} BanDoc;

typedef struct {
    Rectangle rec;
    char text[256];
    int letterCount;
    bool isFocused;
    float backspaceCounter; 
} InputBox_BD;

typedef struct {
    InputBox_BD hoTen;
    InputBox_BD sdt;
    InputBox_BD cccd;
    InputBox_BD hanSD;
    bool showSuccess;    
    float successTimer;  
} Nhap;
 
void InitForm(Nhap *form);
void UpdateFormPosition(Nhap *form); 
// Đã thêm int *currentState vào đây!
void UpdateInputForm(Nhap *form, BanDoc **head, int *currentTotalUsers, char *maThe, int *currentState); 
void DrawLibraryCard(Nhap *form, Texture2D icons[], char *maThe, Font font);
void DrawSuccessMessage(Font font, Texture2D background2, float currentTimer); 

void SinhMaTheTuDong(int currentCount, char *maThe);
void ThemBanDocVaoList(BanDoc **head, char *maThe, Nhap *form);
bool LuuThanhVienVaoFile(char *maThe, Nhap *form);
void FreeMemberList(BanDoc *head);
void DrawTheBanDoc_TimKiem(BanDoc *the, Font font, float toa_do_x, float toa_do_y);
void LayMaTheTiepTheo(BanDoc *head, char *maThe);
#endif