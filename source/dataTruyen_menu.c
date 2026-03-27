#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define NOT_FOUND -1

struct ElementType
{
    char ID[7];
    char Name[200];
    char normName[200];
    char count[4];
    char State[10];
    char fav[4];
    char price[10];
};

struct ListTruyen
{
    struct ElementType *dataType;
    int count;
    int capacity;
};

typedef struct ElementType ElementType;
typedef struct ListTruyen *List;
typedef int position;

List create (unsigned int capacity);
int insert (List L,struct ELementType e, position p);

void getdataTruyen(List dataTruyen, FILE *f);


void main()
{
    SetconsoleoutputCP(65001);
    SetconsoleCP(65001);

    FILE *f_datatruyen = fopen ("../data/dataTruyen.txt", "r+");

    List dataTruyen = create(50);
    
    

}

List create (unsigned int capacity)
{
    List L = malloc (sizeof(struct ListTruyen));
    if (L == NULL)
        return NULL;

    L -> count = 0;
    L -> capacity = capacity;
    L -> dataType = malloc (sizeof(struct ElementType) * capacity);

    if (L -> dataType == NULL)
        return NULL;

    return L;
}
int insert (List L,struct ElementType e, position p)
{
    if (p > L -> count || L -> count == L -> capacity)
        return FALSE;
    
    position cur = L -> count;
    while (cur != p)
    {
        L -> dataType[cur] = L -> dataType[cur - 1];
        cur--;
    }

    L -> dataType[p] = e;
    return TRUE;
}
void getdataTruyen(List dataTruyen, FILE *f)
{
    ElementType e;
    int i = 0;
    while (fscanf(f, " %6[^|] | %199[^|] | %199[^|] | %3[^|] | %[^|] | %3[^|] | %6[^|\n]", 
            e.ID, e.Name, e.normName, e.count, e.State, e.fav, e.price) == 7)
    {
        
        trim(e.ID);
        trim(e.Name);
        trim(e.normName);
        trim(e.count);
        trim(e.State);
        trim(e.fav);
        trim(e.price);

        i++;
        dataTruyen -> count ++;
    }
}

void trim(char *s)
{
    int len = strlen(s);
    while (len > 0 && (s[len-1] == ' ' || s[len - 1] == '\t'))
    {
        s[len] = 0;
        len--;
    }
}

