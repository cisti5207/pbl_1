#ifndef MANAGE_USER_H
#define MANAGE_USER_H

#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_INPUT 256
#define MAX_HISTORY 100
#define VIRTUAL_WIDTH 1200
#define VIRTUAL_HEIGHT 800

// Cấu trúc dữ liệu mượn trả
typedef struct {
    char tenTruyen[MAX_INPUT];
    char ngayMuon[MAX_INPUT];
    char ngayTra[MAX_INPUT];
    int soLuongMuon;
    int trangThai; 
} MuonTra;

// Cấu trúc dữ liệu bạn đọc
typedef struct {
    char maThe[MAX_INPUT];
    char hoTen[MAX_INPUT];
    char cccd[MAX_INPUT];
    char sdt[MAX_INPUT];
    char hanSD[MAX_INPUT];
    MuonTra lichSu[MAX_HISTORY];
    int Soluotmuon;
} BanDoc;

typedef struct Node {
    BanDoc data;
    struct Node *next;
} node;

// Cấu trúc hỗ trợ giao diện nhập liệu (Input Box)
typedef struct {
    Rectangle rec;
    char text[MAX_INPUT];
    int length;
    bool isFocused;
    const char* label;
} InputBox_BD;

// Form chứa 4 ô nhập liệu trên thẻ
typedef struct {
    InputBox_BD inputs[4]; // 0: HoTen, 1: SDT, 2: CCCD, 3: HanSD
} FormNhap;

// Khai báo các hàm để file .c nào cũng có thể gọi
void DrawRectangleRoundedLinesEx(Rectangle rec, float roundness, int segments, float thickness, Color color);
void UpdateInput(InputBox_BD *box, Vector2 mousePos);
void DrawLibraryCard(Rectangle cardRect, FormNhap *form, const char* maTheHienTai);
void SinhMaTheTuDong(int tongSo, char *maTheMoi);

#endif