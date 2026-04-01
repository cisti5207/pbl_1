#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

#define TRUE  1
#define FALSE 0
#define NOT_FOUND -1

typedef int Position;

int trim(char *c);
int normalize(char *word, char *normalword);
int normalize_vi(const char *word, char *result, Position *pos, Position *pos_result);


int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);   
    while (1)
    {
        char x[200], normalizex[200];
        printf ("Nhap ten truyen: ");
        fgets (x, 200, stdin);

        
        printf ("%s\n", x);

        normalize (x, normalizex);

        printf ("%s\n", normalizex);
    }

    return TRUE;
}

int trim(char *c)
{
    int i = strlen (c);
    while ((i < 0) && (c[i] != ' ' || c[i] != '\t'))
    {
        c[i] = 0;
        i--;
    }
    return TRUE;
}
int normalize(char *word, char *normalword)
{
    int i = 0, j = 0;

    while (word[i] != '\0' || i <= 200)
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
