#include "ManageBooks.h"
#include "raylib.h"
#include "libmanage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

void InitManageBooks(Role _role){
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
            (float)GetScreenWidth() / GetMonitorWidth(0),
            (float)GetScreenHeight() / GetMonitorHeight(0)
        },
        GetMousePosition()
    );

    ManageBooksUI customUI;
    LoadManageBooksUI(
        &customUI, 
        ManageBooksSize
    );

    MANAGEBOOKS_STATE State = MANAGEBOOKS_Dashboard;

    Font _Font[10];
    _Font[0] = SetFontUTF8 (ArialBold, 100);
    _Font[1] = SetFontUTF8 (Cooper, 50);
    _Font[2] = SetFontUTF8 (Gill, 50);
    _Font[3] = SetFontUTF8 (Roboto_Semibold, 50);

    BookList* Books = Loadbooks(BOOKS_FILE);
    AuthorList* Authors = LoadAuthor(AUTHORS_FILE);
    Type* Types = LoadType(Books);

    if (Books == NULL)
        printf ("FAILED!! Cann't load dataTruyen.txt");
    if (Authors == NULL)
        printf ("FAILED!! Cann't load author.txt");
    if (Types == NULL)
        printf ("FAILED!! Cann't load Type from Books");

    InputBox FindBar = {0};

    Texture2D Avatar = LoadTexture (MANAGEBOOKS_Avatar);
    Texture2D Icon_Find = LoadTexture (Icon_Find_lnk);


    while (!WindowShouldClose()){
        if (IsWindowResized()){
            LoadSize(
                &ManageBooksSize,
                (Vector2) {0},
                (Vector2) {
                    GetScreenWidth(),
                    GetScreenHeight()
                },
                (Vector2) {
                    (float)GetScreenWidth() / GetMonitorWidth(0),
                    (float)GetScreenHeight() / GetMonitorHeight(0)
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

        if (ManageBooksFunc(&State, &FindBar, Icon_Find, customUI, ManageBooksSize, _Font) == 1)
            Books->page = 1;
        

        switch(State)
        {
            Vector2 TextWidth;
            Rectangle HeaderBox;
            case MANAGEBOOKS_Dashboard:
                ShowBooks_Panel(ManageBooksSize, customUI, _Font, *Books, NULL, NULL);
                break;
            case MANAGEBOOKS_Author:
                TextWidth = MeasureTextEx (
                    _Font[0],
                    MANAGEBOOKS_Func_1,
                    customUI.HeaderBox.height * 0.8f,
                    2
                );

                HeaderBox = (Rectangle) {
                    customUI.HeaderBox.x + (customUI.HeaderBox.width - TextWidth.x) * 0.5f,
                    customUI.HeaderBox.y + customUI.HeaderBox.height * 0.1f,
                    TextWidth.x,
                    TextWidth.y
                };

                DrawTextEx(
                    _Font[0], 
                    MANAGEBOOKS_Func_1,
                    (Vector2) {
                        HeaderBox.x,
                        HeaderBox.y
                    },
                    HeaderBox.height,
                    2,
                    BLACK
                );
                break;
            case MANAGEBOOKS_Publisher:
                 TextWidth = MeasureTextEx (
                    _Font[0],
                    MANAGEBOOKS_Func_2,
                    customUI.HeaderBox.height * 0.8f,
                    2
                );

                HeaderBox = (Rectangle) {
                    customUI.HeaderBox.x + (customUI.HeaderBox.width - TextWidth.x) * 0.5f,
                    customUI.HeaderBox.y + customUI.HeaderBox.height * 0.1f,
                    TextWidth.x,
                    TextWidth.y
                };

                DrawTextEx(
                    _Font[0], 
                    MANAGEBOOKS_Func_2,
                    (Vector2) {
                        HeaderBox.x,
                        HeaderBox.y
                    },
                    HeaderBox.height,
                    2,
                    BLACK
                );
                break;
            case MANAGEBOOKS_Type:
                 TextWidth = MeasureTextEx (
                    _Font[0],
                    MANAGEBOOKS_Func_3,
                    customUI.HeaderBox.height * 0.8f,
                    2
                );

                HeaderBox = (Rectangle) {
                    customUI.HeaderBox.x + (customUI.HeaderBox.width - TextWidth.x) * 0.5f,
                    customUI.HeaderBox.y + customUI.HeaderBox.height * 0.1f,
                    TextWidth.x,
                    TextWidth.y
                };

                DrawTextEx(
                    _Font[0], 
                    MANAGEBOOKS_Func_3,
                    (Vector2) {
                        HeaderBox.x,
                        HeaderBox.y
                    },
                    HeaderBox.height,
                    2,
                    BLACK
                );
                break;
            case MANAGEBOOKS_Find:
                 TextWidth = MeasureTextEx (
                    _Font[0],
                    MANAGEBOOKS_Func_5,
                    customUI.HeaderBox.height * 0.8f,
                    2
                );

                HeaderBox = (Rectangle) {
                    customUI.HeaderBox.x + (customUI.HeaderBox.width - TextWidth.x) * 0.5f,
                    customUI.HeaderBox.y + customUI.HeaderBox.height * 0.1f,
                    TextWidth.x,
                    TextWidth.y
                };

                DrawTextEx(
                    _Font[0], 
                    MANAGEBOOKS_Func_5,
                    (Vector2) {
                        HeaderBox.x,
                        HeaderBox.y
                    },
                    HeaderBox.height,
                    2,
                    BLACK
                );
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

    Savebooks(*Books);/**/
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

void ShowBooks_Panel(Size size, ManageBooksUI UI, Font *_Font, BookList Books, char *author, char *type) {
    Rectangle hitbox = {
        UI.Panel.x + UI.Panel.width * 0.08f,
        UI.Panel.y + DISTANCE_BOOKS,
        UI.Panel.width * 0.8f,
        UI.Panel.height * 0.06f * (pow (0.3, size.Scale.x) + 0.6)
    };

    BeginScissorMode(
        UI.Panel.x,
        UI.Panel.y,
        UI.Panel.width,
        UI.Panel.height
    );

    float Distance_Books = DISTANCE_BOOKS;
    float ScrollBar = UI.Panel.height * 0.1f + UI.Panel.height * 0.05f + (hitbox.height + Distance_Books) * Books.QuantityForOnePage;


    int i = (Books.page - 1) * Books.QuantityForOnePage;
    int count = 0;
    int flag = 0;
    while (count < Books.QuantityForOnePage && i != Books.count){
        if (author != NULL){
            if (strcmp(author, Books.theArray[i].AuthorBook) == 0) 
                flag = 1;
        }
        else if (author != NULL){
            if(strcmp(type, Books.theArray[i].TypeBook) == 0)
                flag = 1;
        }
        else 
            flag = 1;

        if (flag == 1){
            float radius;
            int FontSize;
            float TextWidth;

            hitbox = (Rectangle) {
                hitbox.x,
                hitbox.y + (hitbox.height + Distance_Books) * (bool) count,
                hitbox.width,
                hitbox.height
            };

            radius = FindRoundness(hitbox.width * 0.005f, hitbox.width, hitbox.height);
            DrawRectangleRounded (
                hitbox,
                radius,
                50,
                BLACK    
            );

            Rectangle code_hitbox = {
                hitbox.x,
                hitbox.y,
                hitbox.width * 0.1f / (1/(pow(0.2, size.Scale.x) - 1.5) + 1.77),
                hitbox.height
            };

            radius = FindRoundness(hitbox.width * 0.005f, code_hitbox.width, code_hitbox.height);
            DrawRectangleRounded(
                code_hitbox,
                radius,
                50,
                WHITE
            );

            FontSize = FindFontSize(code_hitbox.width * 0.8f, _Font[0], 1, Books.theArray[i].CodeBook);

            DrawTextEx (
                _Font[0],
                Books.theArray[i].CodeBook,
                (Vector2) {
                    code_hitbox.x + code_hitbox.width * 0.1f,
                    code_hitbox.y + (code_hitbox.height - FontSize) / 2
                },
                FontSize,
                1,
                BLACK
            );

            TextWidth = MeasureTextEx (
                _Font[0],
                Books.theArray[i].NameBook,
                code_hitbox.height * 0.6f,
                1
            ).x;

            Rectangle name_hitbox = {
                code_hitbox.x + hitbox.width,
                code_hitbox.y + code_hitbox.height * 0.1f,
                TextWidth + code_hitbox.width * 0.1f,
                code_hitbox.height * 0.8f
            };

            radius = FindRoundness (name_hitbox.height * 0.1f, name_hitbox.width, name_hitbox.height);
            DrawRectangleRounded (
                name_hitbox,
                radius,
                10,
                SOFTWHITE
            );
            
            count++;
        }
        i++;
    }   
    if (author == NULL && type == NULL)
        Books.page++;

    EndScissorMode();    
}

bool ManageBooksFunc(MANAGEBOOKS_STATE *State, InputBox *FindBar, Texture2D Icon_Find, ManageBooksUI UI, Size size, Font *_Font){
    int _return = 0;
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
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            *State = MANAGEBOOKS_Dashboard;
            _return = 1;
        }
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
            _return = 1;
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
            _return = 1;
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
            _return = 1;
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
            _return = 1;
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

        float dx = ((w - (FindBarTextPos.width - 5)) > 0) ? w - (FindBarTextPos.width - 5) : 0;
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
            _return = 1;
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
    if (CheckCollisionPointRec(size.Mouse, IconFindBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && FindBar -> length > 0){
        *State = MANAGEBOOKS_Find;
        _return = 1;
    }

    return _return;
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

    bookList->page = 1;
    bookList->QuantityForOnePage = QUANTITYFORONEPAGE;

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
bool Savebooks(BookList Books){
    FILE *f = fopen (BOOKS_FILE, "w");

    if (f == NULL){
        printf ("Save FAILED!!!!");
        return 0;
    }

    fprintf (f, "Số quyển truyện: %d\n", Books.count);
    fprintf (f, "Số lượng truyện hiện tại: %d\n", Books.stockBooks);
    fprintf (f, "Số lượng truyện gốc: %d\n", Books.totalImportBooks);

    Book A;
    for (int i = 0; i < Books.count; i++)
    {
        A = Books.theArray[i];

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

AuthorList *LoadAuthor(const char *filename){
    FILE *f = fopen(filename, "r");
    if (f == NULL)
        return NULL;

    int count;
    fscanf (f, "Tổng: %d\n", &count);


    AuthorList *A = malloc (sizeof(AuthorList));
    if (A == NULL) 
        return NULL;

    A->Author = malloc (sizeof(Author) * count);
    if (A->Author == NULL){
        free(A);
        return NULL;
    }
    A->count = 0;
    while (!(fscanf(f, " | %[^|]| %[^|]|\n", A->Author[A->count].code, A->Author[A->count].name) == 2)) {
        trim(A->Author[A->count].code);
        trim(A->Author[A->count].name);
        A->count++;
    }

    fclose(f);
    if (A->count != count){
        free(A);
        return NULL;
    }
    else 
        return A;
}
bool SaveAuthor(AuthorList _Author){
    FILE *f = fopen(AUTHORS_FILE, "w");

    int i;
    for (i = 0; i < _Author.count; i++){
        Author A = _Author.Author[i];

        fprintf (f, "| %*s | %*s |\n",
            UTF8Width(A.code, AUTHORCODE_AUTHORS_LENGTH), A.code,
            UTF8Width(A.name, AUTHOR_AUTHORS_LENGTH), A.name
        );
    }

    fclose(f);

    if (i != _Author.count) 
        return false;
    else 
        return true;

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

    return type;
}