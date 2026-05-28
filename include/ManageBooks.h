#ifndef MANAGE_BOOKS_H
#define MANAGE_BOOKS_H

#include "raylib.h"
#include "libmanage.h"

#define MAX_FONT_MANAGEBOOKS 128

#define CODE_BOOKS_LENGTH 4
#define NORMNAME_BOOKS_LENGTH 64
#define NAME_BOOKS_LENGTH 64
#define TYPE_BOOKS_LENGTH 32
#define AUTHORCODE_AUTHORS_LENGTH 8
#define AUTHOR_AUTHORS_LENGTH 32
#define AUTHOR_BOOKS_LENGTH 32
#define PUBLISHER_BOOKS_LENGTH 32
#define YEAR_BOOKS_LENGTH 5
#define STOCK_BOOKS_LENGTH 4
#define TOTAL_IMPORT_BOOKS_LENGTH 4
#define TOTAL_BORROW_BOOKS_LENGTH 5
#define PRICE_BOOKS_LENGTH 6

#define QUANTITYFORONEPAGE 15
#define DISTANCE_BOOKS 50

#define MAX_BOOKS 1000
#define BOOKS_FILE "data/ManageBooks/dataTruyen.txt"
#define AUTHORS_FILE "data/ManageBooks/author.txt"
#define DESCRIPTION_FILE "data/ManageBooks/description.txt"

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
    Rectangle PaginationBox;
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
    char Description[1024]; 
} Book;

typedef enum {
    IDLE_STATEFINDBOOK,
    CODE,
    NAME,
    AUTHOR,
    TYPE,
    PUBLISHER,
    YEAR,
    STOCK,
    PRICE,
    SEARCH_ALL // Dùng cho thuật toán tìm kiếm đa năng (Greedy Search)
} StateFindBook;

typedef struct {
    Book *theArray;
    int capacity;
    int count;
    int stockBooks;
    int totalImportBooks;
    int pos;
    int QuantityForOnePage;
    int currentPage;
    int totalPages; 
} BookList;

typedef struct {
    char name[256];
    char code[32];
} Author;

typedef struct {
    Author *Author;
    int count;
} AuthorList;

typedef struct {
    char **Type;
    int count;
} Type;

typedef struct {
    char **Publisher;
    int count;
} PublisherList;

typedef enum {
    MANAGEBOOKS_Dashboard,
    MANAGEBOOKS_Author,
    MANAGEBOOKS_Publisher,
    MANAGEBOOKS_Type,
    MANAGEBOOKS_Find,
    MANAGEBOOKS_Detail,  // Trạng thái hiển thị trang chi tiết truyện
    MANAGEBOOKS_Add,     // Trạng thái hiển thị Form thêm truyện mới
    MANAGEBOOKS_Main
} MANAGEBOOKS_STATE;


void InitManageBooks(Role _role);

void ShowBookDetail_Panel(Size size, ManageBooksUI UI, Font *_Font, Book book, MANAGEBOOKS_STATE *State, MANAGEBOOKS_STATE prevState);
int ShowAddBook_Panel(Size size, ManageBooksUI UI, Font *_Font, InputBox *inputs);
int ShowBooks_Panel(Size size, ManageBooksUI UI, float *wheel, Font *_Font, BookList *Books, int filterMode, const char *filterValue, Role _role, bool *showPagination, int *requestDeleteIndex);
void ShowAuthor_Panel(Size size, ManageBooksUI UI, float *wheel, Font *_Font, AuthorList *Authors, char *selectedAuthor, BookList *Books);
void ShowPublisher_Panel(Size size, ManageBooksUI UI, float *wheel, Font *_Font, PublisherList *Publishers, char *selectedPublisher, BookList *Books);
void ShowType_Panel(Size size, ManageBooksUI UI, float *wheel, Font *_Font, Type *Types, char *selectedType, BookList *Books);

void LoadManageBooksUI (ManageBooksUI *UI, Size size);
void ManageBooksTitle(Texture2D icon, ManageBooksUI UI);

void NormalizeString(const char* src, char* dest, int max_len);
float CalculateSimilarity(const char* query, const char* text);
bool ContextualBookSearch(Book book, const char *query);

int CountStrInBooks (BookList *Books, const char *Str, StateFindBook state);
void DrawPagination(ManageBooksUI UI, BookList *Books, Font *_Font, Size size, float *wheel, char *backTarget, bool isVisible);
bool ManageBooksFunc(MANAGEBOOKS_STATE *State, InputBox *FindBar, Texture2D Icon_Find, ManageBooksUI UI, Size size, Font *_Font);

BookList *Loadbooks(const char *filename);
bool Savebooks(BookList Books);
AuthorList *LoadAuthor(const char *filename);
bool SaveAuthor(AuthorList _Author);
Type *LoadType(BookList *Books);
PublisherList *LoadPublisher(BookList *Books);
void LoadDescription(BookList *Books, const char *filename);

#endif // MANAGE_BOOKS_H