#ifndef MANAGE_BOOKS_H
#define MANAGE_BOOKS_H

#include "raylib.h"

#define MAX_FONT_MANAGEBOOKS 128

#define CODE_BOOKS_LENGTH 16
#define NORMNAME_BOOKS_LENGTH 64
#define NAME_BOOKS_LENGTH 128
#define TYPE_BOOKS_LENGTH 32
#define AUTHOR_BOOKS_LENGTH 32
#define PUBLISHER_BOOKS_LENGTH 32
#define YEAR_BOOKS_LENGTH 16
#define STOCK_BOOKS_LENGTH 16
#define TOTAL_IMPORT_BOOKS_LENGTH 16
#define TOTAL_BORROW_BOOKS_LENGTH 32
#define PRICE_BOOKS_LENGTH 16

#define MAX_BOOKS 1000
#define BOOKS_FILE "data/dataTruyen.txt"

#define MANAGEBOOKS_Func_1 "Tác Giả"
#define MANAGEBOOKS_Func_2 "Nhà Xuất Bản"
#define MANAGEBOOKS_Func_3 "Thể loại"
#define MANAGEBOOKS_Func_4 "Trang Điều Hành"

typedef struct {
    char CodeBook[CODE_BOOKS_LENGTH];
    char NormNameBook[NORMNAME_BOOKS_LENGTH];
    char NameBook[NAME_BOOKS_LENGTH];
    char TypeBook[TYPE_BOOKS_LENGTH];
    char AuthorBook[AUTHOR_BOOKS_LENGTH];
    char PublisherBook[PUBLISHER_BOOKS_LENGTH];
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

typedef enum {
    MANAGEBOOKS_Dashboard,
    MANAGEBOOKS_Author,
    MANAGEBOOKS_Publisher,
    MANAGEBOOKS_Type,
    MANAGEBOOKS_Main
} MANAGEBOOKS_STATE;

void InitManageBooks();
BookList *Loadbooks(const char *filename);
void MANAGEBOOKS_UpdateSize();
void MANAGEBOOKS_Title(Texture2D icon);

void MANAGEBOOKS_Func();

#endif // MANAGE_BOOKS_H