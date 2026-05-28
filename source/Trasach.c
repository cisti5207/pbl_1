#include "Trasach.h"
#include "TimkiemLSphieumuon.h" 
#include "menu.h" // Nhúng menu vào để dùng biến APP_MENU
#include <string.h>
#include <stdio.h>
#include <math.h>

// Khai báo các hàm tính toán từ Doanhthu.c 
int laygiathue(const char *masachcantim);
int KhoangCachNgay(const char *ngayBatDau, const char *ngayKetThuc);
double TinhTienThue(int giaGoc, const char *ngayDuKien, const char *ngayTraThucTe);

// ==========================================
// CÁC HÀM XỬ LÝ FILE
// ==========================================
void CapNhatToanBoFilePhieuMuon(PhieuMuonNode *head) {
    FILE *f = fopen("data/Phieumuon/LSPhieumuon.txt", "w"); 
    if (f == NULL) return;
    PhieuMuonNode *curr = head;
    while (curr != NULL) {
        fprintf(f, "%-12s | %-12s | %-19s | %-50s | %-19s | %-19s | %d\n", 
                curr->maPM, curr->mathe, curr->matruyen, 
                curr->tentruyen, curr->ngaymuon, curr->ngaytra, curr->trangthai);
        curr = curr->next;
    }
    fclose(f);
}

void LuuDoanhThuVaoFile(const char *ngay, int tien) {
    FILE *f = fopen("data/Doanhthu.txt", "a");
    if (f != NULL) {
        fprintf(f, "%s | %d\n", ngay, tien);
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
// LOGIC CẬP NHẬT TRẠNG THÁI (KÈM GIỚI HẠN SCROLL)
// ==========================================
// Đã thêm biến currentState vào hàm để điều khiển về Menu
void UpdateLogicTraSach(FormTraSach *form, BanDoc *headBD, PhieuMuonNode *headPM, int *currentState) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;
    
    // --- XỬ LÝ NÚT QUAY LẠI CHUNG TRỰC TIẾP TRONG NÀY ---
    Rectangle btnBack = { 20, 20, 130, 40 };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    
    if (IsKeyPressed(KEY_ESCAPE) || (isHoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
        if (form->state == TRA_B1_TIMKEM) {
            InitFormTraSach(form); // Dọn dẹp form
            *currentState = APP_MENU; // Quay về Menu
        } else if (form->state == TRA_B2_CHONSACH) {
            form->state = TRA_B1_TIMKEM; // Lùi 1 bước
        } else if (form->state == TRA_B3_XULY) {
            form->state = TRA_B2_CHONSACH; // Lùi 1 bước
            form->phieuDangXuLy = NULL;
        }
        return; // Đã xử lý nút back thì ngắt các update khác bên dưới
    }
    // ----------------------------------------------------

    if (form->state == TRA_B1_TIMKEM) {
        form->nhapTimKiem.rec = (Rectangle){ screenW/2 - 300, 150, 600, 50 };
        form->nhapTimKiem.isFocused = true; 
        
        NhapLieuTraSach(&form->nhapTimKiem);
        
        // TÍNH TOÁN GIỚI HẠN CUỘN CHO BƯỚC 1
        float cardHeight = 420.0f * scale;
        int count = 0;
        BanDoc *tmp = headBD;
        while (tmp != NULL) {
            if (KiemTraKhopTimKiem(tmp, form->nhapTimKiem.text)) count++;
            tmp = tmp->next;
        }
        float totalHeight = count * (cardHeight + 40.0f * scale);
        float maxScroll = 0; // Không cho cuộn xuống quá đầu
        float minScroll = screenH - 250.0f - totalHeight; // Không cho cuộn lên quá cuối
        if (minScroll > 0) minScroll = 0; 
        
        form->scrollList += GetMouseWheelMove() * 45.0f;
        if (form->scrollList > maxScroll) form->scrollList = maxScroll;
        if (form->scrollList < minScroll) form->scrollList = minScroll;
        
        float cardWidth = 650.0f * scale; 
        float startX = (screenW - cardWidth) / 2.0f;
        float startY = 250 + form->scrollList;
        
        BanDoc *curr = headBD;
        while (curr != NULL) {
            if (KiemTraKhopTimKiem(curr, form->nhapTimKiem.text)) {
                Rectangle cardHitbox = { startX, startY, cardWidth, cardHeight }; 
                // Chỉ nhận click nếu thẻ nằm dưới vùng search box
                if (startY >= 230 && CheckCollisionPointRec(GetMousePosition(), cardHitbox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    strcpy(form->maTheDaChon, curr->maThe);
                    form->state = TRA_B2_CHONSACH;
                    form->scrollList = 0; 
                    break;
                }
                startY += cardHeight + 40.0f * scale; 
            }
            curr = curr->next;
        }
    } 
    else if (form->state == TRA_B2_CHONSACH) {
        // TÍNH TOÁN GIỚI HẠN CUỘN CHO BƯỚC 2
        float pmHeight = 420.0f * scale;
        int count = 0;
        PhieuMuonNode *tmp = headPM;
        while (tmp != NULL) {
            if (strcmp(tmp->mathe, form->maTheDaChon) == 0 && tmp->trangthai == 0) count++;
            tmp = tmp->next;
        }
        float totalHeight = count * (pmHeight + 40.0f * scale);
        float maxScroll = 0;
        float minScroll = screenH - 150.0f - totalHeight;
        if (minScroll > 0) minScroll = 0;

        form->scrollList += GetMouseWheelMove() * 45.0f;
        if (form->scrollList > maxScroll) form->scrollList = maxScroll;
        if (form->scrollList < minScroll) form->scrollList = minScroll;
        
        float pmWidth = 800.0f * scale; 
        float startX = (screenW - pmWidth) / 2.0f;
        float startY = 150 + form->scrollList;

        PhieuMuonNode *curr = headPM;
        while (curr != NULL) {
            if (strcmp(curr->mathe, form->maTheDaChon) == 0 && curr->trangthai == 0) {
                Rectangle cardHitbox = { startX, startY, pmWidth, pmHeight };
                // Chỉ nhận click khi cuộn dưới thanh banner
                if (startY >= 120 && CheckCollisionPointRec(GetMousePosition(), cardHitbox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    form->phieuDangXuLy = curr;
                    form->state = TRA_B3_XULY;
                    form->nhapNgayTraThucTe.isFocused = true;
                    break;
                }
                startY += pmHeight + 40.0f * scale;
            }
            curr = curr->next;
        }
    }
    else if (form->state == TRA_B3_XULY) {
        float rightPaneX = screenW * 0.65f;
        form->nhapNgayTraThucTe.rec = (Rectangle){ rightPaneX + 40, 120, screenW * 0.35f - 80, 50 };
        form->nhapNgayTraThucTe.isFocused = true; 
        
        NhapLieuTraSach(&form->nhapNgayTraThucTe);
        
        int giaMotNgay = laygiathue(form->phieuDangXuLy->matruyen);
        int soNgayMuon = KhoangCachNgay(form->phieuDangXuLy->ngaymuon, form->phieuDangXuLy->ngaytra);
        if (soNgayMuon <= 0) soNgayMuon = 1; 
        
        int tongGiaGoc = giaMotNgay * soNgayMuon; 
        
        if (form->nhapNgayTraThucTe.letterCount >= 8) { 
            form->tongTien = (int)TinhTienThue(tongGiaGoc, form->phieuDangXuLy->ngaytra, form->nhapNgayTraThucTe.text);
            form->tienPhat = form->tongTien - tongGiaGoc; 
        } else {
            form->tongTien = tongGiaGoc;
            form->tienPhat = 0;
        }

        Rectangle btnThanhToan = { rightPaneX + 40, screenH - 90, screenW * 0.35f - 80, 60 };
        if (CheckCollisionPointRec(GetMousePosition(), btnThanhToan) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (form->nhapNgayTraThucTe.letterCount >= 8) {
                form->phieuDangXuLy->trangthai = 1; 
                CapNhatToanBoFilePhieuMuon(headPM); 
                LuuDoanhThuVaoFile(form->nhapNgayTraThucTe.text, form->tongTien);
                InitFormTraSach(form); 
            }
        }
    }
}

// ==========================================
// GIAO DIỆN VẼ (ÁP DỤNG Z-INDEX ĐỂ CHE THẺ TRƯỢT LÊN)
// ==========================================
void DrawGiaoDienTraSach(FormTraSach *form, BanDoc *headBD, PhieuMuonNode *headPM, Font font) {
    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();
    float scale = (screenW / 1100.0f < screenH / 750.0f) ? screenW / 1100.0f : screenH / 750.0f;
    if (scale < 0.5f) scale = 0.5f;

    ClearBackground(GetColor(0xFFF0F5FF)); 
    
    // ----------------------------------------------------
    // BƯỚC 1: TÌM KIẾM THẺ
    // ----------------------------------------------------
    if (form->state == TRA_B1_TIMKEM) {
        
        // 1. VẼ DANH SÁCH THẺ TRƯỚC (LỚP DƯỚI CÙNG)
        float cardWidth = 650.0f * scale; 
        float cardHeight = 420.0f * scale;
        float startX = (screenW - cardWidth) / 2.0f;
        float startY = 250 + form->scrollList;

        BanDoc *curr = headBD;
        while (curr != NULL) {
            if (KiemTraKhopTimKiem(curr, form->nhapTimKiem.text)) {
                // Tối ưu: Chỉ vẽ thẻ nằm trong khu vực hiển thị màn hình
                if (startY + cardHeight > 0 && startY < screenH) {
                    DrawTheBanDoc_TimKiem(curr, font, startX, startY);
                    
                    Rectangle cardHitbox = { startX, startY, cardWidth, cardHeight };
                    if (CheckCollisionPointRec(GetMousePosition(), cardHitbox)) {
                        DrawRectangleRounded(cardHitbox, 0.1f, 10, Fade(ORANGE, 0.1f));
                        DrawRectangleRoundedLines(cardHitbox, 0.1f, 10, ORANGE);
                    }
                }
                startY += cardHeight + 40.0f * scale;
            }
            curr = curr->next;
        }

        // 2. VẼ MẢNG MÀU NỀN CHE ĐI PHẦN ĐỈNH BỊ CUỘN (LỚP GIỮA)
        DrawRectangle(0, 0, screenW, 230, GetColor(0xFFF0F5FF));

        // 3. VẼ BANNER VÀ THANH TÌM KIẾM (LỚP TRÊN CÙNG)
        DrawRectangleGradientV(0, 0, screenW, 100, GetColor(0xFFB6C1FF), GetColor(0xFFC0CBFF)); 
        DrawTextEx(font, "CHỌN THẺ BẠN ĐỌC ĐỂ TRẢ SÁCH", (Vector2){screenW/2 - 250, 30}, 36, 1, MAROON);
        
        DrawRectangleRounded(form->nhapTimKiem.rec, 0.2f, 10, WHITE);
        DrawRectangleRoundedLines(form->nhapTimKiem.rec, 0.2f, 10, PINK);
        
        BeginScissorMode((int)form->nhapTimKiem.rec.x, (int)form->nhapTimKiem.rec.y, (int)form->nhapTimKiem.rec.width, (int)form->nhapTimKiem.rec.height);
            Vector2 textPos = {form->nhapTimKiem.rec.x + 15, form->nhapTimKiem.rec.y + 12};
            DrawTextEx(font, form->nhapTimKiem.text, textPos, 24, 1, MAROON);
            
            if (form->nhapTimKiem.isFocused && (((int)(GetTime() * 1.5f)) % 2 == 0)) {
                float textWidth = MeasureTextEx(font, form->nhapTimKiem.text, 24, 1).x;
                DrawRectangleV((Vector2){textPos.x + textWidth + 2, textPos.y}, (Vector2){2, 24}, MAROON);
            }
        EndScissorMode();
    } 
    // ----------------------------------------------------
    // BƯỚC 2: CHỌN SÁCH CẦN TRẢ
    // ----------------------------------------------------
    else if (form->state == TRA_B2_CHONSACH) {
        // 1. VẼ DANH SÁCH SÁCH Ở LỚP DƯỚI
        float pmWidth = 800.0f * scale; 
        float pmHeight = 420.0f * scale;
        float startX = (screenW - pmWidth) / 2.0f;
        float startY = 150 + form->scrollList;
        
        int count = 0;
        PhieuMuonNode *curr = headPM;
        while (curr != NULL) {
            if (strcmp(curr->mathe, form->maTheDaChon) == 0 && curr->trangthai == 0) {
                count++;
                if (startY + pmHeight > 0 && startY < screenH) {
                    DrawThePhieuMuon(curr, font, startX, startY, scale);
                    Rectangle cardHitbox = { startX, startY, pmWidth, pmHeight };
                    bool isHover = CheckCollisionPointRec(GetMousePosition(), cardHitbox);
                    
                    if (isHover) {
                        DrawRectangleRounded(cardHitbox, 0.1f, 10, Fade(ORANGE, 0.2f));
                        DrawRectangleRoundedLines(cardHitbox, 0.1f, 10, ORANGE);
                        DrawRectangleRounded((Rectangle){startX + pmWidth - 180*scale, startY + pmHeight/2 - 25*scale, 160*scale, 50*scale}, 0.2f, 10, MAROON);
                        DrawTextEx(font, ">> CHỌN <<", (Vector2){startX + pmWidth - 170*scale, startY + pmHeight/2 - 12*scale}, 24*scale, 1, WHITE);
                    }
                }
                startY += pmHeight + 40.0f * scale;
            }
            curr = curr->next;
        }
        
        // 2. VẼ LỚP MẶT NẠ CHE SCROLL
        DrawRectangle(0, 0, screenW, 120, GetColor(0xFFF0F5FF));
        
        // 3. VẼ BANNER VÀ HEADER LỚP TRÊN
        DrawRectangleGradientV(0, 0, screenW, 100, GetColor(0xFFB6C1FF), GetColor(0xFFC0CBFF));
        DrawTextEx(font, TextFormat("DANH SÁCH CHƯA TRẢ CỦA: %s", form->maTheDaChon), (Vector2){screenW/2 - 350, 30}, 30, 1, MAROON);
        
        if (count == 0) {
            DrawTextEx(font, "Khách hàng này đã trả toàn bộ sách!", (Vector2){screenW/2 - 250, 200}, 24, 1, DARKGRAY);
        }
    }
    // ----------------------------------------------------
    // BƯỚC 3: MÀN HÌNH TÍNH TIỀN CHIA ĐÔI (POS)
    // ----------------------------------------------------
    else if (form->state == TRA_B3_XULY) {
        float leftPaneW = screenW * 0.65f;
        float rightPaneX = leftPaneW;
        float rightPaneW = screenW * 0.35f;
        
        DrawRectangle(rightPaneX, 0, rightPaneW, screenH, WHITE);
        DrawLine(rightPaneX, 0, rightPaneX, screenH, LIGHTGRAY);
        
        DrawTextEx(font, "THÔNG TIN PHIẾU MƯỢN", (Vector2){leftPaneW/2 - 160, 40}, 30, 1, MAROON);
        
        float scaleB3 = (leftPaneW / 900.0f);
        if (scaleB3 > scale) scaleB3 = scale; 
        float pmW = 800 * scaleB3;
        
        DrawThePhieuMuon(form->phieuDangXuLy, font, (leftPaneW - pmW)/2, 120, scaleB3);
        
        DrawTextEx(font, "ĐƠN TRẢ SÁCH", (Vector2){rightPaneX + rightPaneW/2 - 80, 40}, 24, 1, GetColor(0x324376FF));
        
        DrawTextEx(font, "Nhập ngày trả thực tế (dd/mm/yyyy):", (Vector2){form->nhapNgayTraThucTe.rec.x, 90}, 18, 1, GRAY);
        DrawRectangleRounded(form->nhapNgayTraThucTe.rec, 0.2f, 10, WHITE);
        DrawRectangleRoundedLines(form->nhapNgayTraThucTe.rec, 0.2f, 10, form->nhapNgayTraThucTe.isFocused ? GetColor(0x324376FF) : LIGHTGRAY);
        
        float fontSize = 20;
        Vector2 textSize = MeasureTextEx(font, form->nhapNgayTraThucTe.text, fontSize, 1);
        DrawTextEx(font, form->nhapNgayTraThucTe.text, (Vector2){form->nhapNgayTraThucTe.rec.x + 15, form->nhapNgayTraThucTe.rec.y + 15}, fontSize, 1, BLACK);
        if (form->nhapNgayTraThucTe.isFocused && (((int)(GetTime() * 1.5f)) % 2 == 0)) {
            DrawRectangleV((Vector2){form->nhapNgayTraThucTe.rec.x + 15 + textSize.x + 2, form->nhapNgayTraThucTe.rec.y + 10}, (Vector2){2, fontSize + 4}, GetColor(0x324376FF));
        }

        Rectangle billRec = { rightPaneX + 40, 200, rightPaneW - 80, 250 };
        DrawRectangleRounded(billRec, 0.1f, 5, GetColor(0xF9F9F9FF));
        
        int giaGoc = form->tongTien - form->tienPhat; 
        
        DrawTextEx(font, TextFormat("Tiền thuê sách: %d VNĐ", giaGoc), (Vector2){billRec.x + 15, billRec.y + 20}, 20, 1, DARKGRAY);
        
        if (form->tienPhat > 0) {
            DrawTextEx(font, TextFormat("Phí trả chậm: +%d VNĐ", form->tienPhat), (Vector2){billRec.x + 15, billRec.y + 60}, 20, 1, RED);
        } else if (form->tienPhat < 0) {
            DrawTextEx(font, TextFormat("Chiết khấu trả sớm: %d VNĐ", form->tienPhat), (Vector2){billRec.x + 15, billRec.y + 60}, 20, 1, GetColor(0x2ECC71FF)); 
        } else {
            DrawTextEx(font, "Phí chênh lệch: 0 VNĐ", (Vector2){billRec.x + 15, billRec.y + 60}, 20, 1, GRAY);
        }
        
        DrawLine(rightPaneX, screenH - 160, screenW, screenH - 160, LIGHTGRAY);
        DrawTextEx(font, "TỔNG CỘNG:", (Vector2){rightPaneX + 40, screenH - 140}, 24, 1, GRAY);
        DrawTextEx(font, TextFormat("%d VNĐ", form->tongTien), (Vector2){rightPaneX + 40, screenH - 100}, 30, 1, GetColor(0x324376FF));
        
        Rectangle btnThanhToan = { rightPaneX + 40, screenH - 60, rightPaneW - 80, 50 };
        bool hoverBtn = CheckCollisionPointRec(GetMousePosition(), btnThanhToan);
        DrawRectangleRounded(btnThanhToan, 0.2f, 10, hoverBtn ? GetColor(0x5A8BFFFF) : GetColor(0x7EA5FFFF)); 
        DrawTextEx(font, "ĐỒNG Ý TRẢ SÁCH", (Vector2){btnThanhToan.x + btnThanhToan.width/2 - 100, btnThanhToan.y + 15}, 20, 1, WHITE);
    }

    // --- VẼ NÚT QUAY LẠI CHUNG CHO TẤT CẢ CÁC BƯỚC NẰM Ở LỚP TRÊN CÙNG ---
    Rectangle btnBack = { 20, 20, 130, 40 };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    Color btnColor = isHoverBack ? MAROON : WHITE;
    Color textColor = isHoverBack ? WHITE : MAROON;
    DrawRectangleRounded(btnBack, 0.3f, 10, btnColor);
    DrawRectangleRoundedLines(btnBack, 0.3f, 10, MAROON);
    DrawTextEx(font, "< Quay lai", (Vector2){btnBack.x + 15, btnBack.y + 10}, 20, 1, textColor);
    // ----------------------------------------------------------------------
}