#include "Trasach.h"
#include "TimkiemLSphieumuon.h" 
#include "menu.h"
#include "libmanage.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Khai báo các hàm tính toán từ Doanhthu.c 
int laygiathue(const char *masachcantim);
int KhoangCachNgay(const char *ngayBatDau, const char *ngayKetThuc);
double TinhTienThue(int giaMotNgay, const char *ngayMuon, const char *ngayDuKien, const char *ngayTraThucTe);

// Định nghĩa màu sắc giao diện đồng bộ với Tìm kiếm thẻ
static Color COL_HEADER_BG   = { 43, 58, 103, 255 };  // Mã màu 0x2b3a67ff chủ đạo
static Color COL_ROW_EVEN    = { 27, 38, 79, 130 };
static Color COL_ROW_ODD     = { 18, 26, 60, 150 };
static Color COL_ROW_HOVER   = { 55, 90, 175, 200 };
static Color COL_BORDER      = { 90, 130, 210, 55 };

#define ROW_HEIGHT 55.0f
#define HEADER_H   45.0f

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

// ==========================================
// LOGIC CẬP NHẬT TRẠNG THÁI (KÈM SCROLL VÀ ĐIỀU HƯỚNG)
// ==========================================
void UpdateLogicTraSach(FormTraSach *form, BanDoc *headBD, PhieuMuonNode *headPM, int *currentState) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;
    
    // Xử lý nút quay lại chung trực tiếp
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
        
        // Tính toán chiều cao thực tế danh sách lọc được để giới hạn cuộn
        int count = 0;
        BanDoc *tmp = headBD;
        while (tmp != NULL) {
            if (KiemTraKhopTimKiem(tmp, form->nhapTimKiem.text)) count++;
            tmp = tmp->next;
        }
        
        float tableTop = form->nhapTimKiem.rec.y + form->nhapTimKiem.rec.height + 25.0f;
        float viewportH = screenH - tableTop - HEADER_H - 40.0f;
        float totalHeight = count * ROW_HEIGHT;
        float maxScroll = totalHeight - viewportH;
        if (maxScroll < 0) maxScroll = 0;
        
        form->scrollList -= GetMouseWheelMove() * 45.0f;
        if (form->scrollList > maxScroll) form->scrollList = maxScroll;
        if (form->scrollList < 0) form->scrollList = 0;
        
        // Xử lý click chọn Bạn đọc trên bảng
        float tableX = screenW * 0.025f;
        float tableW = screenW * 0.945f;
        float curY = tableTop + HEADER_H - form->scrollList;
        
        BanDoc *curr = headBD;
        while (curr != NULL) {
            if (KiemTraKhopTimKiem(curr, form->nhapTimKiem.text)) {
                float btnW = (tableW * colRatioBD[5]) * 0.85f;
                float btnH = ROW_HEIGHT * 0.65f;
                float btnX = (tableX + tableW) - (tableW * colRatioBD[5]) + ((tableW * colRatioBD[5] - btnW)/2);
                float btnY = curY + (ROW_HEIGHT - btnH)/2;
                Rectangle btnHitbox = { btnX, btnY, btnW, btnH };

                if (curY >= (tableTop + HEADER_H) && curY + ROW_HEIGHT <= (tableTop + HEADER_H + viewportH)) {
                    if (CheckCollisionPointRec(GetMousePosition(), btnHitbox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        strcpy(form->maTheDaChon, curr->maThe);
                        form->state = TRA_B2_CHONSACH;
                        form->scrollList = 0;
                        break;
                    }
                }
                curY += ROW_HEIGHT;
            }
            curr = curr->next;
        }
    } 
    else if (form->state == TRA_B2_CHONSACH) {
        // Tính toán cuộn cho Bước 2 (Phiếu mượn dạng thẻ ngang cực rộng và to)
        int count = 0;
        PhieuMuonNode *tmp = headPM;
        while (tmp != NULL) {
            if (strcmp(tmp->mathe, form->maTheDaChon) == 0 && tmp->trangthai == 0) count++;
            tmp = tmp->next;
        }
        
        float listTop = 140.0f;
        float viewportH = screenH - listTop - 40.0f;
        float itemHeight = 85.0f; // Tinh chỉnh chiều cao dải để hiển thị được 7-8 thẻ
        float spacing = 12.0f;     // Khoảng cách giữa các dải
        float totalHeight = count * (itemHeight + spacing) + 20.0f;
        float maxScroll = totalHeight - viewportH;
        if (maxScroll < 0) maxScroll = 0;
        
        float wheel = GetMouseWheelMove();
        if (wheel != 0) form->scrollList -= wheel * 45.0f;
        if (form->scrollList > maxScroll) form->scrollList = maxScroll;
        if (form->scrollList < 0) form->scrollList = 0;
        
        float itemW = screenW * 0.82f; // Mở rộng dải phiếu mượn (82% màn hình)
        if (itemW < 700.0f) itemW = 700.0f;
        if (itemW > 1250.0f) itemW = 1250.0f;
        float itemX = (screenW - itemW) / 2.0f;
        float curY = listTop - form->scrollList + 10.0f;
        
        PhieuMuonNode *curr = headPM;
        while (curr != NULL) {
            if (strcmp(curr->mathe, form->maTheDaChon) == 0 && curr->trangthai == 0) {
                float btnW = 130.0f; // Tăng nhẹ kích thước nút chọn trả sách
                float btnH = 46.0f;
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
            NhapLieuTraSach(&form->nhapNgayTraThucTe);
        }
        
        int giaMotNgay = laygiathue(form->phieuDangXuLy->matruyen);
        int soNgayGoc = KhoangCachNgay(form->phieuDangXuLy->ngaymuon, form->phieuDangXuLy->ngaytra);
        if (soNgayGoc <= 0) soNgayGoc = 1;
        int tienGoc = giaMotNgay * soNgayGoc;

        if (form->nhapNgayTraThucTe.letterCount >= 8) {
            form->tongTien = (int)TinhTienThue(
                giaMotNgay,
                form->phieuDangXuLy->ngaymuon,
                form->phieuDangXuLy->ngaytra,
                form->nhapNgayTraThucTe.text
            );
            form->tienPhat = form->tongTien - tienGoc;
        } else {
            form->tongTien = tienGoc;
            form->tienPhat = 0;
        }

        Rectangle btnThanhToan = { rightPaneX + 40, screenH - 70, rightPaneW - 80, 55 };
        if (CheckCollisionPointRec(GetMousePosition(), btnThanhToan) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (form->nhapNgayTraThucTe.letterCount >= 8) {
                form->phieuDangXuLy->trangthai = 1; 
                CapNhatToanBoFilePhieuMuon(headPM); 
                LuuDoanhThuVaoFile(form->nhapNgayTraThucTe.text, form->tongTien);
                
                form->daThanhToan = 1; 

                // Reset về bước 1 thành công
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
// HÀM VẼ GIAO DIỆN (ĐỒNG BỘ BACKGROUND & MÀU SẮC)
// ==========================================
void DrawGiaoDienTraSach(FormTraSach *form, BanDoc *headBD, PhieuMuonNode *headPM, Font font) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    // Phông nền tối hạt hoạt cảnh đồng bộ với Timkiemthethuvien.c
    ClearBackground(GetColor(0x080f1eFF));
    DrawBackgroundParticles();
    DrawRectangle(0, 0, (int)screenW, (int)screenH, GetColor(0x1b264f80));

    // ----------------------------------------------------
    // BƯỚC 1: TÌM KIẾM THẺ BẠN ĐỌC (HIỂN THỊ DẠNG LIST)
    // ----------------------------------------------------
    if (form->state == TRA_B1_TIMKEM) {
        float tableTop = form->nhapTimKiem.rec.y + form->nhapTimKiem.rec.height + 25.0f;
        float tableX   = screenW * 0.025f;
        float tableW   = screenW * 0.945f;
        float viewportH = screenH - tableTop - HEADER_H - 40.0f;
        float sbW = 8.0f;

        // Đếm tổng số kết quả khớp lọc
        int matchCount = 0;
        BanDoc *cur = headBD;
        while (cur) {
            if (KiemTraKhopTimKiem(cur, form->nhapTimKiem.text)) matchCount++;
            cur = cur->next;
        }
        float contentH = matchCount * ROW_HEIGHT;

        // 1. Vẽ Danh Sách dạng bảng trong Scissor Mode
        BeginScissorMode((int)tableX, (int)(tableTop + HEADER_H), (int)(tableW + 20), (int)viewportH);
        {
            float rowY = tableTop + HEADER_H - form->scrollList;
            cur = headBD;
            int idx = 0;
            while (cur) {
                if (KiemTraKhopTimKiem(cur, form->nhapTimKiem.text)) {
                    Rectangle rowRect = { tableX, rowY, tableW, ROW_HEIGHT };
                    Color rowBg = (idx % 2 == 0) ? COL_ROW_EVEN : COL_ROW_ODD;
                    bool isHover = CheckCollisionPointRec(GetMousePosition(), rowRect);
                    
                    if (GetMousePosition().y < (tableTop + HEADER_H) || GetMousePosition().y > (tableTop + HEADER_H + viewportH)) {
                        isHover = false;
                    }
                    if (isHover) rowBg = COL_ROW_HOVER;

                    // Vẽ hàng
                    DrawRectangleRec(rowRect, rowBg);
                    DrawRectangle((int)tableX, (int)(rowY + ROW_HEIGHT - 1), (int)tableW, 1, COL_BORDER);

                    // Vẽ cột dữ liệu
                    const char *cells[5] = { cur->maThe, cur->hoTen, cur->sdt, cur->cccd, cur->hanSD };
                    for (int i = 0; i < 5; i++) {
                        float cx = tableX;
                        for (int k = 0; k < i; k++) cx += tableW * colRatioBD[k];
                        float cw = tableW * colRatioBD[i];
                        
                        Vector2 ts = MeasureTextEx(font, cells[i], 16.0f, 1);
                        if (i > 0) DrawRectangle((int)cx, (int)rowY, 1, (int)ROW_HEIGHT, COL_BORDER);
                        DrawTextEx(font, cells[i], (Vector2){ cx + 15.0f, rowY + (ROW_HEIGHT - ts.y)/2.0f }, 16.0f, 1, WHITE);
                    }

                    // Vẽ nút chọn ở cột cuối
                    float cx = tableX;
                    for (int k = 0; k < 5; k++) cx += tableW * colRatioBD[k];
                    float cw = tableW * colRatioBD[5];
                    float btnW = cw * 0.85f, btnH = ROW_HEIGHT * 0.65f;
                    float btnX = cx + (cw - btnW)/2.0f;
                    float btnY = rowY + (ROW_HEIGHT - btnH)/2.0f;
                    Rectangle btnR = { btnX, btnY, btnW, btnH };
                    bool bHov = isHover && CheckCollisionPointRec(GetMousePosition(), btnR);

                    DrawRectangle((int)cx, (int)rowY, 1, (int)ROW_HEIGHT, COL_BORDER);
                    DrawRectangleRounded(btnR, 0.4f, 8, bHov ? GetColor(0x324376FF) : GetColor(0x4A80FFFF));
                    Vector2 bts = MeasureTextEx(font, "Chọn", 15.0f, 1);
                    DrawTextEx(font, "Chọn", (Vector2){ btnX + (btnW - bts.x)/2.0f, btnY + (btnH - bts.y)/2.0f }, 15.0f, 1, WHITE);

                    rowY += ROW_HEIGHT;
                    idx++;
                }
                cur = cur->next;
            }
        }
        EndScissorMode();

        // 2. Vẽ thanh cuộn bên phải (Scrollbar)
        float trackX = tableX + tableW + 8.0f;
        Rectangle trackR = { trackX, tableTop + HEADER_H, 10.0f, viewportH };
        DrawRectangleRounded(trackR, 0.5f, 6, GetColor(0x1b264f80)); // Luôn vẽ nền thanh cuộn

        if (contentH > viewportH) {
            float thumbH = viewportH * (viewportH / contentH);
            if (thumbH < 30.0f) thumbH = 30.0f;
            float maxScroll = contentH - viewportH;
            float thumbY = tableTop + HEADER_H + (form->scrollList / maxScroll) * (viewportH - thumbH);
            
            Rectangle thumbR = { trackX, thumbY, 10.0f, thumbH };
            bool tHov = CheckCollisionPointRec(GetMousePosition(), thumbR);
            DrawRectangleRounded(thumbR, 0.5f, 6, tHov ? GetColor(0x4A80FFFF) : GetColor(0x4A80FF88));

            // Logic kéo thả thanh cuộn
            static bool draggingBD = false;
            static float dragStartY = 0.0f;
            if (draggingBD) {
                if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) draggingBD = false;
                else {
                    float dy = GetMousePosition().y - dragStartY;
                    float ratio = (dy - (tableTop + HEADER_H)) / (viewportH - thumbH);
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
            // Khi không đủ phần tử để lướt, vẽ thanh cuộn giả mờ ảo
            Rectangle thumbR = { trackX, tableTop + HEADER_H, 10.0f, viewportH };
            DrawRectangleRounded(thumbR, 0.5f, 6, GetColor(0x4A80FF33));
        }

        // 3. Vẽ Header của Bảng
        DrawRectangle((int)tableX, (int)tableTop, (int)tableW, (int)HEADER_H, COL_HEADER_BG);
        DrawRectangle((int)tableX, (int)(tableTop + HEADER_H - 2), (int)tableW, 2, GetColor(0x4A80FFFF));
        for (int c = 0; c < 6; c++) {
            float cx = tableX;
            for (int k = 0; k < c; k++) cx += tableW * colRatioBD[k];
            float cw = tableW * colRatioBD[c];
            Vector2 ts = MeasureTextEx(font, colHeadersBD[c], 16.0f, 1);
            if (c > 0) DrawRectangle((int)cx, (int)tableTop, 1, (int)HEADER_H, COL_BORDER);
            DrawTextEx(font, colHeadersBD[c], (Vector2){ cx + (cw - ts.x)/2.0f, tableTop + (HEADER_H - ts.y)/2.0f }, 16.0f, 1, WHITE);
        }

        // 4. Vẽ dải Banner tiêu đề đồng bộ (Mã màu 0x2b3a67ff, Tiêu đề trắng)
        DrawRectangleGradientEx((Rectangle){ 0, 0, screenW, 115 }, GetColor(0x2b3a67ff), GetColor(0x2b3a67ff), GetColor(0x2b3a67ff), GetColor(0x2b3a67ff));
        DrawRectangle(0, 113, (int)screenW, 2, GetColor(0x4A80FF70));

        Vector2 mT = MeasureTextEx(font, "CHỌN THẺ BẠN ĐỌC ĐỂ TRẢ SÁCH", 32.0f, 1);
        DrawTextEx(font, "CHỌN THẺ BẠN ĐỌC ĐỂ TRẢ SÁCH", (Vector2){ (screenW - mT.x)/2.0f, (115 - mT.y)/2.0f }, 32.0f, 1, WHITE);

        // Thanh nhập tìm kiếm
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
    // BƯỚC 2: CHỌN SÁCH CẦN TRẢ (HIỂN THỊ DẠNG THẺ NGANG TO RỘNG HƠN - TỐI ĐA 8 THẺ/TRANG)
    // ----------------------------------------------------
    else if (form->state == TRA_B2_CHONSACH) {
        float listTop = 140.0f;
        float viewportH = screenH - listTop - 40.0f;
        float sbW = 10.0f; // Scrollbar to hơn chút

        int count = 0;
        PhieuMuonNode *cur = headPM;
        while (cur) {
            if (strcmp(cur->mathe, form->maTheDaChon) == 0 && cur->trangthai == 0) count++;
            cur = cur->next;
        }

        float itemHeight = 85.0f; // Tinh chỉnh chiều cao dải phiếu mượn (để hiển thị cỡ 7-8 thẻ)
        float spacing = 12.0f;     // Khoảng cách giữa các thẻ
        float totalHeight = count * (itemHeight + spacing) + 20.0f;

        float itemW = screenW * 0.82f; // Dải phiếu mượn cực rộng
        if (itemW < 700.0f) itemW = 700.0f;
        if (itemW > 1250.0f) itemW = 1250.0f;
        float itemX = (screenW - itemW) / 2.0f;

        // 1. Vẽ Danh sách Phiếu mượn dưới dạng thẻ ngang to rộng cực đẹp
        BeginScissorMode((int)itemX - 10, (int)listTop, (int)(itemW + 35), (int)viewportH);
        {
            float curY = listTop - form->scrollList + 10.0f;
            cur = headPM;
            int idx = 0;
            while (cur) {
                if (strcmp(cur->mathe, form->maTheDaChon) == 0 && cur->trangthai == 0) {
                    Rectangle cardRec = { itemX, curY, itemW, itemHeight };
                    bool isHover = CheckCollisionPointRec(GetMousePosition(), cardRec);

                    if (GetMousePosition().y < listTop || GetMousePosition().y > (listTop + viewportH)) {
                        isHover = false;
                    }

                    // Màu nền dải phiếu mượn (Tone Navy tối sang trọng, viền ngoài bo cong cao cấp)
                    Color cardBg = isHover ? GetColor(0x1e2d5cAA) : GetColor(0x131d3dAA);
                    Color borderCol = isHover ? GetColor(0x4A80FFFF) : GetColor(0x1b264fFF);

                    // Vẽ dải phiếu mượn bo tròn sang trọng
                    DrawRectangleRounded(cardRec, 0.15f, 10, cardBg);
                    DrawRectangleRoundedLines(cardRec, 0.15f, 10, borderCol);

                    // Hiển thị thông tin truyện
                    char line1[512];
                    char line2[512];
                    sprintf(line1, "Truyện: %s (Mã: %s)", cur->tentruyen, cur->matruyen);
                    sprintf(line2, "Ngày mượn: %s   -   Ngày trả dự kiến: %s", cur->ngaymuon, cur->ngaytra);

                    // Chữ hiển thị
                    DrawTextEx(font, line1, (Vector2){ itemX + 30.0f, curY + 18.0f }, 22.0f, 1, WHITE);
                    DrawTextEx(font, line2, (Vector2){ itemX + 30.0f, curY + 50.0f }, 17.0f, 1, GetColor(0x90B8FFFF));

                    // Vẽ nút "Trả sách"
                    float btnW = 120.0f;
                    float btnH = 42.0f;
                    float btnX = itemX + itemW - btnW - 25.0f;
                    float btnY = curY + (itemHeight - btnH) / 2.0f;
                    Rectangle btnR = { btnX, btnY, btnW, btnH };
                    bool btnHov = isHover && CheckCollisionPointRec(GetMousePosition(), btnR);

                    Color btnColor = btnHov ? GetColor(0x2ecc71FF) : GetColor(0x27ae60FF); 
                    DrawRectangleRounded(btnR, 0.25f, 8, btnColor);
                    
                    Vector2 bts = MeasureTextEx(font, "Trả sách", 17.0f, 1);
                    DrawTextEx(font, "Trả sách", (Vector2){ btnX + (btnW - bts.x)/2.0f, btnY + (btnH - bts.y)/2.0f }, 17.0f, 1, WHITE);

                    curY += itemHeight + spacing;
                    idx++;
                }
                cur = cur->next;
            }
        }
        EndScissorMode();

        // 2. Vẽ thanh cuộn (Scrollbar) cho Bước 2
        float trackX = itemX + itemW + 16.0f;
        Rectangle trackR = { trackX, listTop, sbW, viewportH };
        DrawRectangleRounded(trackR, 0.5f, 6, GetColor(0x1b264f80)); // Luôn vẽ Track

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
            // Hiển thị Thumb mờ khi không cuộn
            Rectangle thumbR = { trackX, listTop, sbW, viewportH };
            DrawRectangleRounded(thumbR, 0.5f, 6, GetColor(0x4A80FF33));
        }

        // 3. Vẽ Banner tiêu đề màu sắc 0x2b3a67ff, Chữ Trắng
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
    // BƯỚC 3: MÀN HÌNH TÍNH TIỀN CHIA ĐÔI (XANH PASTEL)
    // ----------------------------------------------------
    else if (form->state == TRA_B3_XULY) {
        float leftPaneW  = screenW * 0.65f;
        float rightPaneX = leftPaneW;
        float rightPaneW = screenW * 0.35f;

        // ── Nền pane TRÁI: xanh pastel nhạt (đồng bộ với pane phải) ──
        DrawRectangle(0, 0, (int)leftPaneW, (int)screenH, GetColor(0xEEF7FBFF));

        // ── Nền pane phải: xanh pastel nhạt ──
        DrawRectangle((int)rightPaneX, 0, (int)rightPaneW, (int)screenH, GetColor(0xE8F4F8FF));
        DrawLine(rightPaneX, 0, rightPaneX, screenH, GetColor(0x8AB6D6FF));

        // ── Banner pane TRÁI: xanh pastel đậm hơn một chút ──
        DrawRectangleGradientEx((Rectangle){ 0, 0, leftPaneW, 115 },
            GetColor(0x8AB6D6FF), GetColor(0x8AB6D6FF),
            GetColor(0xA8CBE0FF), GetColor(0xA8CBE0FF));
        DrawRectangle(0, 113, (int)leftPaneW, 2, GetColor(0x507A9BFF));
        Vector2 titleL = MeasureTextEx(font, "THÔNG TIN PHIẾU MƯỢN", 28.0f, 1);
        DrawTextEx(font, "THÔNG TIN PHIẾU MƯỢN",
            (Vector2){ (leftPaneW - titleL.x)/2.0f, (115 - titleL.y)/2.0f }, 28.0f, 1, GetColor(0x2C5F8AFF));

        // ── Vẽ phiếu mượn pane trái ──
        float scaleB3 = leftPaneW / 900.0f;
        if (scaleB3 > scale) scaleB3 = scale;
        float pmW = 800.0f * scaleB3;
        DrawThePhieuMuon(form->phieuDangXuLy, font, (leftPaneW - pmW)/2.0f, 140.0f, scaleB3);

        // ── Tiêu đề pane PHẢI ──
        Vector2 titleR = MeasureTextEx(font, "ĐƠN TRẢ SÁCH", 26.0f, 1);
        DrawTextEx(font, "ĐƠN TRẢ SÁCH",
            (Vector2){ rightPaneX + (rightPaneW - titleR.x)/2.0f, 42.0f }, 26.0f, 1, GetColor(0x2C5F8AFF));

        // ── Input ngày trả thực tế ──
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

        // ── Bill box ──
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
            DrawTextEx(font, TextFormat("Chiết khấu trả sớm: %d VNĐ", form->tienPhat),
                (Vector2){ billRec.x + 20, billRec.y + 65 }, 20, 1, GetColor(0x2ECC71FF));
        } else {
            DrawTextEx(font, "Phí chênh lệch: 0 VNĐ",
                (Vector2){ billRec.x + 20, billRec.y + 65 }, 20, 1, GetColor(0x507A9BFF));
        }

        // Đường kẻ phân cách
        DrawLine((int)(billRec.x + 15), (int)(billRec.y + 120),
                 (int)(billRec.x + billRec.width - 15), (int)(billRec.y + 120),
                 GetColor(0x8AB6D6FF));

        DrawTextEx(font, "TỔNG CỘNG:",
            (Vector2){ billRec.x + 20, billRec.y + 140 }, 20, 1, GetColor(0x507A9BFF));
        DrawTextEx(font, TextFormat("%d VNĐ", form->tongTien),
            (Vector2){ billRec.x + 20, billRec.y + 175 }, 30, 1, GetColor(0x2C5F8AFF));

        // ── Đường kẻ tổng và nút xác nhận ──
        DrawLine((int)rightPaneX, (int)(screenH - 80), (int)(rightPaneX + rightPaneW), (int)(screenH - 80),
                 GetColor(0x8AB6D6FF));

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

    // --- VẼ NÚT QUAY LẠI CHUNG CHO TẤT CẢ CÁC BƯỚC NẰM TRÊN CÙNG ---
    Rectangle btnBack = { 20, 25, 120, 38 };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);

    // Bước 3 dùng nền sáng → nút dùng màu xanh pastel (giống ảnh mẫu)
    // Bước 1 & 2 nền tối → nút dùng nền trong suốt + chữ trắng như cũ
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