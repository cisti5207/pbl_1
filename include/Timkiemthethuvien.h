#ifndef TIMKIEMTHE_H
#define TIMKIEMTHE_H
#include "raylib.h"
#include "ManageUser.h"

typedef struct Form {
    InputBox_BD nhap;
    float scroll;
    bool showmodal;
    char dachonmathe[15];
    float modalscroll;
} FormTimKiemThe;

int LayCodepointKhongDauVaThuong(int codepoint);
void ChuyenChuoiKhongDauVaThuong(const char *nguon, char *dich);
void DocDuLieuTheBanDoc(const char *filepath, BanDoc **head, int *currenttotalusers);
bool KiemTraKhopTimKiem(BanDoc *the, const char *tukhoa);
void InitListBD(FormTimKiemThe *Form);
void Updatetoado(FormTimKiemThe *Form);

// THÊM int *currentState VÀO ĐÂY NÈ BRO
void kiemtralienquantiengviet(FormTimKiemThe *Form, int *currentState);

void DrawKetQuaTimKiem(BanDoc *head, FormTimKiemThe *Form, Font font);
void DrawTimKiemThe(FormTimKiemThe *Form, Texture2D icons[], Font font, BanDoc *head);
#endif