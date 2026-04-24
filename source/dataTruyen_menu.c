#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

#define TRUE  1
#define FALSE 0
#define NOT_FOUND -1
#define Col 236
#define Lines 72

//=======================================================================
// Quản lí console
// == Khóa khung console ==
void FixAndLockConsole();

// == Điều chỉnh con trỏ console ==
void gotoxy(int x, int y);

void setColor(int Color);
//=========================================================================

// == Cấu trúc của một danh sách truyên ==
struct ElementType 
{
    char ID[20];
    char Name[200];
    char normName[200];
    int Quantity;
    int Soluotmuon;
    char State[20];
    double Price_day;
    int lenName;
};
struct ListInfo 
{
    struct ElementType *dataTruyen;
    int count;
    int capacity;
    int last_page;
};

typedef struct ElementType Element;
typedef struct ListInfo *List;
typedef int Position;

// == Tạo danh sách truyên ==
List createList (FILE *f);
int getdataTruyen(List L, FILE *f);

// == Lọc khoảng trắng ==
int trim(char *);

// == Đếm độ dài thực tế của tên truyện ==
int lenName(const char *);

// == chuyển dấu thành không dấu ==
int normalize(const char *, char *);
int normalize_vi(const char *, char *, Position *, Position *);

// == show Title ==
void Title();

// == show List ==
void showList(List L, Position page);

// == show chức năng quản lí ==
void showFunc();

void show_addData();
int addData(List );
void show_returnaddData(int _returnaddData);

void show_eraseData();
void eraseData(List, Position );
void show_returneraseData(int );

void show_saveData();
void saveData(List L, FILE *);

void show_findTruyen();
void show_choosefindTruyen(int choose);
Position findTruyen(List, char *, Position, Position);
void show_find(List L, Position p);

// == show block để sử dụng function ==
void commandBlock();

// =============== MAIN ===================
int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    FixAndLockConsole();

    FILE *f = fopen ("data\\dataTruyen.txt", "r+");

    List TRUYEN88 = createList(f);
    if (getdataTruyen(TRUYEN88, f) == FALSE)
    {
        printf ("Errol!!!");
        system("pause");
        return FALSE;
    }

    Title();

    Position page = 1; int choose;
    showList(TRUYEN88, page);
    
    while (1)
    {
        showFunc();
        commandBlock(); 
        gotoxy(13, 71);
        scanf (" %d", &choose);
        gotoxy(13, 71);
        printf ("         ");

        switch (choose)
        {
            case 1: 
            {
                gotoxy (0, 48);
                show_addData();
                
                int _returnaddData = addData(TRUYEN88);
                show_returnaddData(_returnaddData);
                
                gotoxy (0, 17);
                showList(TRUYEN88, page);
                
                if (_returnaddData == FALSE) gotoxy(85, 60);
                else gotoxy(65, 60);
                
                Sleep(5000);

                gotoxy (0, 48);
                break;
            }
            case 2:
            {
                gotoxy (0, 48);
                show_eraseData();
            
                gotoxy(83, 57);

                char ID[20];
                scanf (" %s", ID);

                Position p = findTruyen(TRUYEN88, ID, 1, 0);
                int _returneraseData = (p == NOT_FOUND || p == FALSE)? FALSE : TRUE;
                
                if (_returneraseData == TRUE)
                    eraseData(TRUYEN88, p);

                show_returneraseData(_returneraseData);

                gotoxy (0, 17);
                showList(TRUYEN88, page);
                
                if (_returneraseData == FALSE) gotoxy(82, 61);
                else gotoxy(64, 61);
                
                Sleep(5000);

                gotoxy (0, 48);
                break;
            }
            case 3:
                f = fopen ("data\\dataTruyen.txt", "w");
                saveData(TRUYEN88, f);
                
                gotoxy (0, 48);
                show_saveData();

                gotoxy(66, 57);

                for (int i = 1; i <= 107; i++)
                {
                    if (i <= 20) Sleep(100);
                    else if (i <= 70) Sleep(300);
                    else if (i <= 90) Sleep(70);
                    else Sleep(20);
                    printf ("█");
                }
                fflush(f);
                gotoxy (0, 48);
                break;
            case 4:
                int choose_findTruyen;
                gotoxy (0, 48);
                show_findTruyen();
                while (1) 
                {
                    gotoxy(78, 58);
                    
                    scanf (" %d", &choose_findTruyen);
                    if (choose_findTruyen == 3) break;

                    show_choosefindTruyen(choose_findTruyen);
                    char mess[200];
                    scanf (" %[^\n]", mess);

                    gotoxy(67, 58);
                    printf ("🟩 Choose:                                                                                                ");
                    gotoxy(78, 58);

                    Position p = findTruyen(TRUYEN88, mess, choose_findTruyen % 2, choose_findTruyen % 2);

                    show_find(TRUYEN88, p);
                    
                }
                gotoxy (0, 48);
                break;
            case 5:
                if (page != 1) page--;
                gotoxy (0, 17);
                showList(TRUYEN88, page); 
                break;
            case 6:
                if (page != TRUYEN88 -> last_page) page++;
                gotoxy (0, 17);
                showList(TRUYEN88, page); 
                break;
            case 7:
                if (page != 1) page = 1;
                gotoxy (0, 17);
                showList(TRUYEN88, page); 
                break;
            case 8:
                if (page != TRUYEN88 -> last_page) page = TRUYEN88 -> last_page;
                gotoxy (0, 17);
                showList(TRUYEN88, page); 
                break;
            default:
                free(TRUYEN88->dataTruyen);
                fclose(f);
                system("cls");
                if (system("build\\main.exe 2>nul") != 0)
                {
                    system("cls");
                    system("main.exe 2>nul");
                }
                return 0;
        }
    }
}

// == show block để sử dụng function ==
void commandBlock() // Tổng 9 line
{
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║ 🟩 Choose:                                                                                                                                                                                                                               ║\n");
    printf("╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝");
}

// == show chức năng quản lí ==
void showFunc() // Tổng 16 line
{
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                              ╔════════════════════════════════════════════╗                                                                                              ║\n");
    printf("║                                                                                              ║   HỆ THỐNG QUẢN LÝ CHO THUÊ TRUYỆN TRANH   ║                                                                                              ║\n");
    printf("║                                                                                              ╚════════════════════════════════════════════╝                                                                                              ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                           [1] ➕ Thêm truyện                            [3] ⏫ Lưu dữ liệu                                          [5] ◀️ Trang trước                                        [7] ⏮️ Trang đầu                           ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                           [2] ➖ Xóa truyện                             [4] 🔎 Tìm kiếm truyện                                      [6] ▶️ Trang tiếp theo                                    [8] ⏭️ Trang cuối                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("╠══════════════════════════════════════════════════════════╦════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╦══════════════════════════════════════════════════════════╣\n");
    printf("║                                                          ║                                                                                                                    ║                                                          ║\n");
    printf("║                                                          ║                                                [Another key] ❌ Exit                                               ║                                                          ║\n");
    printf("║                                                          ║                                                                                                                    ║                                                          ║\n");
    printf("╠══════════════════════════════════════════════════════════╩════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╩══════════════════════════════════════════════════════════╣\n");
}

void show_addData() // Thêm dữ liệu // 16 line
{
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                            Thêm Truyện                                                                                                                   ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                ╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════╗                                                             ║\n");
    printf("║                                                                ║ Tên truyện   [🏷️]:                                                                                        ║                                                             ║\n");
    printf("║                                                                ╠═══════════════════════════════════════════════════════════════════════════════════════════════════════════╣                                                             ║\n");
    printf("║                                                                ║ Số lượng     [🔢]:                                                                                        ║                                                             ║\n");
    printf("║                                                                ╠═══════════════════════════════════════════════════════════════════════════════════════════════════════════╣                                                             ║\n");
    printf("║                                                                ║ Giá/ngày     [🪙]:                                                                                        ║                                                             ║\n");
    printf("║                                                                ╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════╝                                                             ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
}
int addData(List L) // Thêm dữ liệu
{
    if (L -> count == L -> capacity) return FALSE;
    L -> count++;
    Element e;
    
    gotoxy(86, 52);
    scanf (" %[^\n]", e.Name);
    gotoxy(86, 54);
    scanf (" %d", &e.Quantity);
    gotoxy(86, 56);
    scanf (" %lf", &e.Price_day);

    normalize(e.Name, e.normName);
    
    int ID;
    if (L -> count == 1) ID = 1;
    else ID = atoi (L -> dataTruyen[L -> count - 1].ID) + 1; 
    sprintf(e.ID, "%06d", ID);

    if (e.Quantity > 0) strcpy(e.State, "Còn");
    else strcpy(e.State, "Hết");

    e.Soluotmuon = 0;
    
    e.lenName = lenName(e.Name);

    trim(e.Name);
    trim(e.normName);
    trim(e.ID);
    trim(e.State);

    L -> dataTruyen[L -> count] = e;
    L -> last_page = (L -> count / 15 == 0)? L -> count / 15 : L -> count / 15 + 1;
    return TRUE;
}
void show_returnaddData(int _returnaddData)
{
    gotoxy(0, 59);
    if (_returnaddData == FALSE)
    {
        printf("║                                     ╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗                                     ║\n");
        printf("║                                     ║ Số lượng đã đạt giới hạn, không thể thêm !!!!                                                                                                                ║                                     ║\n");
        printf("║                                     ╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝                                     ║\n");
        gotoxy(85, 60);
    }
    else
    {
        printf("║                                     ╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗                                     ║\n");
        printf("║                                     ║ Đã thêm thành công truyện                                                                                                                                    ║                                     ║\n");
        printf("║                                     ╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝                                     ║\n");
        gotoxy(65, 60);
    }
}

void show_eraseData() // Xóa dữ liệu // 16 line
{
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                            Xóa Truyện                                                                                                                    ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                ╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════╗                                                             ║\n");
    printf("║                                                                ║ Mã truyện [🏷️]:                                                                                           ║                                                             ║\n");
    printf("║                                                                ╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════╝                                                             ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
}
void eraseData(List L, Position p) // Xóa dữ liệu
{
    for (int i = p; i < L -> count; i++)
        L -> dataTruyen[i] = L -> dataTruyen[i + 1];

    L -> count--;
}
void show_returneraseData(int _returneraseData)
{
    gotoxy(0, 60);
    if (_returneraseData == FALSE)
    {
        printf("║                                     ╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗                                     ║\n");
        printf("║                                     ║ Xóa truyện không thực hiện thành công !!!!                                                                                                                   ║                                     ║\n");
        printf("║                                     ╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝                                     ║\n");
        gotoxy(82, 61);
    }
    else
    {
        printf("║                                     ╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗                                     ║\n");
        printf("║                                     ║ Đã xóa thành công truyện                                                                                                                                     ║                                     ║\n");
        printf("║                                     ╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝                                     ║\n");
        gotoxy(64, 61);
    }
}

void show_saveData()
{
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                            Lưu dữ liệu truyện                                                                                                            ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                ╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════╗                                                             ║\n");
    printf("║                                                                ║                                                                                                           ║                                                             ║\n");
    printf("║                                                                ╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════╝                                                             ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
}
void saveData(List L, FILE *tmp) // Save dữ liệu
{

    fprintf (tmp, "Count: %d\n", L -> count);
    fprintf (tmp, "Capacity: %d\n", L -> capacity);

    Element e;
    for (int i = 1; i <= L -> count; i++)
    {
        e = L -> dataTruyen[i];
        fprintf(tmp, "%s | ", e.ID);
        fprintf(tmp, "%*s | ", -((strlen(e.Name) - e.lenName) + 30), e.Name);
        fprintf(tmp, "%*s | ", -30, e.normName);
        fprintf(tmp, "%03d | ", e.Quantity);
        fprintf(tmp, "%03d | ", e.Soluotmuon);
        fprintf(tmp, "%*s | ", -((strlen(e.State) - lenName(e.State)) + 3), e.State);
        fprintf(tmp, "%6.3lf\n", e.Price_day);
    }
}

void show_findTruyen()
{
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                             Tìm Truyện                                                                                                                   ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                ╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════╗                                                             ║\n");
    printf("║                                                                ║                                         [1] Tìm truyện theo mã truyện                                     ║                                                             ║\n");
    printf("║                                                                ╠═══════════════════════════════════════════════════════════════════════════════════════════════════════════╣                                                             ║\n");
    printf("║                                                                ║                                         [2] Tìm truyện theo tên truyện                                    ║                                                             ║\n");
    printf("║                                                                ╠═══════════════════════════════════════════════════════════════════════════════════════════════════════════╣                                                             ║\n");
    printf("║                                                                ║                                         [3] Quay lại                                                      ║                                                             ║\n");
    printf("║                                                                ╠═══════════════════════════════════════════════════════════════════════════════════════════════════════════╣                                                             ║\n");
    printf("║                                                                ║ 🟩 Choose:                                                                                                ║                                                             ║\n");
    printf("║                                                                ╚═══════════════════════════════════════════════════════════════════════════════════════════════════════════╝                                                             ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
}
void show_choosefindTruyen(int choose)
{
    gotoxy(68, 58);
    if (choose == 1) printf ("Mã truyện   [🏷️]: ");
    else printf ("Tên truyện   [🏷️]: ");
    
}
Position findTruyen(List L, char *mess, Position _ID, Position _Name)
{
    if (L -> count == 0) return FALSE;
    Position cur = 1;
    if (_ID == 1)
    {
        for (cur; cur <= L -> count; cur++)
            if (strcmp(L -> dataTruyen[cur].ID, mess) == 0) return cur;
        return cur = NOT_FOUND;
    }
    else 
    {
        trim(mess);
        char normmess[200];

        normalize(mess, normmess);
        for (cur; cur <= L -> count; cur++)
            if (strcmp(L -> dataTruyen[cur].normName, normmess) == 0) return cur;
        return cur = NOT_FOUND;
    }
}
void show_find(List L, Position p)
{
    gotoxy(0, 61);
    if (p == NOT_FOUND)
    {
        printf("║                                     ╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗                                     ║\n");
        printf("║                                     ║ Giá trị trên không phù hợp !!!                                                                                                                               ║                                     ║\n");
        printf("║                                     ╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝                                     ║\n");
        return;
    }
    Element e = L -> dataTruyen[p];
    int len = e.lenName;
    printf("║                              ╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗                              ║\n");
    printf("║                              ║%*s %s %*s║ %s %*s║%*s %03d %*s║%*s %03d %*s║%*s %s %*s║%*s %6.3lf %*s║                              ║\n", 
                                                3, "", e.ID         , 3                 , "",
                                                       e.Name       , 108 - (len + 2)   , "",                                                
                                                2, "", e.Quantity   , 3                 , "",
                                                4, "", e.Soluotmuon , 5                 , "",
                                                3, "", e.State      , 3                 , "",
                                                1, "", e.Price_day  , 1                 , "");
    printf("║                              ╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝                              ║\n");
}


// == show List ==
void showList(List L, Position page) // Tổng 31 line
{
    int y = Col - 74;
    int count = 1;
    Element e;
    int len;
    
    while (count <= 15)
    {
        if ((page - 1)* 15 + count <= L -> count)
        {
            e = L -> dataTruyen[(page - 1) * 15 + count];
            len = e.lenName;
        }
        else {
            strcpy(e.ID, "------");
            strcpy(e.Name, "----------------------------------------------------------------------------------------------------------");
            len = 106;
            e.Quantity = 0;
            e.Soluotmuon = 0;
            strcpy(e.State, "---");
            e.Price_day = 0.000;
        }
        printf("║                              ╠══════════════╬════════════════════════════════════════════════════════════════════════════════════════════════════════════╬══════════╬══════════════╬═══════════╬══════════╣                              ║\n");
        printf("║                              ║%*s %s %*s║ %s %*s║%*s %03d %*s║%*s %03d %*s║%*s %s %*s║%*s %6.3lf %*s║                              ║\n", 
                                                3, "", e.ID         , 3                 , "",
                                                       e.Name       , 108 - (len + 2)   , "",                                                
                                                2, "", e.Quantity   , 3                 , "",
                                                4, "", e.Soluotmuon , 5                 , "",
                                                3, "", e.State      , 3                 , "",
                                                1, "", e.Price_day  , 1                 , "");
        count++;
    }
    printf("╠══════════════════════════════╩══════════════╩════════════════════════════════════════════════════════════════════════════════════════════════════════════╩══════════╩══════════════╩═══════════╩══════════╩══════════════════════════════╣\n");
}

// == show Title ==
void Title() // Tổng 17 line
{
    printf("╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                 ████████╗██████╗ ██╗   ██╗██╗   ██╗███████╗███╗   ██╗   █████╗   █████╗                                                                                  ║\n");
    printf("║                                                                                 ╚══██╔══╝██╔══██╗██║   ██║╚██╗ ██╔╝██╔════╝████╗  ██║  ██╔══██╗ ██╔══██╗                                                                                 ║\n");
    printf("║                                                                                    ██║   ██████╔╝██║   ██║ ╚████╔╝ █████╗  ██╔██╗ ██║  ╚█████╔╝ ╚█████╔                                                                                  ║\n");
    printf("║                                                                                    ██║   ██╔══██╗██║   ██║  ╚██╔╝  ██╔══╝  ██║╚██╗██║  ██╔══██╗ ██╔══██╗                                                                                 ║\n");
    printf("║                                                                                    ██║   ██║  ██║╚██████╔╝   ██║   ███████╗██║ ╚████║  ╚█████╔╝ ╚█████╔╝                                                                                 ║\n");
    printf("║                                                                                    ╚═╝   ╚═╝  ╚═╝ ╚═════╝    ╚═╝   ╚══════╝╚═╝  ╚═══╝   ╚════╝   ╚════╝                                                                                  ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("╠══════════════════════════════╦══════════════╦════════════════════════════════════════════════════════════════════════════════════════════════════════════╦══════════╦══════════════╦═══════════╦══════════╦══════════════════════════════╣\n");
    printf("║                              ║   Mã Truyện  ║                                                 Tên Truyện                                                 ║ Số lượng ║ Số lượt mượn ║Trạng thái ║ Giá/ngày ║                              ║\n");

}

// === Viet Hoa =====
int normalize(const char *word, char *normalword)
{
    int i = 0, j = 0;

    while (word[i] != '\0')
    {
        int len_word, len_result;

        normalize_vi(&word[i], &normalword[j], &len_word, &len_result);

        i += len_word;
        j += len_result;
    }

    normalword[j] = '\0';

    return TRUE;
}
int normalize_vi(const char *word, char *result, Position *pos, Position *pos_result)
{
    unsigned char c = word[0];
    // ===== a (cả hoa và thường) =====
    if (!strncmp(word, "á", 2) || !strncmp(word, "à", 2) || !strncmp(word, "ã", 2) ||
        !strncmp(word, "ả", 3) || !strncmp(word, "ạ", 3) ||
        !strncmp(word, "ă", 2) || !strncmp(word, "ắ", 3) || !strncmp(word, "ằ", 3) ||
        !strncmp(word, "ẳ", 3) || !strncmp(word, "ẵ", 3) || !strncmp(word, "ặ", 3) ||
        !strncmp(word, "â", 2) || !strncmp(word, "ấ", 3) || !strncmp(word, "ầ", 3) ||
        !strncmp(word, "ẩ", 3) || !strncmp(word, "ẫ", 3) || !strncmp(word, "ậ", 3) ||

        !strncmp(word, "Á", 2) || !strncmp(word, "À", 2) || !strncmp(word, "Ã", 2) ||
        !strncmp(word, "Ả", 3) || !strncmp(word, "Ạ", 3) ||
        !strncmp(word, "Ă", 2) || !strncmp(word, "Ắ", 3) || !strncmp(word, "Ằ", 3) ||
        !strncmp(word, "Ẳ", 3) || !strncmp(word, "Ẵ", 3) || !strncmp(word, "Ặ", 3) ||
        !strncmp(word, "Â", 2) || !strncmp(word, "Ấ", 3) || !strncmp(word, "Ầ", 3) ||
        !strncmp(word, "Ẩ", 3) || !strncmp(word, "Ẫ", 3) || !strncmp(word, "Ậ", 3)) {

        result[0] = 'a';
        if ((c & 0xE0) == 0xC0) *pos = 2;
        else if ((c & 0xF0) == 0xE0) *pos = 3;
        *pos_result = 1;
        return TRUE;
    }

    // ===== e =====
    if (!strncmp(word, "é", 2) || !strncmp(word, "è", 2) || !strncmp(word, "ẽ", 2) ||
        !strncmp(word, "ẻ", 3) || !strncmp(word, "ẹ", 3) ||
        !strncmp(word, "ê", 2) || !strncmp(word, "ế", 3) || !strncmp(word, "ề", 3) ||
        !strncmp(word, "ể", 3) || !strncmp(word, "ễ", 3) || !strncmp(word, "ệ", 3) ||

        !strncmp(word, "É", 2) || !strncmp(word, "È", 2) || !strncmp(word, "Ẽ", 2) ||
        !strncmp(word, "Ẻ", 3) || !strncmp(word, "Ẹ", 3) ||
        !strncmp(word, "Ê", 2) || !strncmp(word, "Ế", 3) || !strncmp(word, "Ề", 3) ||
        !strncmp(word, "Ể", 3) || !strncmp(word, "Ễ", 3) || !strncmp(word, "Ệ", 3)) {

        result[0] = 'e';
        if ((c & 0xE0) == 0xC0) *pos = 2;
        else if ((c & 0xF0) == 0xE0) *pos = 3;
        *pos_result = 1;
        return TRUE;
    }

    // ===== i =====
    if (!strncmp(word, "í", 2) || !strncmp(word, "ì", 2) || !strncmp(word, "ĩ", 2) ||
        !strncmp(word, "ỉ", 3) || !strncmp(word, "ị", 3) ||

        !strncmp(word, "Í", 2) || !strncmp(word, "Ì", 2) || !strncmp(word, "Ĩ", 3) ||
        !strncmp(word, "Ỉ", 3) || !strncmp(word, "Ị", 3)) {

        result[0] = 'i';
        if ((c & 0xE0) == 0xC0) *pos = 2;
        else if ((c & 0xF0) == 0xE0) *pos = 3;
        *pos_result = 1;
        return TRUE;
    }

    // ===== o =====
    if (!strncmp(word, "ó", 2) || !strncmp(word, "ò", 2) || !strncmp(word, "õ", 2) ||
        !strncmp(word, "ỏ", 3) || !strncmp(word, "ọ", 3) ||
        !strncmp(word, "ô", 2) || !strncmp(word, "ố", 3) || !strncmp(word, "ồ", 3) ||
        !strncmp(word, "ổ", 3) || !strncmp(word, "ỗ", 3) || !strncmp(word, "ộ", 3) ||
        !strncmp(word, "ơ", 2) || !strncmp(word, "ớ", 3) || !strncmp(word, "ờ", 3) ||
        !strncmp(word, "ở", 3) || !strncmp(word, "ỡ", 3) || !strncmp(word, "ợ", 3) ||

        !strncmp(word, "Ó", 2) || !strncmp(word, "Ò", 2) || !strncmp(word, "Õ", 2) ||
        !strncmp(word, "Ỏ", 3) || !strncmp(word, "Ọ", 3) ||
        !strncmp(word, "Ô", 2) || !strncmp(word, "Ố", 3) || !strncmp(word, "Ồ", 3) ||
        !strncmp(word, "Ổ", 3) || !strncmp(word, "Ỗ", 3) || !strncmp(word, "Ộ", 3) ||
        !strncmp(word, "Ơ", 2) || !strncmp(word, "Ớ", 3) || !strncmp(word, "Ờ", 3) ||
        !strncmp(word, "Ở", 3) || !strncmp(word, "Ỡ", 3) || !strncmp(word, "Ợ", 3)) {

        result[0] = 'o';
        if ((c & 0xE0) == 0xC0) *pos = 2;
        else if ((c & 0xF0) == 0xE0) *pos = 3;
        *pos_result = 1;
        return TRUE;
    }

    // ===== u =====
    if (!strncmp(word, "ú", 2) || !strncmp(word, "ù", 2) || !strncmp(word, "ũ", 2) ||
        !strncmp(word, "ủ", 3) || !strncmp(word, "ụ", 3) ||
        !strncmp(word, "ư", 2) || !strncmp(word, "ứ", 3) || !strncmp(word, "ừ", 3) ||
        !strncmp(word, "ử", 3) || !strncmp(word, "ữ", 3) || !strncmp(word, "ự", 3) ||

        !strncmp(word, "Ú", 2) || !strncmp(word, "Ù", 2) || !strncmp(word, "Ũ", 2) ||
        !strncmp(word, "Ủ", 3) || !strncmp(word, "Ụ", 3) ||
        !strncmp(word, "Ư", 2) || !strncmp(word, "Ứ", 3) || !strncmp(word, "Ừ", 3) ||
        !strncmp(word, "Ử", 3) || !strncmp(word, "Ữ", 3) || !strncmp(word, "Ự", 3)) {

        result[0] = 'u';
        if ((c & 0xE0) == 0xC0) *pos = 2;
        else if ((c & 0xF0) == 0xE0) *pos = 3;
        *pos_result = 1;
        return TRUE;
    }

    // ===== y =====
    if (!strncmp(word, "ý", 2) || !strncmp(word, "ỳ", 2) || !strncmp(word, "ỹ", 2) ||
        !strncmp(word, "ỷ", 3) || !strncmp(word, "ỵ", 3) ||

        !strncmp(word, "Ý", 2) || !strncmp(word, "Ỳ", 2) || !strncmp(word, "Ỹ", 2) ||
        !strncmp(word, "Ỷ", 3) || !strncmp(word, "Ỵ", 3)) {

        result[0] = 'y';
        if ((c & 0xE0) == 0xC0) *pos = 2;
        else if ((c & 0xF0) == 0xE0) *pos = 3;
        *pos_result = 1;
        return TRUE;
    }

    // ===== đ =====
    if (!strncmp(word, "đ", 2) || !strncmp(word, "Đ", 2)) {
        result[0] = 'd';
        *pos = 2;
        *pos_result = 1;
        return TRUE;
    }

    // ===== ASCII =====
    if (c < 0x80) {
        result[0] = (c >= 'A' && c <= 'Z') ? c + 32: c;
        *pos = 1;
        *pos_result = 1;
        return TRUE;
    }

    // ===== UTF-8 khác =====
    if ((c & 0xE0) == 0xC0) *pos = 2;
    else if ((c & 0xF0) == 0xE0) *pos = 3;
    else if ((c & 0xF8) == 0xF0) *pos = 4;
    else *pos = 1;

    // copy nguyên ký tự (emoji, ký tự lạ...)
    for (int i = 0; i < *pos; i++) {
        result[i] = word[i];
    }
    *pos_result = *pos;
    return TRUE;
}

// == Đếm độ dài thực tế của tên truyện ==
int lenName(const char *Name)
{
    int i = 0, len = 0;
    while (Name[i] != '\0')
    {
        if ((Name[i] & 0x80) == 0) 
            i++;
        else if ((Name[i] & 0xE0) == 0xC0) 
            i += 2;
        else if ((Name[i] & 0xF0) == 0xE0) 
            i += 3;
        else if ((Name[i] & 0xF8) == 0xF0) 
            i += 4;
        
        len++;
    }
    return len;
}

// === Lọc khoảng trắn ===
int trim(char *c)
{
    int len_c = strlen(c);
    int i = len_c - 1;
    while ((i >= 0) && (c[i] == ' ' || c[i] == '\t' || c[i] == '\n'))
    {
        c[i] = 0;
        i--;
    }
    if (i == 0) return TRUE;
    
    len_c = strlen(c);
    i = 0;
    while (c[i] == ' ' || c[i] == '\t') i++;

    for (int j = 0; j + i <= len_c; j++)
        c[j] = c[j + i];

    return TRUE;
}

// == Tạo danh sách truyện để quản lí ==
List createList (FILE *f)
{
    List L = malloc (sizeof(struct ListInfo));
    
    if (L == NULL)
        return NULL;
    
    int count, capacity;

    /*
        fscanf (f, " %*[^0-9]%d", &count);
        fscanf (f, " %*[^0-9]%d", &capacity);
    */
    fscanf (f, " Count: %d", &count);
    fscanf (f, " Capacity: %d", &capacity);
    
    L -> count = count;
    L -> capacity = capacity;
    L -> last_page = (count % 15 == 0)? count / 15: count / 15 + 1;
    L -> dataTruyen = malloc (sizeof(struct ElementType) * (capacity + 1));

    if (L -> dataTruyen == NULL)
        return NULL;

    return L;
}
int getdataTruyen(List L, FILE *f)
{
    Position cur = 1;
    Element e;
    while (cur <= L -> count) 
    {
        fscanf (f," %[^|]| %[^|]| %[^|]| %d | %d | %[^|]| %lf",
            e.ID, e.Name, e.normName, &e.Quantity, &e.Soluotmuon, e.State, &e.Price_day);

        trim(e.ID);
        trim(e.Name);
        trim(e.normName);
        trim(e.State);

        e.lenName = lenName(e.Name);

        L -> dataTruyen[cur] = e;
        cur++;
    }
    return TRUE;
}

/*  ===============================================
    =         Điều khiển máy tính                 =
    ===============================================*/
// == set color ==
void setColor(int Color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Color);
}

// == Điều chỉnh con trỏ của console ==
void gotoxy(int x, int y) 
{
    COORD coord;
    coord.X = x; 
    coord.Y = y; 
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// == Khóa khung console ==
void FixAndLockConsole() 
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    HWND hwnd = GetConsoleWindow();

    // 1. Phóng to cửa sổ trước
    ShowWindow(hwnd, SW_MAXIMIZE);
    
    // Đợi 1 chút để Windows ổn định kích thước cửa sổ mới
    Sleep(200); 

    // 2. Lấy thông tin chính xác của cửa sổ sau khi đã phóng to
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);

    // Tính toán kích thước hiện tại của cửa sổ (tính bằng số ký tự)
    short winWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    short winHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    // 3. ÉP Buffer phải bằng khít với Window
    // Điều này sẽ triệt tiêu hoàn toàn thanh cuộn
    COORD newSize;
    newSize.X = winWidth;
    newSize.Y = winHeight;
    
    // Thực hiện ép kích thước bộ đệm
    if (!SetConsoleScreenBufferSize(hOut, newSize)) {
        // Nếu lỗi, có thể do winHeight quá lớn so với giới hạn font
        // Ta thử lại với giá trị an toàn hơn từ csbi
        SetConsoleScreenBufferSize(hOut, newSize);
    }

    // 4. Khóa cứng viền không cho kéo giãn
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~WS_THICKFRAME; // Chặn kéo viền
    style &= ~WS_MAXIMIZEBOX; // Chặn nút phóng to (vì đã max rồi)
    SetWindowLong(hwnd, GWL_STYLE, style);

    // Cập nhật thay đổi
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
}