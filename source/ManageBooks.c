#include "ManageBooks.h"
#include "raylib.h"
#include "libmanage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

// =========================================================================
// BLUE-WHITE UI THEME PALETTE
// =========================================================================
#define BW_TITLE_BG (Color){13, 32, 78, 255}    // Deep navy   – title bar
#define BW_HEADER_BG (Color){20, 48, 108, 255}  // Medium navy – header/toolbar
#define BW_PANEL_BG (Color){240, 246, 255, 255} // Very light blue-white – main panel
#define BW_CARD (Color){18, 44, 100, 255}       // Dark navy   – card default
#define BW_CARD_HOVER (Color){30, 68, 148, 255} // Brighter navy – card hover
#define BW_ACCENT (Color){60, 135, 240, 255}    // Bright blue  – primary accent
#define BW_SKY (Color){105, 170, 255, 255}      // Sky blue     – highlight / active text
#define BW_DIM (Color){150, 185, 230, 255}      // Muted blue   – secondary text
#define BW_DIVIDER (Color){195, 215, 245, 255}  // Light blue   – borders / dividers
#define BW_INPUT_BG (Color){248, 252, 255, 255} // Near-white   – input field bg
#define BW_DETAIL_BG (Color){10, 25, 62, 255}   // Very dark navy – detail panel bg
#define BW_SCROLLBAR (Color){55, 130, 235, 180} // Semi-transparent accent – scrollbar
#define BW_NAV_TEXT (Color){225, 238, 255, 255} // Off-white    – nav link text
#define BW_LABEL (Color){28, 65, 138, 255}      // Mid navy     – form labels
#define BW_FORM_TITLE (Color){13, 32, 78, 255}  // Deep navy    – form heading

// Helpers for rounded scroll handles
#define DRAW_SCROLLBAR(handle, hovered) \
    DrawRectangleRounded((handle), 1.0f, 10, (hovered) ? BW_SKY : Fade(BW_SCROLLBAR, 0.7f))


static MANAGEBOOKS_STATE g_searchContext = MANAGEBOOKS_Dashboard;

void InitManageBooks(Role _role)
{
    Size ManageBooksSize;
    LoadSize(
        &ManageBooksSize,
        (Vector2){GetMonitorWidth(0), GetMonitorHeight(0)},
        (Vector2){GetScreenWidth(), GetScreenHeight()},
        (Vector2){(float)GetScreenWidth() / GetMonitorWidth(0), (float)GetScreenHeight() / GetMonitorHeight(0)},
        GetMousePosition());

    ManageBooksUI customUI;
    LoadManageBooksUI(&customUI, ManageBooksSize);

    MANAGEBOOKS_STATE State = MANAGEBOOKS_Dashboard;
    MANAGEBOOKS_STATE prevState = MANAGEBOOKS_Dashboard;
    MANAGEBOOKS_STATE detailParentState = MANAGEBOOKS_Dashboard;

    Font _Font[4];
    _Font[0] = SetFontUTF8(ArialBold, 100);
    _Font[1] = SetFontUTF8(Cooper, 50);
    _Font[2] = SetFontUTF8(Gill, 50);
    _Font[3] = SetFontUTF8(Roboto_Semibold, 50);

    BookList *Books = Loadbooks(BOOKS_FILE);
    AuthorList *Authors = LoadAuthor(AUTHORS_FILE);
    Type *Types = LoadType(Books);
    PublisherList *Publishers = LoadPublisher(Books);

    if (Books == NULL)
        printf("FAILED!! Cann't load dataTruyen.txt\n");
    if (Authors == NULL)
        printf("FAILED!! Cann't load author.txt\n");
    if (Types == NULL)
        printf("FAILED!! Cann't load Type from Books\n");

    LoadDescription(Books, DESCRIPTION_FILE);

    InputBox FindBar = {0};
    Texture2D Icon_Find = LoadTexture(Icon_Find_lnk);

    float wheel = 0;
    char selectedAuthor[256] = "";
    char selectedPublisher[256] = "";
    char selectedType[256] = "";

    int selectedBookIndex = -1;
    int requestDeleteIndex = -1;

    InputBox addInputs[8] = {0};
    bool isEditMode = false;
    Book tempEditBook = {0};
    float volWheel = 0;

    bool showOnlyInStock = false;
    static float animRatio = 0.0f;

    while (!WindowShouldClose())
    {
        UpdateParticlesPosition(ManageBooksSize);

        if (IsWindowResized())
        {
            LoadSize(&ManageBooksSize, (Vector2){0}, (Vector2){GetScreenWidth(), GetScreenHeight()},
                     (Vector2){(float)GetScreenWidth() / GetMonitorWidth(0), (float)GetScreenHeight() / GetMonitorHeight(0)}, (Vector2){0});
            LoadManageBooksUI(&customUI, ManageBooksSize);
        }

        ManageBooksSize.Mouse = GetMousePosition();

        BeginDrawing();
        ClearBackground(AnimatedBackground());
        DrawBackgroundParticles();

        ManageBooksTitle(customUI);

        MANAGEBOOKS_STATE loopOldState = State;
        if (ManageBooksFunc(&State, &FindBar, Icon_Find, customUI, ManageBooksSize, _Font) == 1)
        {
            if (State == MANAGEBOOKS_Find)
            {
                if (loopOldState == MANAGEBOOKS_Dashboard || loopOldState == MANAGEBOOKS_Author || loopOldState == MANAGEBOOKS_Publisher || loopOldState == MANAGEBOOKS_Type)
                {
                    g_searchContext = loopOldState;
                }
            }
            Books->pos = 0;
            Books->currentPage = 1;
            wheel = 0;
            selectedAuthor[0] = '\0';
            selectedPublisher[0] = '\0';
            selectedType[0] = '\0';
        }

        bool isFiltering = (State == MANAGEBOOKS_Author && selectedAuthor[0] != '\0') ||
                           (State == MANAGEBOOKS_Publisher && selectedPublisher[0] != '\0') ||
                           (State == MANAGEBOOKS_Type && selectedType[0] != '\0') ||
                           (State == MANAGEBOOKS_Find && FindBar.text[0] != '\0');

        float btnHeight = customUI.HeaderBox.height * 0.6f;
        float marginX = customUI.HeaderBox.width * 0.02f;
        int fontSizeBtn = (int)(btnHeight * 0.75f);

        // ── Back button ──────────────────────────────────────────────────────
        if (State == MANAGEBOOKS_Add || State == MANAGEBOOKS_Detail || isFiltering)
        {
            float btnWidthBack = btnHeight * 2.5f;
            Rectangle btnBackHeader = {
                customUI.HeaderBox.x + marginX,
                customUI.HeaderBox.y + (customUI.HeaderBox.height - btnHeight) / 2.0f,
                btnWidthBack, btnHeight};
            bool hoverBack = CheckCollisionPointRec(ManageBooksSize.Mouse, btnBackHeader);

            // Shadow
            DrawRectangleRounded((Rectangle){btnBackHeader.x + 2, btnBackHeader.y + 2, btnBackHeader.width, btnBackHeader.height},
                                 0.45f, 12, Fade(BLACK, 0.25f));
            DrawRectangleRounded(btnBackHeader, 0.45f, 12, hoverBack ? MAROON : ERRORRED);
            DrawRectangleRoundedLinesEx(btnBackHeader, 0.45f, 12, 1.5f, Fade(WHITE, 0.25f));

            const char *textBack = "Back";
            Vector2 sizeBack = MeasureTextEx(_Font[0], textBack, fontSizeBtn, 1);
            DrawTextEx(_Font[0], textBack,
                       (Vector2){btnBackHeader.x + (btnBackHeader.width - sizeBack.x) / 2.0f,
                                 btnBackHeader.y + (btnBackHeader.height - sizeBack.y) / 2.0f},
                       fontSizeBtn, 1, WHITE);

            if (hoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (State == MANAGEBOOKS_Add)
                {
                    State = prevState;
                    isEditMode = false;
                }
                else if (State == MANAGEBOOKS_Detail)
                {
                    State = detailParentState;
                    isEditMode = false;
                }
                else
                {
                    if (State == MANAGEBOOKS_Author)
                        selectedAuthor[0] = '\0';
                    else if (State == MANAGEBOOKS_Publisher)
                        selectedPublisher[0] = '\0';
                    else if (State == MANAGEBOOKS_Type)
                        selectedType[0] = '\0';
                    else if (State == MANAGEBOOKS_Find)
                    {
                        FindBar.text[0] = '\0';
                        FindBar.length = 0;
                        State = MANAGEBOOKS_Dashboard;
                    }
                    wheel = 0;
                    Books->currentPage = 1;
                }
            }
        }

        bool isShowingBookList = (State == MANAGEBOOKS_Dashboard) ||
                                 (State == MANAGEBOOKS_Author && selectedAuthor[0] != '\0') ||
                                 (State == MANAGEBOOKS_Publisher && selectedPublisher[0] != '\0') ||
                                 (State == MANAGEBOOKS_Type && selectedType[0] != '\0') ||
                                 (State == MANAGEBOOKS_Find);

        float targetRatio = showOnlyInStock ? 1.0f : 0.0f;
        animRatio += (targetRatio - animRatio) * 12.0f * GetFrameTime();

        // ── Stock filter toggle ──────────────────────────────────────────────
        if (isShowingBookList)
        {
            float filterW = btnHeight * 5.0f;
            float filterH = btnHeight * 0.95f;
            Rectangle filterBox = {
                customUI.HeaderBox.x + (customUI.HeaderBox.width - filterW) / 2.0f,
                customUI.HeaderBox.y + (customUI.HeaderBox.height - filterH) / 2.0f,
                filterW, filterH};

            Rectangle btnAll = {filterBox.x, filterBox.y, filterBox.width / 2.0f, filterBox.height};
            Rectangle btnInStock = {filterBox.x + filterBox.width / 2.0f, filterBox.y, filterBox.width / 2.0f, filterBox.height};

            bool hoverAll = CheckCollisionPointRec(ManageBooksSize.Mouse, btnAll);
            bool hoverInStock = CheckCollisionPointRec(ManageBooksSize.Mouse, btnInStock);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (hoverAll && showOnlyInStock)
                {
                    showOnlyInStock = false;
                    wheel = 0;
                    Books->currentPage = 1;
                }
                if (hoverInStock && !showOnlyInStock)
                {
                    showOnlyInStock = true;
                    wheel = 0;
                    Books->currentPage = 1;
                }
            }

            // Track
            DrawRectangleRounded(filterBox, 1.0f, 20, Fade(BW_TITLE_BG, 0.85f));
            DrawRectangleRoundedLinesEx(filterBox, 1.0f, 20, 1.5f, Fade(BW_SKY, 0.35f));

            float padding = 4.0f;
            float handleW = filterBox.width / 2.0f - padding;
            float handleH = filterBox.height - padding * 2.0f;
            float leftX = filterBox.x + padding;
            float rightX = filterBox.x + filterBox.width - handleW - padding;
            float currentHandleX = leftX + animRatio * (rightX - leftX);

            Rectangle handleRec = {currentHandleX, filterBox.y + padding, handleW, handleH};

            // Handle shadow
            DrawRectangleRounded((Rectangle){handleRec.x + 2, handleRec.y + 2, handleRec.width, handleRec.height},
                                 1.0f, 20, Fade(BLACK, 0.2f));
            DrawRectangleRounded(handleRec, 1.0f, 20, BW_ACCENT);
            DrawRectangleRoundedLinesEx(handleRec, 1.0f, 20, 1.0f, Fade(WHITE, 0.35f));

            int fSize = (int)(fontSizeBtn * 0.8f);
            Color colorAll = (animRatio < 0.5f) ? WHITE : Fade(BW_DIM, 0.7f);
            Color colorInStock = (animRatio > 0.5f) ? WHITE : Fade(BW_DIM, 0.7f);

            DrawTextEx(_Font[3], "Tất cả",
                       (Vector2){btnAll.x + (btnAll.width - MeasureTextEx(_Font[3], "Tất cả", fSize, 1).x) / 2,
                                 btnAll.y + (btnAll.height - fSize) / 2},
                       fSize, 1, colorAll);
            DrawTextEx(_Font[3], "Còn kho",
                       (Vector2){btnInStock.x + (btnInStock.width - MeasureTextEx(_Font[3], "Còn kho", fSize, 1).x) / 2,
                                 btnInStock.y + (btnInStock.height - fSize) / 2},
                       fSize, 1, colorInStock);
        }

        // ── Add / Edit buttons (admin) ───────────────────────────────────────
        if (_role == ADMINISTRATOR && State != MANAGEBOOKS_Add)
        {
            if (State != MANAGEBOOKS_Detail)
            {
                float btnWidthAdd = btnHeight * 4.5f;
                Rectangle btnAddHeader = {
                    customUI.HeaderBox.x + customUI.HeaderBox.width - btnWidthAdd - marginX,
                    customUI.HeaderBox.y + (customUI.HeaderBox.height - btnHeight) / 2.0f,
                    btnWidthAdd, btnHeight};
                bool hoverAdd = CheckCollisionPointRec(ManageBooksSize.Mouse, btnAddHeader);

                DrawRectangleRounded((Rectangle){btnAddHeader.x + 2, btnAddHeader.y + 2, btnAddHeader.width, btnAddHeader.height},
                                     0.45f, 12, Fade(BLACK, 0.25f));
                DrawRectangleRounded(btnAddHeader, 0.45f, 12, hoverAdd ? BW_SKY : BW_ACCENT);
                DrawRectangleRoundedLinesEx(btnAddHeader, 0.45f, 12, 1.5f, Fade(WHITE, 0.25f));

                const char *textAdd = "Thêm Truyện";
                Vector2 sizeAdd = MeasureTextEx(_Font[3], textAdd, fontSizeBtn, 1);
                DrawTextEx(_Font[3], textAdd,
                           (Vector2){btnAddHeader.x + (btnAddHeader.width - sizeAdd.x) / 2.0f,
                                     btnAddHeader.y + (btnAddHeader.height - sizeAdd.y) / 2.0f},
                           fontSizeBtn, 1, WHITE);

                if (hoverAdd && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    isEditMode = false;
                    for (int i = 0; i < 8; i++)
                    {
                        addInputs[i].text[0] = '\0';
                        addInputs[i].length = 0;
                        addInputs[i].isFocused = false;
                    }

                    memset(&tempEditBook, 0, sizeof(Book));
                    tempEditBook.volumeCount = 0;
                    tempEditBook.TotalStock = 0;

                    int maxId = 0;
                    for (int k = 0; k < Books->count; k++)
                    {
                        if (Books->theArray[k].CodeBook[0] == 'T' || Books->theArray[k].CodeBook[0] == 't')
                        {
                            int currentId = atoi(&Books->theArray[k].CodeBook[1]);
                            if (currentId > maxId)
                                maxId = currentId;
                        }
                    }
                    sprintf(addInputs[0].text, "T%03d", maxId + 1);
                    addInputs[0].length = strlen(addInputs[0].text);
                    strcpy(tempEditBook.CodeBook, addInputs[0].text);

                    FindBar.isFocused = false;
                    prevState = State;
                    State = MANAGEBOOKS_Add;
                }
            }
            else
            {
                float btnWidthEdit = btnHeight * 4.0f;
                Rectangle btnEditHeader = {
                    customUI.HeaderBox.x + customUI.HeaderBox.width - btnWidthEdit - marginX,
                    customUI.HeaderBox.y + (customUI.HeaderBox.height - btnHeight) / 2.0f,
                    btnWidthEdit, btnHeight};
                bool hoverEdit = CheckCollisionPointRec(ManageBooksSize.Mouse, btnEditHeader);

                DrawRectangleRounded((Rectangle){btnEditHeader.x + 2, btnEditHeader.y + 2, btnEditHeader.width, btnEditHeader.height},
                                     0.45f, 12, Fade(BLACK, 0.25f));
                DrawRectangleRounded(btnEditHeader, 0.45f, 12, hoverEdit ? Fade(GOLD, 0.85f) : GOLD);
                DrawRectangleRoundedLinesEx(btnEditHeader, 0.45f, 12, 1.5f, Fade(WHITE, 0.25f));

                const char *textEdit = "Chỉnh Sửa";
                Vector2 sizeEdit = MeasureTextEx(_Font[3], textEdit, fontSizeBtn, 1);
                DrawTextEx(_Font[3], textEdit,
                           (Vector2){btnEditHeader.x + (btnEditHeader.width - sizeEdit.x) / 2.0f,
                                     btnEditHeader.y + (btnEditHeader.height - sizeEdit.y) / 2.0f},
                           fontSizeBtn, 1, (Color){30, 30, 30, 255});

                if (hoverEdit && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    isEditMode = true;
                    tempEditBook = Books->theArray[selectedBookIndex];
                    volWheel = 0;

                    strcpy(addInputs[0].text, tempEditBook.CodeBook);
                    addInputs[0].length = strlen(addInputs[0].text);
                    strcpy(addInputs[1].text, tempEditBook.NameBook);
                    addInputs[1].length = strlen(addInputs[1].text);
                    strcpy(addInputs[2].text, tempEditBook.AuthorBook);
                    addInputs[2].length = strlen(addInputs[2].text);
                    strcpy(addInputs[3].text, tempEditBook.TypeBook);
                    addInputs[3].length = strlen(addInputs[3].text);
                    strcpy(addInputs[4].text, tempEditBook.PublisherBook);
                    addInputs[4].length = strlen(addInputs[4].text);
                    sprintf(addInputs[5].text, "%d", tempEditBook.YearBook);
                    addInputs[5].length = strlen(addInputs[5].text);
                    sprintf(addInputs[6].text, "%d", tempEditBook.PriceBook);
                    addInputs[6].length = strlen(addInputs[6].text);
                    strcpy(addInputs[7].text, tempEditBook.Description);
                    addInputs[7].length = strlen(addInputs[7].text);

                    prevState = State;
                    State = MANAGEBOOKS_Add;
                }
            }

            if (requestDeleteIndex != -1)
            {
                char authorToCheck[256];
                strcpy(authorToCheck, Books->theArray[requestDeleteIndex].AuthorBook);
                char imgPath[512];
                sprintf(imgPath, "img/img_books/%s.jpg", Books->theArray[requestDeleteIndex].CodeBook);
                remove(imgPath);

                for (int k = requestDeleteIndex; k < Books->count - 1; k++)
                    Books->theArray[k] = Books->theArray[k + 1];

                Books->count--;
                Savebooks(*Books);
                requestDeleteIndex = -1;

                bool hasBooksLeft = false;
                for (int i = 0; i < Books->count; i++)
                {
                    if (strcmp(Books->theArray[i].AuthorBook, authorToCheck) == 0)
                    {
                        hasBooksLeft = true;
                        break;
                    }
                }

                if (!hasBooksLeft)
                {
                    for (int i = 0; i < Authors->count; i++)
                    {
                        if (strcmp(Authors->Author[i].name, authorToCheck) == 0)
                        {
                            for (int j = i; j < Authors->count - 1; j++)
                                Authors->Author[j] = Authors->Author[j + 1];
                            Authors->count--;
                            SaveAuthor(*Authors);
                            break;
                        }
                    }
                }
            }
        }

        int totalMatch = 0;
        int clickedBook = -1;
        bool showPagination = false;

        switch (State)
        {
        case MANAGEBOOKS_Dashboard:
            totalMatch = CountStrInBooks(Books, NULL, IDLE_STATEFINDBOOK, showOnlyInStock);
            Books->totalPages = (int)ceil((float)totalMatch / Books->QuantityForOnePage);
            if (Books->totalPages < 1)
                Books->totalPages = 1;
            clickedBook = ShowBooks_Panel(ManageBooksSize, customUI, &wheel, _Font, Books, 0, NULL, _role, &showPagination, &requestDeleteIndex, showOnlyInStock);
            if (clickedBook != -1)
            {
                selectedBookIndex = clickedBook;
                detailParentState = State;
                prevState = State;
                wheel = 0;
                State = MANAGEBOOKS_Detail;
            }
            DrawPagination(customUI, Books, _Font, ManageBooksSize, &wheel, NULL, showPagination);
            break;

        case MANAGEBOOKS_Author:
            if (selectedAuthor[0] == '\0')
                ShowAuthor_Panel(ManageBooksSize, customUI, &wheel, _Font, Authors, selectedAuthor, Books);
            else
            {
                totalMatch = CountStrInBooks(Books, selectedAuthor, AUTHOR, showOnlyInStock);
                Books->totalPages = (int)ceil((float)totalMatch / Books->QuantityForOnePage);
                if (Books->totalPages < 1)
                    Books->totalPages = 1;
                clickedBook = ShowBooks_Panel(ManageBooksSize, customUI, &wheel, _Font, Books, 1, selectedAuthor, _role, &showPagination, &requestDeleteIndex, showOnlyInStock);
                if (clickedBook != -1)
                {
                    selectedBookIndex = clickedBook;
                    detailParentState = State;
                    prevState = State;
                    wheel = 0;
                    State = MANAGEBOOKS_Detail;
                }
                DrawPagination(customUI, Books, _Font, ManageBooksSize, &wheel, NULL, showPagination);
            }
            break;

        case MANAGEBOOKS_Publisher:
            if (selectedPublisher[0] == '\0')
                ShowPublisher_Panel(ManageBooksSize, customUI, &wheel, _Font, Publishers, selectedPublisher, Books);
            else
            {
                totalMatch = CountStrInBooks(Books, selectedPublisher, PUBLISHER, showOnlyInStock);
                Books->totalPages = (int)ceil((float)totalMatch / Books->QuantityForOnePage);
                if (Books->totalPages < 1)
                    Books->totalPages = 1;
                clickedBook = ShowBooks_Panel(ManageBooksSize, customUI, &wheel, _Font, Books, 3, selectedPublisher, _role, &showPagination, &requestDeleteIndex, showOnlyInStock);
                if (clickedBook != -1)
                {
                    selectedBookIndex = clickedBook;
                    detailParentState = State;
                    prevState = State;
                    wheel = 0;
                    State = MANAGEBOOKS_Detail;
                }
                DrawPagination(customUI, Books, _Font, ManageBooksSize, &wheel, NULL, showPagination);
            }
            break;

        case MANAGEBOOKS_Type:
            if (selectedType[0] == '\0')
                ShowType_Panel(ManageBooksSize, customUI, &wheel, _Font, Types, selectedType, Books);
            else
            {
                totalMatch = CountStrInBooks(Books, selectedType, TYPE, showOnlyInStock);
                Books->totalPages = (int)ceil((float)totalMatch / Books->QuantityForOnePage);
                if (Books->totalPages < 1)
                    Books->totalPages = 1;
                clickedBook = ShowBooks_Panel(ManageBooksSize, customUI, &wheel, _Font, Books, 2, selectedType, _role, &showPagination, &requestDeleteIndex, showOnlyInStock);
                if (clickedBook != -1)
                {
                    selectedBookIndex = clickedBook;
                    detailParentState = State;
                    prevState = State;
                    wheel = 0;
                    State = MANAGEBOOKS_Detail;
                }
                DrawPagination(customUI, Books, _Font, ManageBooksSize, &wheel, NULL, showPagination);
            }
            break;

        case MANAGEBOOKS_Detail:
            if (selectedBookIndex >= 0 && selectedBookIndex < Books->count)
            {
                int detailSignal = ShowBookDetail_Panel(ManageBooksSize, customUI, &wheel, _Font, Books, selectedBookIndex, &State, prevState, _role);
                if (detailSignal == 2)
                    Savebooks(*Books);
            }
            break;

        case MANAGEBOOKS_Add:
        {
            int addStatus = ShowAddBook_Panel(ManageBooksSize, customUI, _Font, addInputs, isEditMode, &tempEditBook, &volWheel);
            if (addStatus == 1)
            {
                strcpy(tempEditBook.CodeBook, addInputs[0].text);
                strcpy(tempEditBook.NameBook, addInputs[1].text);
                strcpy(tempEditBook.NormNameBook, addInputs[1].text);
                strcpy(tempEditBook.AuthorBook, addInputs[2].text);
                strcpy(tempEditBook.TypeBook, addInputs[3].text);
                strcpy(tempEditBook.PublisherBook, addInputs[4].text);
                tempEditBook.YearBook = atoi(addInputs[5].text);
                tempEditBook.PriceBook = atoi(addInputs[6].text);
                strcpy(tempEditBook.Description, addInputs[7].text);
                if (isEditMode)
                    Books->theArray[selectedBookIndex] = tempEditBook;
                else if (Books->count < Books->capacity)
                {
                    Books->theArray[Books->count] = tempEditBook;
                    Books->count++;
                }
                Savebooks(*Books);
                State = prevState;
                isEditMode = false;
            }
            break;
        }

        case MANAGEBOOKS_Find:
            totalMatch = CountStrInBooks(Books, FindBar.text, SEARCH_ALL, showOnlyInStock);
            Books->totalPages = (int)ceil((float)totalMatch / Books->QuantityForOnePage);
            if (Books->totalPages < 1)
                Books->totalPages = 1;
            clickedBook = ShowBooks_Panel(ManageBooksSize, customUI, &wheel, _Font, Books, 4, FindBar.text, _role, &showPagination, &requestDeleteIndex, showOnlyInStock);
            if (clickedBook != -1)
            {
                selectedBookIndex = clickedBook;
                detailParentState = State;
                prevState = State;
                wheel = 0;
                State = MANAGEBOOKS_Detail;
            }
            DrawPagination(customUI, Books, _Font, ManageBooksSize, &wheel, NULL, showPagination);
            break;

        case MANAGEBOOKS_Main:
            break;
        }

        EndDrawing();
        if (State == MANAGEBOOKS_Main)
            break;
    }

    for (int i = 0; i < 4; i++)
        UnloadFont(_Font[i]);
    UnloadTexture(Icon_Find);
    Savebooks(*Books);
    if (Books->theArray)
        free(Books->theArray);
    free(Books);
    if (Authors->Author)
        free(Authors->Author);
    free(Authors);
    if (Types)
    {
        for (int i = 0; i < Types->count; i++)
            free(Types->Type[i]);
        free(Types->Type);
        free(Types);
    }
    if (Publishers)
    {
        for (int i = 0; i < Publishers->count; i++)
            free(Publishers->Publisher[i]);
        free(Publishers->Publisher);
        free(Publishers);
    }
}

// =========================================================================
// ShowBookDetail_Panel
// =========================================================================
int ShowBookDetail_Panel(Size size, ManageBooksUI UI, float *wheel, Font *_Font, BookList *Books, int bookIndex, MANAGEBOOKS_STATE *State, MANAGEBOOKS_STATE prevState, Role _role)
{
    int signalReturn = 0;
    Book book = Books->theArray[bookIndex];
    Rectangle detailArea = {UI.Panel.x, UI.Panel.y, UI.Panel.width, UI.Panel.height};

    // Deep-navy background
    DrawRectangleRec(detailArea, BW_DETAIL_BG);
    // Subtle top gradient overlay
    DrawRectangleGradientV((int)detailArea.x, (int)detailArea.y,
                           (int)detailArea.width, 70,
                           Fade(BW_ACCENT, 0.18f), BLANK);

    static Texture2D currentCover = {0};
    static char currentLoadedCode[256] = "";

    if (strcmp(book.CodeBook, currentLoadedCode) != 0)
    {
        if (currentCover.id != 0)
        {
            UnloadTexture(currentCover);
            currentCover.id = 0;
        }
        char imgPath[512];
        sprintf(imgPath, "img/img_books/%s.jpg", book.CodeBook);
        if (FileExists(imgPath))
            currentCover = LoadTexture(imgPath);
        strcpy(currentLoadedCode, book.CodeBook);
    }

    float leftX = detailArea.x + 50.0f;
    float startY = detailArea.y + 40.0f;
    float targetW = 220.0f, targetH = 300.0f;
    Rectangle destRec = {leftX, startY, targetW, targetH};

    // Cover shadow
    DrawRectangleRounded((Rectangle){destRec.x + 4, destRec.y + 4, destRec.width, destRec.height},
                         0.05f, 8, Fade(BLACK, 0.45f));

    if (currentCover.id != 0 && currentCover.width > 0 && currentCover.height > 0)
    {
        float scaleX = targetW / (float)currentCover.width;
        float scaleY = targetH / (float)currentCover.height;
        float scale = (scaleX > scaleY) ? scaleX : scaleY;
        float sourceW = targetW / scale;
        float sourceH = targetH / scale;
        float sourceX = ((float)currentCover.width - sourceW) / 2.0f;
        float sourceY = ((float)currentCover.height - sourceH) / 2.0f;
        DrawTexturePro(currentCover, (Rectangle){sourceX, sourceY, sourceW, sourceH}, destRec, (Vector2){0, 0}, 0.0f, WHITE);
    }
    else
    {
        DrawRectangleRec(destRec, Fade(BW_CARD, 0.9f));
        DrawTextEx(_Font[0], "NO IMAGE", (Vector2){leftX + 45, startY + 140}, 25, 1, BW_DIM);
    }
    // Cover border (blue glow)
    DrawRectangleLinesEx(destRec, 2.5f, Fade(BW_ACCENT, 0.65f));

    float rightX = leftX + targetW + 50.0f;
    float currentY = startY;
    float maxRightX = detailArea.x + detailArea.width - 20.0f;
    Rectangle clipRight = {rightX, detailArea.y, maxRightX - rightX, detailArea.height};
    Rectangle safeClip = GetCollisionRec(clipRight, detailArea);

    if (safeClip.width > 0 && safeClip.height > 0)
        BeginScissorMode((int)safeClip.x, (int)safeClip.y, (int)safeClip.width, (int)safeClip.height);

    DrawTextEx(_Font[0], book.NameBook, (Vector2){rightX, currentY}, 36, 1, WHITE);
    currentY += 50;

    int fontSizeInfo = 20;
    int spacingY = 32;
    Color labelCol = BW_DIM;
    Color valueCol = (Color){220, 235, 255, 255};
    char buffer[256];

    DrawTextEx(_Font[0], "+ Tên khác:", (Vector2){rightX, currentY}, fontSizeInfo, 1, labelCol);
    DrawTextEx(_Font[0], book.NormNameBook, (Vector2){rightX + 220, currentY}, fontSizeInfo, 1, valueCol);
    currentY += spacingY;
    DrawTextEx(_Font[0], "+ Tác giả:", (Vector2){rightX, currentY}, fontSizeInfo, 1, labelCol);
    DrawTextEx(_Font[0], book.AuthorBook, (Vector2){rightX + 220, currentY}, fontSizeInfo, 1, valueCol);
    currentY += spacingY;
    DrawTextEx(_Font[0], "+ Thể loại:", (Vector2){rightX, currentY}, fontSizeInfo, 1, labelCol);
    DrawTextEx(_Font[0], book.TypeBook, (Vector2){rightX + 220, currentY}, fontSizeInfo, 1, BW_SKY);
    currentY += spacingY;
    DrawTextEx(_Font[0], "+ Năm Sản xuất:", (Vector2){rightX, currentY}, fontSizeInfo, 1, labelCol);
    sprintf(buffer, "%d", book.YearBook);
    DrawTextEx(_Font[0], buffer, (Vector2){rightX + 220, currentY}, fontSizeInfo, 1, valueCol);
    currentY += spacingY;
    DrawTextEx(_Font[0], "+ Giá (VND/ngày):", (Vector2){rightX, currentY}, fontSizeInfo, 1, labelCol);
    FormatPriceToVND(book.PriceBook, buffer);
    DrawTextEx(_Font[0], buffer, (Vector2){rightX + 220, currentY}, fontSizeInfo, 1, BW_SKY);
    currentY += spacingY;
    DrawTextEx(_Font[0], "+ Tổng lượng kho:", (Vector2){rightX, currentY}, fontSizeInfo, 1, labelCol);
    sprintf(buffer, "%d quyển", book.TotalStock);
    DrawTextEx(_Font[0], buffer, (Vector2){rightX + 220, currentY}, fontSizeInfo, 1,
               book.TotalStock > 0 ? (Color){80, 220, 130, 255} : (Color){255, 100, 100, 255});

    if (safeClip.width > 0 && safeClip.height > 0)
        EndScissorMode();

    // Description
    float introY = startY + targetH + 20;
    float listY = introY + 130;
    Rectangle descHitbox = {leftX, introY, detailArea.width - 100, 110};
    DrawTextAutoWrap(_Font[0], (book.Description[0] != '\0') ? book.Description : "Chưa có giới thiệu...",
                     descHitbox, 18.0f, 1.0f, Fade(BW_DIM, 0.85f));

    // Volume list header
    DrawTextEx(_Font[0], "DANH SÁCH TẬP", (Vector2){leftX, listY}, 24, 1, BW_SKY);
    DrawLineEx((Vector2){leftX, listY + 35},
               (Vector2){detailArea.x + detailArea.width - 50, listY + 35},
               1.5f, Fade(BW_ACCENT, 0.45f));

    float listStartY = listY + 50;
    float listHeight = detailArea.height - (listStartY - detailArea.y) - 20;
    Rectangle listArea = {leftX, listStartY, detailArea.width - 100, listHeight};

    float itemHeight = listArea.height * 0.15f;
    if (itemHeight < 45.0f)
        itemHeight = 45.0f;
    float itemSpacing = itemHeight * 0.2f;
    int displayCount = book.volumeCount;

    if (displayCount == 0)
    {
        DrawTextEx(_Font[0], "Truyện này chưa có tập nào.", (Vector2){listArea.x + 20, listArea.y + 20}, 20, 1, BW_DIM);
        return signalReturn;
    }

    float TotalContentHeight = displayCount * (itemHeight + itemSpacing);
    if (TotalContentHeight > listArea.height)
    {
        if (CheckCollisionPointRec(size.Mouse, listArea))
        {
            float wheelMove = GetMouseWheelMove();
            if (wheelMove != 0)
                *wheel += wheelMove * 40.0f;
        }
        if (*wheel > 0)
            *wheel = 0;
        if (*wheel < listArea.height - TotalContentHeight)
            *wheel = listArea.height - TotalContentHeight;
    }
    else
        *wheel = 0;

    BeginScissorMode((int)listArea.x, (int)listArea.y, (int)listArea.width, (int)listArea.height);

    for (int j = 0; j < displayCount; j++)
    {
        float yPos = listArea.y + *wheel + j * (itemHeight + itemSpacing);
        if (yPos + itemHeight < listArea.y || yPos > listArea.y + listArea.height)
            continue;

        Rectangle itemRec = {listArea.x, yPos, listArea.width, itemHeight};
        bool isHoverItem = CheckCollisionPointRec(size.Mouse, itemRec);

        // Card shadow
        DrawRectangleRounded((Rectangle){itemRec.x + 2, itemRec.y + 2, itemRec.width, itemRec.height},
                             0.35f, 10, Fade(BLACK, 0.3f));
        DrawRectangleRounded(itemRec, 0.35f, 10,
                             isHoverItem ? BW_CARD_HOVER : BW_CARD);
        // Left accent bar
        DrawRectangleRounded((Rectangle){itemRec.x, itemRec.y + itemHeight * 0.18f, 4.0f, itemHeight * 0.64f},
                             1.0f, 8, BW_ACCENT);

        // Number badge
        float numBoxSize = itemHeight * 0.6f;
        Rectangle numBox = {itemRec.x + 18, itemRec.y + (itemHeight - numBoxSize) / 2, numBoxSize * 1.3f, numBoxSize};
        DrawRectangleRounded(numBox, 0.4f, 10, Fade(BW_ACCENT, 0.22f));
        DrawRectangleRoundedLinesEx(numBox, 0.4f, 10, 1.5f, Fade(BW_SKY, 0.55f));

        char numStr[16];
        snprintf(numStr, sizeof(numStr), "%d", j + 1);
        float numFont = numBoxSize * 0.6f;
        float nw = MeasureTextEx(_Font[0], numStr, numFont, 1).x;
        DrawTextEx(_Font[0], numStr, (Vector2){numBox.x + (numBox.width - nw) / 2, numBox.y + (numBox.height - numFont) / 2}, numFont, 1, BW_DIM);

        // Volume name
        float volNameFont = itemHeight * 0.4f;
        DrawTextEx(_Font[3], book.volumes[j].VolumeName,
                   (Vector2){numBox.x + numBox.width + 15, itemRec.y + (itemHeight - volNameFont) / 2},
                   volNameFont, 1, isHoverItem ? BW_SKY : (Color){220, 235, 255, 255});

        // Stock info + delete button
        char stockText[64];
        Color stCol;
        if (book.volumes[j].Stock > 0)
        {
            sprintf(stockText, "SL Tồn: %d", book.volumes[j].Stock);
            stCol = BW_DIM;
        }
        else
        {
            strcpy(stockText, "Hết hàng");
            stCol = (Color){255, 100, 100, 255};
        }

        float rightEdge = itemRec.x + itemRec.width - 15;
        if (_role == ADMINISTRATOR)
        {
            float btnSz = itemHeight * 0.52f;
            Rectangle btnDel = {rightEdge - btnSz, itemRec.y + (itemHeight - btnSz) / 2.0f, btnSz, btnSz};
            bool hoverDel = CheckCollisionPointRec(size.Mouse, btnDel) && CheckCollisionPointRec(size.Mouse, listArea);
            DrawRectangleRounded(btnDel, 0.35f, 10, hoverDel ? MAROON : Fade(ERRORRED, 0.85f));
            float pad = btnSz * 0.22f;
            DrawLineEx((Vector2){btnDel.x + pad, btnDel.y + pad}, (Vector2){btnDel.x + btnSz - pad, btnDel.y + btnSz - pad}, 2.0f, WHITE);
            DrawLineEx((Vector2){btnDel.x + btnSz - pad, btnDel.y + pad}, (Vector2){btnDel.x + pad, btnDel.y + btnSz - pad}, 2.0f, WHITE);
            if (hoverDel && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Book *bk = &Books->theArray[bookIndex];
                bk->TotalStock -= bk->volumes[j].Stock;
                for (int k = j; k < bk->volumeCount - 1; k++)
                    bk->volumes[k] = bk->volumes[k + 1];
                bk->volumeCount--;
                SaveVolumes(*Books, LIST_BOOKS_FILE);
                book = Books->theArray[bookIndex];
                displayCount = book.volumeCount;
                signalReturn = 2;
            }
            rightEdge = btnDel.x - 8;
        }

        float stockFont = itemHeight * 0.36f;
        float sw = MeasureTextEx(_Font[0], stockText, stockFont, 1).x;
        DrawTextEx(_Font[0], stockText, (Vector2){rightEdge - sw - 10.0f, itemRec.y + (itemHeight - stockFont) / 2}, stockFont, 1, stCol);
        DrawLineEx((Vector2){itemRec.x + 10, itemRec.y + itemHeight + itemSpacing / 2},
                   (Vector2){itemRec.x + itemRec.width - 10, itemRec.y + itemHeight + itemSpacing / 2},
                   1.0f, Fade(BW_CARD_HOVER, 0.4f));
    }

    EndScissorMode();

    if (TotalContentHeight > listArea.height)
    {
        float sWidth = 8.0f;
        Rectangle sTrack = {listArea.x + listArea.width - sWidth, listArea.y, sWidth, listArea.height};
        DrawRectangleRounded(sTrack, 1.0f, 10, Fade(BW_CARD_HOVER, 0.3f));
        float sHandleH = (listArea.height / TotalContentHeight) * sTrack.height;
        if (sHandleH < 30.0f)
            sHandleH = 30.0f;
        float sRatio = -(*wheel) / (TotalContentHeight - listArea.height);
        Rectangle sHandle = {sTrack.x, sTrack.y + sRatio * (sTrack.height - sHandleH), sWidth, sHandleH};

        static bool isDraggingDetailVol = false;
        static float dragOffsetDetailVolY = 0.0f;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(size.Mouse, sHandle))
        {
            isDraggingDetailVol = true;
            dragOffsetDetailVolY = size.Mouse.y - sHandle.y;
        }
        if (isDraggingDetailVol)
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                float newY = size.Mouse.y - dragOffsetDetailVolY;
                if (newY < sTrack.y)
                    newY = sTrack.y;
                if (newY > sTrack.y + sTrack.height - sHandleH)
                    newY = sTrack.y + sTrack.height - sHandleH;
                float newRatio = (newY - sTrack.y) / (sTrack.height - sHandleH);
                *wheel = -(newRatio * (TotalContentHeight - listArea.height));
                sHandle.y = newY;
            }
            else
                isDraggingDetailVol = false;
        }
        bool isHoveredScroll = CheckCollisionPointRec(size.Mouse, sHandle) || isDraggingDetailVol;
        DRAW_SCROLLBAR(sHandle, isHoveredScroll);
    }

    return signalReturn;
}

// =========================================================================
// ShowAddBook_Panel
// =========================================================================
int ShowAddBook_Panel(Size size, ManageBooksUI UI, Font *_Font, InputBox *inputs, bool isEditMode, Book *editBook, float *volWheel)
{
    int result = 0;
    Rectangle panelArea = {UI.Panel.x, UI.Panel.y, UI.Panel.width, UI.Panel.height};

    BeginScissorMode((int)panelArea.x, (int)panelArea.y, (int)panelArea.width, (int)panelArea.height);
    // Light blue-white background
    DrawRectangleRec(panelArea, BW_PANEL_BG);
    // Subtle top accent stripe
    DrawRectangleGradientV((int)panelArea.x, (int)panelArea.y, (int)panelArea.width, 6,
                           BW_ACCENT, BLANK);

    static bool isTouched[8] = {false};
    static bool forceShowAllErrors = false;
    static bool showVolError = false;
    static bool showVolDuplicateError = false;
    static bool showVolPopupError = false;
    static bool showAddVolPopup = false;
    static InputBox volNameIn = {0};
    static InputBox volQtyIn = {0};

    bool clickedAnywhere = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !showAddVolPopup;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int currentYear = tm.tm_year + 1900;

    bool isBasicInfoFilled = true;
    bool fieldError[8] = {false};
    bool hasTextArr[8] = {false};

    for (int i = 1; i < 8; i++)
    {
        for (int j = 0; j < inputs[i].length; j++)
            if (inputs[i].text[j] != ' ' && inputs[i].text[j] != '\n')
            {
                hasTextArr[i] = true;
                break;
            }
        if (!hasTextArr[i])
        {
            fieldError[i] = true;
            isBasicInfoFilled = false;
        }
        else
        {
            if (i == 5)
            {
                bool isNum = true;
                for (int k = 0; k < inputs[i].length; k++)
                    if (!isdigit(inputs[i].text[k]))
                    {
                        isNum = false;
                        break;
                    }
                if (!isNum || atoi(inputs[5].text) > currentYear)
                {
                    fieldError[5] = true;
                    isBasicInfoFilled = false;
                }
            }
            if (i == 6)
            {
                bool isNum = true;
                for (int k = 0; k < inputs[i].length; k++)
                    if (!isdigit(inputs[i].text[k]))
                    {
                        isNum = false;
                        break;
                    }
                if (!isNum || atoi(inputs[6].text) < 500)
                {
                    fieldError[6] = true;
                    isBasicInfoFilled = false;
                }
            }
        }
        if (inputs[i].isFocused)
            isTouched[i] = true;
    }
    if (isBasicInfoFilled)
        showVolError = false;

    float titleFontSize = panelArea.height * 0.045f;
    if (titleFontSize < 25.0f)
        titleFontSize = 25.0f;
    if (titleFontSize > 45.0f)
        titleFontSize = 45.0f;

    float widthRatio = 0.95f - 0.3f * (panelArea.width / 2500.0f);
    if (widthRatio < 0.5f)
        widthRatio = 0.5f;
    if (widthRatio > 0.9f)
        widthRatio = 0.9f;
    float contentWidth = panelArea.width * widthRatio;
    float offsetX = panelArea.x + (panelArea.width - contentWidth) / 2.0f;
    float colGap = contentWidth * 0.06f;
    float colWidth = (contentWidth - colGap) / 2.0f;

    float boxHeight = panelArea.height * 0.055f;
    if (boxHeight < 35.0f)
        boxHeight = 35.0f;
    if (boxHeight > 55.0f)
        boxHeight = 55.0f;
    float labelFontSize = boxHeight * 0.45f;
    if (labelFontSize < 18.0f)
        labelFontSize = 18.0f;
    float inputTextSize = labelFontSize;

    float rowSpacing = panelArea.height * 0.11f;
    float descBoxHeight = boxHeight * 2.0f;
    if (descBoxHeight < boxHeight * 1.5f)
        descBoxHeight = boxHeight * 1.5f;

    float itemHeight = panelArea.height * 0.055f;
    if (itemHeight < 40.0f)
        itemHeight = 40.0f;
    if (itemHeight > 60.0f)
        itemHeight = 60.0f;
    float itemSpacing = itemHeight * 0.2f;
    int maxVisibleVols = 7;
    float fixedListHeight = maxVisibleVols * (itemHeight + itemSpacing);
    float volContentHeight = editBook->volumeCount * (itemHeight + itemSpacing);

    float btnHeight_ = panelArea.height * 0.07f;
    if (btnHeight_ < 40.0f)
        btnHeight_ = 40.0f;
    if (btnHeight_ > 60.0f)
        btnHeight_ = 60.0f;
    float btnWidth = panelArea.width * 0.25f;
    if (btnWidth < 200.0f)
        btnWidth = 200.0f;
    if (btnWidth > 350.0f)
        btnWidth = 350.0f;

    float abs_startY = panelArea.height * 0.13f;
    float abs_descY = abs_startY + 3 * rowSpacing + labelFontSize + 6.0f + boxHeight + panelArea.height * 0.02f;
    float abs_volLabelY = abs_descY + labelFontSize + 8.0f + descBoxHeight + 15.0f;
    float abs_listStartY = abs_volLabelY + labelFontSize + 8.0f;
    float abs_btnSaveY = abs_listStartY + fixedListHeight + 20.0f;
    float totalContentHeight = abs_btnSaveY + btnHeight_ + 40.0f;

    float masterScrollY = *volWheel;
    static float localVolScroll = 0.0f;

    Rectangle listHitbox = {offsetX, panelArea.y + masterScrollY + abs_listStartY, contentWidth, fixedListHeight};
    bool isMouseOverList = CheckCollisionPointRec(size.Mouse, listHitbox) && (volContentHeight > fixedListHeight);

    if (totalContentHeight > panelArea.height)
    {
        if (!isMouseOverList && !showAddVolPopup && CheckCollisionPointRec(size.Mouse, panelArea))
            masterScrollY += GetMouseWheelMove() * 40.0f;
        float minMaster = panelArea.height - totalContentHeight;
        if (masterScrollY > 0)
            masterScrollY = 0;
        if (masterScrollY < minMaster)
            masterScrollY = minMaster;
        *volWheel = masterScrollY;
    }
    else
    {
        *volWheel = 0;
        masterScrollY = 0;
    }

    if (volContentHeight > fixedListHeight)
    {
        if (isMouseOverList && !showAddVolPopup)
            localVolScroll += GetMouseWheelMove() * 40.0f;
        float minLocal = fixedListHeight - volContentHeight;
        if (localVolScroll > 0)
            localVolScroll = 0;
        if (localVolScroll < minLocal)
            localVolScroll = minLocal;
    }
    else
        localVolScroll = 0;

    // Form title
    if (panelArea.y + masterScrollY + panelArea.height * 0.03f > panelArea.y)
    {
        const char *titleText = isEditMode ? "CHỈNH SỬA THÔNG TIN TRUYỆN" : "THÊM TRUYỆN MỚI";
        DrawTextEx(_Font[0], titleText,
                   (Vector2){panelArea.x + panelArea.width * 0.05f, panelArea.y + masterScrollY + panelArea.height * 0.03f},
                   titleFontSize, 1, BW_FORM_TITLE);
        DrawLineEx(
            (Vector2){panelArea.x + panelArea.width * 0.05f, panelArea.y + masterScrollY + panelArea.height * 0.09f},
            (Vector2){panelArea.x + panelArea.width * 0.95f, panelArea.y + masterScrollY + panelArea.height * 0.09f},
            2.0f, BW_DIVIDER);
    }

    const char *labels[] = {"Mã Truyện (Tự động):", "Tên Truyện:", "Tác Giả:", "Thể Loại:", "Nhà Xuất Bản:", "Năm Sản Xuất:", "Giá thuê (VND/ngày):"};

    for (int i = 0; i < 8; i++)
    {
        float xPos = offsetX + (i % 2) * (colWidth + colGap);
        float yPos = panelArea.y + masterScrollY + abs_startY + (i / 2) * rowSpacing;
        float currentBoxH = boxHeight;

        if (i == 7)
        {
            xPos = offsetX;
            yPos = panelArea.y + masterScrollY + abs_descY;
            currentBoxH = descBoxHeight;
        }

        float totalItemH = labelFontSize + 6.0f + currentBoxH;
        inputs[i].box = (Rectangle){xPos, yPos + labelFontSize + 6.0f, (i == 7) ? contentWidth : colWidth, currentBoxH};

        if (clickedAnywhere)
        {
            if (i == 0)
                inputs[i].isFocused = false;
            else
                inputs[i].isFocused = CheckCollisionPointRec(size.Mouse, inputs[i].box);
        }

        if (yPos + totalItemH < panelArea.y || yPos > panelArea.y + panelArea.height)
            continue;

        // Label
        DrawTextEx(_Font[0], (i == 7) ? "Mô Tả Truyện:" : labels[i],
                   (Vector2){xPos, yPos},
                   (i == 7) ? labelFontSize + 2.0f : labelFontSize, 1,
                   (i == 7) ? BW_LABEL : BW_LABEL);

        bool displayError = i > 0 && fieldError[i] && !inputs[i].isFocused && (isTouched[i] || forceShowAllErrors);
        if (displayError)
        {
            const char *errMsg = "(Không hợp lệ!)";
            float errBoxW = (i == 7) ? contentWidth : colWidth;
            float errW = MeasureTextEx(_Font[0], errMsg, labelFontSize * 0.8f, 1).x;
            DrawTextEx(_Font[0], errMsg,
                       (Vector2){xPos + errBoxW - errW, yPos + (labelFontSize - labelFontSize * 0.8f)},
                       labelFontSize * 0.8f, 1, RED);
        }

        // Input box
        float roundness = (i == 7) ? 0.08f : 0.3f;
        Color boxBgColor =
            (i == 0) ? Fade(BW_DIVIDER, 0.6f)
                     : ((inputs[i].isFocused || hasTextArr[i]) ? WHITE : BW_INPUT_BG);
        Color boxOutlineColor =
            (i == 0) ? BW_DIVIDER
                     : (displayError ? RED : (inputs[i].isFocused ? BW_ACCENT : BW_DIVIDER));
        float outlineThick = inputs[i].isFocused ? 2.5f : (displayError ? 2.5f : 1.5f);

        // Input shadow
        DrawRectangleRounded((Rectangle){inputs[i].box.x + 2, inputs[i].box.y + 2, inputs[i].box.width, inputs[i].box.height},
                             roundness, 10, Fade(BW_LABEL, 0.12f));
        DrawRectangleRounded(inputs[i].box, roundness, 10, boxBgColor);
        DrawRectangleRoundedLinesEx(inputs[i].box, roundness, 10, outlineThick, boxOutlineColor);

        if (inputs[i].isFocused && i != 0 && !showAddVolPopup)
        {
            UpdateInputBox(&inputs[i]);
            if (IsKeyPressed(KEY_TAB))
            {
                inputs[i].isFocused = false;
                if (i < 7)
                    inputs[i + 1].isFocused = true;
                else
                    inputs[1].isFocused = true;
            }
        }

        Rectangle textArea = (i < 7)
                                 ? (Rectangle){inputs[i].box.x + 10, inputs[i].box.y, inputs[i].box.width - 20, inputs[i].box.height}
                                 : (Rectangle){inputs[i].box.x + 15, inputs[i].box.y + 15, inputs[i].box.width - 30, inputs[i].box.height - 30};
        Rectangle clipArea = GetCollisionRec(textArea, panelArea);

        if (clipArea.width > 0 && clipArea.height > 0)
        {
            BeginScissorMode((int)clipArea.x, (int)clipArea.y, (int)clipArea.width, (int)clipArea.height);
            if (i < 7)
            {
                float textW = MeasureTextEx(_Font[3], inputs[i].text, inputTextSize, 1).x;
                float textX = textArea.x + 2;
                if (textW > textArea.width - 4)
                    textX = textArea.x + 2 - (textW - (textArea.width - 4));
                float textY = textArea.y + (textArea.height - inputTextSize) / 2.0f;
                DrawTextEx(_Font[3], inputs[i].text, (Vector2){textX, textY}, inputTextSize, 1,
                           (i == 0) ? BW_ACCENT : BW_FORM_TITLE);
                if (inputs[i].isFocused && !showAddVolPopup && ((int)(GetTime() * 2) % 2 == 0))
                    DrawRectangle((int)(textX + textW + 2), (int)textY, 2, (int)(inputTextSize + 2), BW_ACCENT);
            }
            else
            {
                char tempText[8192];
                strcpy(tempText, inputs[i].text);
                if (inputs[i].isFocused && !showAddVolPopup && ((int)(GetTime() * 2) % 2 == 0))
                    strcat(tempText, "|");
                DrawTextAutoWrap(_Font[3], tempText, textArea, inputTextSize, 1.0f, BW_FORM_TITLE);
            }
            EndScissorMode();
            BeginScissorMode((int)panelArea.x, (int)panelArea.y, (int)panelArea.width, (int)panelArea.height);
        }
    }

    // ── "Tập truyện" section header ──────────────────────────────────────
    float volLabelY = panelArea.y + masterScrollY + abs_volLabelY;
    if (volLabelY + labelFontSize + 10.0f >= panelArea.y && volLabelY <= panelArea.y + panelArea.height)
    {
        DrawTextEx(_Font[0], "Tập truyện:", (Vector2){offsetX, volLabelY}, labelFontSize + 2.0f, 1, BW_LABEL);

        Rectangle btnAddVol = {
            offsetX + MeasureTextEx(_Font[0], "Tập truyện:", labelFontSize + 2.0f, 1).x + 30,
            volLabelY, 130, labelFontSize + 10.0f};
        bool hoverAddVol = CheckCollisionPointRec(size.Mouse, btnAddVol) && !showAddVolPopup;
        Color btnAddVolColor = isBasicInfoFilled
                                   ? (hoverAddVol ? BW_SKY : BW_ACCENT)
                                   : Fade(BW_DIM, 0.5f);

        DrawRectangleRounded((Rectangle){btnAddVol.x + 2, btnAddVol.y + 2, btnAddVol.width, btnAddVol.height},
                             0.35f, 10, Fade(BLACK, 0.2f));
        DrawRectangleRounded(btnAddVol, 0.35f, 10, btnAddVolColor);
        float addVolFont = labelFontSize * 0.9f;
        DrawTextEx(_Font[3], "+ Thêm tập",
                   (Vector2){btnAddVol.x + (130 - MeasureTextEx(_Font[3], "+ Thêm tập", addVolFont, 1).x) / 2,
                             btnAddVol.y + (btnAddVol.height - addVolFont) / 2},
                   addVolFont, 1, isBasicInfoFilled ? WHITE : (Color){100, 130, 170, 255});

        if (hoverAddVol && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (isBasicInfoFilled)
            {
                showAddVolPopup = true;
                showVolDuplicateError = false;
                showVolPopupError = false;
                memset(volNameIn.text, 0, sizeof(volNameIn.text));
                volNameIn.length = 0;
                memset(volQtyIn.text, 0, sizeof(volQtyIn.text));
                volQtyIn.length = 0;
                volNameIn.isFocused = true;
                volQtyIn.isFocused = false;
            }
            else
                showVolError = true;
        }
    }

    // ── Volume list ──────────────────────────────────────────────────────
    float listStartY2 = panelArea.y + masterScrollY + abs_listStartY;
    Rectangle listArea2 = {offsetX, listStartY2, contentWidth, fixedListHeight};
    static int activeVolIndex = -1;
    static char volInputText[16] = "";

    if (listArea2.y + listArea2.height >= panelArea.y && listArea2.y <= panelArea.y + panelArea.height)
    {
        // List container
        DrawRectangleRec(listArea2, WHITE);
        DrawRectangleLinesEx(listArea2, 1.5f, BW_DIVIDER);

        Rectangle clipListArea = GetCollisionRec(listArea2, panelArea);
        if (clipListArea.width > 0 && clipListArea.height > 0)
        {
            BeginScissorMode((int)clipListArea.x, (int)clipListArea.y, (int)clipListArea.width, (int)clipListArea.height);

            if (editBook->volumeCount > 0)
            {
                bool clickedOnAnyStockBox = false;
                for (int j = 0; j < editBook->volumeCount; j++)
                {
                    float yPos = listArea2.y + localVolScroll + j * (itemHeight + itemSpacing);
                    if (yPos + itemHeight < clipListArea.y || yPos > clipListArea.y + clipListArea.height)
                        continue;

                    Rectangle itemRec = {listArea2.x + 10, yPos + itemSpacing / 2, listArea2.width - 20, itemHeight};
                    DrawRectangleRounded(itemRec, 0.3f, 10, (Color){240, 246, 255, 255});
                    DrawRectangleRoundedLinesEx(itemRec, 0.3f, 10, 1.0f, BW_DIVIDER);
                    // Left accent
                    DrawRectangleRounded((Rectangle){itemRec.x, itemRec.y + itemHeight * 0.2f, 3.5f, itemHeight * 0.6f},
                                         1.0f, 6, BW_ACCENT);

                    float volNameFont = itemHeight * 0.45f;
                    DrawTextEx(_Font[3], editBook->volumes[j].VolumeName,
                               (Vector2){itemRec.x + 18, itemRec.y + (itemHeight - volNameFont) / 2},
                               volNameFont, 1, BW_FORM_TITLE);

                    float btnSize = itemHeight * 0.65f;
                    float rightEdge = itemRec.x + itemRec.width - 15.0f;

                    Rectangle btnPlus = {rightEdge - btnSize, itemRec.y + (itemHeight - btnSize) / 2, btnSize, btnSize};
                    bool hoverPlus = CheckCollisionPointRec(size.Mouse, btnPlus) && CheckCollisionPointRec(size.Mouse, clipListArea) && !showAddVolPopup;
                    DrawRectangleRounded(btnPlus, 0.35f, 5, hoverPlus ? (Color){60, 200, 100, 255} : BW_ACCENT);
                    float plusFont = btnSize * 0.8f;
                    DrawTextEx(_Font[0], "+",
                               (Vector2){btnPlus.x + (btnSize - MeasureTextEx(_Font[0], "+", plusFont, 1).x) / 2,
                                         btnPlus.y + (btnSize - plusFont) / 2},
                               plusFont, 1, WHITE);

                    if (hoverPlus && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        editBook->volumes[j].ImportStock++;
                        editBook->volumes[j].Stock++;
                        if (activeVolIndex == j)
                            sprintf(volInputText, "%d", editBook->volumes[j].ImportStock);
                    }

                    float stockBoxW = btnSize * 3.5f;
                    Rectangle stockBox = {btnPlus.x - stockBoxW - 10.0f, itemRec.y + (itemHeight - btnSize) / 2, stockBoxW, btnSize};
                    bool hoverStockBox = CheckCollisionPointRec(size.Mouse, stockBox) && CheckCollisionPointRec(size.Mouse, clipListArea) && !showAddVolPopup;

                    if (clickedAnywhere && hoverStockBox)
                    {
                        clickedOnAnyStockBox = true;
                        if (activeVolIndex != j)
                        {
                            activeVolIndex = j;
                            sprintf(volInputText, "%d", editBook->volumes[j].ImportStock);
                        }
                    }

                    if (activeVolIndex == j && !showAddVolPopup)
                    {
                        int key = GetCharPressed();
                        while (key > 0)
                        {
                            if (key >= '0' && key <= '9' && strlen(volInputText) < 6)
                            {
                                int len = strlen(volInputText);
                                volInputText[len] = (char)key;
                                volInputText[len + 1] = '\0';
                            }
                            key = GetCharPressed();
                        }
                        if (IsKeyPressed(KEY_BACKSPACE))
                        {
                            int len = strlen(volInputText);
                            if (len > 0)
                                volInputText[len - 1] = '\0';
                        }
                        if (IsKeyPressed(KEY_ENTER))
                            activeVolIndex = -1;
                        int newImp = atoi(volInputText);
                        int diff = newImp - editBook->volumes[j].ImportStock;
                        editBook->volumes[j].ImportStock = newImp;
                        editBook->volumes[j].Stock += diff;
                        DrawRectangleRounded(stockBox, 0.3f, 5, WHITE);
                        DrawRectangleRoundedLinesEx(stockBox, 0.3f, 5, 2.0f, BW_ACCENT);
                        float textFont = btnSize * 0.7f;
                        float textW = MeasureTextEx(_Font[0], volInputText, textFont, 1).x;
                        DrawTextEx(_Font[0], volInputText, (Vector2){stockBox.x + (stockBoxW - textW) / 2, stockBox.y + (btnSize - textFont) / 2}, textFont, 1, BW_FORM_TITLE);
                        if ((int)(GetTime() * 2) % 2 == 0)
                            DrawRectangle((int)(stockBox.x + (stockBoxW - textW) / 2 + textW + 2), (int)(stockBox.y + btnSize * 0.1f), 2, (int)(btnSize * 0.8f), BW_ACCENT);
                    }
                    else
                    {
                        DrawRectangleRounded(stockBox, 0.3f, 5, (Color){240, 246, 255, 255});
                        DrawRectangleRoundedLinesEx(stockBox, 0.3f, 5, 1.0f, BW_DIVIDER);
                        char stockStr[32];
                        sprintf(stockStr, "Nhập: %d", editBook->volumes[j].ImportStock);
                        float textFont = btnSize * 0.6f;
                        float textW = MeasureTextEx(_Font[0], stockStr, textFont, 1).x;
                        DrawTextEx(_Font[0], stockStr, (Vector2){stockBox.x + (stockBoxW - textW) / 2, stockBox.y + (btnSize - textFont) / 2}, textFont, 1,
                                   editBook->volumes[j].ImportStock > 0 ? (Color){40, 170, 90, 255} : (Color){220, 80, 80, 255});
                    }

                    Rectangle btnMinus = {stockBox.x - btnSize - 10.0f, itemRec.y + (itemHeight - btnSize) / 2, btnSize, btnSize};
                    bool hoverMinus = CheckCollisionPointRec(size.Mouse, btnMinus) && CheckCollisionPointRec(size.Mouse, clipListArea) && !showAddVolPopup;
                    DrawRectangleRounded(btnMinus, 0.35f, 5, hoverMinus ? MAROON : Fade(ERRORRED, 0.85f));
                    float minusFont = btnSize * 0.8f;
                    DrawTextEx(_Font[0], "-", (Vector2){btnMinus.x + (btnSize - MeasureTextEx(_Font[0], "-", minusFont, 1).x) / 2, btnMinus.y + (btnSize - minusFont) / 2}, minusFont, 1, WHITE);
                    if (hoverMinus && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && editBook->volumes[j].ImportStock > 0)
                    {
                        editBook->volumes[j].ImportStock--;
                        editBook->volumes[j].Stock--;
                        if (activeVolIndex == j)
                            sprintf(volInputText, "%d", editBook->volumes[j].ImportStock);
                    }
                }
                if (clickedAnywhere && !clickedOnAnyStockBox)
                    activeVolIndex = -1;
            }
            else
            {
                DrawTextEx(_Font[0], "Truyện này chưa có tập nào.", (Vector2){listArea2.x + 15, listArea2.y + 15}, 20, 1, BW_DIM);
                activeVolIndex = -1;
            }

            EndScissorMode();
            BeginScissorMode((int)panelArea.x, (int)panelArea.y, (int)panelArea.width, (int)panelArea.height);

            // Local vol scrollbar
            if (volContentHeight > fixedListHeight)
            {
                float vTrackWidth = 8.0f;
                Rectangle vTrack = {listArea2.x + listArea2.width - vTrackWidth - 2, listArea2.y, vTrackWidth, listArea2.height};
                Rectangle clipTrack = GetCollisionRec(vTrack, panelArea);
                if (clipTrack.height > 0)
                {
                    float vHandleH = (fixedListHeight / volContentHeight) * vTrack.height;
                    if (vHandleH < 20.0f)
                        vHandleH = 20.0f;
                    float vRatio = -localVolScroll / (volContentHeight - fixedListHeight);
                    Rectangle vHandle = {vTrack.x, vTrack.y + vRatio * (vTrack.height - vHandleH), vTrackWidth, vHandleH};
                    static bool isDraggingLocalVol = false;
                    static float dragOffsetLocalY = 0.0f;
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(size.Mouse, vHandle) && !showAddVolPopup)
                    {
                        isDraggingLocalVol = true;
                        dragOffsetLocalY = size.Mouse.y - vHandle.y;
                    }
                    if (isDraggingLocalVol)
                    {
                        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                        {
                            float newHandleY = size.Mouse.y - dragOffsetLocalY;
                            if (newHandleY < vTrack.y)
                                newHandleY = vTrack.y;
                            if (newHandleY > vTrack.y + vTrack.height - vHandleH)
                                newHandleY = vTrack.y + vTrack.height - vHandleH;
                            localVolScroll = -((newHandleY - vTrack.y) / (vTrack.height - vHandleH)) * (volContentHeight - fixedListHeight);
                        }
                        else
                            isDraggingLocalVol = false;
                    }
                    BeginScissorMode((int)clipTrack.x, (int)clipTrack.y, (int)clipTrack.width, (int)clipTrack.height);
                    bool isHoveredVScroll = CheckCollisionPointRec(size.Mouse, vHandle) || isDraggingLocalVol;
                    DRAW_SCROLLBAR(vHandle, isHoveredVScroll);
                    EndScissorMode();
                    BeginScissorMode((int)panelArea.x, (int)panelArea.y, (int)panelArea.width, (int)panelArea.height);
                }
            }
        }
    }

    // ── Save / Update button ──────────────────────────────────────────────
    Rectangle btnSave = {panelArea.x + (panelArea.width - btnWidth) / 2.0f, panelArea.y + masterScrollY + abs_btnSaveY, btnWidth, btnHeight_};
    if (btnSave.y + btnSave.height >= panelArea.y && btnSave.y <= panelArea.y + panelArea.height)
    {
        bool hoverSave = CheckCollisionPointRec(size.Mouse, btnSave) && CheckCollisionPointRec(size.Mouse, panelArea) && !showAddVolPopup;
        DrawRectangleRounded((Rectangle){btnSave.x + 3, btnSave.y + 3, btnSave.width, btnSave.height},
                             0.45f, 12, Fade(BLACK, 0.22f));
        DrawRectangleRounded(btnSave, 0.45f, 12, hoverSave ? (Color){55, 200, 100, 255} : SUCCESSGREEN);
        DrawRectangleRoundedLinesEx(btnSave, 0.45f, 12, 1.5f, Fade(WHITE, 0.3f));
        const char *btnLabel = isEditMode ? "CẬP NHẬT TRUYỆN" : "LƯU TRUYỆN";
        float wSave = MeasureTextEx(_Font[0], btnLabel, inputTextSize + 2.0f, 1).x;
        DrawTextEx(_Font[0], btnLabel,
                   (Vector2){btnSave.x + (btnSave.width - wSave) / 2, btnSave.y + (btnSave.height - (inputTextSize + 2.0f)) / 2},
                   inputTextSize + 2.0f, 1, WHITE);
        if (hoverSave && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (isBasicInfoFilled)
            {
                int writeIndex = 0;
                editBook->TotalStock = 0;
                for (int k = 0; k < editBook->volumeCount; k++)
                    if (editBook->volumes[k].ImportStock > 0)
                    {
                        editBook->volumes[writeIndex] = editBook->volumes[k];
                        editBook->TotalStock += editBook->volumes[writeIndex].Stock;
                        writeIndex++;
                    }
                editBook->volumeCount = writeIndex;
                result = 1;
                forceShowAllErrors = false;
                for (int z = 0; z < 8; z++)
                    isTouched[z] = false;
            }
            else
                forceShowAllErrors = true;
        }
        if (forceShowAllErrors || showVolError)
        {
            const char *errorMsg = "* Lỗi! Thông tin không hợp lệ";
            DrawTextEx(_Font[0], errorMsg,
                       (Vector2){panelArea.x + (panelArea.width - MeasureTextEx(_Font[0], errorMsg, 20.0f, 1).x) / 2.0f,
                                 btnSave.y + btnSave.height + 10.0f},
                       20.0f, 1, RED);
        }
    }

    // Master scrollbar
    if (totalContentHeight > panelArea.height)
    {
        float scrollTrackWidth = 10.0f;
        Rectangle scrollTrack = {panelArea.x + panelArea.width - scrollTrackWidth - 2, panelArea.y, scrollTrackWidth, panelArea.height};
        DrawRectangleRounded(scrollTrack, 1.0f, 10, Fade(BW_DIVIDER, 0.5f));
        float sHandleH = (panelArea.height / totalContentHeight) * scrollTrack.height;
        if (sHandleH < 30.0f)
            sHandleH = 30.0f;
        float sRatio = -masterScrollY / (totalContentHeight - panelArea.height);
        Rectangle sHandle = {scrollTrack.x, scrollTrack.y + sRatio * (scrollTrack.height - sHandleH), scrollTrackWidth, sHandleH};
        static bool isDraggingMaster = false;
        static float dragOffsetMasterY = 0.0f;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(size.Mouse, sHandle) && !showAddVolPopup)
        {
            isDraggingMaster = true;
            dragOffsetMasterY = size.Mouse.y - sHandle.y;
        }
        if (isDraggingMaster)
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                float newHandleY = size.Mouse.y - dragOffsetMasterY;
                if (newHandleY < scrollTrack.y)
                    newHandleY = scrollTrack.y;
                if (newHandleY > scrollTrack.y + scrollTrack.height - sHandleH)
                    newHandleY = scrollTrack.y + scrollTrack.height - sHandleH;
                masterScrollY = -((newHandleY - scrollTrack.y) / (scrollTrack.height - sHandleH)) * (totalContentHeight - panelArea.height);
                *volWheel = masterScrollY;
            }
            else
                isDraggingMaster = false;
        }
        bool isHoveredScroll = (CheckCollisionPointRec(size.Mouse, sHandle) || isDraggingMaster) && !showAddVolPopup;
        DRAW_SCROLLBAR(sHandle, isHoveredScroll);
    }
    EndScissorMode();

    // ── Add Volume Popup ──────────────────────────────────────────────────
    if (showAddVolPopup)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BW_TITLE_BG, 0.7f));
        float popW = 480, popH = 290;
        Rectangle popRec = {(GetScreenWidth() - popW) / 2.0f, (GetScreenHeight() - popH) / 2.0f, popW, popH};

        // Popup shadow
        DrawRectangleRounded((Rectangle){popRec.x + 4, popRec.y + 4, popRec.width, popRec.height},
                             0.12f, 12, Fade(BLACK, 0.35f));
        DrawRectangleRounded(popRec, 0.12f, 12, BW_PANEL_BG);
        DrawRectangleRoundedLinesEx(popRec, 0.12f, 12, 2.5f, BW_DIVIDER);
        // Top accent bar
        DrawRectangleRounded((Rectangle){popRec.x, popRec.y, popRec.width, 6}, 0.5f, 8, BW_ACCENT);

        DrawTextEx(_Font[0], "THÊM TẬP TRUYỆN MỚI",
                   (Vector2){popRec.x + (popW - MeasureTextEx(_Font[0], "THÊM TẬP TRUYỆN MỚI", 24, 1).x) / 2, popRec.y + 22},
                   24, 1, BW_FORM_TITLE);

        DrawTextEx(_Font[3], "Tên/Số tập (Nhập 0 nếu truyện lẻ):", (Vector2){popRec.x + 30, popRec.y + 78}, 18, 1, BW_LABEL);
        volNameIn.box = (Rectangle){popRec.x + 328, popRec.y + 68, 110, 42};
        DrawTextEx(_Font[3], "Số lượng nhập:", (Vector2){popRec.x + 30, popRec.y + 128}, 18, 1, BW_LABEL);
        volQtyIn.box = (Rectangle){popRec.x + 185, popRec.y + 118, 255, 42};

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            volNameIn.isFocused = CheckCollisionPointRec(size.Mouse, volNameIn.box);
            volQtyIn.isFocused = CheckCollisionPointRec(size.Mouse, volQtyIn.box);
        }

        bool isNameEmpty = (strlen(volNameIn.text) == 0);
        bool isQtyEmpty = (atoi(volQtyIn.text) <= 0);
        bool highlightNameRed = (showVolPopupError && isNameEmpty) || showVolDuplicateError;
        bool highlightQtyRed = (showVolPopupError && isQtyEmpty);

        Color nameLineCol = volNameIn.isFocused ? BW_ACCENT : (highlightNameRed ? RED : BW_DIVIDER);
        Color qtyLineCol = volQtyIn.isFocused ? BW_ACCENT : (highlightQtyRed ? RED : BW_DIVIDER);

        // Input boxes in popup
        DrawRectangleRounded(volNameIn.box, 0.3f, 8, volNameIn.isFocused ? WHITE : BW_INPUT_BG);
        DrawRectangleRoundedLinesEx(volNameIn.box, 0.3f, 8, 2.0f, nameLineCol);
        DrawRectangleRounded(volQtyIn.box, 0.3f, 8, volQtyIn.isFocused ? WHITE : BW_INPUT_BG);
        DrawRectangleRoundedLinesEx(volQtyIn.box, 0.3f, 8, 2.0f, qtyLineCol);

        if (volNameIn.isFocused)
        {
            UpdateInputBox(&volNameIn);
            if (IsKeyPressed(KEY_TAB))
            {
                volNameIn.isFocused = false;
                volQtyIn.isFocused = true;
            }
        }
        if (volQtyIn.isFocused)
        {
            int key = GetCharPressed();
            while (key > 0)
            {
                if (key >= '0' && key <= '9' && volQtyIn.length < 6)
                {
                    volQtyIn.text[volQtyIn.length++] = (char)key;
                    volQtyIn.text[volQtyIn.length] = '\0';
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && volQtyIn.length > 0)
                volQtyIn.text[--volQtyIn.length] = '\0';
            if (IsKeyPressed(KEY_TAB))
            {
                volQtyIn.isFocused = false;
                volNameIn.isFocused = true;
            }
        }

        BeginScissorMode((int)volNameIn.box.x, (int)volNameIn.box.y, (int)volNameIn.box.width, (int)volNameIn.box.height);
        DrawTextEx(_Font[3], volNameIn.text, (Vector2){volNameIn.box.x + 10, volNameIn.box.y + 10}, 18, 1, BW_FORM_TITLE);
        if (volNameIn.isFocused && (int)(GetTime() * 2) % 2 == 0)
            DrawRectangle((int)(volNameIn.box.x + 12 + MeasureTextEx(_Font[3], volNameIn.text, 18, 1).x), (int)(volNameIn.box.y + 5), 2, 30, BW_ACCENT);
        EndScissorMode();
        BeginScissorMode((int)volQtyIn.box.x, (int)volQtyIn.box.y, (int)volQtyIn.box.width, (int)volQtyIn.box.height);
        DrawTextEx(_Font[3], volQtyIn.text, (Vector2){volQtyIn.box.x + 10, volQtyIn.box.y + 10}, 18, 1, BW_FORM_TITLE);
        if (volQtyIn.isFocused && (int)(GetTime() * 2) % 2 == 0)
            DrawRectangle((int)(volQtyIn.box.x + 12 + MeasureTextEx(_Font[3], volQtyIn.text, 18, 1).x), (int)(volQtyIn.box.y + 5), 2, 30, BW_ACCENT);
        EndScissorMode();

        if (showVolDuplicateError)
            DrawTextEx(_Font[0], "* Tên tập truyện này đã tồn tại!", (Vector2){popRec.x + 115, popRec.y + 170}, 16, 1, RED);

        Rectangle btnConf = {popRec.x + 70, popRec.y + 210, 130, 44};
        Rectangle btnCanc = {popRec.x + 275, popRec.y + 210, 130, 44};
        bool hConf = CheckCollisionPointRec(size.Mouse, btnConf);
        bool hCanc = CheckCollisionPointRec(size.Mouse, btnCanc);

        DrawRectangleRounded((Rectangle){btnConf.x + 2, btnConf.y + 2, btnConf.width, btnConf.height}, 0.4f, 10, Fade(BLACK, 0.2f));
        DrawRectangleRounded(btnConf, 0.4f, 10, hConf ? (Color){55, 200, 100, 255} : (Color){40, 170, 80, 255});
        DrawTextEx(_Font[3], "Xác nhận",
                   (Vector2){btnConf.x + (130 - MeasureTextEx(_Font[3], "Xác nhận", 18, 1).x) / 2, btnConf.y + 12}, 18, 1, WHITE);

        DrawRectangleRounded((Rectangle){btnCanc.x + 2, btnCanc.y + 2, btnCanc.width, btnCanc.height}, 0.4f, 10, Fade(BLACK, 0.2f));
        DrawRectangleRounded(btnCanc, 0.4f, 10, hCanc ? MAROON : Fade(ERRORRED, 0.9f));
        DrawTextEx(_Font[3], "Hủy bỏ",
                   (Vector2){btnCanc.x + (130 - MeasureTextEx(_Font[3], "Hủy bỏ", 18, 1).x) / 2, btnCanc.y + 12}, 18, 1, WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (hCanc)
            {
                showAddVolPopup = false;
                showVolDuplicateError = false;
                showVolPopupError = false;
            }
            else if (hConf)
            {
                if (isNameEmpty || isQtyEmpty)
                    showVolPopupError = true;
                else
                {
                    char tempName[256];
                    if (strcmp(volNameIn.text, "0") == 0)
                        strcpy(tempName, "Truyện lẻ (0)");
                    else
                        strcpy(tempName, volNameIn.text);
                    bool isDuplicate = false;
                    for (int k = 0; k < editBook->volumeCount; k++)
                        if (strcmp(editBook->volumes[k].VolumeName, tempName) == 0)
                        {
                            isDuplicate = true;
                            break;
                        }
                    if (isDuplicate)
                    {
                        showVolDuplicateError = true;
                        showVolPopupError = false;
                    }
                    else
                    {
                        if (editBook->volumeCount < 200)
                        {
                            Volume *newVol = &editBook->volumes[editBook->volumeCount];
                            strcpy(newVol->VolumeName, tempName);
                            sprintf(newVol->VolumeCode, "C%03d", editBook->volumeCount + 1);
                            strcpy(newVol->BookCode, inputs[0].text);
                            newVol->ImportStock = atoi(volQtyIn.text);
                            newVol->Stock = newVol->ImportStock;
                            editBook->volumeCount++;
                            editBook->TotalStock += newVol->Stock;
                        }
                        showAddVolPopup = false;
                        showVolDuplicateError = false;
                        showVolPopupError = false;
                    }
                }
            }
        }
    }

    return result;
}

// =========================================================================
// ShowBooks_Panel
// =========================================================================
int ShowBooks_Panel(Size size, ManageBooksUI UI, float *wheel, Font *_Font, BookList *Books, int filterMode, const char *filterValue, Role _role, bool *showPagination, int *requestDeleteIndex, bool showOnlyInStock)
{
    int clickedIndex = -1;
    bool isMouseInPanel = CheckCollisionPointRec(size.Mouse, UI.Panel);

    float distanceBooks = UI.Panel.height * 0.05f;
    if (distanceBooks < 20.0f)
        distanceBooks = 20.0f;
    if (distanceBooks > 60.0f)
        distanceBooks = 60.0f;

    Rectangle hitbox = {UI.Panel.x + UI.Panel.width * 0.05f, *wheel + UI.Panel.y + UI.Panel.height * 0.03f,
                        UI.Panel.width * 0.9f, UI.Panel.height * 0.08f * (pow(0.3, size.Scale.x) + 0.6)};

    int startIndex = (Books->currentPage - 1) * Books->QuantityForOnePage;
    int matchCount = 0, displayCount = 0;
    int renderList[QUANTITYFORONEPAGE];

    for (int i = 0; i < Books->count && displayCount < Books->QuantityForOnePage; i++)
    {
        int flag = 0;
        if (filterMode == 1)
        {
            if (filterValue && strcmp(filterValue, Books->theArray[i].AuthorBook) == 0)
                flag = 1;
        }
        else if (filterMode == 2)
        {
            if (filterValue && strcmp(filterValue, Books->theArray[i].TypeBook) == 0)
                flag = 1;
        }
        else if (filterMode == 3)
        {
            if (filterValue && strcmp(filterValue, Books->theArray[i].PublisherBook) == 0)
                flag = 1;
        }
        else if (filterMode == 4)
        {
            if (ContextualBookSearch(Books->theArray[i], filterValue))
                flag = 1;
        }
        else
            flag = 1;
        if (flag && showOnlyInStock && Books->theArray[i].TotalStock <= 0)
            flag = 0;
        if (flag)
        {
            if (matchCount >= startIndex)
            {
                renderList[displayCount] = i;
                displayCount++;
            }
            matchCount++;
        }
    }

    float bottomPadding = UI.PaginationBox.height + 20.0f;
    float TotalContentHeight = (hitbox.height + distanceBooks) * displayCount + bottomPadding;

    if (TotalContentHeight > UI.Panel.height)
    {
        float wheelMove = GetMouseWheelMove();
        if (wheelMove != 0)
            *wheel += wheelMove * 40.0f;
        float minWheel = UI.Panel.height - TotalContentHeight;
        if (*wheel > 0)
            *wheel = 0;
        if (*wheel < minWheel)
            *wheel = minWheel;
        if (showPagination)
            *showPagination = (*wheel <= minWheel + 5.0f);
    }
    else
    {
        *wheel = 0;
        if (showPagination)
            *showPagination = true;
    }

    BeginScissorMode((int)UI.Panel.x, (int)UI.Panel.y, (int)UI.Panel.width, (int)UI.Panel.height);

    for (int idx = 0; idx < displayCount; idx++)
    {
        int i = renderList[idx];
        hitbox.y = *wheel + UI.Panel.y + UI.Panel.height * 0.03f + (hitbox.height + distanceBooks) * idx;

        bool isHoveredBook = CheckCollisionPointRec(size.Mouse, hitbox) && isMouseInPanel;
        float radius = FindRoundness(hitbox.width * 0.005f, hitbox.width, hitbox.height);

        // Card shadow
        DrawRectangleRounded((Rectangle){hitbox.x + 3, hitbox.y + 3, hitbox.width, hitbox.height},
                             radius, 50, Fade(BLACK, 0.28f));
        // Card background
        DrawRectangleRounded(hitbox, radius, 50, isHoveredBook ? BW_CARD_HOVER : BW_CARD);
        // Left accent strip
        DrawRectangleRounded((Rectangle){hitbox.x, hitbox.y + hitbox.height * 0.15f, 5.0f, hitbox.height * 0.7f},
                             1.0f, 8, isHoveredBook ? BW_SKY : BW_ACCENT);

        float padding = hitbox.width * 0.02f;
        float rightAnchor = hitbox.x + hitbox.width - padding;
        bool clickedTrash = false;

        // Delete button (admin)
        if (_role == ADMINISTRATOR)
        {
            float trashSize = hitbox.height * 0.6f;
            Rectangle trashBox = {rightAnchor - trashSize, hitbox.y + (hitbox.height - trashSize) / 2, trashSize, trashSize};
            if (CheckCollisionPointRec(size.Mouse, trashBox) && isMouseInPanel)
            {
                clickedTrash = true;
                DrawRectangleRounded(trashBox, 0.35f, 10, MAROON);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    *requestDeleteIndex = i;
            }
            else
                DrawRectangleRounded(trashBox, 0.35f, 10, Fade(ERRORRED, 0.85f));
            DrawLineEx((Vector2){trashBox.x + trashSize * 0.25f, trashBox.y + trashSize * 0.25f}, (Vector2){trashBox.x + trashSize * 0.75f, trashBox.y + trashSize * 0.75f}, 2.5f, WHITE);
            DrawLineEx((Vector2){trashBox.x + trashSize * 0.75f, trashBox.y + trashSize * 0.25f}, (Vector2){trashBox.x + trashSize * 0.25f, trashBox.y + trashSize * 0.75f}, 2.5f, WHITE);
            rightAnchor = trashBox.x - padding;
        }

        if (isHoveredBook && !clickedTrash && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            clickedIndex = i;

        // Price & stock badge
        int PriceFontSize = (int)(hitbox.height * 0.35f);
        char priceStr[32], stockStr[32];
        FormatPriceToVND(Books->theArray[i].PriceBook, priceStr);
        sprintf(stockStr, "SL: %d", Books->theArray[i].TotalStock);

        float priceW = MeasureTextEx(_Font[0], priceStr, PriceFontSize, 1).x;
        float stockFontS = PriceFontSize * 0.9f;
        float stockW = MeasureTextEx(_Font[0], stockStr, stockFontS, 1).x;
        float separatorW = MeasureTextEx(_Font[0], " | ", PriceFontSize, 1).x;
        float badgePad = 8.0f;
        float badgeW = stockW + badgePad * 2;
        float badgeH = stockFontS * 1.4f;
        float totalW = priceW + separatorW + badgeW;
        float startX_ = rightAnchor - totalW;
        float textY = hitbox.y + (hitbox.height - PriceFontSize) / 2;

        // Price in sky blue
        DrawTextEx(_Font[0], priceStr, (Vector2){startX_, textY}, PriceFontSize, 1, BW_SKY);
        DrawTextEx(_Font[0], " | ", (Vector2){startX_ + priceW, textY}, PriceFontSize, 1, Fade(WHITE, 0.4f));

        // Stock badge
        Rectangle badgeRec = {startX_ + priceW + separatorW, hitbox.y + (hitbox.height - badgeH) / 2, badgeW, badgeH};
        Color badgeBg = Books->theArray[i].TotalStock > 0 ? Fade((Color){60, 200, 100, 255}, 0.25f) : Fade((Color){255, 80, 80, 255}, 0.25f);
        Color badgeBorder = Books->theArray[i].TotalStock > 0 ? (Color){80, 220, 130, 255} : (Color){255, 100, 100, 255};
        DrawRectangleRounded(badgeRec, 0.45f, 10, badgeBg);
        DrawRectangleRoundedLinesEx(badgeRec, 0.45f, 10, 1.5f, badgeBorder);
        DrawTextEx(_Font[0], stockStr, (Vector2){badgeRec.x + badgePad, badgeRec.y + (badgeRec.height - stockFontS) / 2}, stockFontS, 1, badgeBorder);

        rightAnchor = startX_ - padding;

        // Code box (white pill on navy card)
        Rectangle code_hitbox = {hitbox.x, hitbox.y, hitbox.width * 0.12f, hitbox.height};
        radius = FindRoundness(hitbox.width * 0.005f, code_hitbox.width, code_hitbox.height);
        DrawRectangleRounded(code_hitbox, radius, 50, Fade(WHITE, 0.93f));
        int CodeFontSize = FindFontSize(code_hitbox.width * 0.8f, _Font[0], 1, Books->theArray[i].CodeBook);
        DrawTextEx(_Font[0], Books->theArray[i].CodeBook,
                   (Vector2){code_hitbox.x + (code_hitbox.width - MeasureTextEx(_Font[0], Books->theArray[i].CodeBook, CodeFontSize, 1).x) / 2,
                             code_hitbox.y + (code_hitbox.height - CodeFontSize) / 2},
                   CodeFontSize, 1, BW_TITLE_BG);

        // Name & author
        int NameFontSize = (int)(hitbox.height * 0.6f);
        Vector2 NamePos = {code_hitbox.x + code_hitbox.width + padding, hitbox.y + (hitbox.height - NameFontSize) / 2};
        float nameWidth = MeasureTextEx(_Font[0], Books->theArray[i].NameBook, NameFontSize, 1).x;
        int AuthorFontSize = (int)(NameFontSize * 0.65f);
        Vector2 AuthorPos = {NamePos.x + nameWidth + padding, hitbox.y + (hitbox.height - AuthorFontSize) / 2 + NameFontSize * 0.1f};
        float availableWidthText = rightAnchor - NamePos.x;
        Rectangle textRect_ = {NamePos.x, hitbox.y, availableWidthText, hitbox.height};
        Rectangle panelRect_ = {UI.Panel.x, UI.Panel.y, UI.Panel.width, UI.Panel.height};
        Rectangle clipRect_ = GetCollisionRec(textRect_, panelRect_);

        BeginScissorMode((int)clipRect_.x, (int)clipRect_.y, (int)clipRect_.width, (int)clipRect_.height);
        DrawTextEx(_Font[0], Books->theArray[i].NameBook, NamePos, NameFontSize, 1, isHoveredBook ? BW_SKY : WHITE);
        DrawTextEx(_Font[3], TextFormat("- %s", Books->theArray[i].AuthorBook), AuthorPos, AuthorFontSize, 1, Fade(BW_DIM, 0.75f));
        EndScissorMode();

        BeginScissorMode((int)UI.Panel.x, (int)UI.Panel.y, (int)UI.Panel.width, (int)UI.Panel.height);
    }

    // Scrollbar
    if (TotalContentHeight > UI.Panel.height)
    {
        float scrollTrackWidth = 12.0f;
        Rectangle scrollTrack = {UI.Panel.x + UI.Panel.width - scrollTrackWidth - 5, UI.Panel.y + 5, scrollTrackWidth, UI.Panel.height - 10};
        DrawRectangleRounded(scrollTrack, 1.0f, 10, Fade(BW_CARD_HOVER, 0.35f));
        float handleHeight = (UI.Panel.height / TotalContentHeight) * scrollTrack.height;
        if (handleHeight < 30.0f)
            handleHeight = 30.0f;
        float scrollableWheel = TotalContentHeight - UI.Panel.height;
        float scrollRatio = -(*wheel) / scrollableWheel;
        float handleY = scrollTrack.y + scrollRatio * (scrollTrack.height - handleHeight);
        Rectangle scrollHandle = {scrollTrack.x, handleY, scrollTrackWidth, handleHeight};
        static bool isDraggingBooks = false;
        static float dragOffsetBooksY = 0.0f;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(size.Mouse, scrollHandle))
        {
            isDraggingBooks = true;
            dragOffsetBooksY = size.Mouse.y - scrollHandle.y;
        }
        if (isDraggingBooks)
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                float newHandleY = size.Mouse.y - dragOffsetBooksY;
                if (newHandleY < scrollTrack.y)
                    newHandleY = scrollTrack.y;
                if (newHandleY > scrollTrack.y + scrollTrack.height - handleHeight)
                    newHandleY = scrollTrack.y + scrollTrack.height - handleHeight;
                *wheel = -((newHandleY - scrollTrack.y) / (scrollTrack.height - handleHeight)) * scrollableWheel;
                scrollHandle.y = newHandleY;
            }
            else
                isDraggingBooks = false;
        }
        bool isHoveredScroll = CheckCollisionPointRec(size.Mouse, scrollHandle) || isDraggingBooks;
        DRAW_SCROLLBAR(scrollHandle, isHoveredScroll);
    }

    EndScissorMode();
    return clickedIndex;
}

// =========================================================================
// ShowAuthor_Panel
// =========================================================================
void ShowAuthor_Panel(Size size, ManageBooksUI UI, float *wheel, Font *_Font, AuthorList *Authors, char *selectedAuthor, BookList *Books)
{
    bool isMouseInPanel = CheckCollisionPointRec(size.Mouse, UI.Panel);
    float itemHeight = UI.Panel.height * 0.08f;
    if (itemHeight < 50.0f)
        itemHeight = 50.0f;
    float distance = itemHeight * 0.25f;
    int displayCount = Authors->count;
    float TotalContentHeight = (itemHeight + distance) * displayCount;

    if (TotalContentHeight > UI.Panel.height)
    {
        float wheelMove = GetMouseWheelMove();
        if (wheelMove != 0)
            *wheel += wheelMove * 40.0f;
        if (*wheel > 0)
            *wheel = 0;
        if (*wheel < UI.Panel.height - TotalContentHeight - distance)
            *wheel = UI.Panel.height - TotalContentHeight - distance;
    }
    else
        *wheel = 0;

    BeginScissorMode((int)UI.Panel.x, (int)UI.Panel.y, (int)UI.Panel.width, (int)UI.Panel.height);
    for (int i = 0; i < displayCount; i++)
    {
        Rectangle hitbox = {UI.Panel.x + UI.Panel.width * 0.08f, *wheel + UI.Panel.y + 20.0f + (itemHeight + distance) * i, UI.Panel.width * 0.84f, itemHeight};
        bool isHovered = CheckCollisionPointRec(size.Mouse, hitbox) && isMouseInPanel;

        // Shadow + card
        DrawRectangleRounded((Rectangle){hitbox.x + 3, hitbox.y + 3, hitbox.width, hitbox.height}, 0.4f, 20, Fade(BLACK, 0.25f));
        DrawRectangleRounded(hitbox, 0.4f, 20, isHovered ? BW_ACCENT : BW_CARD);
        // Left accent
        DrawRectangleRounded((Rectangle){hitbox.x, hitbox.y + itemHeight * 0.18f, 5.0f, itemHeight * 0.64f}, 1.0f, 8, isHovered ? WHITE : BW_SKY);

        int nameFont = (int)(itemHeight * 0.5f);
        // Code badge
        DrawTextEx(_Font[0], Authors->Author[i].code, (Vector2){hitbox.x + 22, hitbox.y + (itemHeight - nameFont) / 2}, nameFont, 1, isHovered ? Fade(WHITE, 0.75f) : BW_DIM);
        DrawTextEx(_Font[0], Authors->Author[i].name, (Vector2){hitbox.x + 150, hitbox.y + (itemHeight - nameFont) / 2}, nameFont, 1, WHITE);

        int bookCount = CountStrInBooks(Books, Authors->Author[i].name, AUTHOR, false);
        char countStr[32];
        sprintf(countStr, "%d Truyện", bookCount);
        int countFont = (int)(itemHeight * 0.4f);
        float countW = MeasureTextEx(_Font[0], countStr, countFont, 1).x;
        float badgeW_ = countW + 20.0f, badgeH_ = countFont * 1.5f;
        Rectangle countBadge = {hitbox.x + hitbox.width - badgeW_ - 15, hitbox.y + (itemHeight - badgeH_) / 2, badgeW_, badgeH_};
        DrawRectangleRounded(countBadge, 0.45f, 10, Fade(isHovered ? WHITE : BW_SKY, 0.18f));
        DrawRectangleRoundedLinesEx(countBadge, 0.45f, 10, 1.0f, Fade(isHovered ? WHITE : BW_SKY, 0.5f));
        DrawTextEx(_Font[0], countStr, (Vector2){countBadge.x + 10, countBadge.y + (badgeH_ - countFont) / 2}, countFont, 1, isHovered ? WHITE : BW_DIM);

        if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            strcpy(selectedAuthor, Authors->Author[i].name);
            *wheel = 0;
            Books->currentPage = 1;
        }
    }

    if (TotalContentHeight > UI.Panel.height)
    {
        float scrollTrackWidth = 12.0f;
        Rectangle scrollTrack = {UI.Panel.x + UI.Panel.width - scrollTrackWidth - 5, UI.Panel.y + 5, scrollTrackWidth, UI.Panel.height - 10};
        DrawRectangleRounded(scrollTrack, 1.0f, 10, Fade(BW_CARD_HOVER, 0.35f));
        float handleHeight = (UI.Panel.height / TotalContentHeight) * scrollTrack.height;
        if (handleHeight < 30.0f)
            handleHeight = 30.0f;
        float scrollableWheel = TotalContentHeight + distance - UI.Panel.height;
        float scrollRatio = -(*wheel) / scrollableWheel;
        float handleY = scrollTrack.y + scrollRatio * (scrollTrack.height - handleHeight);
        Rectangle scrollHandle = {scrollTrack.x, handleY, scrollTrackWidth, handleHeight};
        static bool isDraggingAuthors = false;
        static float dragOffsetAuthorsY = 0.0f;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(size.Mouse, scrollHandle))
        {
            isDraggingAuthors = true;
            dragOffsetAuthorsY = size.Mouse.y - scrollHandle.y;
        }
        if (isDraggingAuthors)
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                float newHandleY = size.Mouse.y - dragOffsetAuthorsY;
                if (newHandleY < scrollTrack.y)
                    newHandleY = scrollTrack.y;
                if (newHandleY > scrollTrack.y + scrollTrack.height - handleHeight)
                    newHandleY = scrollTrack.y + scrollTrack.height - handleHeight;
                *wheel = -((newHandleY - scrollTrack.y) / (scrollTrack.height - handleHeight)) * scrollableWheel;
                scrollHandle.y = newHandleY;
            }
            else
                isDraggingAuthors = false;
        }
        bool isHoveredScroll = CheckCollisionPointRec(size.Mouse, scrollHandle) || isDraggingAuthors;
        DRAW_SCROLLBAR(scrollHandle, isHoveredScroll);
    }
    EndScissorMode();
}

// =========================================================================
// ShowPublisher_Panel
// =========================================================================
void ShowPublisher_Panel(Size size, ManageBooksUI UI, float *wheel, Font *_Font, PublisherList *Publishers, char *selectedPublisher, BookList *Books)
{
    bool isMouseInPanel = CheckCollisionPointRec(size.Mouse, UI.Panel);
    float itemHeight = UI.Panel.height * 0.08f;
    if (itemHeight < 50.0f)
        itemHeight = 50.0f;
    float distance = itemHeight * 0.25f;
    int displayCount = Publishers->count;
    float TotalContentHeight = (itemHeight + distance) * displayCount;

    if (TotalContentHeight > UI.Panel.height)
    {
        float wheelMove = GetMouseWheelMove();
        if (wheelMove != 0)
            *wheel += wheelMove * 40.0f;
        if (*wheel > 0)
            *wheel = 0;
        if (*wheel < UI.Panel.height - TotalContentHeight - distance)
            *wheel = UI.Panel.height - TotalContentHeight - distance;
    }
    else
        *wheel = 0;

    BeginScissorMode((int)UI.Panel.x, (int)UI.Panel.y, (int)UI.Panel.width, (int)UI.Panel.height);
    for (int i = 0; i < displayCount; i++)
    {
        Rectangle hitbox = {UI.Panel.x + UI.Panel.width * 0.08f, *wheel + UI.Panel.y + 20.0f + (itemHeight + distance) * i, UI.Panel.width * 0.84f, itemHeight};
        bool isHovered = CheckCollisionPointRec(size.Mouse, hitbox) && isMouseInPanel;

        DrawRectangleRounded((Rectangle){hitbox.x + 3, hitbox.y + 3, hitbox.width, hitbox.height}, 0.4f, 20, Fade(BLACK, 0.25f));
        DrawRectangleRounded(hitbox, 0.4f, 20, isHovered ? BW_ACCENT : BW_CARD);
        DrawRectangleRounded((Rectangle){hitbox.x, hitbox.y + itemHeight * 0.18f, 5.0f, itemHeight * 0.64f}, 1.0f, 8, isHovered ? WHITE : BW_SKY);

        int nameFont = (int)(itemHeight * 0.5f);
        DrawTextEx(_Font[0], Publishers->Publisher[i], (Vector2){hitbox.x + 30, hitbox.y + (itemHeight - nameFont) / 2}, nameFont, 1, isHovered ? WHITE : (Color){220, 235, 255, 255});

        int bookCount = CountStrInBooks(Books, Publishers->Publisher[i], PUBLISHER, false);
        char countStr[32];
        sprintf(countStr, "%d Truyện", bookCount);
        int countFont = (int)(itemHeight * 0.4f);
        float countW = MeasureTextEx(_Font[0], countStr, countFont, 1).x;
        float badgeW_ = countW + 20.0f, badgeH_ = countFont * 1.5f;
        Rectangle countBadge = {hitbox.x + hitbox.width - badgeW_ - 15, hitbox.y + (itemHeight - badgeH_) / 2, badgeW_, badgeH_};
        DrawRectangleRounded(countBadge, 0.45f, 10, Fade(isHovered ? WHITE : BW_SKY, 0.18f));
        DrawRectangleRoundedLinesEx(countBadge, 0.45f, 10, 1.0f, Fade(isHovered ? WHITE : BW_SKY, 0.5f));
        DrawTextEx(_Font[0], countStr, (Vector2){countBadge.x + 10, countBadge.y + (badgeH_ - countFont) / 2}, countFont, 1, isHovered ? WHITE : BW_DIM);

        if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            strcpy(selectedPublisher, Publishers->Publisher[i]);
            *wheel = 0;
            Books->currentPage = 1;
        }
    }

    if (TotalContentHeight > UI.Panel.height)
    {
        float scrollTrackWidth = 12.0f;
        Rectangle scrollTrack = {UI.Panel.x + UI.Panel.width - scrollTrackWidth - 5, UI.Panel.y + 5, scrollTrackWidth, UI.Panel.height - 10};
        DrawRectangleRounded(scrollTrack, 1.0f, 10, Fade(BW_CARD_HOVER, 0.35f));
        float handleHeight = (UI.Panel.height / TotalContentHeight) * scrollTrack.height;
        if (handleHeight < 30.0f)
            handleHeight = 30.0f;
        float scrollableWheel = TotalContentHeight + distance - UI.Panel.height;
        float scrollRatio = -(*wheel) / scrollableWheel;
        float handleY = scrollTrack.y + scrollRatio * (scrollTrack.height - handleHeight);
        Rectangle scrollHandle = {scrollTrack.x, handleY, scrollTrackWidth, handleHeight};
        static bool isDraggingPub = false;
        static float dragOffsetPubY = 0.0f;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(size.Mouse, scrollHandle))
        {
            isDraggingPub = true;
            dragOffsetPubY = size.Mouse.y - scrollHandle.y;
        }
        if (isDraggingPub)
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                float newHandleY = size.Mouse.y - dragOffsetPubY;
                if (newHandleY < scrollTrack.y)
                    newHandleY = scrollTrack.y;
                if (newHandleY > scrollTrack.y + scrollTrack.height - handleHeight)
                    newHandleY = scrollTrack.y + scrollTrack.height - handleHeight;
                *wheel = -((newHandleY - scrollTrack.y) / (scrollTrack.height - handleHeight)) * scrollableWheel;
                scrollHandle.y = newHandleY;
            }
            else
                isDraggingPub = false;
        }
        bool isHoveredScroll = CheckCollisionPointRec(size.Mouse, scrollHandle) || isDraggingPub;
        DRAW_SCROLLBAR(scrollHandle, isHoveredScroll);
    }
    EndScissorMode();
}

// =========================================================================
// ShowType_Panel
// =========================================================================
void ShowType_Panel(Size size, ManageBooksUI UI, float *wheel, Font *_Font, Type *Types, char *selectedType, BookList *Books)
{
    bool isMouseInPanel = CheckCollisionPointRec(size.Mouse, UI.Panel);
    float itemHeight = UI.Panel.height * 0.08f;
    if (itemHeight < 50.0f)
        itemHeight = 50.0f;
    float distance = itemHeight * 0.25f;
    int displayCount = Types->count;
    float TotalContentHeight = (itemHeight + distance) * displayCount;

    if (TotalContentHeight > UI.Panel.height)
    {
        float wheelMove = GetMouseWheelMove();
        if (wheelMove != 0)
            *wheel += wheelMove * 40.0f;
        if (*wheel > 0)
            *wheel = 0;
        if (*wheel < UI.Panel.height - TotalContentHeight - distance)
            *wheel = UI.Panel.height - TotalContentHeight - distance;
    }
    else
        *wheel = 0;

    BeginScissorMode((int)UI.Panel.x, (int)UI.Panel.y, (int)UI.Panel.width, (int)UI.Panel.height);
    for (int i = 0; i < displayCount; i++)
    {
        Rectangle hitbox = {UI.Panel.x + UI.Panel.width * 0.08f, *wheel + UI.Panel.y + 20.0f + (itemHeight + distance) * i, UI.Panel.width * 0.84f, itemHeight};
        bool isHovered = CheckCollisionPointRec(size.Mouse, hitbox) && isMouseInPanel;

        DrawRectangleRounded((Rectangle){hitbox.x + 3, hitbox.y + 3, hitbox.width, hitbox.height}, 0.4f, 20, Fade(BLACK, 0.25f));
        DrawRectangleRounded(hitbox, 0.4f, 20, isHovered ? BW_ACCENT : BW_CARD);
        DrawRectangleRounded((Rectangle){hitbox.x, hitbox.y + itemHeight * 0.18f, 5.0f, itemHeight * 0.64f}, 1.0f, 8, isHovered ? WHITE : BW_SKY);

        int nameFont = (int)(itemHeight * 0.5f);
        DrawTextEx(_Font[0], Types->Type[i], (Vector2){hitbox.x + 30, hitbox.y + (itemHeight - nameFont) / 2}, nameFont, 1, isHovered ? WHITE : (Color){220, 235, 255, 255});

        int bookCount = CountStrInBooks(Books, Types->Type[i], TYPE, false);
        char countStr[32];
        sprintf(countStr, "%d Truyện", bookCount);
        int countFont = (int)(itemHeight * 0.4f);
        float countW = MeasureTextEx(_Font[0], countStr, countFont, 1).x;
        float badgeW_ = countW + 20.0f, badgeH_ = countFont * 1.5f;
        Rectangle countBadge = {hitbox.x + hitbox.width - badgeW_ - 15, hitbox.y + (itemHeight - badgeH_) / 2, badgeW_, badgeH_};
        DrawRectangleRounded(countBadge, 0.45f, 10, Fade(isHovered ? WHITE : BW_SKY, 0.18f));
        DrawRectangleRoundedLinesEx(countBadge, 0.45f, 10, 1.0f, Fade(isHovered ? WHITE : BW_SKY, 0.5f));
        DrawTextEx(_Font[0], countStr, (Vector2){countBadge.x + 10, countBadge.y + (badgeH_ - countFont) / 2}, countFont, 1, isHovered ? WHITE : BW_DIM);

        if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            strcpy(selectedType, Types->Type[i]);
            *wheel = 0;
            Books->currentPage = 1;
        }
    }

    if (TotalContentHeight > UI.Panel.height)
    {
        float scrollTrackWidth = 12.0f;
        Rectangle scrollTrack = {UI.Panel.x + UI.Panel.width - scrollTrackWidth - 5, UI.Panel.y + 5, scrollTrackWidth, UI.Panel.height - 10};
        DrawRectangleRounded(scrollTrack, 1.0f, 10, Fade(BW_CARD_HOVER, 0.35f));
        float handleHeight = (UI.Panel.height / TotalContentHeight) * scrollTrack.height;
        if (handleHeight < 30.0f)
            handleHeight = 30.0f;
        float scrollableWheel = TotalContentHeight + distance - UI.Panel.height;
        float scrollRatio = -(*wheel) / scrollableWheel;
        float handleY = scrollTrack.y + scrollRatio * (scrollTrack.height - handleHeight);
        Rectangle scrollHandle = {scrollTrack.x, handleY, scrollTrackWidth, handleHeight};
        static bool isDraggingTypes = false;
        static float dragOffsetTypesY = 0.0f;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(size.Mouse, scrollHandle))
        {
            isDraggingTypes = true;
            dragOffsetTypesY = size.Mouse.y - scrollHandle.y;
        }
        if (isDraggingTypes)
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                float newHandleY = size.Mouse.y - dragOffsetTypesY;
                if (newHandleY < scrollTrack.y)
                    newHandleY = scrollTrack.y;
                if (newHandleY > scrollTrack.y + scrollTrack.height - handleHeight)
                    newHandleY = scrollTrack.y + scrollTrack.height - handleHeight;
                *wheel = -((newHandleY - scrollTrack.y) / (scrollTrack.height - handleHeight)) * scrollableWheel;
                scrollHandle.y = newHandleY;
            }
            else
                isDraggingTypes = false;
        }
        bool isHoveredScroll = CheckCollisionPointRec(size.Mouse, scrollHandle) || isDraggingTypes;
        DRAW_SCROLLBAR(scrollHandle, isHoveredScroll);
    }
    EndScissorMode();
}

// =========================================================================
// LoadManageBooksUI  +  ManageBooksTitle
// =========================================================================
void LoadManageBooksUI(ManageBooksUI *UI, Size size)
{
    UI->TitleBox = (Rectangle){0, 0, size.Screen.x, size.Screen.y * 0.08f * (float)pow(0.6, (double)size.Scale.x)};
    UI->HeaderBox = (Rectangle){UI->TitleBox.width * 0.15f, UI->TitleBox.height, UI->TitleBox.width * 0.7f, UI->TitleBox.height};
    float totalPanelHeight = size.Screen.y - (UI->HeaderBox.height + UI->TitleBox.height);
    float paginationHeight = 70.0f;
    UI->Panel = (Rectangle){UI->HeaderBox.x, UI->HeaderBox.y + UI->TitleBox.height, UI->HeaderBox.width, totalPanelHeight};
    UI->PaginationBox = (Rectangle){UI->HeaderBox.x, UI->Panel.y + totalPanelHeight - paginationHeight, UI->HeaderBox.width, paginationHeight};
}

void ManageBooksTitle(ManageBooksUI UI)
{
    // ── Deep-navy title bar ────────────────────────────────────────────
    DrawRectangleRec(UI.TitleBox, BW_TITLE_BG);
    // Bottom highlight line
    DrawLineEx((Vector2){0, UI.TitleBox.height - 1.5f},
               (Vector2){UI.TitleBox.width, UI.TitleBox.height - 1.5f},
               1.5f, Fade(BW_ACCENT, 0.35f));

    // ── Medium-navy toolbar bar ────────────────────────────────────────
    DrawRectangleRec(UI.HeaderBox, BW_HEADER_BG);
    // Top/bottom subtle borders
    DrawLineEx((Vector2){UI.HeaderBox.x, UI.HeaderBox.y},
               (Vector2){UI.HeaderBox.x + UI.HeaderBox.width, UI.HeaderBox.y},
               1.0f, Fade(BW_SKY, 0.2f));
    DrawLineEx((Vector2){UI.HeaderBox.x, UI.HeaderBox.y + UI.HeaderBox.height},
               (Vector2){UI.HeaderBox.x + UI.HeaderBox.width, UI.HeaderBox.y + UI.HeaderBox.height},
               1.5f, Fade(BW_ACCENT, 0.4f));

    // ── Light blue-white content panel ────────────────────────────────
    DrawRectangleRec(UI.Panel, BW_PANEL_BG);
}

// =========================================================================
// NormalizeString  (unchanged)
// =========================================================================
void NormalizeString(const char *src, char *dest, int max_len)
{
    const char *vn_utf8[] = {"à", "á", "ạ", "ả", "ã", "â", "ầ", "ấ", "ậ", "ẩ", "ẫ", "ă", "ằ", "ắ", "ặ", "ẳ", "ẵ", "è", "é", "ẹ", "ẻ", "ẽ", "ê", "ề", "ế", "ệ", "ể", "ễ", "ì", "í", "ị", "ỉ", "ĩ", "ò", "ó", "ọ", "ỏ", "õ", "ô", "ồ", "ố", "ộ", "ổ", "ỗ", "ơ", "ờ", "ớ", "ợ", "ở", "ỡ", "ù", "ú", "ụ", "ủ", "ũ", "ư", "ừ", "ứ", "ự", "ử", "ữ", "ỳ", "ý", "ỵ", "ỷ", "ỹ", "đ", "À", "Á", "Ạ", "Ả", "Ã", "Â", "Ầ", "Ấ", "Ậ", "Ẩ", "Ẫ", "Ă", "Ằ", "Ắ", "Ặ", "Ẳ", "Ẵ", "È", "É", "Ẹ", "Ẻ", "Ẽ", "Ê", "Ề", "Ế", "Ệ", "Ể", "Ễ", "Ì", "Í", "Ị", "Ỉ", "Ĩ", "Ò", "Ó", "Ọ", "Ỏ", "Õ", "Ô", "Ồ", "Ố", "Ộ", "Ổ", "Ỗ", "Ơ", "Ờ", "Ớ", "Ợ", "Ở", "Ỡ", "Ù", "Ú", "Ụ", "Ủ", "Ũ", "Ư", "Ừ", "Ứ", "Ự", "Ử", "Ữ", "Ỳ", "Ý", "Ỵ", "Ỷ", "Ỹ", "Đ"};
    const char ascii_repl[] = {'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'i', 'i', 'i', 'i', 'i', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'y', 'y', 'y', 'y', 'y', 'd', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'i', 'i', 'i', 'i', 'i', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'o', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'y', 'y', 'y', 'y', 'y', 'd'};
    int num_vn = sizeof(vn_utf8) / sizeof(vn_utf8[0]);
    int i = 0, j = 0;
    while (src[i] != '\0' && j < max_len - 1)
    {
        if (isspace((unsigned char)src[i]))
        {
            i++;
            continue;
        }
        bool replaced = false;
        for (int k = 0; k < num_vn; k++)
        {
            int len = strlen(vn_utf8[k]);
            if (strncmp(&src[i], vn_utf8[k], len) == 0)
            {
                dest[j++] = ascii_repl[k];
                i += len;
                replaced = true;
                break;
            }
        }
        if (!replaced)
        {
            dest[j++] = tolower((unsigned char)src[i]);
            i++;
        }
    }
    dest[j] = '\0';
}

// =========================================================================
// ContextualBookSearch  (unchanged)
// =========================================================================
bool ContextualBookSearch(Book book, const char *query)
{
    if (query == NULL || strlen(query) == 0)
        return true;
    char normQuery[512] = {0};
    NormalizeString(query, normQuery, sizeof(normQuery));
    char rawCombined[2048];
    sprintf(rawCombined, "%s %s %s %s %s", book.NameBook, book.NormNameBook, book.AuthorBook, book.TypeBook, book.PublisherBook);
    char normCombined[2048] = {0};
    NormalizeString(rawCombined, normCombined, sizeof(normCombined));
    return strstr(normCombined, normQuery) != NULL;
}

// =========================================================================
// CountStrInBooks  (unchanged)
// =========================================================================
int CountStrInBooks(BookList *Books, const char *Str, StateFindBook state, bool showOnlyInStock)
{
    int count = 0;
    for (int i = 0; i < Books->count; i++)
    {
        int match = 0;
        switch (state)
        {
        case CODE:
            if (strcmp(Books->theArray[i].CodeBook, Str) == 0)
                match = 1;
            break;
        case NAME:
            if (strcmp(Books->theArray[i].NameBook, Str) == 0)
                match = 1;
            break;
        case AUTHOR:
            if (strcmp(Books->theArray[i].AuthorBook, Str) == 0)
                match = 1;
            break;
        case TYPE:
            if (strcmp(Books->theArray[i].TypeBook, Str) == 0)
                match = 1;
            break;
        case PUBLISHER:
            if (strcmp(Books->theArray[i].PublisherBook, Str) == 0)
                match = 1;
            break;
        case SEARCH_ALL:
            if (ContextualBookSearch(Books->theArray[i], Str))
                match = 1;
            break;
        default:
            match = 1;
            break;
        }
        if (match && showOnlyInStock && Books->theArray[i].TotalStock <= 0)
            match = 0;
        if (match)
            count++;
    }
    return count;
}

// =========================================================================
// DrawPagination
// =========================================================================
void DrawPagination(ManageBooksUI UI, BookList *Books, Font *_Font, Size size, float *wheel, char *backTarget, bool isVisible)
{
    if (!isVisible || Books->totalPages <= 1)
        return;

    // Light blue-white pagination strip
    DrawRectangleRec(UI.PaginationBox, (Color){230, 240, 255, 248});
    DrawLineEx((Vector2){UI.PaginationBox.x, UI.PaginationBox.y},
               (Vector2){UI.PaginationBox.x + UI.PaginationBox.width, UI.PaginationBox.y},
               1.5f, BW_DIVIDER);

    float btnWidth = 44.0f, btnHeight = 44.0f, spacing = 10.0f;
    int maxVisible = 5;
    int startPage = Books->currentPage - 2;
    if (startPage < 1)
        startPage = 1;
    int endPage = startPage + maxVisible - 1;
    if (endPage > Books->totalPages)
    {
        endPage = Books->totalPages;
        startPage = endPage - maxVisible + 1;
        if (startPage < 1)
            startPage = 1;
    }

    float totalWidth = (endPage - startPage + 1) * (btnWidth + spacing) + 2 * (90.0f + spacing);
    float startX = UI.PaginationBox.x + (UI.PaginationBox.width - totalWidth) / 2;
    float startY = UI.PaginationBox.y + (UI.PaginationBox.height - btnHeight) / 2;

    // First button
    Rectangle firstBtn = {startX, startY, 90.0f, btnHeight};
    if (Books->currentPage > 1)
    {
        bool hov = CheckCollisionPointRec(size.Mouse, firstBtn);
        DrawRectangleRounded((Rectangle){firstBtn.x + 2, firstBtn.y + 2, firstBtn.width, firstBtn.height}, 0.5f, 10, Fade(BLACK, 0.2f));
        DrawRectangleRounded(firstBtn, 0.5f, 10, hov ? BW_ACCENT : BW_CARD);
        DrawTextEx(_Font[3], "First",
                   (Vector2){firstBtn.x + (90.0f - MeasureTextEx(_Font[3], "First", 18, 1).x) / 2, firstBtn.y + (btnHeight - 18) / 2},
                   18, 1, WHITE);
        if (hov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Books->currentPage = 1;
            *wheel = 0;
        }
    }
    startX += 90.0f + spacing;

    // Page number buttons
    for (int p = startPage; p <= endPage; p++)
    {
        Rectangle pageBtn = {startX, startY, btnWidth, btnHeight};
        bool isCurrent = (p == Books->currentPage);
        bool hov = CheckCollisionPointRec(size.Mouse, pageBtn);

        DrawRectangleRounded((Rectangle){pageBtn.x + 2, pageBtn.y + 2, pageBtn.width, pageBtn.height}, 0.5f, 10, Fade(BLACK, 0.2f));
        Color btnCol = isCurrent ? BW_ACCENT : (hov ? BW_CARD_HOVER : BW_CARD);
        DrawRectangleRounded(pageBtn, 0.5f, 10, btnCol);
        if (isCurrent)
            DrawRectangleRoundedLinesEx(pageBtn, 0.5f, 10, 2.0f, Fade(BW_SKY, 0.6f));

        const char *numStr = TextFormat("%d", p);
        DrawTextEx(_Font[3], numStr,
                   (Vector2){pageBtn.x + (btnWidth - MeasureTextEx(_Font[3], numStr, 20, 1).x) / 2,
                             pageBtn.y + (btnHeight - 20) / 2},
                   20, 1, isCurrent ? WHITE : BW_DIM);

        if (!isCurrent && hov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Books->currentPage = p;
            *wheel = 0;
        }
        startX += btnWidth + spacing;
    }

    // Last button
    Rectangle lastBtn = {startX, startY, 90.0f, btnHeight};
    if (Books->currentPage < Books->totalPages)
    {
        bool hov = CheckCollisionPointRec(size.Mouse, lastBtn);
        DrawRectangleRounded((Rectangle){lastBtn.x + 2, lastBtn.y + 2, lastBtn.width, lastBtn.height}, 0.5f, 10, Fade(BLACK, 0.2f));
        DrawRectangleRounded(lastBtn, 0.5f, 10, hov ? BW_ACCENT : BW_CARD);
        DrawTextEx(_Font[3], "Last",
                   (Vector2){lastBtn.x + (90.0f - MeasureTextEx(_Font[3], "Last", 18, 1).x) / 2, lastBtn.y + (btnHeight - 18) / 2},
                   18, 1, WHITE);
        if (hov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Books->currentPage = Books->totalPages;
            *wheel = 0;
        }
    }
}

// =========================================================================
// ManageBooksFunc  –  Navigation bar + search bar
// =========================================================================
bool ManageBooksFunc(MANAGEBOOKS_STATE *State, InputBox *FindBar, Texture2D Icon_Find, ManageBooksUI UI, Size size, Font *_Font)
{
    int _return = 0;

    // Shared metrics
    const int NAV_FONT = 24;
    const int NAV_SPACING = 2;

    // Active-state indicator helper (draws sky-blue underline + bold text when hovered/active)
    // We compute boxes first, then draw.

    Vector2 TextWidth_Home = MeasureTextEx(_Font[0], "Home", NAV_FONT, NAV_SPACING);
    Rectangle HomeBox = {
        UI.TitleBox.width * 0.03f,
        UI.TitleBox.height * 0.9f - TextWidth_Home.y,
        TextWidth_Home.x, TextWidth_Home.y};

    bool hovHome = CheckCollisionPointRec(size.Mouse, HomeBox);
    bool isHome = (*State == MANAGEBOOKS_Dashboard);
    DrawTextEx(_Font[0], "Home", (Vector2){HomeBox.x, HomeBox.y}, NAV_FONT, NAV_SPACING,
               (hovHome || isHome) ? WHITE : BW_NAV_TEXT);
    // Underline: sky-blue if active/hovered, dim otherwise
    DrawLineEx((Vector2){HomeBox.x, HomeBox.y + HomeBox.height + 2},
               (Vector2){HomeBox.x + HomeBox.width, HomeBox.y + HomeBox.height + 2},
               2.5f, (hovHome || isHome) ? BW_SKY : Fade(BW_DIM, 0.35f));
    if (hovHome && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        *State = MANAGEBOOKS_Dashboard;
        _return = 1;
    }

    // Helper lambda (as local variables) for each nav item
    float navX = HomeBox.x + HomeBox.width + UI.TitleBox.width * 0.04f;

    // Tác Giả
    Vector2 tw = MeasureTextEx(_Font[0], MANAGEBOOKS_Func_1, NAV_FONT, NAV_SPACING);
    Rectangle AuthorBox = {navX, HomeBox.y, tw.x, tw.y};
    bool hovAuth = CheckCollisionPointRec(size.Mouse, AuthorBox);
    bool isAuthor = (*State == MANAGEBOOKS_Author);
    DrawTextEx(_Font[0], MANAGEBOOKS_Func_1, (Vector2){AuthorBox.x, AuthorBox.y}, NAV_FONT, NAV_SPACING, (hovAuth || isAuthor) ? WHITE : BW_NAV_TEXT);
    DrawLineEx((Vector2){AuthorBox.x, AuthorBox.y + AuthorBox.height + 2}, (Vector2){AuthorBox.x + AuthorBox.width, AuthorBox.y + AuthorBox.height + 2}, 2.5f, (hovAuth || isAuthor) ? BW_SKY : Fade(BW_DIM, 0.35f));
    if (hovAuth && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        *State = MANAGEBOOKS_Author;
        _return = 1;
    }
    navX = AuthorBox.x + AuthorBox.width + UI.TitleBox.width * 0.03f;

    // Nhà Xuất Bản
    tw = MeasureTextEx(_Font[0], MANAGEBOOKS_Func_2, NAV_FONT, NAV_SPACING);
    Rectangle PublisherBox = {navX, HomeBox.y, tw.x, tw.y};
    bool hovPub = CheckCollisionPointRec(size.Mouse, PublisherBox);
    bool isPub = (*State == MANAGEBOOKS_Publisher);
    DrawTextEx(_Font[0], MANAGEBOOKS_Func_2, (Vector2){PublisherBox.x, PublisherBox.y}, NAV_FONT, NAV_SPACING, (hovPub || isPub) ? WHITE : BW_NAV_TEXT);
    DrawLineEx((Vector2){PublisherBox.x, PublisherBox.y + PublisherBox.height + 2}, (Vector2){PublisherBox.x + PublisherBox.width, PublisherBox.y + PublisherBox.height + 2}, 2.5f, (hovPub || isPub) ? BW_SKY : Fade(BW_DIM, 0.35f));
    if (hovPub && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        *State = MANAGEBOOKS_Publisher;
        _return = 1;
    }
    navX = PublisherBox.x + PublisherBox.width + UI.TitleBox.width * 0.03f;

    // Thể Loại
    tw = MeasureTextEx(_Font[0], MANAGEBOOKS_Func_3, NAV_FONT, NAV_SPACING);
    Rectangle TypeBox = {navX, HomeBox.y, tw.x, tw.y};
    bool hovType = CheckCollisionPointRec(size.Mouse, TypeBox);
    bool isType = (*State == MANAGEBOOKS_Type);
    DrawTextEx(_Font[0], MANAGEBOOKS_Func_3, (Vector2){TypeBox.x, TypeBox.y}, NAV_FONT, NAV_SPACING, (hovType || isType) ? WHITE : BW_NAV_TEXT);
    DrawLineEx((Vector2){TypeBox.x, TypeBox.y + TypeBox.height + 2}, (Vector2){TypeBox.x + TypeBox.width, TypeBox.y + TypeBox.height + 2}, 2.5f, (hovType || isType) ? BW_SKY : Fade(BW_DIM, 0.35f));
    if (hovType && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        *State = MANAGEBOOKS_Type;
        _return = 1;
    }
    navX = TypeBox.x + TypeBox.width + UI.TitleBox.width * 0.03f;

    // Trang Điều Hành
    tw = MeasureTextEx(_Font[0], MANAGEBOOKS_Func_4, NAV_FONT, NAV_SPACING);
    Rectangle MainBox = {navX, HomeBox.y, tw.x, tw.y};
    bool hovMain = CheckCollisionPointRec(size.Mouse, MainBox);
    DrawTextEx(_Font[0], MANAGEBOOKS_Func_4, (Vector2){MainBox.x, MainBox.y}, NAV_FONT, NAV_SPACING, hovMain ? WHITE : BW_NAV_TEXT);
    DrawLineEx((Vector2){MainBox.x, MainBox.y + MainBox.height + 2}, (Vector2){MainBox.x + MainBox.width, MainBox.y + MainBox.height + 2}, 2.5f, hovMain ? BW_SKY : Fade(BW_DIM, 0.35f));
    if (hovMain && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        *State = MANAGEBOOKS_Main;
        _return = 1;
    }

    // ── Search bar ──────────────────────────────────────────────────────
    float findStartX = MainBox.x + MainBox.width + UI.TitleBox.width * 0.03f;
    float findWidth = UI.TitleBox.width - findStartX - 20.0f;
    if (findWidth < UI.TitleBox.height * 2.5f)
        findWidth = UI.TitleBox.height * 2.5f;

    FindBar->box = (Rectangle){findStartX, UI.TitleBox.height * 0.1f, findWidth, UI.TitleBox.height * 0.8f};
    Rectangle IconFindBox = {FindBar->box.x + (FindBar->box.width - FindBar->box.height), FindBar->box.y, FindBar->box.height, FindBar->box.height};
    Rectangle FindBarTextBox = {FindBar->box.x, FindBar->box.y, FindBar->box.width - IconFindBox.width, FindBar->box.height};
    Rectangle FindBarTextPos = {FindBarTextBox.x + 10, FindBarTextBox.y + (FindBarTextBox.height - 20.0f) / 2.0f, FindBarTextBox.width - 15.0f, 20.0f};

    if (CheckCollisionPointRec(size.Mouse, FindBarTextBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        FindBar->isFocused = true;
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        FindBar->isFocused = false;

    float roundness = FindRoundness(0.05f * FindBar->box.width, FindBar->box.width, FindBar->box.height);

    // Search bar background (always white, border changes)
    bool searchHasContent = (FindBar->length > 0);
    Color searchBorder = FindBar->isFocused ? BW_ACCENT
                                            : (searchHasContent ? BW_SKY
                                                                : Fade(BW_DIM, 0.45f));
    float borderThick = FindBar->isFocused ? 2.5f : 1.5f;
    DrawRectangleRounded(FindBar->box, roundness, 10, WHITE);
    DrawRectangleRoundedLinesEx(FindBar->box, roundness, 10, borderThick, searchBorder);

    if (FindBar->isFocused)
    {
        BeginScissorMode((int)FindBarTextPos.x, (int)FindBarTextPos.y, (int)FindBarTextPos.width, (int)FindBarTextPos.height);
        UpdateInputBox(FindBar);
        float w = MeasureTextEx(_Font[0], FindBar->text, 20, 2).x;
        float dx = ((w - (FindBarTextPos.width - 5)) > 0) ? w - (FindBarTextPos.width - 5) : 0;
        DrawTextEx(_Font[0], FindBar->text, (Vector2){FindBarTextPos.x - dx, FindBarTextPos.y}, 20, 2, BW_FORM_TITLE);
        if ((int)(GetTime() * 2) % 2 == 0)
            DrawRectangle((int)(FindBarTextPos.x + w - dx + 2), (int)FindBarTextPos.y, 2, 20, BW_ACCENT);
        EndScissorMode();
        if (IsKeyPressed(KEY_ENTER))
        {
            *State = MANAGEBOOKS_Find;
            _return = 1;
        }
    }
    else if (FindBar->length > 0)
    {
        BeginScissorMode((int)FindBarTextPos.x, (int)FindBarTextPos.y, (int)FindBarTextPos.width, (int)FindBarTextPos.height);
        float w = MeasureTextEx(_Font[0], FindBar->text, 20, 2).x;
        float dx = ((w - FindBarTextPos.width) > 0) ? w - FindBarTextPos.width : 0;
        DrawTextEx(_Font[0], FindBar->text, (Vector2){FindBarTextPos.x - dx, FindBarTextPos.y}, 20, 2, BW_FORM_TITLE);
        EndScissorMode();
    }
    else
    {
        BeginScissorMode((int)FindBarTextPos.x, (int)FindBarTextPos.y, (int)FindBarTextPos.width, (int)FindBarTextPos.height);
        DrawTextEx(_Font[0], "Tìm kiếm...", (Vector2){FindBarTextPos.x, FindBarTextPos.y}, 20, 2, BW_DIM);
        EndScissorMode();
    }

    // Search icon button (accent blue)
    float iconRound = FindRoundness(0.05f * FindBar->box.width, IconFindBox.width, IconFindBox.height);
    bool hovIcon = CheckCollisionPointRec(size.Mouse, IconFindBox);
    DrawRectangleRounded(IconFindBox, iconRound, 10, hovIcon ? BW_CARD_HOVER : BW_ACCENT);
    DrawIcon(IconFindBox, Icon_Find);
    if (hovIcon && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        *State = MANAGEBOOKS_Find;
        _return = 1;
    }

    return _return;
}

// =========================================================================
// LoadVolumes / SaveVolumes  (unchanged)
// =========================================================================
void LoadVolumes(BookList *Books, const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return;
    char line[1024];
    fgets(line, sizeof(line), f);
    char vName[256], vCode[32], bCode[32];
    int stock, impStock;
    while (fscanf(f, " %[^|]| %[^|]| %[^|]| %d | %d \n", vName, vCode, bCode, &stock, &impStock) == 5)
    {
        trim(vName);
        trim(vCode);
        trim(bCode);
        for (int i = 0; i < Books->count; i++)
        {
            if (strcmp(Books->theArray[i].CodeBook, bCode) == 0)
            {
                int v = Books->theArray[i].volumeCount;
                if (v < 200)
                {
                    strcpy(Books->theArray[i].volumes[v].VolumeName, vName);
                    strcpy(Books->theArray[i].volumes[v].VolumeCode, vCode);
                    strcpy(Books->theArray[i].volumes[v].BookCode, bCode);
                    Books->theArray[i].volumes[v].Stock = stock;
                    Books->theArray[i].volumes[v].ImportStock = impStock;
                    Books->theArray[i].volumeCount++;
                    Books->theArray[i].TotalStock += stock;
                }
                break;
            }
        }
    }
    fclose(f);
}

void SaveVolumes(BookList Books, const char *filename)
{
    FILE *f = fopen(filename, "w");
    if (!f)
        return;
    fprintf(f, "Tên truyện có cả tập (Nếu không nó từ ghi lại y tên đầu tiên) | CXXX (ví dụ tập 1 là 001 nếu không có tập thì 000) | Mã truyện | Số lượng hiện tại | Số lượng nhập vào\n");
    for (int i = 0; i < Books.count; i++)
        for (int j = 0; j < Books.theArray[i].volumeCount; j++)
        {
            Volume v = Books.theArray[i].volumes[j];
            fprintf(f, "%s | %s | %s | %d | %d\n", v.VolumeName, v.VolumeCode, v.BookCode, v.Stock, v.ImportStock);
        }
    fclose(f);
}

// =========================================================================
// Loadbooks / Savebooks  (unchanged)
// =========================================================================
BookList *Loadbooks(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
        return NULL;
    BookList *bookList = (BookList *)malloc(sizeof(BookList));
    if (!bookList)
    {
        fclose(file);
        return NULL;
    }
    bookList->theArray = (Book *)malloc(MAX_BOOKS * sizeof(Book));
    if (!bookList->theArray)
    {
        free(bookList);
        fclose(file);
        return NULL;
    }
    bookList->capacity = MAX_BOOKS;
    fscanf(file, "Số quyển truyện: %d\n", &(bookList->count));
    bookList->pos = 0;
    bookList->QuantityForOnePage = QUANTITYFORONEPAGE;
    bookList->currentPage = 1;
    bookList->totalPages = 1;
    Book book;
    int count = 0;
    while (fscanf(file, " | %[^|]| %[^|]| %[^|]| %[^|]| %[^|]| %[^|]| %d | %d |\n",
                  book.CodeBook, book.NormNameBook, book.NameBook, book.AuthorBook, book.TypeBook, book.PublisherBook,
                  &book.YearBook, &book.PriceBook) == 8)
    {
        trim(book.CodeBook);
        trim(book.NormNameBook);
        trim(book.NameBook);
        trim(book.AuthorBook);
        trim(book.TypeBook);
        trim(book.PublisherBook);
        book.Description[0] = '\0';
        book.volumeCount = 0;
        book.TotalStock = 0;
        bookList->theArray[count++] = book;
    }
    bookList->count = count;
    fclose(file);
    LoadVolumes(bookList, LIST_BOOKS_FILE);
    return bookList;
}

bool Savebooks(BookList Books)
{
    FILE *f = fopen(BOOKS_FILE, "w");
    if (f == NULL)
        return false;
    fprintf(f, "Số quyển truyện: %d\n", Books.count);
    for (int i = 0; i < Books.count; i++)
    {
        Book A = Books.theArray[i];
        fprintf(f, "| %-*s | %-*s | %-*s | %-*s | %-*s | %-*s | %-*d | %-*d |\n",
                UTF8Width(A.CodeBook, CODE_BOOKS_LENGTH) - 2, A.CodeBook,
                UTF8Width(A.NormNameBook, NORMNAME_BOOKS_LENGTH) - 2, A.NormNameBook,
                UTF8Width(A.NameBook, NAME_BOOKS_LENGTH) - 2, A.NameBook,
                UTF8Width(A.AuthorBook, AUTHOR_BOOKS_LENGTH) - 2, A.AuthorBook,
                UTF8Width(A.TypeBook, TYPE_BOOKS_LENGTH) - 2, A.TypeBook,
                UTF8Width(A.PublisherBook, PUBLISHER_BOOKS_LENGTH) - 2, A.PublisherBook,
                YEAR_BOOKS_LENGTH - 2, A.YearBook,
                PRICE_BOOKS_LENGTH - 2, A.PriceBook);
    }
    fclose(f);
    SaveVolumes(Books, LIST_BOOKS_FILE);
    return true;
}

// =========================================================================
// LoadAuthor / SaveAuthor  (unchanged)
// =========================================================================
AuthorList *LoadAuthor(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (f == NULL)
        return NULL;
    int count;
    fscanf(f, "Tổng: %d\n", &count);
    AuthorList *A = malloc(sizeof(AuthorList));
    if (A == NULL)
        return NULL;
    A->Author = malloc(sizeof(Author) * count);
    if (A->Author == NULL)
    {
        free(A);
        return NULL;
    }
    A->count = 0;
    while ((fscanf(f, " | %[^|]| %[^|]|\n", A->Author[A->count].code, A->Author[A->count].name) == 2))
    {
        trim(A->Author[A->count].code);
        trim(A->Author[A->count].name);
        A->count++;
    }
    fclose(f);
    if (A->count != count)
    {
        free(A);
        return NULL;
    }
    return A;
}

bool SaveAuthor(AuthorList _Author)
{
    FILE *f = fopen(AUTHORS_FILE, "w");
    if (f == NULL)
        return false;
    fprintf(f, "Tổng: %d\n", _Author.count);
    int i;
    for (i = 0; i < _Author.count; i++)
    {
        Author A = _Author.Author[i];
        fprintf(f, "| %*s | %*s |\n", UTF8Width(A.code, AUTHORCODE_AUTHORS_LENGTH), A.code, UTF8Width(A.name, AUTHOR_AUTHORS_LENGTH), A.name);
    }
    fclose(f);
    if (i != _Author.count)
        return false;
    return true;
}

// =========================================================================
// LoadType / LoadPublisher / LoadDescription  (unchanged)
// =========================================================================
Type *LoadType(BookList *Books)
{
    if (Books == NULL || Books->theArray == NULL || Books->count <= 0)
        return NULL;
    Type *type = malloc(sizeof(Type));
    if (type == NULL)
        return NULL;
    type->Type = malloc(sizeof(char *) * Books->count);
    if (type->Type == NULL)
    {
        free(type);
        return NULL;
    }
    type->count = 0;
    for (int i = 0; i < Books->count; i++)
    {
        int flag = 0;
        for (int j = 0; j < type->count; j++)
            if (strcmp(type->Type[j], Books->theArray[i].TypeBook) == 0)
            {
                flag = 1;
                break;
            }
        if (!flag)
        {
            type->Type[type->count] = malloc(sizeof(char) * 32);
            if (type->Type[type->count] == NULL)
                continue;
            strcpy(type->Type[type->count], Books->theArray[i].TypeBook);
            type->count++;
        }
    }
    return type;
}

PublisherList *LoadPublisher(BookList *Books)
{
    if (Books == NULL || Books->theArray == NULL || Books->count <= 0)
        return NULL;
    PublisherList *pub = malloc(sizeof(PublisherList));
    if (pub == NULL)
        return NULL;
    pub->Publisher = malloc(sizeof(char *) * Books->count);
    if (pub->Publisher == NULL)
    {
        free(pub);
        return NULL;
    }
    pub->count = 0;
    for (int i = 0; i < Books->count; i++)
    {
        int flag = 0;
        for (int j = 0; j < pub->count; j++)
            if (strcmp(pub->Publisher[j], Books->theArray[i].PublisherBook) == 0)
            {
                flag = 1;
                break;
            }
        if (!flag)
        {
            pub->Publisher[pub->count] = malloc(sizeof(char) * 64);
            if (pub->Publisher[pub->count] == NULL)
                continue;
            strcpy(pub->Publisher[pub->count], Books->theArray[i].PublisherBook);
            pub->count++;
        }
    }
    return pub;
}

void LoadDescription(BookList *Books, const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return;
    char line[1024];
    Book *currentBook = NULL;
    while (fgets(line, sizeof(line), f))
    {
        if (line[0] == 'T' && isdigit(line[1]) && isdigit(line[2]) && isdigit(line[3]))
        {
            char code[10];
            sscanf(line, "%s", code);
            currentBook = NULL;
            for (int i = 0; i < Books->count; i++)
                if (strcmp(Books->theArray[i].CodeBook, code) == 0)
                {
                    currentBook = &Books->theArray[i];
                    break;
                }
        }
        else if (currentBook != NULL)
            strcat(currentBook->Description, line);
    }
    fclose(f);
}