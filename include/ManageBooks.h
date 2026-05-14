#ifndef MANAGE_BOOKS_H
#define MANAGE_BOOKS_H

#include "raylib.h"
#include "libmanage.h"

#define MAX_FONT_MANAGEBOOKS 128

#define CODE_BOOKS_LENGTH 4
#define NORMNAME_BOOKS_LENGTH 64
#define NAME_BOOKS_LENGTH 64
#define TYPE_BOOKS_LENGTH 32
#define AUTHOR_BOOKS_LENGTH 32
#define PUBLISHER_BOOKS_LENGTH 32
#define YEAR_BOOKS_LENGTH 5
#define STOCK_BOOKS_LENGTH 4
#define TOTAL_IMPORT_BOOKS_LENGTH 4
#define TOTAL_BORROW_BOOKS_LENGTH 5
#define PRICE_BOOKS_LENGTH 6

#define MAX_BOOKS 1000
#define BOOKS_FILE "data/dataTruyen.txt"

#define MANAGEBOOKS_Func_1 "Tác Giả"
#define MANAGEBOOKS_Func_2 "Nhà Xuất Bản"
#define MANAGEBOOKS_Func_3 "Thể loại"
#define MANAGEBOOKS_Func_4 "Trang Điều Hành"
#define MANAGEBOOKS_Func_5 "Tìm kiếm"

#define MANAGEBOOKS_Avatar "img/dut.png"

typedef struct {
    Rectangle TitleBox;
    Rectangle HeaderBox;
    Rectangle Panel;
} ManageBooksUI;
typedef struct {
    char CodeBook[256];
    char NormNameBook[256];
    char NameBook[256];
    char TypeBook[256];
    char AuthorBook[256];
    char PublisherBook[256];
    int YearBook;
    int StockBook;
    int TotalImportBook;
    int TotalBorrowBook;
    int PriceBook;
} Book;

typedef struct {
    Book *theArray;
    int capacity;
    int count;
    int stockBooks;
    int totalImportBooks;
} BookList;

typedef struct {
    char **Author;
    int count;
} Author;

typedef struct {
    char **Type;
    int count;
} Type;

typedef enum {
    MANAGEBOOKS_Dashboard,
    MANAGEBOOKS_Author,
    MANAGEBOOKS_Publisher,
    MANAGEBOOKS_Type,
    MANAGEBOOKS_Find,
    MANAGEBOOKS_Main
} MANAGEBOOKS_STATE;

void InitManageBooks();
BookList *Loadbooks(const char *filename);
Author *LoadAuthor(BookList *Books);
Type *LoadType(BookList *Books);
bool Savebooks(BookList *Books);

void LoadManageBooksUI (ManageBooksUI *UI, Size size);
void ManageBooksTitle(Texture2D icon, ManageBooksUI UI);

void ManageBooksFunc(MANAGEBOOKS_STATE *State, InputBox *FindBar, Texture2D Icon_Find, ManageBooksUI UI, Size size, Font *_Font);



#endif // MANAGE_BOOKS_H