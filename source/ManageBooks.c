#include "ManageBooks.h"
#include "raylib.h"
#include "libmanage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

Font MANAGEBOOKS_FONT[MAX_FONT_MANAGEBOOKS];

Rectangle MANAGEBOOKS_SCREEN;
Rectangle MANAGEBOOKS_MONITOR;
Rectangle MANAGEBOOKS_TitleBox;
Rectangle MANAGEBOOKS_HeaderBox;
Rectangle MANAGEBOOKS_Panel;
Rectangle MANAGEBOOKS_TitleBarBox;
Vector2 MANAGEBOOKS_SCALE;

void InitManageBooks()
{
    SetWindowTitle("Manage Books");

    MANAGEBOOKS_UpdateSize();

    BookList* Books = Loadbooks(BOOKS_FILE);

    if (Books == NULL)
    {
        printf ("Failed!!! Can not load dataTruyen.txt!!!");
        return;
    }

    Texture2D Avatar = LoadTexture ("img/dut.png");

    

    while (!WindowShouldClose())
    {
        MANAGEBOOKS_UpdateSize();

         

        BeginDrawing();
        ClearBackground(WHITESMOKE);

        MANAGEBOOKS_Title(Avatar);

        EndDrawing();
    }

    return;
}

void MANAGEBOOKS_UpdateSize()
{
    MANAGEBOOKS_SCREEN = (Rectangle) {
        0, 0,
        (float)GetScreenWidth(),
        (float)GetScreenHeight()
    };

    MANAGEBOOKS_MONITOR = (Rectangle) {
        0, 0,
        (float)GetMonitorWidth(0),
        (float)GetMonitorHeight(0)
    };

    MANAGEBOOKS_SCALE = (Vector2) {
        MANAGEBOOKS_SCREEN.width / MANAGEBOOKS_MONITOR.width,
        MANAGEBOOKS_SCREEN.height / MANAGEBOOKS_MONITOR.height
    };

    MANAGEBOOKS_TitleBox = (Rectangle) {
        0, 0,
        MANAGEBOOKS_SCREEN.width,
        MANAGEBOOKS_SCREEN.height * 0.08f * (float) pow(0.6, (double) MANAGEBOOKS_SCALE.y)
    };
    
    MANAGEBOOKS_HeaderBox = (Rectangle) {
        MANAGEBOOKS_TitleBox.width * 0.15f,
        MANAGEBOOKS_TitleBox.height,
        MANAGEBOOKS_TitleBox.width * 0.7f,
        MANAGEBOOKS_TitleBox.height
    };

    MANAGEBOOKS_Panel = (Rectangle) {
        MANAGEBOOKS_HeaderBox.x,
        MANAGEBOOKS_HeaderBox.y + MANAGEBOOKS_TitleBox.height,
        MANAGEBOOKS_HeaderBox.width,
        MANAGEBOOKS_SCREEN.height - (MANAGEBOOKS_HeaderBox.y + MANAGEBOOKS_HeaderBox.height)
    };

}

void MANAGEBOOKS_Title(Texture2D icon)
{
    Rectangle IconBox = {
        MANAGEBOOKS_TitleBox.width * 0.02f,
        MANAGEBOOKS_TitleBox.height * 0.5f,
        MANAGEBOOKS_TitleBox.width * 0.08f,
        MANAGEBOOKS_TitleBox.width * 0.08f
    };

    DrawRectangleRec(MANAGEBOOKS_TitleBox, BRIGHTWHITE);
    DrawRectangleRec(MANAGEBOOKS_HeaderBox, LIGHTGRAY);
    DrawRectangleRec(MANAGEBOOKS_Panel, SILVERGRAY);

    DrawIcon(IconBox, icon);
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
