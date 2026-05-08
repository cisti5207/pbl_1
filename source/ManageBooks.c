#include "ManageBooks.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Font MANAGEBOOKS_FONT[MAX_FONT_MANAGEBOOKS];

typedef struct {
    char CodeBooks[CODE_BOOKS_LENGTH];
    char NormNameBooks[NORMNAME_BOOKS_LENGTH];
    char NameBooks[NAME_BOOKS_LENGTH];
    char TypeBooks[TYPE_BOOKS_LENGTH];
    char AuthorBooks[AUTHOR_BOOKS_LENGTH];
    char PublisherBooks[PUBLISHER_BOOKS_LENGTH];
    int YearBooks;
    int StockBooks;
    int TotalImportBooks;
    int TotalBorrowBooks;
    float PriceBooks;
} Book;

typedef struct {
    Book *books;
    int capacity;
    int count;
    int stockBooks;
    int totalImportBooks;
} BookList;

BookList *Loadbooks(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return NULL;
    }

    BookList *bookList = (BookList *)malloc(sizeof(BookList));
    if (!bookList) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    bookList->books = (Book *)malloc(MAX_BOOKS * sizeof(Book));
    if (!bookList->books) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free(bookList);
        fclose(file);
        return NULL;
    }

    bookList->capacity = MAX_BOOKS;
    bookList->count;
    bookList->stockBooks;
    bookList->totalImportBooks;

    Book book;
    
    //T002     | tham-tu-lung-danh-conan                  | Thám tử lừng danh Conan                  | Gosho Aoyama               | Trinh thám     | Kim Đồng   | 2012  | 140    | 140  | 0         | 2500   |

    fclose(file);
    return bookList;
}

void InitManageBooks()
{
    SetWindowTitle("Manage Books");

    Vector2 MANAGEBOOKS_SCREEN = {
        getScreenWidth(),
        getScreenHeight()
    };

    Vector2 MONITOR_SIZE = {
        GetMonitorWidth(0),
        GetMonitorHeight(0)
    };



    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Manage Books Screen", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    return;
}