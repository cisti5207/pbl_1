#include "Doanhthu.h"
#include "menu.h" // Dùng APP_MENU
#include "libmanage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct tạm để chứa dữ liệu đã gộp (Ngày/Tuần/Tháng...)
typedef struct {
    char label[50];
    double amount;
    int sortKey;
} AggItem;

int laygiathue(const char *masachcantim) {
    FILE *f = fopen("data/ManageBooks/dataTruyen.txt", "r"); 
    if (f == NULL) {
        return 2000; 
    }
    char line[1024]; 
    int lineCount = 0;

    while (fgets(line, sizeof(line), f)) {
        lineCount++;
        if (lineCount <= 3) continue;
        char ma[20];
       if (sscanf(line, " | %[^|]", ma) == 1) {
    int len = strlen(ma);
    while (len > 0 && ma[len-1] == ' ') ma[--len] = '\0';
    if (strcmp(ma, masachcantim) == 0) {
                int giathue;
                sscanf(line, " | %*[^|] | %*[^|] | %*[^|] | %*[^|] | %*[^|] | %*[^|] | %*[^|] | %*[^|] | %*[^|] | %*[^|] | %d |", &giathue);
                fclose(f);
                return giathue;
            }
        }
    }
    fclose(f);
    return 2000; 
}

time_t ParseDateStr(const char *dateStr) {
    struct tm timeinfo = {0};
    int d, m, y;
    if (sscanf(dateStr, "%d/%d/%d", &d, &m, &y) == 3) {
        timeinfo.tm_mday = d;
        timeinfo.tm_mon = m - 1;    
        timeinfo.tm_year = y - 1900; 
        return mktime(&timeinfo);
    }
    return -1;
}

int KhoangCachNgay(const char *ngayBatDau, const char *ngayKetThuc) {
    time_t t1 = ParseDateStr(ngayBatDau);
    time_t t2 = ParseDateStr(ngayKetThuc);
    if (t1 == -1 || t2 == -1) return 0;
    
    double diff = difftime(t2, t1);
    return (int)(diff / (60.0 * 60.0 * 24.0));
}

double TinhTienThue(int giaMotNgay, const char *ngayMuon, const char *ngayDuKien, const char *ngayTraThucTe) {
    
    // Số ngày thuê gốc (ngayMuon -> ngayDuKien)
    int soNgayGoc = KhoangCachNgay(ngayMuon, ngayDuKien);
    if (soNgayGoc <= 0) soNgayGoc = 1;
    
    double tienGoc = (double)giaMotNgay * soNgayGoc;
    
    // Số ngày lệch (âm = trả sớm, dương = trả trễ, 0 = đúng hạn)
    int lechNgay = KhoangCachNgay(ngayDuKien, ngayTraThucTe);
    
    if (lechNgay == 0) {
        // Đúng hạn: giữ nguyên
        return tienGoc;
    } 
    else if (lechNgay < 0) {
        // Trả sớm: giảm 5%/ngày trên số ngày sớm
        int ngaySom = -lechNgay;
        double giamGia = (double)giaMotNgay * ngaySom * 0.1;
        return tienGoc - giamGia;
    } 
    else {
        // Trả trễ: cộng thêm 5%/ngày trên số ngày trễ
        double phuThu = (double)giaMotNgay * lechNgay * 0.1;
        return tienGoc + phuThu;
    }
}

unsigned int HashNgay(const char *ngay) {
    unsigned int hash = 0;
    while (*ngay) {
        hash = (hash * 31) + *ngay;
        ngay++;
    }
    return hash % DOANHTHU_HASH_SIZE;
}

void InitDoanhThuMap(DoanhThuMap *map) {
    for (int i = 0; i < DOANHTHU_HASH_SIZE; i++) {
        map->mang[i] = NULL;
    }
}

void ThemDoanhThu(DoanhThuMap *map, const char *ngay, double soTien) {
    unsigned int index = HashNgay(ngay);
    DoanhThuNode *curr = map->mang[index];
    
    while (curr != NULL) {
        if (strcmp(curr->ngay, ngay) == 0) {
            curr->tongTien += soTien; 
            return;
        }
        curr = curr->next;
    }
    
    DoanhThuNode *newNode = (DoanhThuNode*)malloc(sizeof(DoanhThuNode));
    strcpy(newNode->ngay, ngay);
    newNode->tongTien = soTien;
    newNode->next = map->mang[index];
    map->mang[index] = newNode;
}

void GiaiPhongDoanhThuMap(DoanhThuMap *map) {
    for (int i = 0; i < DOANHTHU_HASH_SIZE; i++) {
        DoanhThuNode *curr = map->mang[i];
        while (curr != NULL) {
            DoanhThuNode *temp = curr;
            curr = curr->next;
            free(temp);
        }
        map->mang[i] = NULL;
    }
}

void InitFormDoanhThu(FormDoanhThu *form) {
    form->currentView = VIEW_NGAY;
    form->scroll = 0;
    memset(form->nhapNam.text, 0, 256);
    form->nhapNam.letterCount = 0;
    form->nhapNam.isFocused = false;
}

// =========================================================
// PHẦN 3: VẼ GIAO DIỆN UI DASHBOARD DOANH THU (XANH PASTEL)
// =========================================================

void DrawDashboardDoanhThu(DoanhThuMap *map, FormDoanhThu *form, Font font, int *currentState) {
    float sw = (float)GetScreenWidth();
    float sh = (float)GetScreenHeight();

    // 1. NỀN & NÚT QUAY LẠI TỰ CHỦ
    ClearBackground(GetColor(0xE8F4F8FF)); // Xanh lơ nhạt toàn màn hình

    Rectangle btnBack = { 20, 20, 130, 40 };
    bool isHoverBack = CheckCollisionPointRec(GetMousePosition(), btnBack);
    if (IsKeyPressed(KEY_ESCAPE) || (isHoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
        InitFormDoanhThu(form);
        *currentState = HOME; 
        return; 
    }
    
    Color btnColor = isHoverBack ? GetColor(0x507A9BFF) : WHITE;
    Color textColor = isHoverBack ? WHITE : GetColor(0x507A9BFF);
    DrawRectangleRounded(btnBack, 0.3f, 10, btnColor);
    DrawRectangleRoundedLines(btnBack, 0.3f, 10, GetColor(0x507A9BFF));
    DrawTextEx(font, "< Quay lai", (Vector2){btnBack.x + 15, btnBack.y + 10}, 20, 1, textColor);

    // 2. KHUNG DASHBOARD CHÍNH
    Rectangle board = { 50, 80, sw - 100, sh - 120 };
    DrawRectangleRounded(board, 0.05f, 10, WHITE);
    DrawRectangleRoundedLines(board, 0.05f, 10, GetColor(0x8AB6D6FF)); // Viền xanh pastel

    DrawTextEx(font, "BÁO CÁO DOANH THU THƯ VIỆN", (Vector2){board.x + 30, board.y + 20}, 32, 1, GetColor(0x3B5998FF));

    // 3. MENU TABS (Ngày/Tuần/Tháng/Quý/Năm)
    const char *tabs[] = {"Theo Ngày", "Theo Tuần", "Theo Tháng", "Theo Quý", "Theo Năm"};
    float tabX = board.x + 30;
    float tabY = board.y + 70;
    
    for (int i = 0; i < 5; i++) {
        Rectangle tabRec = {tabX, tabY, 130, 40};
        bool isHover = CheckCollisionPointRec(GetMousePosition(), tabRec);
        bool isActive = (form->currentView == i);

        if (isActive) DrawRectangleRounded(tabRec, 0.2f, 10, GetColor(0x8AB6D6FF)); 
        else if (isHover) DrawRectangleRounded(tabRec, 0.2f, 10, GetColor(0xD9EBF9FF)); 
        else DrawRectangleRounded(tabRec, 0.2f, 10, GetColor(0xF0F4F8FF)); 

        Color txtCol = isActive ? WHITE : GetColor(0x507A9BFF);
        float txtW = MeasureTextEx(font, tabs[i], 18, 1).x;
        DrawTextEx(font, tabs[i], (Vector2){tabX + (130 - txtW)/2, tabY + 11}, 18, 1, txtCol);

        if (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            form->currentView = i;
            form->scroll = 0; // Reset scroll khi chuyển tab
        }
        tabX += 140;
    }

    // 4. Ô NHẬP LỌC THEO NĂM (Góc phải)
    DrawTextEx(font, "Lọc Năm:", (Vector2){board.x + board.width - 230, tabY + 11}, 20, 1, GetColor(0x507A9BFF));
    form->nhapNam.rec = (Rectangle){ board.x + board.width - 140, tabY, 100, 40 };
    
    if (CheckCollisionPointRec(GetMousePosition(), form->nhapNam.rec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        form->nhapNam.isFocused = true;
    } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        form->nhapNam.isFocused = false;
    }

    // Xử lý gõ phím cho ô Nhập Năm (Chỉ cho nhập số)
    if (form->nhapNam.isFocused) {
        int key = GetKeyPressed();
        while (key > 0) {
            if (key >= '0' && key <= '9' && form->nhapNam.letterCount < 4) {
                form->nhapNam.text[form->nhapNam.letterCount++] = (char)key;
                form->nhapNam.text[form->nhapNam.letterCount] = '\0';
                form->scroll = 0;
            } else if (key == KEY_BACKSPACE && form->nhapNam.letterCount > 0) {
                form->nhapNam.text[--form->nhapNam.letterCount] = '\0';
                form->scroll = 0;
            }
            key = GetKeyPressed();
        }
    }

    DrawRectangleRounded(form->nhapNam.rec, 0.2f, 10, WHITE);
    DrawRectangleRoundedLines(form->nhapNam.rec, 0.2f, 10, form->nhapNam.isFocused ? GetColor(0x8AB6D6FF) : LIGHTGRAY);
    DrawTextEx(font, form->nhapNam.text, (Vector2){form->nhapNam.rec.x + 15, form->nhapNam.rec.y + 10}, 20, 1, GetColor(0x3B5998FF));

    if (form->nhapNam.letterCount == 0 && !form->nhapNam.isFocused) {
        DrawTextEx(font, "Tất cả", (Vector2){form->nhapNam.rec.x + 15, form->nhapNam.rec.y + 10}, 20, 1, LIGHTGRAY);
    }

    // 5. HEADER BẢNG DỮ LIỆU
    float listY = tabY + 60;
    DrawRectangle(board.x + 2, listY, board.width - 4, 40, GetColor(0xE4F1F9FF));
    DrawTextEx(font, "Thời gian", (Vector2){board.x + 50, listY + 10}, 20, 1, GetColor(0x3B5998FF));
    DrawTextEx(font, "Doanh Thu (VNĐ)", (Vector2){board.x + board.width - 250, listY + 10}, 20, 1, GetColor(0x3B5998FF));
    
    listY += 40;

    // 6. THUẬT TOÁN GỘP DỮ LIỆU (AGGREGATION)
    AggItem items[1000];
    int itemCount = 0;
    double tongTatCa = 0;
    
    int filterYear = (form->nhapNam.letterCount > 0) ? atoi(form->nhapNam.text) : 0;

    for (int i = 0; i < DOANHTHU_HASH_SIZE; i++) {
        DoanhThuNode *curr = map->mang[i];
        while (curr != NULL) {
            int d, m, y;
            if (sscanf(curr->ngay, "%d/%d/%d", &d, &m, &y) == 3) {
                // Áp dụng bộ lọc năm
                if (filterYear > 0 && y != filterYear) {
                    curr = curr->next;
                    continue;
                }

                char label[50];
                int sortKey = 0;

                if (form->currentView == VIEW_NGAY) {
                    sprintf(label, "%02d/%02d/%d", d, m, y);
                    sortKey = y * 10000 + m * 100 + d;
                }
                else if (form->currentView == VIEW_TUAN) {
                    int diff = KhoangCachNgay("01/01/2026", curr->ngay);
                    int week = (diff >= 0) ? (diff / 7) + 1 : (diff / 7) - 1; 
                    sprintf(label, "Tuần %d (Năm %d)", week, y);
                    sortKey = y * 100 + week;
                }
                else if (form->currentView == VIEW_THANG) {
                    sprintf(label, "Tháng %02d/%d", m, y);
                    sortKey = y * 100 + m;
                }
                else if (form->currentView == VIEW_QUY) {
                    int q = (m - 1) / 3 + 1;
                    sprintf(label, "Quý %d/%d", q, y);
                    sortKey = y * 10 + q;
                }
                else if (form->currentView == VIEW_NAM) {
                    sprintf(label, "Năm %d", y);
                    sortKey = y;
                }

                // Gộp doanh thu nếu label đã tồn tại
                bool found = false;
                for (int k = 0; k < itemCount; k++) {
                    if (strcmp(items[k].label, label) == 0) {
                        items[k].amount += curr->tongTien;
                        found = true;
                        break;
                    }
                }
                // Thêm label mới
                if (!found && itemCount < 1000) {
                    strcpy(items[itemCount].label, label);
                    items[itemCount].amount = curr->tongTien;
                    items[itemCount].sortKey = sortKey;
                    itemCount++;
                }
                tongTatCa += curr->tongTien;
            }
            curr = curr->next;
        }
    }

    // Sắp xếp dữ liệu giảm dần (Mới nhất lên đầu)
    for (int i = 0; i < itemCount - 1; i++) {
        for (int j = i + 1; j < itemCount; j++) {
            if (items[i].sortKey < items[j].sortKey) {
                AggItem temp = items[i];
                items[i] = items[j];
                items[j] = temp;
            }
        }
    }

    // 7. XỬ LÝ CUỘN (SCROLL)
    float rowHeight = 45.0f;
    float contentH = board.height - (listY - board.y) - 60; // Trừ hao Header và Footer
    float totalH = itemCount * rowHeight;
    float maxScroll = 0;
    float minScroll = (totalH > contentH) ? (contentH - totalH) : 0;

    form->scroll += GetMouseWheelMove() * 30.0f;
    if (form->scroll > maxScroll) form->scroll = maxScroll;
    if (form->scroll < minScroll) form->scroll = minScroll;

    // 8. VẼ DANH SÁCH BẰNG SCISSOR
    BeginScissorMode((int)board.x, (int)listY, (int)board.width, (int)contentH);
    float startY = listY + form->scroll;
    
    for (int i = 0; i < itemCount; i++) {
        if (startY > listY - rowHeight && startY < listY + contentH) {
            // Xen kẽ màu dòng
            if (i % 2 == 1) DrawRectangle(board.x + 2, startY, board.width - 4, rowHeight, GetColor(0xF8FBFDFF));
            
            DrawTextEx(font, items[i].label, (Vector2){board.x + 50, startY + 12}, 20, 1, DARKGRAY);
            DrawTextEx(font, TextFormat("%.0f VNĐ", items[i].amount), (Vector2){board.x + board.width - 250, startY + 12}, 22, 1, GetColor(0x27AE60FF)); // Màu xanh lá nhẹ
        } 
        startY += rowHeight;
    }
    
    if (itemCount == 0) {
        DrawTextEx(font, "Không có dữ liệu doanh thu!", (Vector2){board.x + board.width/2 - 150, listY + 50}, 24, 1, GRAY);
    }
    EndScissorMode();

    // 9. FOOTER TỔNG KẾT
    float footerY = board.y + board.height - 60;
    DrawRectangle(board.x + 2, footerY, board.width - 4, 58, GetColor(0xE4F1F9FF));
    DrawLine(board.x, footerY, board.x + board.width, footerY, GetColor(0x8AB6D6FF));
    
    DrawTextEx(font, "TỔNG CỘNG:", (Vector2){board.x + 50, footerY + 15}, 26, 1, GetColor(0x3B5998FF));
    DrawTextEx(font, TextFormat("%.0f VNĐ", tongTatCa), (Vector2){board.x + board.width - 300, footerY + 15}, 28, 1, RED);
}