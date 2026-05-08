#include "ManageBooks.h"
#include "raylib.h"
#include "libmanage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Font MANAGEBOOKS_FONT[MAX_FONT_MANAGEBOOKS];

Vector2 MANAGEBOOKS_SCREEN;
Vector2 MONITOR_SIZE;

void InitManageBooks()
{
    SetWindowTitle("Manage Books");

    MANAGEBOOKS_SCREEN = (Vector2) {
        GetScreenWidth(),
        GetScreenHeight()
    };

    MONITOR_SIZE = (Vector2) {
        GetMonitorWidth(0),
        GetMonitorHeight(0)
    };

    BookList* Books = Loadbooks(BOOKS_FILE);

    if (Books == NULL)
    {
        printf ("Failed!!! Can not load dataTruyen.txt!!!");
        return;
    }

    

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(LIGHTGRAY);

        DrawRectangle

        EndDrawing();
    }

    return;
}

void MANAGEBOOKS_Title(Texture2D icon)
{

}

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

    bookList->theArray = (Book *)malloc(MAX_BOOKS * sizeof(Book));
    if (!bookList->theArray) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free(bookList);
        fclose(file);
        return NULL;
    }

    bookList->capacity = MAX_BOOKS;

    fscanf(file, "Số quyển truyện: %d\n", &(bookList->count));
    fscanf(file, "Số lượng truyện hiện tại: %d\n", &(bookList->stockBooks));
    fscanf(file, "Số lượng truyện gốc: %d\n", &(bookList->totalImportBooks));

    Book book;

    //| T002     | tham-tu-lung-danh-conan                  | Thám tử lừng danh Conan                  | Gosho Aoyama               | Trinh thám     | Kim Đồng   | 2012  | 140    | 140  | 0         | 2500   |
    int count = 0;
    while (fscanf(file, " | %[^|]| %[^|]| %[^|]| %[^|]| %[^|]| %[^|]| %d | %d | %d | %d | %d |", book.CodeBook, book.NormNameBook, book.NameBook, book.AuthorBook, book.TypeBook
    , book.PublisherBook, &book.YearBook, &book.StockBook, &book.TotalImportBook, &book.TotalBorrowBook, &book.PriceBook) == 11)
    {
        trim(book.CodeBook);
        trim(book.NormNameBook);
        trim(book.NameBook);
        trim(book.AuthorBook);
        trim(book.TypeBook);
        trim(book.PublisherBook);

        bookList -> theArray[count++] = book;
    }

    if (count != bookList->count) 
    {
        printf ("Failed!!! Read %d of %d\n", count, bookList->count);
        return NULL;
    }
    

    fclose(file);
    return bookList;
}
