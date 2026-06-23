#include "Doanhthu.h"
#include "menu.h" 
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

// Hàm hỗ trợ tính ngày đầu tuần từ số thứ tự tuần trong năm
void GetWeekRange(int year, int week, char *outBuffer) {
    struct tm tm = {0};
    tm.tm_year = year - 1900;
    tm.tm_mon = 0;
    tm.tm_mday = 1;
    mktime(&tm);
    
    int dayOfYear = (week - 1) * 7;
    tm.tm_mday += dayOfYear;
    mktime(&tm);
    
    int startD = tm.tm_mday, startM = tm.tm_mon + 1;
    
    tm.tm_mday += 6;
    mktime(&tm);
    int endD = tm.tm_mday, endM = tm.tm_mon + 1;
    
    sprintf(outBuffer, "%02d/%02d - %02d/%02d", startD, startM, endD, endM);
}

static int LayNgayTrongNam(int d, int m, int y) {
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) daysInMonth[2] = 29;
    int day = 0;
    for (int i = 1; i < m; i++) day += daysInMonth[i];
    day += d;
    return day;
}

int laygiathue(const char *masachcantim) {
    char maBook[20] = {0};
    int i = 0;
    while (masachcantim[i] && masachcantim[i] != 'C' && i < 19) {
        maBook[i] = masachcantim[i];
        i++;
    }
    maBook[i] = '\0';

    FILE *f = fopen("data/ManageBooks/dataTruyen.txt", "r"); 
    if (f == NULL) return 2000;

    char line[1024]; 
    int lineCount = 0;

    while (fgets(line, sizeof(line), f)) {
        lineCount++;
        if (lineCount <= 1) continue;

        char ma[20];
        if (sscanf(line, " | %[^|]", ma) == 1) {
            int len = strlen(ma);
            while (len > 0 && ma[len-1] == ' ') ma[--len] = '\0';

            if (strcmp(ma, maBook) == 0) {  
                int giathue;
                sscanf(line, " | %*[^|] | %*[^|] | %*[^|] | %*[^|] | %*[^|] | %*[^|] | %*[^|] | %d |", &giathue);
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

double TinhTienThue(int giaMotNgay, const char *ngayMuon, const char *soNgayMuonStr, const char *ngayTraThucTe) {
    int soNgayGoc = atoi(soNgayMuonStr);
    if (soNgayGoc <= 0) soNgayGoc = 1;
    
    double tienGoc = (double)giaMotNgay * soNgayGoc;
    
    int soNgayThucTe = KhoangCachNgay(ngayMuon, ngayTraThucTe);
    if (soNgayThucTe < 0) return -1.0;
    
    int lechNgay = soNgayThucTe - soNgayGoc;
    
    if (lechNgay == 0) {
        return tienGoc;
    } else if (lechNgay < 0) {
        double giamGia = (double)giaMotNgay * (-lechNgay) * 0.1;
        return tienGoc - giamGia;
    } else {
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

    // Ô lọc năm (giữ nguyên)
    memset(form->nhapNam.text, 0, 256);
    form->nhapNam.letterCount = 0;
    form->nhapNam.isFocused = false;

    // Ô lọc khoảng ngày (mới)
    memset(form->nhapTuNgay.text, 0, 256);
    form->nhapTuNgay.letterCount = 0;
    form->nhapTuNgay.isFocused = false;

    memset(form->nhapDenNgay.text, 0, 256);
    form->nhapDenNgay.letterCount = 0;
    form->nhapDenNgay.isFocused = false;

    form->dateRangeActive = false;
}

// =========================================================
// HÀM PHỤ TRỢ NHẬP NGÀY (chỉ số, tự chèn '/', max 10 ký tự)
// =========================================================
// HÀM PHỤ TRỢ NHẬP NGÀY (chỉ số, tự chèn '/', max 10 ký tự)
// Trả về true nếu user bấm ký tự không phải số
// =========================================================
static bool NhapNgayDoanhThu(InputBox_BD *box) {
    bool hasInvalid = false;

    // --- Xóa ký tự ---
    int key = GetKeyPressed();
    while (key > 0) {
        if (key == KEY_BACKSPACE && box->letterCount > 0) {
            if (box->text[box->letterCount - 1] == '/') {
                box->letterCount--;
                box->text[box->letterCount] = '\0';
            }
            if (box->letterCount > 0) {
                box->letterCount--;
                box->text[box->letterCount] = '\0';
            }
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
        }
    } else {
        box->backspaceCounter = 0;
    }

    // --- Nhập ký tự: chỉ nhận số, báo lỗi nếu không phải ---
    int charcode = GetCharPressed();
    while (charcode > 0) {
        if (charcode >= '0' && charcode <= '9') {
            if (box->letterCount < 10) {
                int pos = box->letterCount;
                if (pos == 2 || pos == 5) {
                    box->text[box->letterCount++] = '/';
                    box->text[box->letterCount]   = '\0';
                }
                if (box->letterCount < 10) {
                    box->text[box->letterCount++] = (char)charcode;
                    box->text[box->letterCount]   = '\0';
                }
            }
        } else {
            hasInvalid = true;
        }
        charcode = GetCharPressed();
    }

    return hasInvalid;
}

// Kiểm tra ngày dd/mm/yyyy hợp lệ
static bool KiemTraNgayHopLe_DT(const char *ngay) {
    if (!ngay || strlen(ngay) < 10) return false;
    int d = 0, m = 0, y = 0;
    if (sscanf(ngay, "%d/%d/%d", &d, &m, &y) != 3) return false;
    if (y < 2000 || y > 2100) return false;
    if (m < 1 || m > 12) return false;
    int maxDay[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) maxDay[2] = 29;
    if (d < 1 || d > maxDay[m]) return false;
    return true;
}

// So sánh 2 ngày dd/mm/yyyy: <0 nếu a trước b, 0 nếu bằng, >0 nếu a sau b
static int SoSanhNgay_DT(const char *a, const char *b) {
    int da,ma,ya, db,mb,yb;
    if (sscanf(a, "%d/%d/%d", &da, &ma, &ya) != 3) return 0;
    if (sscanf(b, "%d/%d/%d", &db, &mb, &yb) != 3) return 0;
    if (ya != yb) return ya - yb;
    if (ma != mb) return ma - mb;
    return da - db;
}

// Kiểm tra ngày `ngay` có nằm trong [tuNgay, denNgay] không
static bool TrongKhoangNgay(const char *ngay, const char *tuNgay, const char *denNgay) {
    return (SoSanhNgay_DT(ngay, tuNgay) >= 0) && (SoSanhNgay_DT(ngay, denNgay) <= 0);
}

// Vẽ 1 ô nhập ngày với label phía trên
static void VeInputNgay(InputBox_BD *box, Font font, const char *label, bool hasError) {
    // Label
    DrawTextEx(font, label, (Vector2){ box->rec.x, box->rec.y - 22 }, 16, 1, GetColor(0x507A9BFF));

    // Nền ô
    Color borderCol;
    if (hasError)                    borderCol = GetColor(0xE74C3CFF); // Đỏ = lỗi
    else if (box->isFocused)         borderCol = GetColor(0x8AB6D6FF); // Xanh = focus
    else if (box->letterCount == 10) borderCol = GetColor(0x27AE60FF); // Xanh lá = hợp lệ
    else                             borderCol = LIGHTGRAY;

    DrawRectangleRounded(box->rec, 0.2f, 10, WHITE);
    DrawRectangleRoundedLines(box->rec, 0.2f, 10, borderCol);

    // Placeholder hoặc nội dung
    if (box->letterCount == 0 && !box->isFocused) {
        DrawTextEx(font, "dd/mm/yyyy", (Vector2){ box->rec.x + 10, box->rec.y + 10 }, 18, 1, LIGHTGRAY);
    } else {
        DrawTextEx(font, box->text, (Vector2){ box->rec.x + 10, box->rec.y + 10 }, 18, 1, GetColor(0x2C3E50FF));
    }

    // Con trỏ nhấp nháy
    if (box->isFocused && (((int)(GetTime() * 1.5f)) % 2 == 0)) {
        float tw = MeasureTextEx(font, box->text, 18, 1).x;
        DrawRectangleV(
            (Vector2){ box->rec.x + 10 + tw + 2, box->rec.y + 8 },
            (Vector2){ 2, 20 },
            GetColor(0x2C5F8AFF)
        );
    }
}

// =========================================================
// PHẦN 3: VẼ GIAO DIỆN UI DASHBOARD DOANH THU & BIỂU ĐỒ
// =========================================================
void DrawDashboardDoanhThu(DoanhThuMap *map, FormDoanhThu *form, Font font, int *currentState) {
    float sw = (float)GetScreenWidth();
    float sh = (float)GetScreenHeight();

    ClearBackground(GetColor(0xE8F4F8FF)); 

    // --- Nút Quay lại ---
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

    Rectangle board = { 50, 80, sw - 100, sh - 120 };
    DrawRectangleRounded(board, 0.05f, 10, WHITE);
    DrawRectangleRoundedLines(board, 0.05f, 10, GetColor(0x8AB6D6FF)); 

    DrawTextEx(font, "BÁO CÁO DOANH THU THƯ VIỆN", (Vector2){board.x + 30, board.y + 20}, 32, 1, GetColor(0x3B5998FF));

    // --- 5 Tab xem ---
    const char *tabs[] = {"Theo Ngày", "Theo Tuần", "Theo Tháng", "Theo Quý", "Theo Năm"};
    float tabX = board.x + 30;
    float tabY = board.y + 70;
    
    for (int i = 0; i < 5; i++) {
        Rectangle tabRec = {tabX, tabY, 130, 40};
        bool isHover = CheckCollisionPointRec(GetMousePosition(), tabRec);
        bool isActive = (form->currentView == i);

        if (isActive)       DrawRectangleRounded(tabRec, 0.2f, 10, GetColor(0x8AB6D6FF));
        else if (isHover)   DrawRectangleRounded(tabRec, 0.2f, 10, GetColor(0xD9EBF9FF));
        else                DrawRectangleRounded(tabRec, 0.2f, 10, GetColor(0xF0F4F8FF));

        Color txtCol = isActive ? WHITE : GetColor(0x507A9BFF);
        float txtW = MeasureTextEx(font, tabs[i], 18, 1).x;
        DrawTextEx(font, tabs[i], (Vector2){tabX + (130 - txtW)/2, tabY + 11}, 18, 1, txtCol);

        if (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            form->currentView = i;
            form->scroll = 0; 
        }
        tabX += 140;
    }

    // =========================================================
    // PHẦN LỌC NĂM — neo sát phải board
    // =========================================================
    // Ô năm: label "Nam:" + ô 80px, cách mép phải 20px
    float namBoxW    = 80.0f;
    float namLabelW  = MeasureTextEx(font, "Năm:", 18, 1).x;
    float namGap     = 8.0f;
    float namRightPad = 20.0f;
    form->nhapNam.rec = (Rectangle){ board.x + board.width - namRightPad - namBoxW, tabY, namBoxW, 40 };
    float namLabelX   = form->nhapNam.rec.x - namGap - namLabelW;
    DrawTextEx(font, "Năm:", (Vector2){ namLabelX, tabY + 11 }, 18, 1, GetColor(0x507A9BFF));

    bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    if (CheckCollisionPointRec(GetMousePosition(), form->nhapNam.rec) && mousePressed) {
        form->nhapNam.isFocused = true;
        form->nhapTuNgay.isFocused = false;
        form->nhapDenNgay.isFocused = false;
    } else if (mousePressed && !CheckCollisionPointRec(GetMousePosition(), form->nhapNam.rec)) {
        form->nhapNam.isFocused = false;
    }

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
    DrawTextEx(font, form->nhapNam.text, (Vector2){form->nhapNam.rec.x + 10, form->nhapNam.rec.y + 10}, 18, 1, GetColor(0x3B5998FF));
    if (form->nhapNam.letterCount == 0 && !form->nhapNam.isFocused) {
        DrawTextEx(font, "Tất cả", (Vector2){form->nhapNam.rec.x + 6, form->nhapNam.rec.y + 10}, 16, 1, LIGHTGRAY);
    }

    // =========================================================
    // PHẦN LỌC KHOẢNG NGÀY: chỉ hiện khi tab VIEW_NGAY
    // Cùng hàng tabY, bên trái ô Năm, cách ô Năm 40px
    // =========================================================
    float filterRowY = tabY;   // cùng hàng với tabs và ô Năm

    float boxW      = 160.0f;  // ô to hơn so với trước
    float boxH      = 40.0f;   // khớp chiều cao ô Năm
    float arrowW    = 36.0f;
    float gapBtn    = 16.0f;
    float btnClearW = 85.0f;
    float gapNam    = 40.0f;   // khoảng cách từ cụm filter tới label "Nam:"

    // Tổng cụm filter: boxW + arrowW + boxW (không kèm nút Xóa lọc vào tổng neo)
    float totalFilterW = boxW + arrowW + boxW;
    // Bắt đầu cụm filter = bên trái label Năm - gap - tổng cụm - (nút xóa nếu có)
    float filterStartX = namLabelX - gapNam - btnClearW - gapBtn - totalFilterW;

    form->nhapTuNgay.rec  = (Rectangle){ filterStartX,                 filterRowY, boxW, boxH };
    form->nhapDenNgay.rec = (Rectangle){ filterStartX + boxW + arrowW, filterRowY, boxW, boxH };
    Rectangle btnClear    = { filterStartX + totalFilterW + gapBtn,    filterRowY, btnClearW, boxH };

    // Track nếu user bấm ký tự không phải số (để hiện thông báo)
    static bool showCharWarn = false;
    static float charWarnTimer = 0.0f;

    bool tuNgayOK  = false;
    bool denNgayOK = false;
    bool rangeError = false;

    if (form->currentView == VIEW_NGAY) {
        // Xử lý focus khi click
        if (mousePressed) {
            if (CheckCollisionPointRec(GetMousePosition(), form->nhapTuNgay.rec)) {
                form->nhapTuNgay.isFocused  = true;
                form->nhapDenNgay.isFocused = false;
                form->nhapNam.isFocused     = false;
            } else if (CheckCollisionPointRec(GetMousePosition(), form->nhapDenNgay.rec)) {
                form->nhapDenNgay.isFocused = true;
                form->nhapTuNgay.isFocused  = false;
                form->nhapNam.isFocused     = false;
            }
        }

        // Nhập liệu + detect ký tự lạ từ return value
        if (form->nhapTuNgay.isFocused) {
            if (NhapNgayDoanhThu(&form->nhapTuNgay)) {
                showCharWarn  = true;
                charWarnTimer = 2.0f;
            }
        }
        if (form->nhapDenNgay.isFocused) {
            if (NhapNgayDoanhThu(&form->nhapDenNgay)) {
                showCharWarn  = true;
                charWarnTimer = 2.0f;
            }
        }

        // Đếm ngược timer cảnh báo
        if (charWarnTimer > 0.0f) {
            charWarnTimer -= GetFrameTime();
            if (charWarnTimer <= 0.0f) showCharWarn = false;
        }

        // Validate
        tuNgayOK  = KiemTraNgayHopLe_DT(form->nhapTuNgay.text);
        denNgayOK = KiemTraNgayHopLe_DT(form->nhapDenNgay.text);
        rangeError = (tuNgayOK && denNgayOK &&
                      SoSanhNgay_DT(form->nhapTuNgay.text, form->nhapDenNgay.text) > 0);

        // Vẽ 2 ô nhập ngày
        VeInputNgay(&form->nhapTuNgay,  font, "Từ ngày:",
            (!tuNgayOK  && form->nhapTuNgay.letterCount  == 10) || rangeError);
        VeInputNgay(&form->nhapDenNgay, font, "Đến ngày:",
            (!denNgayOK && form->nhapDenNgay.letterCount == 10) || rangeError);

        // Dấu "->" ở giữa 2 ô (ASCII thuần, không lỗi font)
        float arrowX = filterStartX + boxW + (arrowW - MeasureTextEx(font, "->", 20, 1).x) / 2.0f;
        DrawTextEx(font, "->",
            (Vector2){ arrowX, filterRowY + (boxH - 20) / 2.0f },
            20, 1, GetColor(0x8AB6D6FF));

        // Nút "Xoa loc"
        bool hasDateInput = (form->nhapTuNgay.letterCount > 0 || form->nhapDenNgay.letterCount > 0);
        if (hasDateInput) {
            bool clearHov = CheckCollisionPointRec(GetMousePosition(), btnClear);
            DrawRectangleRounded(btnClear, 0.25f, 8,
                clearHov ? GetColor(0xE74C3CFF) : GetColor(0xFFCDD2FF));
            DrawTextEx(font, "Xóa lọc",
                (Vector2){ btnClear.x + 10, btnClear.y + 10 },
                16, 1, clearHov ? WHITE : GetColor(0xC0392BFF));

            if (clearHov && mousePressed) {
                memset(form->nhapTuNgay.text,  0, 256); form->nhapTuNgay.letterCount  = 0;
                memset(form->nhapDenNgay.text, 0, 256); form->nhapDenNgay.letterCount = 0;
                form->nhapTuNgay.isFocused  = false;
                form->nhapDenNgay.isFocused = false;
                form->scroll = 0;
                showCharWarn = false;
            }
        }

        // Thông báo lỗi khoảng ngày — căn giữa cụm filter
        if (rangeError) {
            const char *errMsg = "! Ngày bắt đầu phải trước ngày kết thúc";
            float errW = MeasureTextEx(font, errMsg, 14, 1).x;
            DrawTextEx(font, errMsg,
                (Vector2){ filterStartX + (totalFilterW - errW) / 2.0f, filterRowY + boxH + 5 },
                14, 1, GetColor(0xE74C3CFF));
        }

        // Thông báo ký tự không hợp lệ — hiện dưới cụm, fade out theo timer
        if (showCharWarn) {
            const char *warnMsg = "! Chỉ được nhập số (0-9)";
            float warnW = MeasureTextEx(font, warnMsg, 14, 1).x;
            // Alpha fade: 2s -> 0
            unsigned char alpha = (unsigned char)(255 * (charWarnTimer / 2.0f));
            Color warnCol = { 231, 76, 60, alpha };
            DrawTextEx(font, warnMsg,
                (Vector2){ filterStartX + (totalFilterW - warnW) / 2.0f,
                           filterRowY + boxH + (rangeError ? 22 : 5) },
                14, 1, warnCol);
        }
    } else {
        // Không phải VIEW_NGAY: reset focus và dateRangeActive
        form->nhapTuNgay.isFocused  = false;
        form->nhapDenNgay.isFocused = false;
        showCharWarn = false;
    }

    // Xác định dateRangeActive (chỉ có hiệu lực ở VIEW_NGAY)
    form->dateRangeActive = (form->currentView == VIEW_NGAY &&
                             tuNgayOK && denNgayOK && !rangeError);

    // =========================================================
    // TỔNG HỢP DỮ LIỆU (có lọc khoảng ngày nếu active)
    // =========================================================
    AggItem items[1000];
    int itemCount = 0;
    double tongTatCa = 0;

    int filterYear = (form->nhapNam.letterCount == 4) ? atoi(form->nhapNam.text) : 0;

    // Khởi tạo sẵn các bucket cố định nếu lọc theo năm (giữ nguyên logic cũ)
    if (filterYear > 0 && !form->dateRangeActive) {
        if (form->currentView == VIEW_THANG) {
            for (int i = 1; i <= 12; i++) {
                sprintf(items[itemCount].label, "Tháng %02d/%d", i, filterYear);
                items[itemCount].amount = 0;
                items[itemCount].sortKey = filterYear * 100 + i;
                itemCount++;
            }
        } else if (form->currentView == VIEW_QUY) {
            for (int i = 1; i <= 4; i++) {
                sprintf(items[itemCount].label, "Quý %d/%d", i, filterYear);
                items[itemCount].amount = 0;
                items[itemCount].sortKey = filterYear * 10 + i;
                itemCount++;
            }
        } else if (form->currentView == VIEW_TUAN) {
            for (int i = 1; i <= 52; i++) {
                char range[30];
                GetWeekRange(filterYear, i, range);
                sprintf(items[itemCount].label, "Tuần %d (%s)", i, range);
                items[itemCount].amount = 0;
                items[itemCount].sortKey = filterYear * 100 + i;
                itemCount++;
            }
        }
    }

    // Duyệt hashmap tổng hợp dữ liệu
    for (int i = 0; i < DOANHTHU_HASH_SIZE; i++) {
        DoanhThuNode *curr = map->mang[i];
        while (curr != NULL) {
            int d, m, y;
            if (sscanf(curr->ngay, "%d/%d/%d", &d, &m, &y) == 3) {

                // ---- LỌC KHOẢNG NGÀY (ưu tiên hơn lọc năm) ----
                if (form->dateRangeActive) {
                    if (!TrongKhoangNgay(curr->ngay, form->nhapTuNgay.text, form->nhapDenNgay.text)) {
                        curr = curr->next;
                        continue;
                    }
                } else {
                    // Lọc năm như cũ
                    if (filterYear > 0 && y != filterYear) {
                        curr = curr->next;
                        continue;
                    }
                }

                char label[50];
                int sortKey = 0;

                if (form->currentView == VIEW_NGAY) {
                    sprintf(label, "%02d/%02d/%d", d, m, y);
                    sortKey = y * 10000 + m * 100 + d;
                } else if (form->currentView == VIEW_TUAN) {
                    int week = (LayNgayTrongNam(d, m, y) - 1) / 7 + 1;
                    if (week > 52) week = 52;
                    char range[30];
                    GetWeekRange(y, week, range);
                    sprintf(label, "Tuần %d (%s)", week, range);
                    sortKey = y * 100 + week;
                } else if (form->currentView == VIEW_THANG) {
                    sprintf(label, "Tháng %02d/%d", m, y);
                    sortKey = y * 100 + m;
                } else if (form->currentView == VIEW_QUY) {
                    int q = (m - 1) / 3 + 1;
                    sprintf(label, "Quý %d/%d", q, y);
                    sortKey = y * 10 + q;
                } else if (form->currentView == VIEW_NAM) {
                    sprintf(label, "Năm %d", y);
                    sortKey = y;
                }

                bool found = false;
                for (int k = 0; k < itemCount; k++) {
                    if (strcmp(items[k].label, label) == 0) {
                        items[k].amount += curr->tongTien;
                        found = true;
                        break;
                    }
                }
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

    // Bubble sort theo sortKey
    for (int i = 0; i < itemCount - 1; i++) {
        for (int j = i + 1; j < itemCount; j++) {
            if (items[i].sortKey > items[j].sortKey) {
                AggItem temp = items[i];
                items[i] = items[j];
                items[j] = temp;
            }
        }
    }

    // =========================================================
    // VẼ BIỂU ĐỒ
    // VIEW_NGAY: filter cùng hàng tabY, label "Tu/Den ngay" ở trên nên
    //            chart bắt đầu từ tabY + boxH + 22(label) + padding + lỗi nếu có
    // Tab khác:  bắt đầu sát dưới hàng tabs như cũ
    // =========================================================
    float chartTop;
    if (form->currentView == VIEW_NGAY) {
        // 22 = chiều cao label phía trên ô; 10 = padding
        chartTop = tabY + 22 + boxH + 10 + (rangeError ? 22 : 0);
    } else {
        chartTop = tabY + boxH + 10;
    }
    float chartX = board.x + 80;
    float chartY = chartTop;
    float chartW = board.width - 120;
    float chartH = 185.0f;

    // Nhãn khoảng ngày đang lọc (hiện ở góc phải trong khung biểu đồ)
    if (form->dateRangeActive) {
        char filterLabel[64];
        sprintf(filterLabel, "Lọc: %s → %s", form->nhapTuNgay.text, form->nhapDenNgay.text);
        float flW = MeasureTextEx(font, filterLabel, 15, 1).x;
        DrawTextEx(font, filterLabel,
            (Vector2){ chartX + chartW - flW - 5, chartY - 20 },
            15, 1, GetColor(0x27AE60FF));
    }

    DrawRectangle(chartX, chartY, chartW, chartH, GetColor(0xF8FBFDFF));
    DrawLine(chartX, chartY + chartH, chartX + chartW, chartY + chartH, GetColor(0x8AB6D6FF));
    DrawLine(chartX, chartY, chartX, chartY + chartH, GetColor(0x8AB6D6FF));

    double maxAmount = 0;
    for (int i = 0; i < itemCount; i++) {
        if (items[i].amount > maxAmount) maxAmount = items[i].amount;
    }

    if (maxAmount == 0) {
        maxAmount = 1000;
    } else {
        double step = maxAmount / 4.0;
        double mag  = pow(10.0, floor(log10(step)));
        double lead = step / mag;
        if      (lead <= 1.0) lead = 1.0;
        else if (lead <= 2.0) lead = 2.0;
        else if (lead <= 2.5) lead = 2.5;
        else if (lead <= 4.0) lead = 4.0;
        else if (lead <= 5.0) lead = 5.0;
        else lead = 10.0;
        maxAmount = lead * mag * 4.0;
    }

    for (int i = 0; i <= 4; i++) {
        float yPos = chartY + chartH - (i * chartH / 4.0f);
        if (i > 0) DrawLine(chartX, yPos, chartX + chartW, yPos, Fade(GetColor(0x8AB6D6FF), 0.3f));
        char labelY[32];
        sprintf(labelY, "%.0f", maxAmount * i / 4.0f);
        float lblW = MeasureTextEx(font, labelY, 14, 1).x;
        DrawTextEx(font, labelY, (Vector2){chartX - lblW - 10, yPos - 8}, 14, 1, GetColor(0x507A9BFF));
    }

    char tooltipText[128] = "";
    Vector2 tooltipPos = {0};
    bool showTooltip = false;

    if (itemCount > 0) {
        float barSpacing = chartW / itemCount;
        float barW = barSpacing * 0.70f;
        if (barW > 50.0f) barW = 50.0f;

        for (int i = 0; i < itemCount; i++) {
            float barH = (float)((items[i].amount / maxAmount) * chartH);
            float barX = chartX + i * barSpacing + (barSpacing - barW) / 2.0f;
            float barY = chartY + chartH - barH;

            Rectangle barRec = { barX, barY, barW, barH };
            bool isHover = CheckCollisionPointRec(GetMousePosition(), (Rectangle){barX, chartY, barW, chartH});

            Color barColor = isHover ? GetColor(0x27AE60FF) : GetColor(0x5A9FD4FF);

            if (barH > 5.0f) DrawRectangleRounded(barRec, 0.30f, 8, barColor);
            else             DrawRectangleRec(barRec, barColor);

            // Label dưới cột
            char shortLabel[20] = "";
            if      (form->currentView == VIEW_THANG) sprintf(shortLabel, "T%d",  items[i].sortKey % 100);
            else if (form->currentView == VIEW_QUY)   sprintf(shortLabel, "Q%d",  items[i].sortKey % 10);
            else if (form->currentView == VIEW_TUAN)  sprintf(shortLabel, "%d",   items[i].sortKey % 100);
            else if (form->currentView == VIEW_NAM)   sprintf(shortLabel, "%d",   items[i].sortKey);
            else { strncpy(shortLabel, items[i].label, 5); shortLabel[5] = '\0'; }

            if (itemCount <= 15 || i % (itemCount / 12 + 1) == 0) {
                float txtW = MeasureTextEx(font, shortLabel, 14, 1).x;
                DrawTextEx(font, shortLabel,
                    (Vector2){barX + barW/2 - txtW/2, chartY + chartH + 8},
                    14, 1, GetColor(0x507A9BFF));
            }

            if (isHover) {
                sprintf(tooltipText, "%s: %.0f VNĐ", items[i].label, items[i].amount);
                tooltipPos = GetMousePosition();
                showTooltip = true;
            }
        }
    }

    // =========================================================
    // DANH SÁCH BẢNG (giống cũ, chỉ dời Y xuống theo chartY mới)
    // =========================================================
    float listHeaderY = chartY + chartH + 35;
    DrawRectangle(board.x + 2, listHeaderY, board.width - 4, 40, GetColor(0xE4F1F9FF));
    DrawTextEx(font, "Thời gian",        (Vector2){board.x + 50, listHeaderY + 10}, 20, 1, GetColor(0x3B5998FF));
    DrawTextEx(font, "Doanh Thu (VNĐ)",  (Vector2){board.x + board.width - 250, listHeaderY + 10}, 20, 1, GetColor(0x3B5998FF));

    float listY     = listHeaderY + 40;
    float rowHeight = 45.0f;
    float footerY   = board.y + board.height - 60;
    float contentH  = footerY - listY;
    float totalH    = itemCount * rowHeight;
    float maxScroll = 0;
    float minScroll = (totalH > contentH) ? (contentH - totalH) : 0;

    form->scroll += GetMouseWheelMove() * 30.0f;
    if (form->scroll > maxScroll) form->scroll = maxScroll;
    if (form->scroll < minScroll) form->scroll = minScroll;

    BeginScissorMode((int)board.x, (int)listY, (int)board.width, (int)contentH);
    float startY = listY + form->scroll;

    int displayIndex = 0;
    for (int i = itemCount - 1; i >= 0; i--) {
        if (startY > listY - rowHeight && startY < listY + contentH) {
            if (displayIndex % 2 == 1)
                DrawRectangle(board.x + 2, startY, board.width - 4, rowHeight, GetColor(0xF8FBFDFF));
            DrawTextEx(font, items[i].label,
                (Vector2){board.x + 50, startY + 12}, 20, 1, DARKGRAY);
            DrawTextEx(font, TextFormat("%.0f VNĐ", items[i].amount),
                (Vector2){board.x + board.width - 250, startY + 12}, 22, 1, GetColor(0x27AE60FF));
        }
        startY += rowHeight;
        displayIndex++;
    }

    if (itemCount == 0) {
        DrawTextEx(font, "Không có dữ liệu trong khoảng thời gian này!",
            (Vector2){board.x + board.width/2 - 200, listY + 50}, 22, 1, GRAY);
    }
    EndScissorMode();

    // Scrollbar danh sách
    float sbW   = 8.0f;
    float trackX = board.x + board.width - 15;
    if (totalH > contentH) {
        float thumbH = contentH * (contentH / totalH);
        if (thumbH < 30.0f) thumbH = 30.0f;

        float scrollRange = totalH - contentH;
        float scrollRatio = -form->scroll / scrollRange;
        float thumbY = listY + scrollRatio * (contentH - thumbH);

        Rectangle trackRec = { trackX, listY, sbW, contentH };
        Rectangle thumbRec = { trackX, thumbY, sbW, thumbH };
        bool isThumbHover = CheckCollisionPointRec(GetMousePosition(), thumbRec);

        DrawRectangleRounded(trackRec, 0.5f, 4, Fade(LIGHTGRAY, 0.3f));

        static bool draggingSB = false;
        static float dragStartY = 0.0f;
        Color thumbColor = draggingSB ? GetColor(0x3B5998FF)
                         : (isThumbHover ? GetColor(0x8AB6D6FF) : GetColor(0x8AB6D6CC));
        DrawRectangleRounded(thumbRec, 0.5f, 4, thumbColor);

        if (draggingSB) {
            if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                draggingSB = false;
            } else {
                float dy = GetMousePosition().y - dragStartY;
                float ratio = (dy - listY) / (contentH - thumbH);
                if (ratio < 0) ratio = 0;
                if (ratio > 1) ratio = 1;
                form->scroll = -ratio * scrollRange;
            }
        } else {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && isThumbHover) {
                draggingSB = true;
                dragStartY = GetMousePosition().y - thumbY;
            }
        }
    }

    // Footer tổng cộng
    DrawRectangle(board.x + 2, footerY, board.width - 4, 58, GetColor(0xE4F1F9FF));
    DrawLine(board.x, footerY, board.x + board.width, footerY, GetColor(0x8AB6D6FF));
    DrawTextEx(font, "TỔNG CỘNG:", (Vector2){board.x + 50, footerY + 15}, 26, 1, GetColor(0x3B5998FF));
    DrawTextEx(font, TextFormat("%.0f VNĐ", tongTatCa),
        (Vector2){board.x + board.width - 300, footerY + 15}, 28, 1, RED);

    // Tooltip hover biểu đồ
    if (showTooltip) {
        float ttW = MeasureTextEx(font, tooltipText, 16, 1).x;
        DrawRectangleRounded(
            (Rectangle){tooltipPos.x + 15, tooltipPos.y - 30, ttW + 20, 30},
            0.2f, 5, Fade(GetColor(0x3B5998FF), 0.9f));
        DrawTextEx(font, tooltipText, (Vector2){tooltipPos.x + 25, tooltipPos.y - 23}, 16, 1, WHITE);
    }
}