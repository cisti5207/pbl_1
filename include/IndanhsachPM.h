#ifndef INDANHSACHPM_H
#define INDANHSACHPM_H

#include "raylib.h"
#include "Phieumuon.h"

// Định nghĩa 2 chế độ xem (Tabs)
typedef enum {
    VIEW_TAT_CA = 0,
    VIEW_CHUA_TRA
} CheDoXemPM;

typedef struct {
    CheDoXemPM currentTab;
    float scrollList;
} FormInDanhSachPM;

void InitFormInDanhSachPM(FormInDanhSachPM *form);
void UpdateLogicInDanhSachPM(FormInDanhSachPM *form, PhieuMuonNode *headPM, int *currentState);
void DrawGiaoDienInDanhSachPM(FormInDanhSachPM *form, PhieuMuonNode *headPM, Font font);

#endif