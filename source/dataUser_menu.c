#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#define TRUE  1
#define FALSE 0
#define NOT_FOUND -1
typedef int Position;
int trim(char *c)
{
    int i = strlen (c);
    while ((i >= 0) && (c[i] == ' ' || c[i] == '\t'))
    {
        c[i] = 0;
        i--;
    }
    return TRUE;
}
int normalize(char *word, char *normalword)
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

//Struct lưu dữ liệu của từng người thuê.
struct Node{
  char maphieuthue[10];
  char realname[100];
  char normalname[100];
  char matruyen[10];
  char tentruyenreal[100];
  char tentruyennormal[100];
  char ngaythue[20];
  char ngaytra[20];
  int songaythue;
  int soluongthue;
  int tiencoc;
  int trangthai;
  struct Node *next;
};
typedef struct Node User;
//Hàm tạo user;
User *TaoUser() {
    //1. Cấp phát bộ nhớ và kiểm tra xem liệu RAM còn chỗ hay ko.
    User *NewNode = (User*)malloc(sizeof(User));
    if(NewNode == NULL) {
        printf("Lỗi không đủ bộ nhớ để cấp phát");
        return NULL;
    };
    NewNode->trangthai = 0;
    //2. Nhập mã phiếu thuê
    printf("Nhập mã phiếu thuê(Bao gồm 6 số): ");
    scanf("%s", NewNode->maphieuthue);
      while(getchar()!= '\n');
    //3.Nhập tên người thuê:
    printf("Nhập tên người thuê: ");
    fgets(NewNode->realname,100, stdin);  
    trim(NewNode->realname);
    normalize(NewNode->realname, NewNode->normalname);
    //4.Nhập mã truyện:
    printf("Nhập mã truyện: ");
    scanf("%s", NewNode->matruyen);
      while(getchar()!= '\n');
    //5. Nhập tên truyện:
    fgets(NewNode->tentruyenreal, 100, stdin);
    trim(NewNode->tentruyenreal);
    normalize(NewNode->tentruyenreal, NewNode->tentruyennormal);
    //6.Ngày thuê, ngày trả, số ngày thuê:
    printf("Nhập ngày thuê: ");
    scanf("%s", NewNode->ngaythue);
      while(getchar()!= '\n');
    printf("Số ngày thuê: ");
    scanf("%d", NewNode->songaythue);
      while(getchar()!= '\n');
    //7. Nhập số lượng thuê:
    printf("Nhập số lượng thuê: ");
    scanf("%d", NewNode-> soluongthue);
    while(getchar()!= '\n');
    NewNode->next = NULL;
    return NewNode;
}
void themUser(User **head) {
    User *NewNode = TaoUser();
    if(NewNode == NULL) return;
    if(*head == NULL) {
        *head = NewNode;
        return;
}
 User *temp = *head;
    while (temp->next != NULL) {
        temp = temp->next; 
    }

    temp->next = NewNode;
}
void xoaUser(User **head, char*mathuecanxoa) {
    if(*head == NULL) return;
    User *p = *head;
    User *q = NULL;
    while(p != NULL && strcmp(p->maphieuthue, mathuecanxoa) !=0) {
        q=p;
        p=p->next;
    }
    if(p == NULL) {
    printf("Không tìm thấy phiếu thuê.");
    return;
}
if (q == NULL) {
        *head = p->next; 
    }
    else {
        q->next  = p -> next;
    }
    free(p);
}
int tinhKhoangCach(User *p) {
    struct tm start = {0}, end = {0};
    int d1, m1, y1, d2, m2, y2;

    // Lấy ngày thuê có sẵn trong Node và ngày trả vừa được cập nhật
    if (sscanf(p->ngaythue, "%d/%d/%d", &d1, &m1, &y1) != 3) return 0;
    if (sscanf(p->ngaytra, "%d/%d/%d", &d2, &m2, &y2) != 3) return 0;

    // Nạp vào bộ máy tính toán của C
    start.tm_mday = d1; start.tm_mon = m1 - 1; start.tm_year = y1 - 1900;
    end.tm_mday = d2;   end.tm_mon = m2 - 1;   end.tm_year = y2 - 1900;

    time_t t1 = mktime(&start);
    time_t t2 = mktime(&end);

    double seconds = difftime(t2, t1);
    int soNgay = (int)(seconds / 86400);

    return (soNgay < 0) ? 0 : soNgay;
}
void xuLyTraSach(User *head) {
    char maCanTim[10];
    char ngayTraMoi[20];
//1. Nhập mã phiếu thuê cần trả(sẽ tìm sau khi search danh sách người chưa trả)
    printf("\n[>] Nhập mã phiếu thuê cần trả: ");
    fflush(stdin);
    scanf("%s", maCanTim);
    User *p = head;
    while (p != NULL) {
        if (strcmp(p->maphieuthue, maCanTim) == 0 && p->trangthai == 0) {
            break; 
        }
        p = p->next;
    }

    if (p == NULL) {
        printf("[!] Không tìm thấy mã phiếu đang thuê này!\n");
        return;
    }

    // 2. Nhập ngày trả thực tế để chuẩn bị tính toán
    printf("[>] Nhập ngày trả thực tế (dd/mm/yyyy): ");
    scanf("%s", ngayTraMoi);

    // 3. Cập nhật trạng thái và ngày trả vào Node
    p->trangthai = 1; 
    strcpy(p->ngaytra, ngayTraMoi);

    
    printf("\n Đã trả sách.");
}
void inDanhSachChuaTra(User *head) {
    printf("\n┌──────┬───────────────────────────────────┬──────┬───────────────────────────────────┬───┬──────────┬───┬──────────┬───────────────┐");
    
    printf("\n│ %-4s │ %-33s │ %-4s │ %-33s │ %-1s │ %-8s │ %-1s │ %-8s │ %-13s │", 
           "Mã User", "Tên người thuê", "Mã truyện", "Tên truyện", "Số lượng", "Ngày thuê", "Số ngày", "Tiền cọc", "Trạng thái");
    
    printf("\n├──────┼───────────────────────────────────┼──────┼───────────────────────────────────┼───┼──────────┼───┼──────────┼───────────────┤");

    User *p = head;
    int count = 0;

    while (p != NULL) {
        if (p->trangthai == 0) {
            printf("\n│ %-4s │ %-33s │ %-4s │ %-33s │ %-1d │ %-8s │ %-1d │ %-8d │ %-13s │", 
                   p->maphieuthue, 
                   p->realname, 
                   p->matruyen, 
                   p->tentruyenreal, 
                   p->soluongthue, 
                   p->ngaythue, 
                   p->songaythue, 
                   p->tiencoc,
                   "Chưa trả");
            count++;
        }
        p = p->next;
    }

    printf("\n└──────┴───────────────────────────────────┴──────┴───────────────────────────────────┴───┴──────────┴───┴──────────┴───────────────┘");

        printf("\n[💡] Tổng cộng có %d người chưa trả.", count);
    printf("\n");
}


int main() {
    User *head = NULL;
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001); 
}