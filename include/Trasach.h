#ifndef TRASACH_H
#define TRASACH_H

#include "raylib.h"
#include "ManageUser.h"
#include "Phieumuon.h"
#include "Timkiemthethuvien.h"

// Chia giao diện làm 3 giai đoạn
typedef enum {
    TRA_B1_TIMKEM = 0,    // Bước 1: Tìm mã thẻ bạn đọc
    TRA_B2_CHONSACH,      // Bước 2: Hiện danh sách phiếu CHƯA TRẢ để click chọn
    TRA_B3_XULY           // Bước 3: Giao diện chia đôi màn hình (Tính tiền)
} TrangThaiTraSach;

typedef struct {
    TrangThaiTraSach state;
    
    // Dùng cho Bước 1 & 2
    InputBox_BD nhapTimKiem;
    char maTheDaChon[15];
    float scrollList;
    
    // Dùng cho Bước 3
    PhieuMuonNode *phieuDangXuLy; // Con trỏ lưu cái phiếu vừa click vào
    InputBox_BD nhapNgayTraThucTe; // Ô nhập ngày trả
    
    int tienPhat; // Tiền phạt tính toán được
    int tongTien;
} FormTraSach;

void InitFormTraSach(FormTraSach *form);
void UpdateLogicTraSach(FormTraSach *form, BanDoc *headBD, PhieuMuonNode *headPM,int *currentState);
void DrawGiaoDienTraSach(FormTraSach *form, BanDoc *headBD, PhieuMuonNode *headPM, Font font);

#endif