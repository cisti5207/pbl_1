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

//=========================================================================

// == Cấu trúc của một danh sách truyên ==
struct ElementType 
{
    char ID[8];
    char Name[200];
    char normName[200];
    char Quantity[5];
    char State[10];
    char Price_day[10];
    int lenName;
};
struct ListInfo 
{
    struct ElementType *dataTruyen;
    int count;
    int capacity;
};

typedef struct ElementType Element;
typedef struct ListInfo *List;
typedef int Position;

// == Tạo danh sách truyên ==
List createList (FILE *f);
int getdataTruyen(List L, FILE *f);

// == Lọc khoảng trắng ==
int trim(char *c);

// == Đếm độ dài thực tế của tên truyện ==
int lenName(const char *Name);

// == chuyển dấu thành không dấu ==
int normalize(const char *word, char *normalword);
int normalize_vi(const char *word, char *result, Position *pos, Position *pos_result);

// == show List ==
void showList(List L, Position page);

// == show Title ==
void Title();

// == show chức năng quản lí ==
void showFunc();

// == show block để sử dụng function ==
void commandBlock();

// =============== MAIN ===================
int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    FixAndLockConsole();

    FILE *f = fopen ("data\\dataTruyen.txt", "r+");
    if (f == NULL)
    {
        f = fopen ("..\\data\\dataTruyen.txt", "r+");
        if (f == NULL) 
        {
            printf ("Errol!!");
            system("pause");
            return FALSE;
        }
    }

    List TRUYEN88 = createList(f);
    if (getdataTruyen(TRUYEN88, f) == FALSE)
    {
        printf ("Errol!!!");
        system("pause");
        return FALSE;
    }

    Title();

    Position page = 1;

    while (1)
    {
        while (1)
        {
            showList(TRUYEN88, page);
            showFunc();
            commandBlock();
            gotoxy(13, 70);
            break;
        }
        break;
    }
    getchar();
    return TRUE;
    
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
    printf("╠═════════════════════════════════════╦══════════════╦════════════════════════════════════════════════════════════════════════════════════════════════════════════╦══════════╦════════════╦══════════╦═════════════════════════════════════╣\n");
    printf("║                                     ║   Mã Truyện  ║                                                 Tên Truyện                                                 ║ Số lượng ║ Trạng thái ║ Giá/ngày ║                                     ║\n");
}

// == show List ==
void showList(List L, Position page) // Tổng 31 line
{
    int y = Col - 74;
    int count = 1;
    Element e;
    
    while (count <= 15)
    {
        e = L -> dataTruyen[(page - 1) * 10 + count];
        int len = e.lenName;
        printf("║                                     ╠══════════════╬════════════════════════════════════════════════════════════════════════════════════════════════════════════╬══════════╬════════════╬══════════╣                                     ║\n");
        printf("║                                     ║%*s %s %*s║ %s %*s║%*s %s %*s║%*s %s %*s║%*s %s %*s║                                     ║\n", 
                                                3, "", e.ID         , 3                 , "",
                                                       e.Name       , 108 - (len + 2)   , "",                                                
                                                2, "", e.Quantity   , 3                 , "",
                                                3, "", e.State      , 4                 , "",
                                                1, "", e.Price_day  , 1                 , "");
        count++;
    }
    printf("╠═════════════════════════════════════╩══════════════╩════════════════════════════════════════════════════════════════════════════════════════════════════════════╩══════════╩════════════╩══════════╩═════════════════════════════════════╣\n");
}

// == show chức năng quản lí ==
void showFunc() // Tổng 19 line
{
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                               ╔════════════════════════════════════════════╗                                                                                                             ║\n");
    printf("║                                                                               ║   HỆ THỐNG QUẢN LÝ CHO THUÊ TRUYỆN TRANH   ║                                                                                                             ║\n");
    printf("║                                                                               ╚════════════════════════════════════════════╝                                                                                                             ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                           [1] ➕ Thêm truyện                            [3] ⏫ Lưu dữ liệu                                          [5] ◀️ Trang trước                                        [7] ⏮️ Trang đầu                           ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                           [2] ➖ Xóa truyện                             [4] 🔄️ Làm mới dữ liệu                                     [6] ▶️ Trang tiếp theo                                    [8] ⏭️ Trang cuối                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("╠══════════════════════════════════════════════════════════╦════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╦══════════════════════════════════════════════════════════╣\n");
    printf("║                                                          ║                                                [9] 🔎 Tìm kiếm truyện                                              ║                                                          ║\n");
    printf("║                                                          ╠════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣                                                          ║\n");
    printf("║                                                          ║                                                [10] ❌ Exit                                                        ║                                                          ║\n");
    printf("╠══════════════════════════════════════════════════════════╩════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╩══════════════════════════════════════════════════════════╣\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
}

void addData() // Thêm dữ liệu // 19 line
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
}

void eraseData()
{}














// == show block để sử dụng function ==
void commandBlock() // Tổng 6 line
{
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║                                                                                                                                                                                                                                          ║\n");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║ 🟩 Choose:                                                                                                                                                                                                                               ║\n");
    printf("╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n");
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
    L -> dataTruyen = malloc (sizeof(struct ElementType) * (capacity + 1));

    if (L -> dataTruyen == NULL)
        return NULL;

    return L;
}
int getdataTruyen(List L, FILE *f)
{
    Position cur = 1;
    Element e;
    while (1) 
    {
        if (fscanf (f," %[^|]| %[^|]| %[^|]| %[^|]| %[^|]| %[^\n] ",
            e.ID, e.Name, e.normName, e.Quantity, e.State, e.Price_day) != 6)
            return FALSE;

        trim(e.ID);
        trim(e.Name);
        trim(e.normName);
        trim(e.Quantity);
        trim(e.State);
        trim(e.Price_day);

        e.lenName = lenName(e.Name);

        L -> dataTruyen[cur] = e;

        if (cur == L -> count) break;
        cur++;
    }
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

// === Tim Truyen =====
int findTruyen(FILE *f, List L)
{
    char name_find[200];
    scanf (" %[^\n]", name_find);


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

// == Điều chỉnh con trỏ của console ==
void gotoxy(int x, int y) 
{
    COORD coord;
    coord.X = x; 
    coord.Y = y; 
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}