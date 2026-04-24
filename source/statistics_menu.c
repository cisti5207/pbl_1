#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USER 10000
#define TRUE 1
#define FALSE 0
#define NOT_FOUND -1

struct ElementTruyen 
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
struct ListTruyen 
{
    struct ElementTruyen *dataTruyen;
    int count;
    int capacity;
    int Tong_Soluotmuon;
};

struct ElementUser
{
    char ID_User[20];
    char Name[200];
    char ID_Truyen[20];
    char NameTruyen[200];
    int Soluongmuon;
    char Date_muon[20];
    char Date_tra[20];
    int Songaymuon;
    double Tiencoc;
    char State[10];
};

typedef struct ElementUser ElementUser;
typedef struct ElementTruyen ElementTruyen;
typedef struct ListTruyen *TRUYEN88;
typedef int Position;

ElementUser _User[MAX_USER];

// === Khung console ===
void Khung();

// Set color UI
void setColorUI(int Text, int Background);

// == Set pos con tro ==
void gotoxy(int x, int y);

// == Khoa console ==
void FixAndLockConsole();

int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);  

    FixAndLockConsole();

    Khung();

    gotoxy (20, 71);
    getchar();
    return 0;
}

void Khung()
{
    printf("╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                    ████████╗██╗  ██╗ ██████╗ ███╗   ██╗ ██████╗     ██╗  ██╗███████╗                                                                                     ║\n");
    printf("║                                                                                    ╚══██╔══╝██║  ██║██╔═══██╗████╗  ██║██╔════╝     ██║ ██╔╝██╔════╝                                                                                     ║\n");
    printf("║                                                                                       ██║   ███████║██║   ██║██╔██╗ ██║██║  ███╗    █████╔╝ █████╗                                                                                       ║\n");
    printf("║                                                                                       ██║   ██╔══██║██║   ██║██║╚██╗██║██║   ██║    ██╔═██╗ ██╔══╝                                                                                       ║\n");
    printf("║                                                                                       ██║   ██║  ██║╚██████╔╝██║ ╚████║╚██████╔╝    ██║  ██╗███████╗                                                                                     ║\n");
    printf("║                                                                                       ╚═╝   ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝     ╚═╝  ╚═╝╚══════╝                                                                                     ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
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
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                          [1 - 📊] Thống kê mượn trả                                            [2 - 📊] Thống kê doanh thu                                           [3 - ❌] Exit                                       ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║ 🟩 Choose:                                                                                                                                                                                                                               ║\n");
    printf("╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝");
}

void statistics_muontra()
{

}

void statistics_muonTruyen(TRUYEN88 L) // top 3 truyện được mượn nhiều nhất -- chỉ mới tìm 3 vị trí, chưa show và thiết kế UI
{
    Position top[3] = {1, 2, 3};
    int Tong_Soluotmuon = L -> Tong_Soluotmuon;

    for (Position p = 4; p <= L -> count; p++)
    {
        if (p == top[0] || p == top[1] || p == top[2]) continue;

        if (L -> dataTruyen[p].Soluotmuon > L -> dataTruyen[top[0]].Soluotmuon) top[0] = p;
        else if (L -> dataTruyen[p].Soluotmuon > L -> dataTruyen[top[1]].Soluotmuon) top[1] = p;
        else if (L -> dataTruyen[p].Soluotmuon > L -> dataTruyen[top[2]].Soluotmuon) top[2] = p;
    }

    for(int i = 0; i < 3; i++)
        top[i] = L -> dataTruyen[top[i]].Soluotmuon * 50 / Tong_Soluotmuon / 2;

    int t;
    do
    {
        t = 1;
        for (int i = 0; i < 3; i++)
        {
            top[i] = top[i] * 2;
            if (top[i] < 1) t = 0;
        }
    } while(t == 0);
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

// == Tạo danh sách truyện ==
TRUYEN88 createTRUYEN88 (FILE *_readdataTruyen)
{
    TRUYEN88 L = malloc (sizeof(struct ListTruyen));
    
    if (L == NULL)
        return NULL;
    
    int count, capacity;

    /*
        fscanf (_readdataTruyen, " %*[^0-9]%d", &count);
        fscanf (_readdataTruyen, " %*[^0-9]%d", &capacity);
    */
    fscanf (_readdataTruyen, " Count: %d", &count);
    fscanf (_readdataTruyen, " Capacity: %d", &capacity);
    
    L -> count = count;
    L -> capacity = capacity;
    L -> Tong_Soluotmuon = 0;
    L -> dataTruyen = malloc (sizeof(struct ElementTruyen) * (capacity + 1));

    if (L -> dataTruyen == NULL)
        return NULL;

    return L;
}
int getdataTruyen(TRUYEN88 L, FILE *_readdataTruyen)
{
    Position cur = 1;
    ElementTruyen e;

    while (cur <= L -> count) 
    {
        fscanf (_readdataTruyen," %[^|]| %[^|]| %[^|]| %d | %d | %[^|]| %lf",
            e.ID, e.Name, e.normName, &e.Quantity, &e.Soluotmuon, e.State, &e.Price_day);

        trim(e.ID);
        trim(e.Name);
        trim(e.normName);
        trim(e.State);

        e.lenName = lenName(e.Name);

        L -> dataTruyen[cur] = e;
        L -> Tong_Soluotmuon += e.Soluotmuon;
        cur++;
    }

    return TRUE;
}

// == Tạo danh sách người dùng ==
int getdataUser(int *Quantity)
{
    FILE *_readdataUser = fopen("\\data\\dataUser.txt", "r");
    ElementUser e;

    int count = 1;

    while (1)
    {
        if (fscanf (_readdataUser, " %[^|]| %[^|]| %[^|]| %[^|]| %d | %[^|]| %[^|]| %d | %lf | %19[^\n]",
        e.ID_User, e.Name, e.ID_Truyen, e.NameTruyen, &e.Soluongmuon, e.Date_muon, e.Date_tra, &e.Songaymuon, &e.Tiencoc, e.State) != 9) break;

        trim(e.ID_User);
        trim(e.Name);
        trim(e.ID_Truyen);
        trim(e.NameTruyen);
        trim(e.Date_muon);
        trim(e.Date_tra);
        trim(e.State);

        _User[count++] = e;
    }

    *Quantity = count - 1;
    fclose(_readdataUser);
    return TRUE;
}

/* == Set color ==
| Giá trị | Tên màu (EN)       | Mô tả           |
| ------- | ------------------ | --------------- |
| 0       | Black              | Đen             |
| 1       | Blue               | Xanh dương đậm  |
| 2       | Green              | Xanh lá đậm     |
| 3       | Aqua (Cyan)        | Xanh cyan       |
| 4       | Red                | Đỏ đậm          |
| 5       | Purple (Magenta)   | Tím             |
| 6       | Yellow (Brown)     | Vàng đậm        |
| 7       | White (Light Gray) | Trắng xám       |
| 8       | Gray               | Xám             |
| 9       | Light Blue         | Xanh dương sáng |
| 10      | Light Green        | Xanh lá sáng    |
| 11      | Light Aqua         | Cyan sáng       |
| 12      | Light Red          | Đỏ sáng         |
| 13      | Light Purple       | Tím sáng        |
| 14      | Light Yellow       | Vàng sáng       |
| 15      | Bright White       | Trắng sáng      |
*/
void setColorUI(int Text, int Background)
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(h, Text | (Background << 4));
}

// == Control con trỏ ==
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