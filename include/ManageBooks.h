#ifndef MANAGE_BOOKS_H
#define MANAGE_BOOKS_H

#include <stdbool.h>
#include "raylib.h"
#include "libmanage.h"

// =========================================================================
// DEFINITIONS & CONSTANTS
// =========================================================================
#define MAX_FONT_MANAGEBOOKS       128
#define MAX_BOOKS                  1000

// Book String Lengths
#define CODE_BOOKS_LENGTH          4
#define NORMNAME_BOOKS_LENGTH      64
#define NAME_BOOKS_LENGTH          64
#define TYPE_BOOKS_LENGTH          32
#define AUTHORCODE_AUTHORS_LENGTH  8
#define AUTHOR_AUTHORS_LENGTH      32
#define AUTHOR_BOOKS_LENGTH        32
#define PUBLISHER_BOOKS_LENGTH     32
#define YEAR_BOOKS_LENGTH          5
#define PRICE_BOOKS_LENGTH         6

// UI Layout Constants
#define QUANTITYFORONEPAGE         15

// File Paths
#define BOOKS_FILE                 "data/ManageBooks/dataTruyen.txt"
#define LIST_BOOKS_FILE            "data/ManageBooks/dataListTruyen.txt"
#define AUTHORS_FILE               "data/ManageBooks/author.txt"
#define DESCRIPTION_FILE           "data/ManageBooks/description.txt"

// UI Text & Assets
#define MANAGEBOOKS_Func_1         "Tác Giả"
#define MANAGEBOOKS_Func_2         "Nhà Xuất Bản"
#define MANAGEBOOKS_Func_3         "Thể loại"
#define MANAGEBOOKS_Func_4         "Trang Điều Hành"
#define MANAGEBOOKS_Func_5         "Tìm kiếm"
#define MANAGEBOOKS_Avatar         "img/dut.png"

// =========================================================================
// ENUMERATIONS
// =========================================================================
typedef enum {
    MANAGEBOOKS_Dashboard,
    MANAGEBOOKS_Author,
    MANAGEBOOKS_Publisher,
    MANAGEBOOKS_Type,
    MANAGEBOOKS_Find,
    MANAGEBOOKS_Detail,  
    MANAGEBOOKS_Add,     
    MANAGEBOOKS_Main
} MANAGEBOOKS_STATE;

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
    SEARCH_ALL 
} StateFindBook;

// =========================================================================
// DATA STRUCTURES
// =========================================================================
typedef struct {
    Rectangle TitleBox;
    Rectangle HeaderBox;
    Rectangle Panel;
    Rectangle PaginationBox;
} ManageBooksUI;

// Struct lưu thông tin từng Tập truyện
typedef struct {
    char VolumeName[256];
    char VolumeCode[32];
    char BookCode[32];
    int Stock;
    int ImportStock;
} Volume;

// Cấu trúc Truyện
typedef struct {
    char CodeBook[256];
    char NormNameBook[256];
    char NameBook[256];
    char TypeBook[256];
    char AuthorBook[256];
    char PublisherBook[256];
    int YearBook;
    int PriceBook;
    char Description[1024]; 

    Volume volumes[200]; // Mảng lưu các tập truyện (Tối đa 200 tập/bộ)
    int volumeCount;     // Số lượng tập hiện có
    int TotalStock;      // Tổng số lượng của tất cả các tập
} Book;

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

// =========================================================================
// FUNCTION PROTOTYPES
// =========================================================================

// --- Initialization ---
void InitManageBooks(Role _role);
void LoadManageBooksUI(ManageBooksUI *UI, Size size);

// --- UI Panels ---
void ManageBooksTitle(ManageBooksUI UI);
int ShowBookDetail_Panel(Size size, ManageBooksUI UI, float *wheel, Font *_Font, BookList *Books, int bookIndex, MANAGEBOOKS_STATE *State, MANAGEBOOKS_STATE prevState, Role _role);
int ShowAddBook_Panel(Size size, ManageBooksUI UI, Font *_Font, InputBox *inputs, bool isEditMode, Book *editBook, float *volWheel);
int ShowBooks_Panel(Size size, ManageBooksUI UI, float *wheel, Font *_Font, BookList *Books, int filterMode, const char *filterValue, Role _role, bool *showPagination, int *requestDeleteIndex, bool showOnlyInStock);
void ShowAuthor_Panel(Size size, ManageBooksUI UI, float *wheel, Font *_Font, AuthorList *Authors, char *selectedAuthor, BookList *Books);
void ShowPublisher_Panel(Size size, ManageBooksUI UI, float *wheel, Font *_Font, PublisherList *Publishers, char *selectedPublisher, BookList *Books);
void ShowType_Panel(Size size, ManageBooksUI UI, float *wheel, Font *_Font, Type *Types, char *selectedType, BookList *Books);
void DrawPagination(ManageBooksUI UI, BookList *Books, Font *_Font, Size size, float *wheel, char *backTarget, bool isVisible);

// --- State & Logic Handlers ---
bool ManageBooksFunc(MANAGEBOOKS_STATE *State, InputBox *FindBar, Texture2D Icon_Find, ManageBooksUI UI, Size size, Font *_Font);
void NormalizeString(const char* src, char* dest, int max_len);
bool ContextualBookSearch(Book book, const char *query);
int CountStrInBooks(BookList *Books, const char *Str, StateFindBook state, bool showOnlyInStock);

// --- File Operations ---
BookList *Loadbooks(const char *filename);
bool Savebooks(BookList Books);

void LoadVolumes(BookList *Books, const char *filename);
void SaveVolumes(BookList Books, const char *filename);

AuthorList *LoadAuthor(const char *filename);
bool SaveAuthor(AuthorList _Author);

Type *LoadType(BookList *Books);
PublisherList *LoadPublisher(BookList *Books);
void LoadDescription(BookList *Books, const char *filename);

#endif // MANAGE_BOOKS_H