#ifndef PHIEUMUON_H
#define PHIEUMUON_H
#include "raylib.h"
typedef struct PhieuMuonNode {
    char maPM[15];
    char mathe[15];
    char tentruyen[256];
    char ngaymuon[20];
    char ngaytra[20]; 
    char matruyen[20];
    int trangthai;
    struct PhieuMuonNode* next;
} PhieuMuonNode;
typedef struct {
    Rectangle rec;
    char text[512]; 
    int lettercount;
    bool isfocused;
    float backspacecounter;
} InputBox_PM;
typedef struct {
    InputBox_PM mathe;
    InputBox_PM tentruyen;
    InputBox_PM ngaymuon;
    InputBox_PM ngaytra; 
    InputBox_PM matruyen;
    InputBox_PM ngaydukien;
    bool showsuccess;
    float successtimer;
} FormPhieuMuon;
int DemSoPhieuMuon(PhieuMuonNode *head);
void InitPhieumuon(FormPhieuMuon *Form);
void UpdateVitri(FormPhieuMuon *Form);
void SinhMaPM(int currentcount, char *mathe);
void UpdateInputPM(FormPhieuMuon *Form, PhieuMuonNode **head, int *currentTotalUsers, char *mathe);
void DrawPM(FormPhieuMuon *Form, Texture2D icons[], char *mathe, Font font);
void DrawSuccess(Font font, Texture2D background2, float currenttimer);
bool LuuPhieuMuonVaoFile(char *maphieu, FormPhieuMuon *Form);
void ThemPhieuMuonVaoList(PhieuMuonNode **head, char *maPM, FormPhieuMuon *Form);
int UTF8Width(const char *str, int displayWidth);
void TimTenTruyenTheoMa(const char *matruyen, char *tentruyen_out);
#endif 