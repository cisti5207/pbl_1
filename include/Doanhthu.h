#ifndef DOANHTHU_H
#define DOANHTHU_H

#include "raylib.h"
#include "ManageUser.h" // Dùng InputBox_BD cho ô nhập năm
#include <time.h>
#include <math.h>

#define DOANHTHU_HASH_SIZE 365 // Bảng băm cho 365 ngày

// 1. Cấu trúc lưu trữ Doanh Thu (Map theo Ngày)
typedef struct DoanhThuNode {
    char ngay[15];       // Format: "DD/MM/YYYY"
    double tongTien;     // Tổng doanh thu của ngày đó
    struct DoanhThuNode *next;
} DoanhThuNode;

typedef struct {
    DoanhThuNode *mang[DOANHTHU_HASH_SIZE];
} DoanhThuMap;

// 2. Trạng thái xem UI
typedef enum {
    VIEW_NGAY = 0,
    VIEW_TUAN,           // Đã thêm chế độ xem theo TUẦN
    VIEW_THANG,
    VIEW_QUY,
    VIEW_NAM
} ViewDoanhThuMode;

// Form chứa trạng thái UI Doanh Thu
typedef struct {
    ViewDoanhThuMode currentView;
    float scroll;
    InputBox_BD nhapNam; // Ô nhập năm để lọc dữ liệu
} FormDoanhThu;

// 3. Các hàm xử lý Logic Ngày & Toán học
int KhoangCachNgay(const char *ngayBatDau, const char *ngayKetThuc);
void CongThemNgay(const char *ngayGoc, int soNgayThem, char *ngayKetQua);
double TinhTienThue(int giaGoc, const char *ngayDuKien, const char *ngayTraThucTe);

// 4. Các hàm xử lý Map Doanh Thu
void InitDoanhThuMap(DoanhThuMap *map);
void ThemDoanhThu(DoanhThuMap *map, const char *ngay, double soTien);
void GiaiPhongDoanhThuMap(DoanhThuMap *map);

// 5. Hàm giao diện UI
void InitFormDoanhThu(FormDoanhThu *form);
void DrawDashboardDoanhThu(DoanhThuMap *map, FormDoanhThu *form, Font font, int *currentState);

#endif