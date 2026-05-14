#include "ManageBooks.h"
#include "raylib.h"
#include "libmanage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

void InitManageBooks(){
    Size ManageBooksSize;
    LoadSize(
        &ManageBooksSize,
        (Vector2) {
            GetMonitorWidth(0),
            GetMonitorWidth(0)
        },
        (Vector2) {
            GetScreenWidth(),
            GetScreenHeight()
        },
        (Vector2) {
            GetScreenWidth() / GetMonitorWidth(0),
            GetScreenHeight() / GetMonitorHeight(0)
        },
        GetMousePosition()
    );

    ManageBooksUI customUI;
    LoadManageBooksUI(
        &customUI, 
        ManageBooksSize
    );

    MANAGEBOOKS_STATE State = MANAGEBOOKS_Dashboard;

    Font _Font[2];
    _Font[0] = SetFontUTF8 (ArialBold, 50);

    BookList* Books = Loadbooks(BOOKS_FILE);
    Author* Authors = LoadAuthor(Books);
    Type* Types = LoadType(Books);

    if (Books == NULL || Authors == NULL || Types == NULL){
        printf ("Failed!!! Can not load dataTruyen.txt!!!");
        State = MANAGEBOOKS_Main;
        return;
    }

    InputBox FindBar = {0};

    Texture2D Avatar = LoadTexture (MANAGEBOOKS_Avatar);
    Texture2D Icon_Find = LoadTexture (Icon_Find_lnk);


    while (!WindowShouldClose())
    {

        if (IsWindowResized()){
            LoadSize(
                &ManageBooksSize,
                (Vector2) {0},
                (Vector2) {
                    GetScreenWidth(),
                    GetScreenHeight()
                },
                (Vector2) {
                    GetScreenWidth() / GetMonitorWidth(0),
                    GetScreenHeight() / GetMonitorHeight(0)
                },
                (Vector2) {0}
            );

            LoadManageBooksUI(
                &customUI, 
                ManageBooksSize
            );
        }
        ManageBooksSize.Mouse = GetMousePosition();

        BeginDrawing();
        ClearBackground(WHITESMOKE);

        ManageBooksTitle(Avatar, customUI);

        ManageBooksFunc(&State, &FindBar, Icon_Find, customUI, ManageBooksSize, _Font);

        switch(State)
        {
            case MANAGEBOOKS_Dashboard:
                break;
            case MANAGEBOOKS_Author:
                break;
            case MANAGEBOOKS_Publisher:
                break;
            case MANAGEBOOKS_Type:
                break;
            case MANAGEBOOKS_Find:
                break;
            case MANAGEBOOKS_Main:
                break;
        }
        
        EndDrawing();
        if (State == MANAGEBOOKS_Main) {
            break;
        }
    }

    UnloadFont(_Font[0]);

    UnloadTexture(Avatar);
    UnloadTexture(Icon_Find);

    Savebooks(Books);
    free(Books);
    return;
}

void LoadManageBooksUI (ManageBooksUI *UI, Size size){
    UI->TitleBox = (Rectangle) {
        0,
        0,
        size.Screen.x,
        size.Screen.y * 0.08f * (float) pow(0.6, (double)size.Scale.x)
    };

    UI->HeaderBox = (Rectangle) {
        UI->TitleBox.width * 0.15f,
        UI->TitleBox.height,
        UI->TitleBox.width * 0.7f,
        UI->TitleBox.height
    };

    UI->Panel = (Rectangle) {
        UI->HeaderBox.x,
        UI->HeaderBox.y + UI->TitleBox.height,
        UI->HeaderBox.width,
        size.Screen.y - (UI->HeaderBox.height + UI->TitleBox.height)
    };
}

void ManageBooksTitle(Texture2D icon, ManageBooksUI UI){
    Rectangle IconBox = {
        UI.TitleBox.width * 0.02f,
        UI.TitleBox.height * 0.5f,
        UI.TitleBox.width * 0.08f,
        UI.TitleBox.width * 0.08f
    };

    DrawRectangleRec(
        UI.TitleBox, 
        BRIGHTWHITE
    );

    DrawRectangleRec(
        UI.HeaderBox, 
        GRAY
    );
    
    DrawRectangleRec(
        UI.Panel, 
        LIGHTGRAY
    );

    DrawIcon(
        IconBox, 
        icon
    );
}

void ManageBooksFunc(MANAGEBOOKS_STATE *State, InputBox *FindBar, Texture2D Icon_Find, ManageBooksUI UI, Size size, Font *_Font){
    // khởi tạo hitbox cho Avatar
    Rectangle IconBox = {
        UI.TitleBox.width * 0.02f,
        UI.TitleBox.height * 0.5f,
        UI.TitleBox.width * 0.08f,
        UI.TitleBox.width * 0.08f
    };

    // kiểm tra focus
    if (CheckCollisionPointRec(size.Mouse, IconBox))
    {
        // Thể hiện đã focus
        DrawRectangleRec(
            IconBox, 
            Fade(TEALBLUE, 0.1f)
        );
        
        // Nếu mà bấm vào avatar thì *State -> Dashboard, hay quay lại màn hình chính cuả ManageBooks
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            *State = MANAGEBOOKS_Dashboard;
    }
    
    // Khởi tạo biến TextWidth để lấy độ rộng (x, y) của Text sau khi in
    Vector2 TextWidth;

    // "Tác giả"
    TextWidth = MeasureTextEx (
        _Font[0], 
        MANAGEBOOKS_Func_1, 
        24, 
        2
    );

    // Hit box của "Tác giả"
    Rectangle AuthorBox = {
        // Lấy vị trí bắt đầu vẽ (x, y)
        UI.TitleBox.width * 0.15f,
        UI.TitleBox.height * 0.9f - TextWidth.y,
        
        // Chiều dài và rộng của hitbox Rectangle
        TextWidth.x,
        TextWidth.y
    };
    
    // Kiểm tra điều kiện, nếu có focus thì highlight dòng chữ
    if (CheckCollisionPointRec(size.Mouse, AuthorBox)){
        DrawTextEx(
            _Font[0], 
            MANAGEBOOKS_Func_1, 
            (Vector2) {
                AuthorBox.x - 8, 
                AuthorBox.y - 4
            }, 
            26, 
            3, 
            GOLDACCENT
        );

        DrawLineEx(
            (Vector2) {
                AuthorBox.x, 
                AuthorBox.y + AuthorBox.height
            }, 
            (Vector2) {
                AuthorBox.x + AuthorBox.width, 
                AuthorBox.y + AuthorBox.height
            }, 
            3, 
            BLACK
        );
        
        //Nếu bấm chọn chức năng này thì State -> Author, trang ứng dụng sẽ chuyển sang Tác giả
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            *State = MANAGEBOOKS_Author;
        }
    }
    else {
        // Nếu không có focus thì không highlight
        DrawTextEx(
            _Font[0], 
            MANAGEBOOKS_Func_1, 
            (Vector2) {
                AuthorBox.x, 
                AuthorBox.y
            }, 
            24, 
            2, 
            BLACK
        );

        DrawLineEx(
            (Vector2) {
                AuthorBox.x, 
                AuthorBox.y + AuthorBox.height
            }, 
            (Vector2) {
                AuthorBox.x + AuthorBox.width, 
                AuthorBox.y + AuthorBox.height
            }, 
            3, 
            BLACK
        );
    }

    // "Nhà xuất bản"
    TextWidth = MeasureTextEx (
        _Font[0], 
        MANAGEBOOKS_Func_2, 
        24, 
        2
    );

    Rectangle PublisherBox = {
        AuthorBox.x + AuthorBox.width + UI.TitleBox.width * 0.03f,
        AuthorBox.y,
        TextWidth.x,
        TextWidth.y
    };

    if (CheckCollisionPointRec(size.Mouse, PublisherBox)){
        DrawTextEx (
            _Font[0], 
            MANAGEBOOKS_Func_2, 
            (Vector2) {
                PublisherBox.x - 10, 
                PublisherBox.y - 4
            }, 
            26, 
            3, 
            GOLDACCENT
        );

        DrawLineEx(
            (Vector2) {
                PublisherBox.x, 
                PublisherBox.y + PublisherBox.height
            }, 
            (Vector2) {
                PublisherBox.x + PublisherBox.width, 
                PublisherBox.y + PublisherBox.height
            }, 
            3, 
            BLACK
        );
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            *State = MANAGEBOOKS_Publisher;
        }
    }
    else {
        DrawTextEx (
            _Font[0], 
            MANAGEBOOKS_Func_2, 
            (Vector2) {
                PublisherBox.x, 
                PublisherBox.y
            }, 
            24, 
            2, 
            BLACK
        );
        
        DrawLineEx (
            (Vector2) {
                PublisherBox.x, 
                PublisherBox.y + PublisherBox.height
            }, 
            (Vector2) {
                PublisherBox.x + PublisherBox.width, 
                PublisherBox.y + PublisherBox.height
            }, 
            3, 
            BLACK
        );
    }

    // "Thể loại"
    TextWidth = MeasureTextEx (
        _Font[0], 
        MANAGEBOOKS_Func_3, 
        24, 
        2
    );

    Rectangle TypeBox = {
        PublisherBox.x + PublisherBox.width + UI.TitleBox.width * 0.03f,
        PublisherBox.y,
        TextWidth.x,
        TextWidth.y
    };

    if (CheckCollisionPointRec(size.Mouse, TypeBox)){
        DrawTextEx (
            _Font[0], 
            MANAGEBOOKS_Func_3, 
            (Vector2) {
                TypeBox.x - 8, 
                TypeBox.y - 4
            }, 
            26, 
            3, 
            GOLDACCENT
        );

        DrawLineEx(
            (Vector2) {
                TypeBox.x, 
                TypeBox.y + TypeBox.height
            }, 
            (Vector2) {
                TypeBox.x + TypeBox.width, 
                TypeBox.y + TypeBox.height
            }, 
            3, 
            BLACK
        );
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            *State = MANAGEBOOKS_Type;
        }
    }
    else {
        DrawTextEx (
            _Font[0], 
            MANAGEBOOKS_Func_3, 
            (Vector2) {
                TypeBox.x, 
                TypeBox.y
            }, 
            24, 
            2, 
            BLACK
        );

        DrawLineEx(
            (Vector2) {
                TypeBox.x, 
                TypeBox.y + TypeBox.height
            }, 
            (Vector2) {
                TypeBox.x + TypeBox.width, 
                TypeBox.y + TypeBox.height
            }, 
            3, 
            BLACK
        );
    }

    // Trang màn hình chính
    TextWidth = MeasureTextEx (
        _Font[0], 
        MANAGEBOOKS_Func_4, 
        24, 
        2
    );

    Rectangle MainBox = {
        TypeBox.x + TypeBox.width + UI.TitleBox.width * 0.03f,
        TypeBox.y,
        TextWidth.x,
        TextWidth.y
    };

    if (CheckCollisionPointRec(size.Mouse, MainBox)){
        DrawTextEx (
            _Font[0], 
            MANAGEBOOKS_Func_4, 
            (Vector2) {
                MainBox.x - 8, 
                MainBox.y - 4
            }, 
            26, 
            3, 
            GOLDACCENT
        );

        DrawLineEx(
            (Vector2) {
                MainBox.x, 
                MainBox.y + MainBox.height
            }, 
            (Vector2) {
                MainBox.x + MainBox.width, 
                MainBox.y + MainBox.height
            }, 
            3, 
            BLACK
        );

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            *State = MANAGEBOOKS_Main;
        }
    }
    else {
        DrawTextEx (
            _Font[0], 
            MANAGEBOOKS_Func_4, 
            (Vector2) {
                MainBox.x, 
                MainBox.y
            }, 
            24, 
            2, 
            BLACK
        );
        DrawLineEx(
            (Vector2) {
                MainBox.x, 
                MainBox.y + MainBox.height
            }, 
            (Vector2) {
                MainBox.x + MainBox.width, 
                MainBox.y + MainBox.height
            }, 
            3, 
            BLACK
        );
    }

    // Thanh tìm kiếm
    FindBar -> box = (Rectangle) {
        MainBox.x + MainBox.width + UI.TitleBox.width * 0.1f * (float) pow (0.9 ,(double)size.Scale.x),
        UI.TitleBox.height * 0.1f,
        (UI.TitleBox.width - FindBar -> box.x) * 0.9f,
        UI.TitleBox.height * 0.8f
    };

    Rectangle FindBarTextPos = {
        FindBar -> box.x + 10,
        FindBar -> box.y + (FindBar -> box.height - 20.0f) / 2,
        FindBar -> box.width - (15 + FindBar -> box.height),
        20
    };

    Rectangle FindBarTextBox = {
        FindBar -> box.x,
        FindBar -> box.y,
        FindBar -> box.width - FindBar -> box.height,
        FindBar -> box.height
    };

    Rectangle IconFindBox = {
        FindBar -> box.x + (FindBar -> box.width - FindBar -> box.height),
        FindBar -> box.y,
        FindBar -> box.height,
        FindBar -> box.height
    };

    if (CheckCollisionPointRec(size.Mouse, FindBarTextBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        FindBar->isFocused = true;
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        FindBar->isFocused = false;

    float roundness = FindRoundness (
        0.05f * FindBar->box.width, 
        FindBar->box.width, 
        FindBar->box.height
    );

    DrawRectangleRoundedLinesEx (
        FindBar->box, 
        roundness, 
        10, 
        2.0f, 
        LIGHTGRAY
    );

    DrawRectangleRounded (
        FindBar->box, 
        roundness, 
        10, 
        GRAY
    );

    if (FindBar->isFocused == true){
        DrawRectangleRounded (
            FindBar->box, 
            roundness, 
            10, 
            WHITESMOKE
        );
        BeginScissorMode(
            FindBarTextPos.x, 
            FindBarTextPos.y, 
            FindBarTextPos.width, 
            FindBarTextPos.height
        );

        UpdateInputBox(FindBar);
        
        float w = MeasureTextEx(
            _Font[0], 
            FindBar->text, 
            20, 
            2
        ).x;

        float dx = ((w - FindBarTextPos.width) > 0) ? w - FindBarTextPos.width : 0;
        Vector2 position = {
            FindBarTextPos.x - dx,
            FindBarTextPos.y
        };

        DrawTextEx (
            _Font[0], 
            FindBar->text, 
            position, 
            20, 
            2, 
            GRAY
        );

        if ((int)(GetTime() * 2) % 2 == 0){
            DrawRectangle(
                FindBarTextPos.x + w - dx + 2,
                FindBarTextPos.y,
                2,
                20,
                BLACK
            );
        }
        EndScissorMode();

        if (IsKeyPressed(KEY_ENTER)){
            *State = MANAGEBOOKS_Find;
        }
    }
    else if (FindBar->length > 0){
        DrawRectangleRounded (
            FindBar->box, 
            roundness, 
            10, 
            WHITESMOKE
        );
        BeginScissorMode(
            FindBarTextPos.x, 
            FindBarTextPos.y, 
            FindBarTextPos.width, 
            FindBarTextPos.height
        );
        
        float w = MeasureTextEx(
            _Font[0], 
            FindBar->text, 
            20, 
            2
        ).x;

        float dx = ((w - FindBarTextPos.width) > 0) ? w - FindBarTextPos.width : 0;
        Vector2 position = {
            FindBarTextPos.x - dx,
            FindBarTextPos.y
        };

        DrawTextEx (
            _Font[0], 
            FindBar->text, 
            position, 
            20, 
            2, 
            GRAY
        );

        if ((int)(GetTime() * 2) % 2 == 0){
            DrawRectangle(
                FindBarTextPos.x + w - dx + 2,
                FindBarTextPos.y,
                2,
                20,
                BLACK
            );
        }
        EndScissorMode();
    }
    else {
        DrawText(
            "FIND TRUYEN", 
            FindBarTextPos.x, 
            FindBarTextPos.y, 
            20, 
            BLACK
        );
    }

    roundness = FindRoundness (
        0.05f * FindBar->box.width, 
        IconFindBox.width, 
        IconFindBox.height
    );

    DrawRectangleRounded(
        IconFindBox,
        roundness,
        10,
        BRIGHTGRAY
    );

    DrawIcon(IconFindBox, Icon_Find);
    if (CheckCollisionPointRec(size.Mouse, IconFindBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && FindBar -> length > 0)
        *State = MANAGEBOOKS_Find;
}

BookList *Loadbooks(const char *filename){
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
    while (fscanf(file, " | %[^|]| %[^|]| %[^|]| %[^|]| %[^|]| %[^|]| %d | %d | %d | %d | %d |\n", 
        book.CodeBook, 
        book.NormNameBook, 
        book.NameBook, 
        book.AuthorBook, 
        book.TypeBook, 
        book.PublisherBook, 
        
        &book.YearBook, 
        &book.StockBook, 
        &book.TotalImportBook, 
        &book.TotalBorrowBook, 
        &book.PriceBook) == 11)
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
bool Savebooks(BookList *Books){
    FILE *f = fopen (BOOKS_FILE, "w");

    if (f == NULL){
        printf ("Save FAILED!!!!");
        return 0;
    }

    fprintf (f, "Số quyển truyện: %d\n", Books -> count);
    fprintf (f, "Số lượng truyện hiện tại: %d\n", Books -> stockBooks);
    fprintf (f, "Số lượng truyện gốc: %d\n", Books -> totalImportBooks);

    Book A;
    for (int i = 0; i < Books -> count; i++)
    {
        A = Books -> theArray[i];

        fprintf (f, "| %-*s | %-*s | %-*s | %-*s | %-*s | %-*s | %-*d | %-*d | %-*d | %-*d | %-*d |\n",
            UTF8Width(A.CodeBook, CODE_BOOKS_LENGTH)            - 2, A.CodeBook,
            UTF8Width(A.NormNameBook, NORMNAME_BOOKS_LENGTH)    - 2, A.NormNameBook,
            UTF8Width(A.NameBook, NAME_BOOKS_LENGTH)            - 2, A.NameBook,
            UTF8Width(A.AuthorBook, AUTHOR_BOOKS_LENGTH)        - 2, A.AuthorBook,
            UTF8Width(A.TypeBook, TYPE_BOOKS_LENGTH)            - 2, A.TypeBook,
            UTF8Width(A.PublisherBook, PUBLISHER_BOOKS_LENGTH)  - 2, A.PublisherBook,

            YEAR_BOOKS_LENGTH               - 2, A.YearBook,
            STOCK_BOOKS_LENGTH              - 2, A.StockBook,
            TOTAL_IMPORT_BOOKS_LENGTH       - 2, A.TotalImportBook,
            TOTAL_BORROW_BOOKS_LENGTH       - 2, A.TotalBorrowBook,
            PRICE_BOOKS_LENGTH              - 2, A.PriceBook 
        );
    }

    fclose(f);
    return 1;
}

Author *LoadAuthor(BookList *Books){
    if (Books == NULL || Books->theArray == NULL || Books->count <= 0)
        return NULL;

    Author *author;

    author = malloc (sizeof(Author));
    if (author == NULL)
        return NULL;

    author->Author = malloc (sizeof(char *) * Books -> count);
    if (author->Author == NULL){
        free(author);
        return NULL;
    }

    author -> count = 0;

    for (int i = 0; i < Books -> count; i++){
        int flag = 0;

        for (int j = 0; j < author->count; j++){
            if (strcmp(author -> Author[j], Books->theArray[i].AuthorBook) == 0){
                flag = 1;
                break;
            }
        }

        if (!flag){
            author->Author[author->count] = malloc (sizeof(char) * 32);
            if (author->Author[author->count] == NULL)
                continue;

            strcpy(author->Author[author->count], Books->theArray[i].AuthorBook);
            author->count++;
        }
    }

    for (int i = 0; i < author->count; i++){
        printf ("%s\n", author->Author[i]);
    }
    return author;
}
Type *LoadType(BookList *Books){
    if (Books == NULL || Books->theArray == NULL || Books->count <= 0)
        return NULL;

    Type *type;

    type = malloc (sizeof(Author));
    if (type == NULL)
        return NULL;

    type->Type = malloc (sizeof(char *) * Books -> count);
    if (type->Type == NULL){
        free(type);
        return NULL;
    }

    type->count = 0;

    for (int i = 0; i < Books -> count; i++){
        int flag = 0;

        for (int j = 0; j < type->count; j++){
            if (strcmp(type->Type[j], Books->theArray[i].TypeBook) == 0){
                flag = 1;
                break;
            }
        }

        if (!flag){
            type->Type[type->count] = malloc (sizeof(char) * 32);
            if (type->Type[type->count] == NULL)
                continue;

            strcpy(type->Type[type->count], Books->theArray[i].TypeBook);
            type->count++;
        }
    }

    for (int i = 0; i < type->count; i++){
        printf ("-%s\n", type->Type[i]);
    }

    return type;
}