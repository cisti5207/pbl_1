#ifndef MANAGEUSER_H
#define MANAGEUSER_H
#include <stdio.h>
#include "raylib.h"
#include "../include/ManageUser.h"
// --- 1. PHẦN STRUCT (Giữ nguyên như cũ) ---
#define MAX_INPUT 256
#define MAX_HISTORY 100 

typedef struct {
    char tenTruyen[MAX_INPUT];
    char ngayMuon[MAX_INPUT];
    char ngayTra[MAX_INPUT];
    int soLuongMuon;
    int trangThai;
} MuonTra;

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
} Node;

typedef struct {
    Rectangle rec;
    char text[MAX_INPUT];
    int length;
    bool isFocused;
} InputBox_BD;

typedef struct {
    InputBox_BD inputHoTen;
    InputBox_BD inputCCCD;
    InputBox_BD inputSDT;
    InputBox_BD inputHanSD;
} Nhap;

// --- 2. DANH SÁCH TẤT CẢ CÁC HÀM (Prototypes) ---

// Hàm khởi tạo Font và Cửa sổ
void APP();
Font SetFontUTF8_BanDoc(char *_fontPath, int _fontSize);

// Hàm xử lý Danh sách liên kết (Linked List)
void InitList(Node **head);
void SinhMaTheTuDong(int tongSo, char *maTheMoi);
Node* ThemBanDocMoi(Node *head, Nhap *form, int *tongSo);

// Hàm vẽ giao diện (UI)
void VeTheThuVien(Font fontPBL, BanDoc *bd, Nhap*form);
void InitForm_BD(Nhap *form);
void XuLyNhap(InputBox_BD *box);
#endif