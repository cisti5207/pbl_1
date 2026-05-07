#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int tongSo = 0;
extern Font font[6];
Font SetFontUTF8_BanDoc(char *_fontPath, int _fontSize) {
    // Copy y chang bảng mã "quyền lực" từ code của bạn ông
    int codepoints[] = {
        // Basic Latin
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

        // Vietnamese đầy đủ dấu (Quan trọng cho PBL 1)
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

    Font finalFont = LoadFontEx(_fontPath, _fontSize, codepoints, codepointCount);

    return finalFont;
}
void APP() {
    InitWindow(1200, 800, "Login");
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(1000, 600);

    font[0] = SetFontUTF8_BanDoc("font/arial/arial.ttf", 24);
    font[1] = SetFontUTF8_BanDoc("font/cooper/COOPBL.ttf", 100);
    font[2] = SetFontUTF8_BanDoc("font/times/times.ttf", 50);
    font[3] = SetFontUTF8_BanDoc("font/impact/impact.ttf", 50);
    font[4] = SetFontUTF8_BanDoc("font/roboto/Roboto-Regular.ttf", 50);
    font[5] = SetFontUTF8_BanDoc("font/gill/gillubcd.ttf", 100);
}
#ifndef BANDOC_H
#define BANDOC_H
#define MAX_INPUT 256
#define MAX_HISTORY 100 
typedef struct {
    char tenTruyen[MAX_INPUT];   // Tên truyện/sách mượn
    char ngayMuon[MAX_INPUT];    // Ngày mượn (dd/mm/yyyy)
    char ngayTra[MAX_INPUT];     // Ngày trả (hoặc để trống nếu chưa trả)
    int soLuongMuon;             // Số lượng mượn (ví dụ: mượn 2 cuốn cùng lúc)
    int trangThai;               // 0: Chưa trả, 1: Đã trả
} MuonTra;
typedef struct {
    char maThe[MAX_INPUT];       // 8 số
    char hoTen[MAX_INPUT];       // Tiếng Việt có dấu
    char cccd[MAX_INPUT];        // 12 số
    char sdt[MAX_INPUT];         // 10 số
    char hanSD[MAX_INPUT];       // Hạn sử dụng thẻ
    
    MuonTra lichSu[MAX_HISTORY];  // Mảng chứa lịch sử
    int Soluotmuon;           // Tổng số lượt mượn trong lịch sử
} BanDoc;
typedef struct Node {
    BanDoc data;
    struct Node *next;
} Node;
typedef struct {
    Rectangle rec;            // Vị trí và kích thước ô nhập trên màn hình
    char text[MAX_INPUT];     // Nội dung chữ đang nhập
    int length;               // Độ dài chuỗi hiện tại
    bool isFocused;           // Ô này có đang được chọn để gõ không?
} InputBox_BD;
typedef struct {
    InputBox_BD inputHoTen;  // Ô nhập họ tên
    InputBox_BD inputCCCD;   // Ô nhập căn cước
    InputBox_BD inputSDT;    // Ô nhập số điện thoại
    InputBox_BD inputHanSD;  // Ô nhập hạn sử dụng
} Nhap;
#endif
void InitList(Node **head) {
    *head = NULL; 
}
void SinhMaTheTuDong(int tongSo, char *maTheMoi) {
    sprintf(maTheMoi, "%08d", tongSo + 1);
}
Node* ThemBanDocMoi(Node *head, Nhap *form, int *tongSo) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    
    if (newNode == NULL) {
        printf("Loi: Khong the cap phat bo nho!\n");
        return head;
    }
    SinhMaTheTuDong(*tongSo, newNode->data.maThe);
    
    strcpy(newNode->data.hoTen, form->inputHoTen.text);
    strcpy(newNode->data.cccd, form->inputCCCD.text);
    strcpy(newNode->data.sdt, form->inputSDT.text);
    strcpy(newNode->data.hanSD, form->inputHanSD.text);
    newNode->data.Soluotmuon = 0;

    newNode->next = head; 
    head = newNode;       
    (*tongSo)++;

    return head;
}

