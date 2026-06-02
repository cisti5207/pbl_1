#ifndef TIMKIEMTHETHUVIEN_H
#define TIMKIEMTHETHUVIEN_H

#include <stdbool.h>
#include "raylib.h"
#include "ManageUser.h"

// ---- Form tìm kiếm thẻ bạn đọc ----
typedef struct {
    InputBox_BD nhap;

    float scroll;           // Scroll offset của danh sách (pixel)

    // Scrollbar kéo tay
    bool  scrollbar_dragging;
    float scrollbar_drag_startY;

    // Modal chi tiết thẻ
    bool  showmodal;
    char  dachonmathe[16]; // Mã thẻ đang được chọn xem chi tiết
    float modalscroll;
    bool  modal_sb_dragging;      // <-- THÊM DÒNG NÀY
    float modal_sb_dragStartY;
} FormTimKiemThe;

// ---- Khai báo hàm ----
int  LayCodepointKhongDauVaThuong(int codepoint);
void ChuyenChuoiKhongDauVaThuong(const char *nguon, char *dich);

void DocDuLieuTheBanDoc(const char *filepath, BanDoc **head, int *currenttotalusers);
bool KiemTraKhopTimKiem(BanDoc *the, const char *tukhoa);

void InitListBD(FormTimKiemThe *Form);
void Updatetoado(FormTimKiemThe *Form);
void kiemtralienquantiengviet(FormTimKiemThe *Form, int *currentState);

void DrawKetQuaTimKiem(BanDoc *head, FormTimKiemThe *Form, Font font);
void DrawTimKiemThe(FormTimKiemThe *Form, Texture2D icons[], Font font, BanDoc *head);

#endif // TIMKIEMTHETHUVIEN_H