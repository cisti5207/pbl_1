#include "libmanage.h"
#include "Phieumuon.h"
#include "menu.h"
#include <stdio.h>
#include "raylib.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

// ============================================================
// CẤU TRÚC NỘI BỘ
// ============================================================
#define MAX_TRUYEN_PM 200
#define MAX_TAP_PM    800

typedef struct {
    char ma[16];
    char ten[256];
} TruyenInfo_PM;

typedef struct {
    char tenTap[256];
    char maTap[16];
    char maTruyen[16];
    int  soLuong;
} TapInfo_PM;

static TruyenInfo_PM s_dsTruyen[MAX_TRUYEN_PM];
static int           s_soTruyen = 0;
static bool          s_loaded   = false;

static TapInfo_PM    s_dsTap[MAX_TAP_PM];
static int           s_soTap = 0;

// Dropdown state
static char s_filter[256]          = {0};
static int  s_matchIdx[MAX_TRUYEN_PM];
static int  s_matchCount           = 0;
static bool s_showDropdown         = false;
static bool s_showTapDropdown      = false;
static int  s_dropScroll           = 0;
static int  s_tapScroll            = 0;

// Mã truyện & mã tập đã chọn
static char s_selectedMaTruyen[16] = {0};
static char s_selectedMaTap[16]    = {0};

// Cờ báo lỗi số ngày mượn
static bool s_ngaytraLoi = false;
// Cờ báo lỗi hết sách
static bool s_hetSachLoi = false;
// Cờ báo lỗi mã thẻ không hợp lệ hoặc không tồn tại
static bool s_matheLoi = false;

// ============================================================
// Tiện ích
// ============================================================
static void TrimStr_PM(char *s) {
    if (!s) return;
    int len = (int)strlen(s);
    while (len > 0 && (s[len-1]==' '||s[len-1]=='\r'||s[len-1]=='\n')) s[--len]='\0';
    char *p = s;
    while (*p == ' ') p++;
    if (p != s) memmove(s, p, strlen(p)+1);
}

static bool ContainsCI(const char *hay, const char *needle) {
    if (!hay||!needle||needle[0]=='\0') return true;
    int nlen=(int)strlen(needle), hlen=(int)strlen(hay);
    for (int i=0; i<=hlen-nlen; i++) {
        int j;
        for (j=0; j<nlen; j++)
            if (tolower((unsigned char)hay[i+j])!=tolower((unsigned char)needle[j])) break;
        if (j==nlen) return true;
    }
    return false;
}

static bool KiemTraCoTap(const char *maTruyen) {
    if (!maTruyen || maTruyen[0] == '\0') return false;
    for (int i = 0; i < s_soTap; i++) {
        if (strcmp(s_dsTap[i].maTruyen, maTruyen) == 0) return true;
    }
    return false;
}

// --- Hàm kiểm tra mã thẻ tồn tại trong User.txt ---
static bool KiemTraTheTonTai(const char *mathe) {
    if (!mathe || mathe[0] == '\0') return false;

    // Trim bản sao input để so sánh chính xác
    char input[64] = {0};
    strncpy(input, mathe, 63);
    TrimStr_PM(input);
    if (input[0] == '\0') return false;

    FILE *f = fopen("data/Phieumuon/User.txt", "r");
    if (!f) return false;

    char line[512];
    bool first = true;
    while (fgets(line, sizeof(line), f)) {
        // Bỏ qua dòng tiêu đề đầu tiên
        if (first) { first = false; continue; }

        // Bỏ qua dòng trống
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0' || *p == '\r' || *p == '\n') continue;

        // Cột đầu tiên là mã thẻ, ngăn cách bởi '|'
        char mt[64] = {0};
        if (sscanf(p, " %63[^|]", mt) == 1) {
            TrimStr_PM(mt);
            if (strcmp(mt, input) == 0) {
                fclose(f);
                return true;
            }
        }
    }
    fclose(f);
    return false;
}

// ============================================================
// Load file (1 lần duy nhất)
// ============================================================
static void LoadDuLieuTruyen_PM(void) {
    if (s_loaded) return;
    s_loaded = true; s_soTruyen = 0; s_soTap = 0;

    FILE *f = fopen("data/ManageBooks/dataTruyen.txt", "r");
    if (f) {
        char line[1024];
        while (fgets(line, sizeof(line), f) && s_soTruyen < MAX_TRUYEN_PM) {
            char *p = line;
            while (*p==' '||*p=='\t') p++;
            if (*p != '|') continue;
            char ma[64]={0}, slug[256]={0}, ten[256]={0};
            if (sscanf(p," | %63[^|] | %255[^|] | %255[^|]", ma, slug, ten)==3) {
                TrimStr_PM(ma); TrimStr_PM(ten);
                strncpy(s_dsTruyen[s_soTruyen].ma,  ma,  15);
                strncpy(s_dsTruyen[s_soTruyen].ten, ten, 255);
                s_soTruyen++;
            }
        }
        fclose(f);
    }

    FILE *f2 = fopen("data/ManageBooks/dataListTruyen.txt", "r");
    if (f2) {
        char line[512];
        bool first = true;
        while (fgets(line, sizeof(line), f2) && s_soTap < MAX_TAP_PM) {
            if (first) { first=false; continue; }
            char tenTap[256]={0}, maTap[16]={0}, maTruyen[16]={0};
            int sl=0, slg=0;
            if (sscanf(line," %255[^|] | %15[^|] | %15[^|] | %d | %d",
                       tenTap, maTap, maTruyen, &sl, &slg) >= 3) {
                TrimStr_PM(tenTap); TrimStr_PM(maTap); TrimStr_PM(maTruyen);
                strncpy(s_dsTap[s_soTap].tenTap,   tenTap,   255);
                strncpy(s_dsTap[s_soTap].maTap,    maTap,    15);
                strncpy(s_dsTap[s_soTap].maTruyen, maTruyen, 15);
                s_dsTap[s_soTap].soLuong = sl;
                s_soTap++;
            }
        }
        fclose(f2);
    }
}

static void CapNhatMatch(void) {
    s_matchCount=0; s_dropScroll=0; s_tapScroll=0;
    for (int i=0; i<s_soTruyen && s_matchCount<MAX_TRUYEN_PM; i++)
        if (ContainsCI(s_dsTruyen[i].ten, s_filter))
            s_matchIdx[s_matchCount++] = i;
}

// ============================================================
// Load phiếu mượn từ file vào linked list
// ============================================================
void LoadPhieuMuonTuFile(PhieuMuonNode **head) {
    FILE *f = fopen("data/Phieumuon/LSPhieumuon.txt", "r");
    if (!f) return;

    char line[512];
    while (fgets(line, sizeof(line), f)) {
        char maPM[16]={0}, mathe[16]={0}, matruyen[64]={0};
        char tentruyen[256]={0}, ngaymuon[32]={0}, ngaytra[32]={0};
        int tt = 0;
        if (sscanf(line, " %15[^|] | %15[^|] | %63[^|] | %255[^|] | %31[^|] | %31[^|] | %d",
                   maPM, mathe, matruyen, tentruyen, ngaymuon, ngaytra, &tt) >= 6) {
            TrimStr_PM(maPM); TrimStr_PM(mathe);
            TrimStr_PM(matruyen); TrimStr_PM(tentruyen);
            TrimStr_PM(ngaymuon); TrimStr_PM(ngaytra);
            if (maPM[0] != 'P') continue;
            PhieuMuonNode *n = (PhieuMuonNode*)malloc(sizeof(PhieuMuonNode));
            if (n) {
                strcpy(n->maPM,      maPM);
                strcpy(n->mathe,     mathe);
                strcpy(n->matruyen,  matruyen);
                strcpy(n->tentruyen, tentruyen);
                strcpy(n->ngaymuon,  ngaymuon);
                strcpy(n->ngaytra,   ngaytra);
                n->trangthai = tt;
                n->next = *head;
                *head = n;
            }
        }
    }
    fclose(f);
}

// ============================================================
// Giảm số lượng tập trong dataListTruyen.txt khi mượn
// ============================================================
bool GiamSoLuongTap(const char *maTap, const char *maTruyen) {
    const char *filepath = "data/ManageBooks/dataListTruyen.txt";
    FILE *f = fopen(filepath, "r");
    if (!f) return true;

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = (char*)malloc(fsize + 32);
    if (!buf) { fclose(f); return true; }
    int nread = (int)fread(buf, 1, fsize, f);
    buf[nread] = '\0';
    fclose(f);

    char searchMaTap[16] = {0};
    if (maTap && maTap[0] != '\0') {
        strncpy(searchMaTap, maTap, 15);
        TrimStr_PM(searchMaTap);
    } else {
        strcpy(searchMaTap, "C000");
    }

    bool found = false;
    bool canBorrow = true;

    char *lineStart = buf;
    while (*lineStart) {
        char *eol = strchr(lineStart, '\n');
        int lineLen = eol ? (int)(eol - lineStart + 1) : (int)strlen(lineStart);

        char linecopy[512] = {0};
        int cpLen = lineLen < 511 ? lineLen : 511;
        memcpy(linecopy, lineStart, cpLen);
        int lc = (int)strlen(linecopy);
        while (lc > 0 && (linecopy[lc-1]=='\r'||linecopy[lc-1]=='\n')) linecopy[--lc]='\0';

        char tenTap[256]={0}, mt[16]={0}, mtr[16]={0};
        int sl=0, slg=0;
        int parsed = sscanf(linecopy, " %255[^|] | %15[^|] | %15[^|] | %d | %d",
                            tenTap, mt, mtr, &sl, &slg);

        if (!found && parsed >= 4) {
            TrimStr_PM(mt); TrimStr_PM(mtr);
            if (strcmp(mt, searchMaTap) == 0 && strcmp(mtr, maTruyen) == 0) {
                found = true;
                if (sl <= 0) {
                    canBorrow = false;
                    break;
                }
                sl -= 1;
                for (int ki = 0; ki < s_soTap; ki++) {
                    if (strcmp(s_dsTap[ki].maTap, searchMaTap) == 0 &&
                        strcmp(s_dsTap[ki].maTruyen, maTruyen) == 0) {
                        s_dsTap[ki].soLuong = sl;
                        break;
                    }
                }

                char *p = lineStart;
                int pipes = 0;
                while (p < lineStart + lineLen && pipes < 3) {
                    if (*p == '|') pipes++;
                    p++;
                }
                while (*p == ' ') p++;

                int oldNumStart = (int)(p - buf);
                int oldNumLen = 0;
                while (p[oldNumLen] >= '0' && p[oldNumLen] <= '9') oldNumLen++;

                char newNum[16];
                snprintf(newNum, sizeof(newNum), "%d", sl);
                int newNumLen = (int)strlen(newNum);

                int diff = newNumLen - oldNumLen;
                if (diff != 0) {
                    memmove(buf + oldNumStart + newNumLen,
                            buf + oldNumStart + oldNumLen,
                            nread - (oldNumStart + oldNumLen) + 1);
                    nread += diff;
                }
                memcpy(buf + oldNumStart, newNum, newNumLen);
                break;
            }
        }
        lineStart = eol ? eol + 1 : lineStart + lineLen;
    }

    if (!canBorrow) {
        free(buf);
        return false;
    }

    if (found) {
        FILE *fw = fopen(filepath, "w");
        if (fw) {
            fwrite(buf, 1, nread, fw);
            fclose(fw);
        }
    }
    free(buf);
    return true;
}

void TimTenTruyenTheoMa(const char *matruyen, char *tentruyen_out) {
    tentruyen_out[0] = '\0';
    LoadDuLieuTruyen_PM();
    for (int i=0; i<s_soTruyen; i++)
        if (strcmp(s_dsTruyen[i].ma, matruyen)==0)
            { strcpy(tentruyen_out, s_dsTruyen[i].ten); return; }
}

int DemSoPhieuMuon(PhieuMuonNode *head) {
    int c=0; for (PhieuMuonNode *t=head; t; t=t->next) c++; return c;
}

bool KiemTraNgayHopLe(const char *ngay) {
    if (!ngay || ngay[0] == '\0') return false;
    int d = 0, m = 0, y = 0;
    if (sscanf(ngay, "%d/%d/%d", &d, &m, &y) != 3) return false;
    if (y < 1000 || y > 9999) return false;
    if (m < 1 || m > 12) return false;
    int ngayToiDa[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) ngayToiDa[2] = 29;
    if (d < 1 || d > ngayToiDa[m]) return false;
    char expected_full[12], expected_short1[12], expected_short2[12], expected_short3[12];
    snprintf(expected_full,   sizeof(expected_full),   "%02d/%02d/%04d", d, m, y);
    snprintf(expected_short1, sizeof(expected_short1), "%d/%02d/%04d",   d, m, y);
    snprintf(expected_short2, sizeof(expected_short2), "%02d/%d/%04d",   d, m, y);
    snprintf(expected_short3, sizeof(expected_short3), "%d/%d/%04d",     d, m, y);
    return (strcmp(ngay, expected_full)   == 0 ||
            strcmp(ngay, expected_short1) == 0 ||
            strcmp(ngay, expected_short2) == 0 ||
            strcmp(ngay, expected_short3) == 0);
}

void InitPhieumuon(FormPhieuMuon *Form) {
    memset(Form->mathe.text,     0, 512);
    memset(Form->tentruyen.text, 0, 512);
    memset(Form->ngaymuon.text,  0, 512);
    memset(Form->ngaytra.text,   0, 512);
    memset(Form->matruyen.text,  0, 512);
    Form->mathe.lettercount=Form->tentruyen.lettercount=Form->ngaymuon.lettercount=
    Form->ngaytra.lettercount=Form->matruyen.lettercount=0;
    Form->mathe.backspacecounter=Form->tentruyen.backspacecounter=Form->ngaymuon.backspacecounter=
    Form->ngaytra.backspacecounter=Form->matruyen.backspacecounter=0;
    Form->mathe.isfocused=true;
    Form->tentruyen.isfocused=Form->ngaymuon.isfocused=
    Form->ngaytra.isfocused=Form->matruyen.isfocused=false;
    Form->showsuccess=false; Form->successtimer=0;

    s_ngaytraLoi = false;
    s_hetSachLoi = false;
    s_matheLoi = false;

    memset(s_filter,0,sizeof(s_filter));
    s_showDropdown=false; s_showTapDropdown=false;
    s_matchCount=0; s_dropScroll=0; s_tapScroll=0;
    s_selectedMaTruyen[0]='\0';
    s_selectedMaTap[0]='\0';

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(Form->ngaymuon.text, 511, "%02d/%02d/%04d",
             t->tm_mday, t->tm_mon+1, t->tm_year+1900);
    Form->ngaymuon.lettercount = (int)strlen(Form->ngaymuon.text);
}

void UpdateVitri(FormPhieuMuon *Form) {
    float sw=(float)GetScreenWidth(), sh=(float)GetScreenHeight();
    float sc=(sw/1100.0f<sh/750.0f)?sw/1100.0f:sh/750.0f;
    if(sc<0.5f)sc=0.5f;

    Rectangle card={(sw-800*sc)/2, (sh-620*sc)/2, 800*sc, 620*sc};
    float ix=card.x+50*sc, iy=card.y+135*sc, iw=450*sc, ih=48*sc, sp=94*sc;

    Form->mathe.rec     =(Rectangle){ix, iy+0*sp, iw, ih};
    Form->tentruyen.rec =(Rectangle){ix, iy+1*sp, iw, ih};
    Form->matruyen.rec  =(Rectangle){ix, iy+2*sp, iw, ih};
    Form->ngaymuon.rec  =(Rectangle){ix, iy+3*sp, iw, ih};
    Form->ngaytra.rec   =(Rectangle){ix, iy+4*sp, iw, ih};
}

void SinhMaPM(int currentcount, char *mathe) {
    sprintf(mathe,"PM%06d",currentcount+1);
}

// ============================================================
// UpdateInputPM
// ============================================================
void UpdateInputPM(FormPhieuMuon *Form, PhieuMuonNode **head, int *currentState, char *mathe) {
    LoadDuLieuTruyen_PM();
    if (mathe[0]=='\0') {
        // SinhMaPM đếm đúng số lượng từ linked list dữ liệu thực tế
        SinhMaPM(DemSoPhieuMuon(*head), mathe);
        if (Form->ngaymuon.lettercount == 0) {
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            snprintf(Form->ngaymuon.text, 511, "%02d/%02d/%04d",
                     t->tm_mday, t->tm_mon+1, t->tm_year+1900);
            Form->ngaymuon.lettercount = (int)strlen(Form->ngaymuon.text);
        }
    }

    Rectangle btnBack={20,20,130,40};
    if (IsKeyPressed(KEY_ESCAPE)||
        (CheckCollisionPointRec(GetMousePosition(),btnBack)&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
        InitPhieumuon(Form); mathe[0]='\0'; *currentState=APP_MENU; return;
    }

    if (Form->showsuccess) {
        Form->successtimer -= GetFrameTime();
        if (Form->successtimer<=0) {
            Form->showsuccess=false;
            InitPhieumuon(Form);
            SinhMaPM(DemSoPhieuMuon(*head), mathe);
        }
        return;
    }

    InputBox_PM *boxes[]={&Form->mathe,&Form->tentruyen,&Form->matruyen,&Form->ngaymuon,&Form->ngaytra};
    Vector2 mp = GetMousePosition();
    float sw=(float)GetScreenWidth(), sh=(float)GetScreenHeight();
    float sc=(sw/1100.0f<sh/750.0f)?sw/1100.0f:sh/750.0f;
    if(sc<0.5f)sc=0.5f;

    // ── Xử lý click ──────────────────────────────────────────
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mp, boxes[1]->rec)) {
            bool hasTaps = (s_selectedMaTruyen[0] != '\0' && KiemTraCoTap(s_selectedMaTruyen));
            Rectangle arrowBtn = { boxes[1]->rec.x + boxes[1]->rec.width - 35*sc, boxes[1]->rec.y, 35*sc, boxes[1]->rec.height };

            if (hasTaps && CheckCollisionPointRec(mp, arrowBtn)) {
                s_showTapDropdown = !s_showTapDropdown;
                s_showDropdown = false;
                s_tapScroll = 0;
            } else {
                for(int j=0;j<5;j++) boxes[j]->isfocused=false;
                boxes[1]->isfocused=true;
                strncpy(s_filter, Form->tentruyen.text, 255);
                CapNhatMatch();
                s_showDropdown = true;
                s_showTapDropdown = false;
            }
        } else {
            bool inDropdown = false;
            float dropX = Form->tentruyen.rec.x;
            float dropY = Form->tentruyen.rec.y + Form->tentruyen.rec.height + 2;
            float dropW = Form->tentruyen.rec.width;
            float rowH = 40*sc;
            int maxVis = 5;

            if (s_showDropdown && s_matchCount > 0) {
                int start = s_dropScroll, end = start + maxVis;
                if(end > s_matchCount) end = s_matchCount;
                float curY = dropY;
                for (int vi=start; vi<end && !inDropdown; vi++) {
                    int ti = s_matchIdx[vi];
                    Rectangle rowRec = {dropX, curY, dropW, rowH};
                    if (CheckCollisionPointRec(mp, rowRec)) {
                        strncpy(Form->tentruyen.text, s_dsTruyen[ti].ten, 511);
                        Form->tentruyen.lettercount = (int)strlen(Form->tentruyen.text);
                        strncpy(s_filter, s_dsTruyen[ti].ten, 255);
                        strncpy(Form->matruyen.text, s_dsTruyen[ti].ma, 511);
                        Form->matruyen.lettercount = (int)strlen(Form->matruyen.text);
                        strncpy(s_selectedMaTruyen, s_dsTruyen[ti].ma, 15);
                        s_selectedMaTap[0] = '\0'; s_hetSachLoi = false;
                        s_showDropdown = false;
                        if (KiemTraCoTap(s_selectedMaTruyen)) {
                            boxes[1]->isfocused = true;
                            s_showTapDropdown = true;
                        } else {
                            boxes[1]->isfocused = false;
                            boxes[3]->isfocused = true;
                        }
                        inDropdown = true;
                    }
                    curY += rowH;
                }
                if (!inDropdown) {
                    Rectangle dropZone = {dropX, dropY, dropW, rowH*(end-start)};
                    if (CheckCollisionPointRec(mp, dropZone)) inDropdown = true;
                }
            }

            if (s_showTapDropdown && s_selectedMaTruyen[0] != '\0') {
                TapInfo_PM tapBuf[MAX_TAP_PM];
                int tapCount = 0;
                for(int k=0; k<s_soTap; k++)
                    if(strcmp(s_dsTap[k].maTruyen, s_selectedMaTruyen)==0)
                        tapBuf[tapCount++] = s_dsTap[k];

                float tapRowH = 36*sc;
                float tapZoneY = dropY;
                int tapMaxVis = 6;
                int start = s_tapScroll, end = start + tapMaxVis;
                if(end > tapCount) end = tapCount;

                float curY = tapZoneY;
                for(int ti2=start; ti2<end && !inDropdown; ti2++) {
                    Rectangle tapRec = {dropX, curY, dropW, tapRowH};
                    if(CheckCollisionPointRec(mp, tapRec)) {
                        strncpy(Form->tentruyen.text, tapBuf[ti2].tenTap, 511);
                        Form->tentruyen.lettercount = (int)strlen(Form->tentruyen.text);
                        strncpy(s_filter, tapBuf[ti2].tenTap, 255);
                        snprintf(Form->matruyen.text, 511, "%s%s", s_selectedMaTruyen, tapBuf[ti2].maTap);
                        Form->matruyen.lettercount = (int)strlen(Form->matruyen.text);
                        strncpy(s_selectedMaTap, tapBuf[ti2].maTap, 15);
                        s_showTapDropdown = false;
                        boxes[1]->isfocused = false;
                        boxes[3]->isfocused = true;
                        inDropdown = true;
                    }
                    curY += tapRowH;
                }
                if (!inDropdown) {
                    Rectangle tapZone = {dropX, tapZoneY, dropW, tapRowH*(end-start)};
                    if(CheckCollisionPointRec(mp, tapZone)) inDropdown = true;
                }
            }

            if (!inDropdown) {
                s_showDropdown = false;
                s_showTapDropdown = false;
                for(int j=0;j<5;j++) boxes[j]->isfocused=false;
                for(int i=0;i<5;i++) {
                    if(i==1||i==2) continue;
                    if(CheckCollisionPointRec(mp,boxes[i]->rec)) {
                        boxes[i]->isfocused=true; break;
                    }
                }
            }
        }
    }

    // ── Scroll wheel ─────────────────────────────────────────
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        if (s_showTapDropdown) {
            s_tapScroll -= (int)wheel;
            if(s_tapScroll < 0) s_tapScroll = 0;
            int tapCount=0;
            for(int k=0;k<s_soTap;k++) if(strcmp(s_dsTap[k].maTruyen, s_selectedMaTruyen)==0) tapCount++;
            int maxVis = 6;
            if(s_tapScroll > tapCount - maxVis) s_tapScroll = (tapCount > maxVis) ? tapCount - maxVis : 0;
        } else if (s_showDropdown) {
            s_dropScroll -= (int)wheel;
            if(s_dropScroll < 0) s_dropScroll = 0;
            int maxVis = 5;
            if(s_dropScroll > s_matchCount - maxVis) s_dropScroll = (s_matchCount > maxVis) ? s_matchCount - maxVis : 0;
        }
    }

    // ── Input ô tentruyen (search) ───────────────────────────
    if (boxes[1]->isfocused) {
        int key=GetKeyPressed();
        while(key>0) {
            if(key==KEY_BACKSPACE) {
                int len=(int)strlen(s_filter);
                if(len>0) {
                    do{ len--; } while(len>0&&(s_filter[len]&0xC0)==0x80);
                    s_filter[len]='\0';
                    Form->matruyen.text[0]='\0'; Form->matruyen.lettercount=0;
                    s_selectedMaTruyen[0] = '\0'; s_hetSachLoi = false;
                    s_selectedMaTap[0] = '\0'; s_hetSachLoi = false;
                    CapNhatMatch();
                    s_showDropdown = true;
                    s_showTapDropdown = false;
                }
            }
            key=GetKeyPressed();
        }
        if(IsKeyDown(KEY_BACKSPACE)) {
            boxes[1]->backspacecounter+=GetFrameTime();
            if(boxes[1]->backspacecounter>=0.5f) {
                int len=(int)strlen(s_filter);
                if(len>0) {
                    do{ len--; } while(len>0&&(s_filter[len]&0xC0)==0x80);
                    s_filter[len]='\0';
                    Form->matruyen.text[0]='\0'; Form->matruyen.lettercount=0;
                    s_selectedMaTruyen[0] = '\0'; s_hetSachLoi = false;
                    s_selectedMaTap[0] = '\0'; s_hetSachLoi = false;
                    CapNhatMatch();
                    s_showDropdown = true;
                    s_showTapDropdown = false;
                }
            }
        } else boxes[1]->backspacecounter=0;

        int ch=GetCharPressed();
        while(ch>0) {
            if(ch>=32) {
                int bs=0; const char *u=CodepointToUTF8(ch,&bs);
                int flen=(int)strlen(s_filter);
                if(flen+bs<255) {
                    for(int j=0;j<bs;j++) s_filter[flen+j]=u[j];
                    s_filter[flen+bs]='\0';
                    Form->matruyen.text[0]='\0'; Form->matruyen.lettercount=0;
                    s_selectedMaTruyen[0] = '\0'; s_hetSachLoi = false;
                    s_selectedMaTap[0] = '\0'; s_hetSachLoi = false;
                    CapNhatMatch();
                    s_showDropdown = true;
                    s_showTapDropdown = false;
                }
            }
            ch=GetCharPressed();
        }
        strncpy(Form->tentruyen.text, s_filter, 511);
        Form->tentruyen.lettercount=(int)strlen(s_filter);

        if(IsKeyPressed(KEY_ESCAPE)) { s_showDropdown=false; s_showTapDropdown=false; boxes[1]->isfocused=false; }
        if(IsKeyPressed(KEY_TAB)) {
            boxes[1]->isfocused=false; s_showDropdown=false; s_showTapDropdown=false;
            boxes[3]->isfocused=true;
        }
    }

    // ── Input các ô thường ──
    for(int i=0;i<5;i++) {
        if(i==1||i==2) continue;
        if(!boxes[i]->isfocused) continue;
        int key=GetKeyPressed();
        while(key>0) {
            if(key==KEY_BACKSPACE&&boxes[i]->lettercount>0) {
                if(i == 4) {
                    boxes[i]->lettercount--;
                    boxes[i]->text[boxes[i]->lettercount] = '\0';
                    s_ngaytraLoi = false;
                } else if(i == 0) {
                    boxes[i]->lettercount--;
                    boxes[i]->text[boxes[i]->lettercount] = '\0';
                    s_matheLoi = false;
                } else {
                    do{ boxes[i]->lettercount--; }
                    while(boxes[i]->lettercount>0&&(boxes[i]->text[boxes[i]->lettercount]&0xC0)==0x80);
                    boxes[i]->text[boxes[i]->lettercount]='\0';
                }
            }
            key=GetKeyPressed();
        }
        if(IsKeyDown(KEY_BACKSPACE)) {
            boxes[i]->backspacecounter+=GetFrameTime();
            if(boxes[i]->backspacecounter>=0.5f&&boxes[i]->lettercount>0) {
                if(i == 4) {
                    boxes[i]->lettercount--;
                    boxes[i]->text[boxes[i]->lettercount] = '\0';
                    s_ngaytraLoi = false;
                } else if(i == 0) {
                    boxes[i]->lettercount--;
                    boxes[i]->text[boxes[i]->lettercount] = '\0';
                    s_matheLoi = false;
                } else {
                    do{ boxes[i]->lettercount--; }
                    while(boxes[i]->lettercount>0&&(boxes[i]->text[boxes[i]->lettercount]&0xC0)==0x80);
                    boxes[i]->text[boxes[i]->lettercount]='\0';
                }
            }
        } else boxes[i]->backspacecounter=0;

        int ch=GetCharPressed();
        while(ch>0) {
            if(i == 0) {
                if(ch >= '0' && ch <= '9' && boxes[i]->lettercount < 9) {
                    boxes[i]->text[boxes[i]->lettercount++] = (char)ch;
                    boxes[i]->text[boxes[i]->lettercount] = '\0';
                    s_matheLoi = false;
                }
            } else if(i == 4) {
                if(ch >= '0' && ch <= '9' && boxes[i]->lettercount < 4) {
                    boxes[i]->text[boxes[i]->lettercount++] = (char)ch;
                    boxes[i]->text[boxes[i]->lettercount] = '\0';
                    s_ngaytraLoi = false;
                }
            } else {
                if(ch>=32&&boxes[i]->lettercount<250) {
                    int bs=0; const char *u=CodepointToUTF8(ch,&bs);
                    if(boxes[i]->lettercount+bs<255) {
                        for(int j=0;j<bs;j++) boxes[i]->text[boxes[i]->lettercount++]=u[j];
                        boxes[i]->text[boxes[i]->lettercount]='\0';
                    }
                }
            }
            ch=GetCharPressed();
        }

        if(IsKeyPressed(KEY_TAB)) {
            boxes[i]->isfocused=false;
            int next=(i+1)%5;
            if(next==1) {
                boxes[1]->isfocused=true;
                strncpy(s_filter,Form->tentruyen.text,255);
                CapNhatMatch(); s_showDropdown=true;
            } else if(next==2) { next=3; boxes[3]->isfocused=true; }
            else boxes[next]->isfocused=true;
            break;
        }
    }

    // ── Tọa độ nút XÁC NHẬN — phải khớp với DrawPM ──
    Rectangle cr={(sw-800*sc)/2+600*sc,(sh-620*sc)/2,200*sc,620*sc};
    Rectangle Confirm={
        cr.x + (cr.width - 160*sc)/2,   // Căn giữa panel phải
        cr.y + cr.height + 10*sc,        // Ngay bên dưới đáy card (ngoài card)
        160*sc, 48*sc
    };

    if(IsKeyPressed(KEY_ENTER)||
       (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&CheckCollisionPointRec(mp,Confirm))) {

        // 1. Kiểm tra Mã thẻ độc giả
        bool matheHopLe = KiemTraTheTonTai(Form->mathe.text);
        s_matheLoi = !matheHopLe;

        // 2. Kiểm tra số ngày mượn hợp lệ (> 0)
        int soNgay = atoi(Form->ngaytra.text);
        if(Form->ngaytra.lettercount == 0 || soNgay <= 0) {
            s_ngaytraLoi = true;
            s_hetSachLoi = false;
        } else {
            s_ngaytraLoi = false;
        }

        // Nếu tất cả hợp lệ thì mới thực hiện tạo phiếu mượn
        if(matheHopLe && !s_ngaytraLoi && Form->matruyen.lettercount > 0) {
            if(GiamSoLuongTap(s_selectedMaTap, s_selectedMaTruyen)) {
                s_hetSachLoi = false;
                if(LuuPhieuMuonVaoFile(mathe,Form)) {
                    ThemPhieuMuonVaoList(head,mathe,Form);
                    Form->showsuccess=true; Form->successtimer=3.0f;
                }
            } else {
                s_hetSachLoi = true;
            }
        }
    }
}

// ============================================================
// DrawPM — vẽ toàn bộ giao diện
// ============================================================
void DrawPM(FormPhieuMuon *Form, Texture2D icons[], char *mathe, Font font) {
    float sw=(float)GetScreenWidth(), sh=(float)GetScreenHeight();
    float sc=(sw/1100.0f<sh/750.0f)?sw/1100.0f:sh/750.0f;
    if(sc<0.5f)sc=0.5f;

    ClearBackground(AnimatedBackground());
    DrawBackgroundParticles();

    Rectangle btnBack={20,20,130,40};
    bool hoverBack=CheckCollisionPointRec(GetMousePosition(),btnBack);
    DrawRectangleRounded(btnBack,0.3f,10, hoverBack?MAROON:WHITE);
    DrawRectangleRoundedLines(btnBack,0.3f,10,MAROON);
    DrawTextEx(font,"< QUAY LẠI",(Vector2){btnBack.x+15,btnBack.y+10},20,1,hoverBack?WHITE:SKYBLUE);

    Rectangle cl={(sw-800*sc)/2,(sh-620*sc)/2,600*sc,620*sc};
    Rectangle cr={(sw-800*sc)/2+600*sc,(sh-620*sc)/2,200*sc,620*sc};

    DrawRectangleRec((Rectangle){cl.x+6*sc,cl.y+6*sc,cl.width,cl.height},Fade(BLACK,0.2f));
    DrawRectangleRec((Rectangle){cr.x+6*sc,cr.y+6*sc,cr.width,cr.height},Fade(BLACK,0.2f));
    DrawRectangleGradientEx(cl,GetColor(0x2f6690ff),GetColor(0x2f6690ff),GetColor(0x2f6690ff),GetColor(0x2f6690ff));
    DrawRectangleGradientEx(cr,GetColor(0x81c3d7ff),GetColor(0x81c3d7ff),GetColor(0x81c3d7ff),GetColor(0x81c3d7ff));

    DrawTextEx(font,"Phiếu mượn sách",(Vector2){cl.x+136*sc,cr.y+36*sc},36*sc,1, GetColor(0xeff2f1ff));

// IDPM — 1 dòng căn giữa panel phải cr
    char idLine[32];
    snprintf(idLine, sizeof(idLine), "IDPM: %s", mathe);
    Vector2 idLineSize = MeasureTextEx(font, idLine, 20*sc, 1);
    float idY = cr.y + cr.height - 78*sc;
    DrawTextEx(font, idLine,
        (Vector2){cr.x + (cr.width - idLineSize.x) / 2.0f, idY},
        20*sc, 1, WHITE);

    const char *labels[]={"Mã thẻ bạn đọc:","Tên truyện:","Mã truyện:","Ngày mượn:","Số ngày mượn:"};
    InputBox_PM *boxes[]={&Form->mathe,&Form->tentruyen,&Form->matruyen,&Form->ngaymuon,&Form->ngaytra};

    for(int i=0;i<5;i++) {
        DrawTextEx(font,labels[i],(Vector2){boxes[i]->rec.x,boxes[i]->rec.y-25*sc},18*sc,1,GetColor(0xeff2f1ff));

        bool rdonly=(i==2);
        bool isNgaytraLoi = (i==4 && s_ngaytraLoi);
        bool isMatheLoi   = (i==0 && s_matheLoi);
        bool isLoi        = isNgaytraLoi || isMatheLoi;

        Color borderColor = rdonly ? LIGHTGRAY : (isLoi ? RED : (boxes[i]->isfocused ? WHITE : LIGHTGRAY));
        Color bgColor = rdonly ? GetColor(0xF0F0F0FF) : (isLoi ? GetColor(0xFFF0F0FF) : WHITE);
        DrawRectangleRounded(boxes[i]->rec,0.2f,10, bgColor);
        DrawRectangleRoundedLines(boxes[i]->rec,0.2f,10, borderColor);

        if(isNgaytraLoi) {
            DrawTextEx(font,"! Vui lòng nhập số ngày mượn (> 0)",
                (Vector2){boxes[i]->rec.x, boxes[i]->rec.y+boxes[i]->rec.height+6*sc},
                14*sc,1,RED);
        }
        if(isMatheLoi) {
            DrawTextEx(font,"! Mã thẻ không hợp lệ hoặc không tồn tại",
                (Vector2){boxes[i]->rec.x, boxes[i]->rec.y+boxes[i]->rec.height+6*sc},
                14*sc,1,RED);
        }

        float fs=20*sc;
        bool hasTaps = false;
        float scissorW = boxes[i]->rec.width - 10;

        if (i == 1 && s_selectedMaTruyen[0] != '\0' && KiemTraCoTap(s_selectedMaTruyen)) {
            hasTaps = true;
            scissorW -= 35*sc;
            Rectangle arrowBtn = { boxes[i]->rec.x + boxes[i]->rec.width - 35*sc, boxes[i]->rec.y, 35*sc, boxes[i]->rec.height };
            DrawLineEx((Vector2){arrowBtn.x, arrowBtn.y + 8*sc}, (Vector2){arrowBtn.x, arrowBtn.y + arrowBtn.height - 8*sc}, 1.5f, LIGHTGRAY);
            const char* arrowIcon = s_showTapDropdown ? "^" : "v";
            Color arrowColor = s_showTapDropdown ? SKYBLUE : GRAY;
            if (CheckCollisionPointRec(GetMousePosition(), arrowBtn)) arrowColor = SKYBLUE;
            DrawTextEx(font, arrowIcon,
                (Vector2){arrowBtn.x + 10*sc, arrowBtn.y + (arrowBtn.height - MeasureTextEx(font, arrowIcon, 20*sc, 1).y)/2},
                20*sc, 1, arrowColor);
        }

        Vector2 tsz=MeasureTextEx(font,boxes[i]->text,fs,1);
        float offX=(tsz.x > scissorW - 10) ? tsz.x - (scissorW - 10) : 0;

        BeginScissorMode((int)boxes[i]->rec.x+5, (int)boxes[i]->rec.y, (int)scissorW, (int)boxes[i]->rec.height);
            Vector2 tp={boxes[i]->rec.x+10-offX, boxes[i]->rec.y+(boxes[i]->rec.height-fs)/2};
            DrawTextEx(font,boxes[i]->text,tp,fs,1, rdonly?GRAY:BLACK);
            if(boxes[i]->isfocused&&!rdonly&&(((int)(GetTime()*1.5f))%2==0))
                DrawRectangleV((Vector2){tp.x+tsz.x+2,tp.y+2},(Vector2){2,fs-4},BLACK);
        EndScissorMode();
    }

    // ── Dropdown Tìm Kiếm Truyện ─────────────────────────────
    if (s_showDropdown && s_matchCount>0) {
        float dropX=Form->tentruyen.rec.x;
        float dropY=Form->tentruyen.rec.y+Form->tentruyen.rec.height+2;
        float dropW=Form->tentruyen.rec.width;
        float rowH=40*sc;
        int maxVis=5;
        int start=s_dropScroll, end=start+maxVis;
        if(end>s_matchCount) end=s_matchCount;

        Vector2 mp=GetMousePosition();
        float totalH=rowH*(end-start);

        DrawRectangleRec((Rectangle){dropX+3,dropY+3,dropW,totalH},Fade(BLACK,0.15f));
        DrawRectangleRec((Rectangle){dropX,dropY,dropW,totalH},WHITE);
        DrawRectangleLinesEx((Rectangle){dropX,dropY,dropW,totalH},1.5f,SKYBLUE);

        float curY=dropY;
        for(int vi=start; vi<end; vi++) {
            int ti=s_matchIdx[vi];
            Rectangle rowRec={dropX,curY,dropW,rowH};
            bool hoverRow=CheckCollisionPointRec(mp,rowRec);
            DrawRectangleRec(rowRec, hoverRow?GetColor(0xceeaf7ff):WHITE);
            DrawRectangleLinesEx(rowRec,0.5f,GetColor(0x386fa4ff));
            BeginScissorMode((int)dropX+5,(int)curY,(int)(dropW-10*sc),(int)rowH);
                DrawTextEx(font,s_dsTruyen[ti].ten,
                           (Vector2){dropX+10,curY+(rowH-18*sc)/2},18*sc,1,GetColor(0x2f6690ff));
            EndScissorMode();
            curY+=rowH;
        }
        if(s_matchCount>maxVis) {
            char hint[32]; snprintf(hint,sizeof(hint),"%d/%d",s_dropScroll+1,s_matchCount);
            DrawTextEx(font,hint,(Vector2){dropX+dropW-60*sc,dropY-18*sc},13*sc,1,SKYBLUE);
        }
    }

    // ── Dropdown Danh Sách Tập ────────────────────────────────
    if (s_showTapDropdown && s_selectedMaTruyen[0] != '\0') {
        TapInfo_PM tapBuf[MAX_TAP_PM];
        int tapCount=0;
        for(int k=0;k<s_soTap&&tapCount<MAX_TAP_PM;k++)
            if(strcmp(s_dsTap[k].maTruyen, s_selectedMaTruyen)==0)
                tapBuf[tapCount++]=s_dsTap[k];

        float dropX=Form->tentruyen.rec.x;
        float dropY=Form->tentruyen.rec.y+Form->tentruyen.rec.height+2;
        float dropW=Form->tentruyen.rec.width;
        float tapRowH=36*sc;
        int tapMaxVis=6;
        if(s_tapScroll<0) s_tapScroll=0;
        if(tapCount>tapMaxVis&&s_tapScroll>tapCount-tapMaxVis)
            s_tapScroll=tapCount-tapMaxVis;

        int start = s_tapScroll;
        int end = start + tapMaxVis;
        if(end > tapCount) end = tapCount;

        float totalH = tapRowH * (end - start);
        DrawRectangleRec((Rectangle){dropX+3,dropY+3,dropW,totalH},Fade(BLACK,0.15f));
        DrawRectangleRec((Rectangle){dropX,dropY,dropW,totalH},GetColor(0x59a5d8ff));
        DrawRectangleLinesEx((Rectangle){dropX,dropY,dropW,totalH},1.5f,GetColor(0x87f5fbff));

        Vector2 mp=GetMousePosition();
        float curY = dropY;
        for(int ti2=start; ti2<end; ti2++) {
            Rectangle tapRec={dropX,curY,dropW,tapRowH};
            bool hoverTap=CheckCollisionPointRec(mp,tapRec);
            if(hoverTap) DrawRectangleRec(tapRec,SKYBLUE);
            DrawRectangleLinesEx(tapRec,0.5f,GetColor(0xfbf9ffff));

            char badge[20];
            snprintf(badge,sizeof(badge)," [%s]",tapBuf[ti2].maTap);
            Vector2 bsz=MeasureTextEx(font,badge,13*sc,1);
            DrawTextEx(font,badge,(Vector2){dropX+dropW-bsz.x-7,curY+(tapRowH-13*sc)/2},13*sc,1,WHITE);

            char tapLine[512];
            snprintf(tapLine,sizeof(tapLine),"  %s  (Còn: %d)", tapBuf[ti2].tenTap, tapBuf[ti2].soLuong);
            BeginScissorMode((int)dropX+4, (int)curY, (int)(dropW-bsz.x-20), (int)tapRowH);
                DrawTextEx(font,tapLine,(Vector2){dropX+8,curY+(tapRowH-16*sc)/2}, 16*sc,1, WHITE);
            EndScissorMode();
            curY+=tapRowH;
        }
    }

    // ── Nút XÁC NHẬN — nằm BÊN NGOÀI card, phía dưới góc phải (căn theo cr) ──
    Rectangle Confirm={
        cr.x + (cr.width - 160*sc)/2,   // Căn giữa theo panel phải
        cr.y + cr.height + 10*sc,        // Ngay bên dưới đáy card (ngoài card)
        160*sc, 48*sc
    };
    bool isHover=CheckCollisionPointRec(GetMousePosition(),Confirm);
    DrawRectangleRounded(Confirm,0.3f,10, isHover?DARKBLUE:SKYBLUE);
    DrawTextEx(font,"XÁC NHẬN",
           (Vector2){Confirm.x+(Confirm.width-MeasureTextEx(font,"XÁC NHẬN",20*sc,1).x)/2,
                     Confirm.y+(Confirm.height-20*sc)/2},20*sc,1,WHITE);
    // Thông báo hết sách
    if(s_hetSachLoi) {
        const char *hetMsg = "! Sách đã hết, không thể mượn!";
        Vector2 hmSz = MeasureTextEx(font, hetMsg, 15*sc, 1);
        DrawTextEx(font, hetMsg,
                   (Vector2){Confirm.x + (Confirm.width - hmSz.x)/2, Confirm.y - 24*sc},
                   15*sc, 1, RED);
    }

    if(Form->showsuccess) DrawSuccess(font,(Texture2D){0},Form->successtimer);
}

// ============================================================
// DrawSuccess
// ============================================================
void DrawSuccess(Font font, Texture2D bg, float t) {
    float sw=(float)GetScreenWidth(), sh=(float)GetScreenHeight();
    if(bg.id!=0)
        DrawTexturePro(bg,(Rectangle){0,0,(float)bg.width,(float)bg.height},
                       (Rectangle){0,0,sw,sh},(Vector2){0,0},0,WHITE);
    DrawRectangle(0,0,(int)sw,(int)sh,Fade(BLACK,0.4f));
    Rectangle panel={sw/2-250,sh/2-150,500,300};
    DrawRectangleRec(panel, SKYBLUE);
    DrawCircle((int)sw/2,(int)sh/2-60,45,WHITE);
    DrawTextEx(font,"V",(Vector2){sw/2-15,sh/2-85},60,1,SKYBLUE);
    const char *msg="TẠO PHIẾU MƯỢN THÀNH CÔNG!";
    Vector2 ms=MeasureTextEx(font,msg,32,1);
    DrawTextEx(font,msg,(Vector2){(sw-ms.x)/2,sh/2+20},32,1,WHITE);
    float prog=t/3.0f;
    DrawRectangle((int)panel.x+50,(int)panel.y+panel.height-50,400,12,Fade(WHITE,0.4f));
    DrawRectangle((int)panel.x+50,(int)panel.y+panel.height-50,(int)(400*(1.0f-prog)),12,WHITE);
}

// ============================================================
// Lưu & Thêm node
// ============================================================
bool LuuPhieuMuonVaoFile(char *maphieu, FormPhieuMuon *Form) {
    FILE *f=fopen("data/Phieumuon/LSPhieumuon.txt","a");
    if(!f) return false;
    for(int i=strlen(Form->mathe.text)-1;     i>=0&&Form->mathe.text[i]==' ';     i--) Form->mathe.text[i]='\0';
    for(int i=strlen(Form->matruyen.text)-1;  i>=0&&Form->matruyen.text[i]==' ';  i--) Form->matruyen.text[i]='\0';
    for(int i=strlen(Form->tentruyen.text)-1; i>=0&&Form->tentruyen.text[i]==' '; i--) Form->tentruyen.text[i]='\0';
    for(int i=strlen(Form->ngaymuon.text)-1;  i>=0&&Form->ngaymuon.text[i]==' ';  i--) Form->ngaymuon.text[i]='\0';
    for(int i=strlen(Form->ngaytra.text)-1;   i>=0&&Form->ngaytra.text[i]==' ';   i--) Form->ngaytra.text[i]='\0';
    fprintf(f,"%-*s | %-*s | %-*s | %-*s | %-*s | %-*s | %d\n",
        UTF8Width(maphieu,12),              maphieu,
        UTF8Width(Form->mathe.text,12),     Form->mathe.text,
        UTF8Width(Form->matruyen.text,19),  Form->matruyen.text,
        UTF8Width(Form->tentruyen.text,50), Form->tentruyen.text,
        UTF8Width(Form->ngaymuon.text,19),  Form->ngaymuon.text,
        UTF8Width(Form->ngaytra.text,19),   Form->ngaytra.text,
        0);
    fflush(f); fclose(f);
    return true;
}

void ThemPhieuMuonVaoList(PhieuMuonNode **head, char *maPM, FormPhieuMuon *Form) {
    PhieuMuonNode *n=(PhieuMuonNode*)malloc(sizeof(PhieuMuonNode));
    if(n) {
        strcpy(n->maPM,     maPM);
        strcpy(n->mathe,    Form->mathe.text);
        strcpy(n->matruyen, Form->matruyen.text);
        strcpy(n->tentruyen,Form->tentruyen.text);
        strcpy(n->ngaymuon, Form->ngaymuon.text);
        strcpy(n->ngaytra,  Form->ngaytra.text);
        n->trangthai=0; n->next=*head; *head=n;
    }
}