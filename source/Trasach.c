#include "Trasach.h"
#include "TimkiemLSphieumuon.h" 
#include "menu.h"
#include "Doanhthu.h"
#include "libmanage.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Khai báo các hàm tính toán từ Doanhthu.c 
int laygiathue(const char *masachcantim);
int KhoangCachNgay(const char *ngayBatDau, const char *ngayKetThuc);
double TinhTienThue(int giaMotNgay, const char *ngayMuon, const char *soNgayMuonStr, const char *ngayTraThucTe);

// Định nghĩa màu sắc giao diện đồng bộ với Tìm kiếm thẻ
static Color COL_HEADER_BG   = { 43, 58, 103, 255 };  // Mã màu 0x2b3a67ff chủ đạo
static Color COL_ROW_EVEN    = { 27, 38, 79, 130 };
static Color COL_ROW_ODD     = { 18, 26, 60, 150 };
static Color COL_ROW_HOVER   = { 55, 90, 175, 200 };
static Color COL_BORDER      = { 90, 130, 210, 55 };

// Tỷ lệ cột bảng Bạn đọc (Bước 1)
static float colRatioBD[] = { 0.12f, 0.28f, 0.18f, 0.20f, 0.12f, 0.10f };
static const char *colHeadersBD[] = { "Mã thẻ", "Họ và tên", "Số ĐT", "CCCD", "Hạn dùng", "Chọn" };

// ==========================================
// CÁC HÀM XỬ LÝ FILE
// ==========================================
void CapNhatToanBoFilePhieuMuon(PhieuMuonNode *head) {
    FILE *f = fopen("data/Phieumuon/LSPhieumuon.txt", "w"); 
    if (f == NULL) return;

    const char *h1="Phiếu mượn", *h2="Mã thẻ", *h3="Mã truyện",
               *h4="Tên truyện", *h5="Ngày mượn", *h6="Ngày trả DK", *h7="TT";
    fprintf(f, "%-*s | %-*s | %-*s | %-*s | %-*s | %-*s | %s\n",
        UTF8Width(h1,12), h1,
        UTF8Width(h2,12), h2,
        UTF8Width(h3,19), h3,
        UTF8Width(h4,50), h4,
        UTF8Width(h5,19), h5,
        UTF8Width(h6,19), h6,
        h7);

    PhieuMuonNode *curr = head;
    while (curr != NULL) {
        fprintf(f, "%-*s | %-*s | %-*s | %-*s | %-*s | %-*s | %d\n",
            UTF8Width(curr->maPM,      12), curr->maPM,
            UTF8Width(curr->mathe,     12), curr->mathe,
            UTF8Width(curr->matruyen,  19), curr->matruyen,
            UTF8Width(curr->tentruyen, 50), curr->tentruyen,
            UTF8Width(curr->ngaymuon,  19), curr->ngaymuon,
            UTF8Width(curr->ngaytra,   19), curr->ngaytra,
            curr->trangthai);
        curr = curr->next;
    }
    fclose(f);
}

void LuuDoanhThuVaoFile(const char *ngay, int tien) {
    FILE *f = fopen("data/Doanhthu.txt", "a");
    if (f != NULL) {
        fprintf(f, "%s | %d VND\n", ngay, tien);
        fclose(f);
    }
}

// Hàm phụ trợ xóa khoảng trắng thừa cho chuỗi
void TrimStringSpace(char *str) {
    int start = 0, end = strlen(str) - 1;
    while (start <= end && (str[start] == ' ' || str[start] == '\t' || str[start] == '\n' || str[start] == '\r')) start++;
    while (end >= start && (str[end] == ' ' || str[end] == '\n' || str[end] == '\r' || str[end] == '\t')) end--;
    int i;
    for (i = 0; i <= end - start; i++) {
        str[i] = str[start + i];
    }
    str[i] = '\0';
}

// LOGIC CỘNG LẠI SỐ LƯỢNG SÁCH KHI TRẢ THÀNH CÔNG (ĐÃ FIX LỖI)
void CapNhatSoLuongSachTra(const char *maSachDayDu) {
    // 1. Phân tách mã (VD: "T001C002 " -> maTruyen="T001", maTap="C002") an toàn
    char maTruyen[50] = {0};
    char maTap[50] = {0};
    char maTemp[100];
    
    strcpy(maTemp, maSachDayDu);
    TrimStringSpace(maTemp); // Loại bỏ sạch sẽ khoảng trắng thừa

    char *c_pos = strchr(maTemp, 'C');
    if (c_pos != NULL) {
        *c_pos = '\0'; // Cắt đôi chuỗi tại vị trí chữ C
        strcpy(maTruyen, maTemp);
        strcpy(maTap, "C");
        strcat(maTap, c_pos + 1);
    } else {
        strcpy(maTruyen, maTemp);
        strcpy(maTap, "C000"); // Mặc định nếu không có chữ C
    }

    TrimStringSpace(maTruyen);
    TrimStringSpace(maTap);

    // 2. Tìm đúng đường dẫn file để mở
    const char *filepath = "data/ManageBooks/dataListTruyen.txt";
    FILE *fIn = fopen(filepath, "r");
    if (fIn == NULL) {
        filepath = "data/dataListTruyen.txt";
        fIn = fopen(filepath, "r");
        if (fIn == NULL) {
            filepath = "dataListTruyen.txt"; // Fallback
            fIn = fopen(filepath, "r");
            if (fIn == NULL) return; 
        }
    }

    // 3. Tạo file tạm ở CÙNG THƯ MỤC với file gốc (Chống lỗi Rename của Win/Mac)
    char tempPath[256];
    strcpy(tempPath, filepath);
    char *lastSlash = strrchr(tempPath, '/');
    if (lastSlash != NULL) {
        strcpy(lastSlash + 1, "tempList.txt");
    } else {
        strcpy(tempPath, "tempList.txt");
    }

    FILE *fOut = fopen(tempPath, "w");
    if (fOut == NULL) {
        fclose(fIn);
        return;
    }

    char line[1024];
    int lineCount = 0;
    bool isUpdated = false;

    while (fgets(line, sizeof(line), fIn)) {
        lineCount++;
        if (lineCount == 1) { // Dòng Header giữ nguyên
            fprintf(fOut, "%s", line);
            continue;
        }

        char ten[256], tapDoc[50], truyenDoc[50];
        int slHT, slNhap;
        
        // Cắt chuỗi theo định dạng Tên | CXXX | TXXX | Số hiện tại | Số nhập vào
        if (sscanf(line, "%[^|]|%[^|]|%[^|]|%d|%d", ten, tapDoc, truyenDoc, &slHT, &slNhap) == 5) {
            TrimStringSpace(ten);
            TrimStringSpace(tapDoc);
            TrimStringSpace(truyenDoc);

            // Kiểm tra trùng khớp
            if (strcmp(truyenDoc, maTruyen) == 0 && strcmp(tapDoc, maTap) == 0) {
                slHT++; // CỘNG LẠI SỐ LƯỢNG KHI TRẢ SÁCH
                if (slHT > slNhap) slHT = slNhap; // Tránh lỗi cộng lố số lượng nhập
                isUpdated = true;
            }
            
            // Ghi lại vào file với định dạng chuẩn
            fprintf(fOut, "%s | %s | %s | %d | %d\n", ten, tapDoc, truyenDoc, slHT, slNhap);
        } else {
            fprintf(fOut, "%s", line); // Nếu dòng bị hỏng thì ghi lại y hệt
        }
    }

    fclose(fIn);
    fclose(fOut);

    // 4. Đè file tạm lên file gốc (Xóa file cũ trước)
    if (isUpdated) {
        remove(filepath);
        rename(tempPath, filepath);
    } else {
        remove(tempPath); // Hủy file tạm nếu không tìm thấy truyện để cộng
    }
}

// ==========================================
// KHỞI TẠO FORM
// ==========================================
void InitFormTraSach(FormTraSach *form) {
    form->state = TRA_B1_TIMKEM;
    memset(form->maTheDaChon, 0, 15);
    
    memset(form->nhapTimKiem.text, 0, 256);
    form->nhapTimKiem.letterCount = 0;
    form->nhapTimKiem.isFocused = true; 
    form->scrollList = 0;
    
    memset(form->nhapNgayTraThucTe.text, 0, 256);
    form->nhapNgayTraThucTe.letterCount = 0;
    form->nhapNgayTraThucTe.isFocused = false;
    
    form->phieuDangXuLy = NULL;
    form->tienPhat = 0;
    form->tongTien = 0;
    form->daThanhToan = 0;
}

// ==========================================
// LOGIC NHẬP LIỆU CHUNG
// ==========================================

// Hàm nhập tìm kiếm bạn đọc (giữ nguyên, nhận mọi ký tự)
void NhapLieuTraSach(InputBox_BD *box) {
    int key = GetKeyPressed();
    while (key > 0) {
        if (key == KEY_BACKSPACE && box->letterCount > 0) {
            do { box->letterCount--; } 
            while (box->letterCount > 0 && (box->text[box->letterCount] & 0xC0) == 0x80);
            box->text[box->letterCount] = '\0';
        }
        key = GetKeyPressed();
    }
    
    if (IsKeyDown(KEY_BACKSPACE)) {
        box->backspaceCounter += GetFrameTime();
        if (box->backspaceCounter >= 0.1f) {
            if (box->letterCount > 0) {
                do { box->letterCount--; } 
                while (box->letterCount > 0 && (box->text[box->letterCount] & 0xC0) == 0x80);
                box->text[box->letterCount] = '\0';
            }
        }
    } else box->backspaceCounter = 0;

    int charcode = GetCharPressed();
    while (charcode > 0) {
        if ((charcode >= 32) && (box->letterCount < 250)) {
            int bytesize = 0;
            const char* utf8Char = CodepointToUTF8(charcode, &bytesize);
            if (box->letterCount + bytesize < 255) {
                for (int j = 0; j < bytesize; j++) {
                    box->text[box->letterCount++] = utf8Char[j];
                }
                box->text[box->letterCount] = '\0';
            }
        }
        charcode = GetCharPressed();
    }
}

// Hàm nhập ngày chuyên biệt: chỉ nhận số, tự chèn '/', giới hạn dd/mm/yyyy (10 ký tự)
// Trả về true nếu nội dung thay đổi (để caller biết cần recheck lỗi)
static bool NhapNgayTraThucTe(InputBox_BD *box) {
    bool changed = false;

    // --- Xóa ký tự ---
    int key = GetKeyPressed();
    while (key > 0) {
        if (key == KEY_BACKSPACE && box->letterCount > 0) {
            // Nếu ký tự trước con trỏ là '/', xóa thêm 1 nữa để xóa cả dấu '/' tự chèn
            if (box->text[box->letterCount - 1] == '/') {
                box->letterCount--;
                box->text[box->letterCount] = '\0';
            }
            if (box->letterCount > 0) {
                box->letterCount--;
                box->text[box->letterCount] = '\0';
            }
            changed = true;
        }
        key = GetKeyPressed();
    }

    if (IsKeyDown(KEY_BACKSPACE)) {
        box->backspaceCounter += GetFrameTime();
        if (box->backspaceCounter >= 0.4f && box->letterCount > 0) {
            if (box->text[box->letterCount - 1] == '/') {
                box->letterCount--;
                box->text[box->letterCount] = '\0';
            }
            if (box->letterCount > 0) {
                box->letterCount--;
                box->text[box->letterCount] = '\0';
            }
            changed = true;
        }
    } else {
        box->backspaceCounter = 0;
    }

    // --- Nhập ký tự số ---
    int charcode = GetCharPressed();
    while (charcode > 0) {
        if (charcode >= '0' && charcode <= '9' && box->letterCount < 10) {
            // Tự chèn '/' sau vị trí 1 (dd/) và vị trí 4 (mm/)
            int pos = box->letterCount;
            if (pos == 2 || pos == 5) {
                box->text[box->letterCount++] = '/';
                box->text[box->letterCount] = '\0';
            }
            // Kiểm tra lại vì có thể vừa chèn '/' xong đủ 10 rồi
            if (box->letterCount < 10) {
                box->text[box->letterCount++] = (char)charcode;
                box->text[box->letterCount] = '\0';
                changed = true;
            }
        }
        charcode = GetCharPressed();
    }

    return changed;
}

// Kiểm tra ngày dd/mm/yyyy có hợp lệ không
static bool KiemTraNgayHopLe_Tra(const char *ngay) {
    if (!ngay || strlen(ngay) < 8) return false;
    int d = 0, m = 0, y = 0;
    if (sscanf(ngay, "%d/%d/%d", &d, &m, &y) != 3) return false;
    if (y < 2000 || y > 2100) return false;
    if (m < 1 || m > 12) return false;
    int maxDay[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) maxDay[2] = 29;
    if (d < 1 || d > maxDay[m]) return false;
    return true;
}

// So sánh 2 ngày dd/mm/yyyy: trả về <0 nếu a trước b, 0 nếu bằng, >0 nếu a sau b
static int SoSanhNgay(const char *a, const char *b) {
    int da,ma,ya, db,mb,yb;
    if (sscanf(a, "%d/%d/%d", &da, &ma, &ya) != 3) return 0;
    if (sscanf(b, "%d/%d/%d", &db, &mb, &yb) != 3) return 0;
    if (ya != yb) return ya - yb;
    if (ma != mb) return ma - mb;
    return da - db;
}

// ==========================================
// LOGIC CẬP NHẬT TRẠNG THÁI
// ==========================================
void UpdateLogicTraSach(FormTraSach *form, BanDoc *headBD, PhieuMuonNode *headPM, int *currentState) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;
    
    // Xử lý nút quay lại
    Rectangle btnBack = { 20, 25, 120, 38 };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    
    if (IsKeyPressed(KEY_ESCAPE) || (isHoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
        if (form->state == TRA_B1_TIMKEM) {
            InitFormTraSach(form); 
            *currentState = APP_MENU; 
        } else if (form->state == TRA_B2_CHONSACH) {
            form->state = TRA_B1_TIMKEM; 
            form->scrollList = 0;
        } else if (form->state == TRA_B3_XULY) {
            form->state = TRA_B2_CHONSACH; 
            form->scrollList = 0;
            form->phieuDangXuLy = NULL;
        }
        return; 
    }

    if (form->state == TRA_B1_TIMKEM) {
        float inputW = screenW * 0.60f;
        if (inputW < 500.0f) inputW = 500.0f;
        if (inputW > 1000.0f) inputW = 1000.0f;
        form->nhapTimKiem.rec = (Rectangle){ (screenW - inputW) / 2.0f, 135.0f, inputW, 50.0f };
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            form->nhapTimKiem.isFocused = CheckCollisionPointRec(GetMousePosition(), form->nhapTimKiem.rec);
        }
        
        if (form->nhapTimKiem.isFocused) {
            NhapLieuTraSach(&form->nhapTimKiem);
        }
        
        int count = 0;
        BanDoc *tmp = headBD;
        while (tmp != NULL) {
            if (KiemTraKhopTimKiem(tmp, form->nhapTimKiem.text)) count++;
            tmp = tmp->next;
        }
        
        // --- TÍNH TOÁN KÍCH THƯỚC BẢNG ---
        float tableTop = form->nhapTimKiem.rec.y + form->nhapTimKiem.rec.height + 25.0f;
        float headerH = 50.0f * scale; 
        if (headerH < 40.0f) headerH = 40.0f;

        float availableH = screenH - tableTop - headerH - 20.0f;
        float rowH = availableH / 12.0f; 
        if (rowH < 45.0f) rowH = 45.0f; 
        
        float viewportH = rowH * 12.0f; 
        
        float totalHeight = count * rowH;
        float maxScroll = totalHeight - viewportH;
        if (maxScroll < 0) maxScroll = 0;
        
        form->scrollList -= GetMouseWheelMove() * 45.0f;
        if (form->scrollList > maxScroll) form->scrollList = maxScroll;
        if (form->scrollList < 0) form->scrollList = 0;
        
        float tableX = screenW * 0.025f;
        float tableW = screenW * 0.945f;
        float curY = tableTop + headerH - form->scrollList;
        
        BanDoc *curr = headBD;
        while (curr != NULL) {
            if (KiemTraKhopTimKiem(curr, form->nhapTimKiem.text)) {
                float btnW = (tableW * colRatioBD[5]) * 0.85f;
                float btnH = rowH * 0.65f;
                float btnX = (tableX + tableW) - (tableW * colRatioBD[5]) + ((tableW * colRatioBD[5] - btnW)/2);
                float btnY = curY + (rowH - btnH)/2;
                Rectangle btnHitbox = { btnX, btnY, btnW, btnH };

                if (curY >= (tableTop + headerH) && curY + rowH <= (tableTop + headerH + viewportH)) {
                    if (CheckCollisionPointRec(GetMousePosition(), btnHitbox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        strcpy(form->maTheDaChon, curr->maThe);
                        form->state = TRA_B2_CHONSACH;
                        form->scrollList = 0;
                        break;
                    }
                }
                curY += rowH;
            }
            curr = curr->next;
        }
    } 
    else if (form->state == TRA_B2_CHONSACH) {
        int count = 0;
        PhieuMuonNode *tmp = headPM;
        while (tmp != NULL) {
            if (strcmp(tmp->mathe, form->maTheDaChon) == 0 && tmp->trangthai == 0) count++;
            tmp = tmp->next;
        }
        
        float listTop = 140.0f;
        float viewportH = screenH - listTop - 40.0f;
        
        // Tăng chiều cao của card mượn sách từ 85 lên 100 để giao diện thoáng đãng, không lo đè chữ
        float itemHeight = 100.0f * scale; 
        if (itemHeight < 90.0f) itemHeight = 90.0f; 
        float spacing = 14.0f;    
        float totalHeight = count * (itemHeight + spacing) + 20.0f;
        float maxScroll = totalHeight - viewportH;
        if (maxScroll < 0) maxScroll = 0;
        
        float wheel = GetMouseWheelMove();
        if (wheel != 0) form->scrollList -= wheel * 45.0f;
        if (form->scrollList > maxScroll) form->scrollList = maxScroll;
        if (form->scrollList < 0) form->scrollList = 0;
        
        float itemW = screenW * 0.82f; 
        if (itemW < 700.0f) itemW = 700.0f;
        if (itemW > 1250.0f) itemW = 1250.0f;
        float itemX = (screenW - itemW) / 2.0f;
        float curY = listTop - form->scrollList + 10.0f;
        
        PhieuMuonNode *curr = headPM;
        while (curr != NULL) {
            if (strcmp(curr->mathe, form->maTheDaChon) == 0 && curr->trangthai == 0) {
                // Định hình nút bấm "Trả sách" an toàn phía bên phải
                float btnW = 125.0f * scale; 
                if (btnW < 105.0f) btnW = 105.0f;
                float btnH = itemHeight * 0.52f;
                float btnX = itemX + itemW - btnW - 30.0f;
                float btnY = curY + (itemHeight - btnH) / 2.0f;
                Rectangle btnHitbox = { btnX, btnY, btnW, btnH };
                
                if (curY >= listTop && curY + itemHeight <= (listTop + viewportH)) {
                    if (CheckCollisionPointRec(GetMousePosition(), btnHitbox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        form->phieuDangXuLy = curr;
                        form->state = TRA_B3_XULY;
                        form->nhapNgayTraThucTe.isFocused = true;
                        break;
                    }
                }
                curY += itemHeight + spacing;
            }
            curr = curr->next;
        }
    }
    else if (form->state == TRA_B3_XULY) {
        float leftPaneW  = screenW * 0.65f;
        float rightPaneX = leftPaneW;
        float rightPaneW = screenW * 0.35f;
        form->nhapNgayTraThucTe.rec = (Rectangle){ rightPaneX + 40, 120, rightPaneW - 80, 48 };
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            form->nhapNgayTraThucTe.isFocused = CheckCollisionPointRec(GetMousePosition(), form->nhapNgayTraThucTe.rec);
        }
        
        if (form->nhapNgayTraThucTe.isFocused) {
            NhapNgayTraThucTe(&form->nhapNgayTraThucTe);
        }

        int giaMotNgay = laygiathue(form->phieuDangXuLy->matruyen);
        int soNgayDuKien = atoi(form->phieuDangXuLy->ngaytra);
        if (soNgayDuKien <= 0) soNgayDuKien = 1;
        int tienGoc = giaMotNgay * soNgayDuKien;

        // TÍNH TOÁN VÀ KIỂM TRA LỖI NGÀY
        // Phải nhập đủ 10 ký tự (dd/mm/yyyy) và đúng format mới tính
        if (form->nhapNgayTraThucTe.letterCount == 10 &&
            KiemTraNgayHopLe_Tra(form->nhapNgayTraThucTe.text)) {
            // Chặn ngay tại đây: ngày trả phải >= ngày mượn
            if (SoSanhNgay(form->nhapNgayTraThucTe.text, form->phieuDangXuLy->ngaymuon) < 0) {
                form->tongTien = -1;
                form->tienPhat = 0;
            } else {
                double ketQuaTinh = TinhTienThue(
                    giaMotNgay,
                    form->phieuDangXuLy->ngaymuon,
                    form->phieuDangXuLy->ngaytra,
                    form->nhapNgayTraThucTe.text
                );
                form->tongTien = (int)ketQuaTinh;
                if (form->tongTien != -1) {
                    form->tienPhat = form->tongTien - tienGoc;
                } else {
                    form->tienPhat = 0;
                }
            }
        } else if (form->nhapNgayTraThucTe.letterCount > 0 &&
                   form->nhapNgayTraThucTe.letterCount < 10) {
            // Đang nhập dở — chưa đủ, không báo lỗi, hiện tiền gốc tạm
            form->tongTien = tienGoc;
            form->tienPhat = 0;
        } else if (form->nhapNgayTraThucTe.letterCount == 0) {
            form->tongTien = tienGoc;
            form->tienPhat = 0;
        } else {
            // Đủ 10 ký tự nhưng sai format (ngày/tháng không hợp lệ)
            form->tongTien = -1;
            form->tienPhat = 0;
        }

        Rectangle btnThanhToan = { rightPaneX + 30, screenH - 70, rightPaneW - 60, 55 }; // Tọa độ đồng bộ UI
        
        // CHỈ XỬ LÝ CLICK NẾU KHÔNG CÓ LỖI (tongTien != -1) VÀ ĐỦ 10 KÝ TỰ dd/mm/yyyy
        if (form->tongTien != -1 && CheckCollisionPointRec(GetMousePosition(), btnThanhToan) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (form->nhapNgayTraThucTe.letterCount == 10) {
                form->phieuDangXuLy->trangthai = 1; 
                CapNhatToanBoFilePhieuMuon(headPM); 
                LuuDoanhThuVaoFile(form->nhapNgayTraThucTe.text, form->tongTien);
                
                // === THỰC HIỆN CỘNG SỐ LƯỢNG SÁCH TRỞ LẠI KHO ===
                CapNhatSoLuongSachTra(form->phieuDangXuLy->matruyen);
                // =================================================

                form->daThanhToan = 1; 
                form->state = TRA_B1_TIMKEM; 
                memset(form->maTheDaChon, 0, 15);
                memset(form->nhapNgayTraThucTe.text, 0, 256);
                form->nhapNgayTraThucTe.letterCount = 0;
                form->scrollList = 0;
                form->phieuDangXuLy = NULL;
                form->tienPhat = 0;
                form->tongTien = 0; 
            }
        }
    }
}

// ==========================================
// HÀM VẼ GIAO DIỆN
// ==========================================
void DrawGiaoDienTraSach(FormTraSach *form, BanDoc *headBD, PhieuMuonNode *headPM, Font font) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    ClearBackground(GetColor(0x080f1eFF));
    DrawBackgroundParticles();
    DrawRectangle(0, 0, (int)screenW, (int)screenH, GetColor(0x1b264f80));

    // ----------------------------------------------------
    // BƯỚC 1: TÌM KIẾM THẺ BẠN ĐỌC
    // ----------------------------------------------------
    if (form->state == TRA_B1_TIMKEM) {
        float tableTop = form->nhapTimKiem.rec.y + form->nhapTimKiem.rec.height + 25.0f;
        float headerH  = 50.0f * scale; 
        if (headerH < 40.0f) headerH = 40.0f;
        
        float tableX   = screenW * 0.025f;
        float tableW   = screenW * 0.945f;
        
        float availableH = screenH - tableTop - headerH - 20.0f;
        float rowH = availableH / 12.0f; 
        if (rowH < 45.0f) rowH = 45.0f; 
        
        float viewportH = rowH * 12.0f; 
        float sbW = 10.0f;
        
        float fzRow = rowH * 0.35f; 
        if (fzRow < 15.0f) fzRow = 15.0f; 

        int matchCount = 0;
        BanDoc *cur = headBD;
        while (cur) {
            if (KiemTraKhopTimKiem(cur, form->nhapTimKiem.text)) matchCount++;
            cur = cur->next;
        }
        float contentH = matchCount * rowH;

        BeginScissorMode((int)tableX, (int)(tableTop + headerH), (int)tableW, (int)viewportH);
        
        int startBgRow = (int)(form->scrollList / rowH);
        int maxVisibleRows = 13; 
        for (int i = 0; i < maxVisibleRows; i++) {
            int rIdx = startBgRow + i;
            float y = tableTop + headerH - form->scrollList + rIdx * rowH;
            if (y > tableTop + headerH + viewportH) break;
            
            Color rowBg = (rIdx % 2 == 0) ? COL_ROW_EVEN : COL_ROW_ODD;
            DrawRectangleRec((Rectangle){tableX, y, tableW, rowH}, rowBg);
        }

        float rowY = tableTop + headerH - form->scrollList;
        cur = headBD;
        while (cur) {
            if (KiemTraKhopTimKiem(cur, form->nhapTimKiem.text)) {
                if (rowY + rowH > (tableTop + headerH) && rowY < (tableTop + headerH + viewportH)) {
                    Rectangle rowRect = { tableX, rowY, tableW, rowH };
                    bool isHover = CheckCollisionPointRec(GetMousePosition(), rowRect);
                    if (GetMousePosition().y < (tableTop + headerH) || GetMousePosition().y > (tableTop + headerH + viewportH)) {
                        isHover = false;
                    }
                    if (isHover) DrawRectangleRec(rowRect, COL_ROW_HOVER);

                    const char *cells[5] = { cur->maThe, cur->hoTen, cur->sdt, cur->cccd, cur->hanSD };
                    for (int i = 0; i < 5; i++) {
                        float cx = tableX;
                        for (int k = 0; k < i; k++) cx += tableW * colRatioBD[k];
                        float cw = tableW * colRatioBD[i];
                        
                        Vector2 ts = MeasureTextEx(font, cells[i], fzRow, 1);
                        DrawTextEx(font, cells[i], (Vector2){ cx + 15.0f, rowY + (rowH - ts.y)/2.0f }, fzRow, 1, WHITE);
                    }

                    float cx = tableX;
                    for (int k = 0; k < 5; k++) cx += tableW * colRatioBD[k];
                    float cw = tableW * colRatioBD[5];
                    float btnW = cw * 0.80f, btnH = rowH * 0.65f;
                    float btnX = cx + (cw - btnW)/2.0f;
                    float btnY = rowY + (rowH - btnH)/2.0f;
                    Rectangle btnR = { btnX, btnY, btnW, btnH };
                    bool bHov = isHover && CheckCollisionPointRec(GetMousePosition(), btnR);

                    DrawRectangleRounded(btnR, 0.4f, 8, bHov ? GetColor(0x324376FF) : GetColor(0x4A80FFFF));
                    Vector2 bts = MeasureTextEx(font, "Chọn", fzRow, 1);
                    DrawTextEx(font, "Chọn", (Vector2){ btnX + (btnW - bts.x)/2.0f, btnY + (btnH - bts.y)/2.0f }, fzRow, 1, WHITE);
                }
                rowY += rowH;
            }
            cur = cur->next;
        }

        for (int i = 0; i < maxVisibleRows; i++) {
            int rIdx = startBgRow + i;
            float y = tableTop + headerH - form->scrollList + rIdx * rowH;
            if (y > tableTop + headerH + viewportH) break;
            DrawRectangle((int)tableX, (int)(y + rowH - 1), (int)tableW, 1, COL_BORDER);
        }
        for (int c = 1; c < 6; c++) {
            float cx = tableX;
            for (int k = 0; k < c; k++) cx += tableW * colRatioBD[k];
            DrawRectangle((int)cx, (int)(tableTop + headerH), 1, (int)viewportH, COL_BORDER);
        }

        EndScissorMode();

        float trackX = tableX + tableW + 8.0f;
        Rectangle trackR = { trackX, tableTop + headerH, sbW, viewportH };
        DrawRectangleRounded(trackR, 0.5f, 6, GetColor(0x1b264f80)); 

        if (contentH > viewportH) {
            float thumbH = viewportH * (viewportH / contentH);
            if (thumbH < 30.0f) thumbH = 30.0f;
            float maxScroll = contentH - viewportH;
            float thumbY = tableTop + headerH + (form->scrollList / maxScroll) * (viewportH - thumbH);
            
            Rectangle thumbR = { trackX, thumbY, sbW, thumbH };
            bool tHov = CheckCollisionPointRec(GetMousePosition(), thumbR);
            DrawRectangleRounded(thumbR, 0.5f, 6, tHov ? GetColor(0x4A80FFFF) : GetColor(0x4A80FF88));

            static bool draggingBD = false;
            static float dragStartY = 0.0f;
            if (draggingBD) {
                if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) draggingBD = false;
                else {
                    float dy = GetMousePosition().y - dragStartY;
                    float ratio = (dy - (tableTop + headerH)) / (viewportH - thumbH);
                    if (ratio < 0) ratio = 0;
                    if (ratio > 1) ratio = 1;
                    form->scrollList = ratio * maxScroll;
                }
            } else {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), thumbR)) {
                    draggingBD = true;
                    dragStartY = GetMousePosition().y - thumbY;
                }
            }
        } else {
            Rectangle thumbR = { trackX, tableTop + headerH, sbW, viewportH };
            DrawRectangleRounded(thumbR, 0.5f, 6, GetColor(0x4A80FF33));
        }

        DrawRectangle((int)tableX, (int)tableTop, (int)tableW, (int)headerH, COL_HEADER_BG);
        DrawRectangle((int)tableX, (int)(tableTop + headerH - 2), (int)tableW, 2, GetColor(0x4A80FFFF));
        for (int c = 0; c < 6; c++) {
            float cx = tableX;
            for (int k = 0; k < c; k++) cx += tableW * colRatioBD[k];
            float cw = tableW * colRatioBD[c];
            Vector2 ts = MeasureTextEx(font, colHeadersBD[c], fzRow + 2.0f, 1);
            if (c > 0) DrawRectangle((int)cx, (int)tableTop, 1, (int)headerH, COL_BORDER);
            DrawTextEx(font, colHeadersBD[c], (Vector2){ cx + (cw - ts.x)/2.0f, tableTop + (headerH - ts.y)/2.0f }, fzRow + 2.0f, 1, WHITE);
        }

        DrawRectangleGradientEx((Rectangle){ 0, 0, screenW, 115 }, GetColor(0x2b3a67ff), GetColor(0x2b3a67ff), GetColor(0x2b3a67ff), GetColor(0x2b3a67ff));
        DrawRectangle(0, 113, (int)screenW, 2, GetColor(0x4A80FF70));

        Vector2 mT = MeasureTextEx(font, "CHỌN THẺ BẠN ĐỌC ĐỂ TRẢ SÁCH", 32.0f, 1);
        DrawTextEx(font, "CHỌN THẺ BẠN ĐỌC ĐỂ TRẢ SÁCH", (Vector2){ (screenW - mT.x)/2.0f, (115 - mT.y)/2.0f }, 32.0f, 1, WHITE);

        DrawRectangleRounded(form->nhapTimKiem.rec, 0.25f, 10, GetColor(0xFFFFFF15));
        DrawRectangleRoundedLines(form->nhapTimKiem.rec, 0.25f, 10, form->nhapTimKiem.isFocused ? GetColor(0x4A80FFFF) : GetColor(0x4A80FF77));

        BeginScissorMode((int)form->nhapTimKiem.rec.x + 5, (int)form->nhapTimKiem.rec.y, (int)form->nhapTimKiem.rec.width - 10, (int)form->nhapTimKiem.rec.height);
            Vector2 textPos = { form->nhapTimKiem.rec.x + 15, form->nhapTimKiem.rec.y + 13 };
            if (form->nhapTimKiem.letterCount == 0) {
                DrawTextEx(font, "Tìm thẻ bằng Tên, SĐT hoặc CCCD...", textPos, 22.0f, 1, GetColor(0xFFFFFF44));
            } else {
                DrawTextEx(font, form->nhapTimKiem.text, textPos, 22.0f, 1, WHITE);
            }
            if (form->nhapTimKiem.isFocused && (((int)(GetTime() * 1.5f)) % 2 == 0)) {
                float textWidth = MeasureTextEx(font, form->nhapTimKiem.text, 22.0f, 1).x;
                DrawRectangleV((Vector2){ textPos.x + textWidth + 2, textPos.y }, (Vector2){ 2, 22.0f }, WHITE);
            }
        EndScissorMode();
    } 
    // ----------------------------------------------------
    // BƯỚC 2: CHỌN SÁCH CẦN TRẢ (CẢI TIẾN TRÁNH ĐÈ CHỮ)
    // ----------------------------------------------------
    else if (form->state == TRA_B2_CHONSACH) {
        float listTop = 140.0f;
        float viewportH = screenH - listTop - 40.0f;
        float sbW = 10.0f; 

        int count = 0;
        PhieuMuonNode *cur = headPM;
        while (cur) {
            if (strcmp(cur->mathe, form->maTheDaChon) == 0 && cur->trangthai == 0) count++;
            cur = cur->next;
        }

        // Tăng chiều cao lên 100.0f để chứa văn bản thoải mái hơn
        float itemHeight = 100.0f * scale; 
        if (itemHeight < 90.0f) itemHeight = 90.0f; 
        float spacing = 14.0f;    
        float totalHeight = count * (itemHeight + spacing) + 20.0f;

        float itemW = screenW * 0.82f; 
        if (itemW < 700.0f) itemW = 700.0f;
        if (itemW > 1250.0f) itemW = 1250.0f;
        float itemX = (screenW - itemW) / 2.0f;

        BeginScissorMode((int)itemX - 10, (int)listTop, (int)(itemW + 35), (int)viewportH);
        {
            float curY = listTop - form->scrollList + 10.0f;
            cur = headPM;
            while (cur) {
                if (strcmp(cur->mathe, form->maTheDaChon) == 0 && cur->trangthai == 0) {
                    Rectangle cardRec = { itemX, curY, itemW, itemHeight };
                    bool isHover = CheckCollisionPointRec(GetMousePosition(), cardRec);

                    if (GetMousePosition().y < listTop || GetMousePosition().y > (listTop + viewportH)) {
                        isHover = false;
                    }

                    Color cardBg = isHover ? GetColor(0x1e2d5cAA) : GetColor(0x131d3dAA);
                    Color borderCol = isHover ? GetColor(0x4A80FFFF) : GetColor(0x1b264fFF);

                    DrawRectangleRounded(cardRec, 0.12f, 10, cardBg);
                    DrawRectangleRoundedLines(cardRec, 0.12f, 10, borderCol);

                    char line1[512];
                    char line2[512];
                    sprintf(line1, "Truyện: %s (Mã: %s)", cur->tentruyen, cur->matruyen);
                    sprintf(line2, "Ngày mượn: %s   -   Số ngày dự kiến: %s", cur->ngaymuon, cur->ngaytra);

                    // Tinh chỉnh thu nhỏ font chữ mặc định cho cân đối
                    float fzTitle = 20.0f * scale; if(fzTitle < 18.0f) fzTitle = 18.0f;
                    float fzSub   = 15.0f * scale; if(fzSub < 13.0f) fzSub = 13.0f;

                    // Tính toán kích thước nút "Trả sách" vừa vặn
                    float btnW = 120.0f * scale; 
                    if(btnW < 100.0f) btnW = 100.0f;

                    // --- GIẢI PHÁP MỚI: TỰ ĐỘNG THU NHỎ FONT NẾU CHỮ QUÁ DÀI ---
                    float maxTextWidth = itemW - btnW - 60.0f; // Khoảng cách tối đa cho phép hiển thị chữ
                    // Vòng lặp hạ dần cỡ chữ xuống đến khi nhét vừa khoảng trống (không đè vào nút)
                    while (MeasureTextEx(font, line1, fzTitle, 1).x > maxTextWidth && fzTitle > 11.0f) {
                        fzTitle -= 0.5f;
                    }
                    
                    // Căn toạ độ Y vào giữa linh hoạt theo cỡ chữ hiện tại để trông luôn đẹp mắt
                    float titleY = curY + itemHeight * 0.35f - fzTitle / 2.0f;
                    float subY = curY + itemHeight * 0.70f - fzSub / 2.0f;

                    DrawTextEx(font, line1, (Vector2){ itemX + 25.0f, titleY }, fzTitle, 1, WHITE);
                    DrawTextEx(font, line2, (Vector2){ itemX + 25.0f, subY }, fzSub, 1, GetColor(0x90B8FFFF));

                    float btnH = itemHeight * 0.46f; // Chiều cao nút vừa vặn tỉ lệ thẻ
                    float btnX = itemX + itemW - btnW - 25.0f;
                    float btnY = curY + (itemHeight - btnH) / 2.0f;
                    Rectangle btnR = { btnX, btnY, btnW, btnH };
                    bool btnHov = isHover && CheckCollisionPointRec(GetMousePosition(), btnR);

                    Color btnColor = btnHov ? GetColor(0x2ecc71FF) : GetColor(0x27ae60FF); 
                    DrawRectangleRounded(btnR, 0.25f, 8, btnColor);
                    
                    Vector2 bts = MeasureTextEx(font, "Trả sách", fzSub + 1.0f, 1);
                    DrawTextEx(font, "Trả sách", (Vector2){ btnX + (btnW - bts.x)/2.0f, btnY + (btnH - bts.y)/2.0f }, fzSub + 1.0f, 1, WHITE);

                    curY += itemHeight + spacing;
                }
                cur = cur->next;
            }
        }
        EndScissorMode();

        float trackX = itemX + itemW + 16.0f;
        Rectangle trackR = { trackX, listTop, sbW, viewportH };
        DrawRectangleRounded(trackR, 0.5f, 6, GetColor(0x1b264f80)); 

        if (totalHeight > viewportH) {
            float thumbH = viewportH * (viewportH / totalHeight);
            if (thumbH < 40.0f) thumbH = 40.0f;
            float maxScroll = totalHeight - viewportH;
            float thumbY = listTop + (form->scrollList / maxScroll) * (viewportH - thumbH);

            Rectangle thumbR = { trackX, thumbY, sbW, thumbH };
            bool tHov = CheckCollisionPointRec(GetMousePosition(), thumbR);
            DrawRectangleRounded(thumbR, 0.5f, 6, tHov ? GetColor(0x4A80FFFF) : GetColor(0x4A80FF88));

            static bool draggingPM = false;
            static float dragStartY = 0.0f;
            if (draggingPM) {
                if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) draggingPM = false;
                else {
                    float dy = GetMousePosition().y - dragStartY;
                    float ratio = (dy - listTop) / (viewportH - thumbH);
                    if (ratio < 0) ratio = 0;
                    if (ratio > 1) ratio = 1;
                    form->scrollList = ratio * maxScroll;
                }
            } else {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), thumbR)) {
                    draggingPM = true;
                    dragStartY = GetMousePosition().y - thumbY;
                }
            }
        } else {
            Rectangle thumbR = { trackX, listTop, sbW, viewportH };
            DrawRectangleRounded(thumbR, 0.5f, 6, GetColor(0x4A80FF33));
        }

        DrawRectangleGradientEx((Rectangle){ 0, 0, screenW, 115 }, GetColor(0x2b3a67ff), GetColor(0x2b3a67ff), GetColor(0x2b3a67ff), GetColor(0x2b3a67ff));
        DrawRectangle(0, 113, (int)screenW, 2, GetColor(0x4A80FF70));

        char bannerText[128];
        sprintf(bannerText, "CÁC PHIẾU CHƯA TRẢ CỦA BẠN ĐỌC: %s", form->maTheDaChon);
        Vector2 mTitle = MeasureTextEx(font, bannerText, 28.0f, 1);
        DrawTextEx(font, bannerText, (Vector2){ (screenW - mTitle.x)/2.0f, (115 - mTitle.y)/2.0f }, 28.0f, 1, WHITE);

        if (count == 0) {
            DrawTextEx(font, "Bạn đọc này đã thanh toán toàn bộ sách mượn!", (Vector2){ screenW/2.0f - 260, screenH/2.0f }, 22, 1, GetColor(0xFF6B6BFF));
        }
    }
    // ----------------------------------------------------
    // BƯỚC 3: MÀN HÌNH TÍNH TIỀN CHIA ĐÔI
    // ----------------------------------------------------
    else if (form->state == TRA_B3_XULY) {
        float leftPaneW  = screenW * 0.65f;
        float rightPaneX = leftPaneW;
        float rightPaneW = screenW * 0.35f;

        DrawRectangle(0, 0, (int)leftPaneW, (int)screenH, GetColor(0xEEF7FBFF));
        DrawRectangle((int)rightPaneX, 0, (int)rightPaneW, (int)screenH, GetColor(0xE8F4F8FF));
        DrawLine(rightPaneX, 0, rightPaneX, screenH, GetColor(0x8AB6D6FF));

        DrawRectangleGradientEx((Rectangle){ 0, 0, leftPaneW, 115 },
            GetColor(0x8AB6D6FF), GetColor(0x8AB6D6FF),
            GetColor(0xA8CBE0FF), GetColor(0xA8CBE0FF));
        DrawRectangle(0, 113, (int)leftPaneW, 2, GetColor(0x507A9BFF));
        Vector2 titleL = MeasureTextEx(font, "THÔNG TIN PHIẾU MƯỢN", 28.0f, 1);
        DrawTextEx(font, "THÔNG TIN PHIẾU MƯỢN",
            (Vector2){ (leftPaneW - titleL.x)/2.0f, (115 - titleL.y)/2.0f }, 28.0f, 1, GetColor(0x2C5F8AFF));

        float scaleB3 = leftPaneW / 900.0f;
        if (scaleB3 > scale) scaleB3 = scale;
        float pmW = 800.0f * scaleB3;
        DrawThePhieuMuon(form->phieuDangXuLy, font, (leftPaneW - pmW)/2.0f, 140.0f, scaleB3);

        Vector2 titleR = MeasureTextEx(font, "ĐƠN TRẢ SÁCH", 26.0f, 1);
        DrawTextEx(font, "ĐƠN TRẢ SÁCH",
            (Vector2){ rightPaneX + (rightPaneW - titleR.x)/2.0f, 42.0f }, 26.0f, 1, GetColor(0x2C5F8AFF));

        DrawTextEx(font, "Nhập ngày trả thực tế (dd/mm/yyyy):",
            (Vector2){ form->nhapNgayTraThucTe.rec.x, 92.0f }, 17, 1, GetColor(0x507A9BFF));
        DrawRectangleRounded(form->nhapNgayTraThucTe.rec, 0.2f, 10, WHITE);
        DrawRectangleRoundedLines(form->nhapNgayTraThucTe.rec, 0.2f, 10,
            form->nhapNgayTraThucTe.isFocused ? GetColor(0x8AB6D6FF) : GetColor(0xB3D1E8FF));

        float fs = 20.0f;
        Vector2 textSz = MeasureTextEx(font, form->nhapNgayTraThucTe.text, fs, 1);
        DrawTextEx(font, form->nhapNgayTraThucTe.text,
            (Vector2){ form->nhapNgayTraThucTe.rec.x + 15, form->nhapNgayTraThucTe.rec.y + 14 },
            fs, 1, GetColor(0x2C3E50FF));
        if (form->nhapNgayTraThucTe.isFocused && (((int)(GetTime() * 1.5f)) % 2 == 0)) {
            DrawRectangleV(
                (Vector2){ form->nhapNgayTraThucTe.rec.x + 15 + textSz.x + 2, form->nhapNgayTraThucTe.rec.y + 10 },
                (Vector2){ 2, fs + 4 }, GetColor(0x2C5F8AFF));
        }

        // ==========================================
        // VẼ KHUNG BÁO LỖI HOẶC VẼ BIÊN LAI (NẾU ĐÚNG)
        // ==========================================
        if (form->tongTien == -1 && form->nhapNgayTraThucTe.letterCount >= 8) {
            // --- TRƯỜNG HỢP NHẬP SAI: BÁO LỖI ---
            Rectangle warningBox = { rightPaneX + 30, 195, rightPaneW - 60, 80 };
            DrawRectangleRounded(warningBox, 0.2f, 10, Fade(RED, 0.1f));
            DrawRectangleRoundedLines(warningBox, 0.2f, 10, Fade(RED, 0.6f));
            
            DrawTextEx(font, "CẢNH BÁO: Ngày trả không hợp lệ!", (Vector2){warningBox.x + 15, warningBox.y + 15}, 18, 1, RED);
            DrawTextEx(font, "Ngày không tồn tại hoặc trước ngày mượn.", (Vector2){warningBox.x + 15, warningBox.y + 45}, 16, 1, MAROON);

            // Nút bấm bị xám đi, không cho bấm
            Rectangle btnThanhToan = { rightPaneX + 30, screenH - 70, rightPaneW - 60, 55 };
            DrawRectangleRounded(btnThanhToan, 0.2f, 10, Fade(LIGHTGRAY, 0.6f));
            Vector2 sizeBtn = MeasureTextEx(font, "ĐỒNG Ý TRẢ SÁCH", 20.0f, 1);
            DrawTextEx(font, "ĐỒNG Ý TRẢ SÁCH",
                (Vector2){ btnThanhToan.x + (btnThanhToan.width - sizeBtn.x)/2.0f,
                           btnThanhToan.y + (btnThanhToan.height - sizeBtn.y)/2.0f },
                20.0f, 1, GRAY);
        } else {
            // --- TRƯỜNG HỢP BÌNH THƯỜNG: VẼ BIÊN LAI ---
            Rectangle billRec = { rightPaneX + 30, 195, rightPaneW - 60, 250 };
            DrawRectangleRounded(billRec, 0.1f, 10, GetColor(0xF0F8FCFF));
            DrawRectangleRoundedLines(billRec, 0.1f, 10, GetColor(0x8AB6D6FF));

            int giaGoc = form->tongTien - form->tienPhat;

            DrawTextEx(font, TextFormat("Tiền thuê sách: %d VNĐ", giaGoc),
                (Vector2){ billRec.x + 20, billRec.y + 20 }, 20, 1, DARKGRAY);

            if (form->tienPhat > 0) {
                DrawTextEx(font, TextFormat("Phí trả chậm: +%d VNĐ", form->tienPhat),
                    (Vector2){ billRec.x + 20, billRec.y + 65 }, 20, 1, GetColor(0xE74C3CFF));
            } else if (form->tienPhat < 0) {
                DrawTextEx(font, TextFormat("Chiết khấu trả sớm: %d VNĐ", -form->tienPhat),
                    (Vector2){ billRec.x + 20, billRec.y + 65 }, 20, 1, GetColor(0x2ECC71FF));
            } else {
                DrawTextEx(font, "Phí chênh lệch: 0 VNĐ",
                    (Vector2){ billRec.x + 20, billRec.y + 65 }, 20, 1, GetColor(0x507A9BFF));
            }

            DrawLine((int)(billRec.x + 15), (int)(billRec.y + 120),
                     (int)(billRec.x + billRec.width - 15), (int)(billRec.y + 120),
                     GetColor(0x8AB6D6FF));

            DrawTextEx(font, "TỔNG CỘNG:",
                (Vector2){ billRec.x + 20, billRec.y + 140 }, 20, 1, GetColor(0x507A9BFF));
            DrawTextEx(font, TextFormat("%d VNĐ", form->tongTien),
                (Vector2){ billRec.x + 20, billRec.y + 175 }, 30, 1, GetColor(0x2C5F8AFF));

            // Nút bấm sáng lên, có hover
            Rectangle btnThanhToan = { rightPaneX + 30, screenH - 70, rightPaneW - 60, 55 };
            bool hoverBtn = CheckCollisionPointRec(GetMousePosition(), btnThanhToan);
            DrawRectangleRounded(btnThanhToan, 0.2f, 10,
                hoverBtn ? GetColor(0x507A9BFF) : GetColor(0x8AB6D6FF));
            Vector2 sizeBtn = MeasureTextEx(font, "ĐỒNG Ý TRẢ SÁCH", 20.0f, 1);
            DrawTextEx(font, "ĐỒNG Ý TRẢ SÁCH",
                (Vector2){ btnThanhToan.x + (btnThanhToan.width - sizeBtn.x)/2.0f,
                           btnThanhToan.y + (btnThanhToan.height - sizeBtn.y)/2.0f },
                20.0f, 1, WHITE);
        }
    }

    // --- VẼ NÚT QUAY LẠI CHUNG ---
    Rectangle btnBack = { 20, 25, 120, 38 };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);

    if (form->state == TRA_B3_XULY) {
        Color btnCol  = isHoverBack ? GetColor(0x507A9BFF) : WHITE;
        Color txtCol  = isHoverBack ? WHITE : GetColor(0x507A9BFF);
        DrawRectangleRounded(btnBack, 0.35f, 10, btnCol);
        DrawRectangleRoundedLines(btnBack, 0.35f, 10, GetColor(0x8AB6D6FF));
        DrawTextEx(font, "< Quay lại", (Vector2){ btnBack.x + 15, btnBack.y + 10 }, 18, 1, txtCol);
    } else {
        Color btnCol = isHoverBack ? RED : GetColor(0xFFFFFF20);
        DrawRectangleRounded(btnBack, 0.35f, 10, btnCol);
        DrawRectangleRoundedLines(btnBack, 0.35f, 10, GetColor(0xFFFFFF44));
        DrawTextEx(font, "< Quay lại", (Vector2){ btnBack.x + 15, btnBack.y + 10 }, 18, 1, WHITE);
    }
}