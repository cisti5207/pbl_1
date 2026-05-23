#ifndef TIMKIEMTHE_H
#define TIMKIEMTHE_H
#include "raylib.h"
#include "ManageUser.h"
typedef struct Form {
    InputBox_BD nhap;
    float scroll;

} FormTimKiemThe;
int LayCodepointKhongDauVaThuong(int codepoint);
void ChuyenChuoiKhongDauVaThuong(const char *nguon, char *dich);
void DocDuLieuTheBanDoc(const char *filepath, BanDoc **head, int *currenttotalusers);
bool KiemTraKhopTimKiem(BanDoc *the, const char *tukhoa);
void InitListBD(FormTimKiemThe *Form);
void Updatetoado(FormTimKiemThe *Form);
void kiemtralienquantiengviet(FormTimKiemThe *Form);
void DrawKetQuaTimKiem(BanDoc *head, FormTimKiemThe *Form, Font font);
void DrawTimKiemThe(FormTimKiemThe *Form, Texture2D icons[], Font font, BanDoc *head);
#endif 
