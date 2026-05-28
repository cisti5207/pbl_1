#include "ManageBooks.h"
#include "raylib.h"
#include "libmanage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

// Biến nhớ toàn cục để hệ thống biết bạn đang search từ Tab nào
static MANAGEBOOKS_STATE g_searchContext = MANAGEBOOKS_Dashboard;

// =========================================================================
// HÀM KHỞI TẠO VÀ VÒNG LẶP CHÍNH
// =========================================================================
void InitManageBooks(Role _role)
{
    Size ManageBooksSize;
    LoadSize(
        &ManageBooksSize,
        (Vector2) { GetMonitorWidth(0), GetMonitorHeight(0) }, 
        (Vector2) { GetScreenWidth(), GetScreenHeight() },
        (Vector2) { (float)GetScreenWidth() / GetMonitorWidth(0), (float)GetScreenHeight() / GetMonitorHeight(0) },
        GetMousePosition()
    );

    ManageBooksUI customUI;
    LoadManageBooksUI(&customUI, ManageBooksSize);

    MANAGEBOOKS_STATE State = MANAGEBOOKS_Dashboard;
    MANAGEBOOKS_STATE prevState = MANAGEBOOKS_Dashboard;

    Font _Font[4]; 
    _Font[0] = SetFontUTF8(ArialBold, 100);
    _Font[1] = SetFontUTF8(Cooper, 50);
    _Font[2] = SetFontUTF8(Gill, 50);
    _Font[3] = SetFontUTF8(Roboto_Semibold, 50);

    BookList* Books = Loadbooks(BOOKS_FILE);
    AuthorList* Authors = LoadAuthor(AUTHORS_FILE);
    Type* Types = LoadType(Books);
    PublisherList* Publishers = LoadPublisher(Books);

    if (Books == NULL) printf("FAILED!! Cann't load dataTruyen.txt\n");
    if (Authors == NULL) printf("FAILED!! Cann't load author.txt\n");
    if (Types == NULL) printf("FAILED!! Cann't load Type from Books\n");

    LoadDescription(Books, DESCRIPTION_FILE);

    InputBox FindBar = {0};
    Texture2D Avatar = LoadTexture(MANAGEBOOKS_Avatar); 
    Texture2D Icon_Find = LoadTexture(Icon_Find_lnk);

    float wheel = 0;
    
    char selectedAuthor[256] = "";
    char selectedPublisher[256] = "";
    char selectedType[256] = "";
    
    int selectedBookIndex = -1; 
    int requestDeleteIndex = -1; 
    
    InputBox addInputs[8] = {0}; 

    while (!WindowShouldClose()){
        UpdateParticlesPosition(ManageBooksSize);

        if (IsWindowResized()){
            LoadSize(
                &ManageBooksSize, 
                (Vector2) {0}, 
                (Vector2) {GetScreenWidth(), GetScreenHeight()},
                (Vector2) {(float)GetScreenWidth()/GetMonitorWidth(0), (float)GetScreenHeight()/GetMonitorHeight(0)}, 
                (Vector2) {0}
            );
            LoadManageBooksUI(&customUI, ManageBooksSize);
        }
        ManageBooksSize.Mouse = GetMousePosition();

        BeginDrawing();
        ClearBackground(AnimatedBackground());
        DrawBackgroundParticles();

        ManageBooksTitle(Avatar, customUI);

        MANAGEBOOKS_STATE loopOldState = State;
        if (ManageBooksFunc(&State, &FindBar, Icon_Find, customUI, ManageBooksSize, _Font) == 1){
            if (State == MANAGEBOOKS_Find) {
                if (loopOldState == MANAGEBOOKS_Dashboard || 
                    loopOldState == MANAGEBOOKS_Author || 
                    loopOldState == MANAGEBOOKS_Publisher || 
                    loopOldState == MANAGEBOOKS_Type) {
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

        bool isFiltering = (State == MANAGEBOOKS_Author     && selectedAuthor[0] != '\0') ||
                           (State == MANAGEBOOKS_Publisher  && selectedPublisher[0] != '\0') ||
                           (State == MANAGEBOOKS_Type       && selectedType[0] != '\0') ||
                           (State == MANAGEBOOKS_Find       && FindBar.text[0] != '\0');

        float btnHeight = customUI.HeaderBox.height * 0.6f; 
        float marginX = customUI.HeaderBox.width * 0.02f;   
        int fontSizeBtn = (int)(btnHeight * 0.75f);          

        if (State == MANAGEBOOKS_Add || State == MANAGEBOOKS_Detail || isFiltering) {
            float btnWidthBack = btnHeight * 2.5f; 
            Rectangle btnBackHeader = {
                customUI.HeaderBox.x + marginX,
                customUI.HeaderBox.y + (customUI.HeaderBox.height - btnHeight) / 2.0f,
                btnWidthBack, btnHeight
            };
            
            bool hoverBack = CheckCollisionPointRec(ManageBooksSize.Mouse, btnBackHeader);
            DrawRectangleRounded(btnBackHeader, 0.2f, 10, hoverBack ? MAROON : ERRORRED);
            
            const char* textBack = "Back";
            Vector2 sizeBack = MeasureTextEx(_Font[0], textBack, fontSizeBtn, 1);
            Vector2 posBack = {
                btnBackHeader.x + (btnBackHeader.width - sizeBack.x) / 2.0f,
                btnBackHeader.y + (btnBackHeader.height - sizeBack.y) / 2.0f
            };
            DrawTextEx(_Font[0], textBack, posBack, fontSizeBtn, 1, WHITE);
            
            if (hoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (State == MANAGEBOOKS_Add || State == MANAGEBOOKS_Detail) {
                    State = prevState;
                } else {
                    if (State == MANAGEBOOKS_Author) selectedAuthor[0] = '\0';
                    else if (State == MANAGEBOOKS_Publisher) selectedPublisher[0] = '\0';
                    else if (State == MANAGEBOOKS_Type) selectedType[0] = '\0';
                    else if (State == MANAGEBOOKS_Find) {
                        FindBar.text[0] = '\0';
                        FindBar.length = 0;
                        State = MANAGEBOOKS_Dashboard;
                    }
                    wheel = 0;
                    Books->currentPage = 1;
                }
            }
        }
        
        if (_role == ADMINISTRATOR && State != MANAGEBOOKS_Add && State != MANAGEBOOKS_Detail) {
            float btnWidthAdd = btnHeight * 4.5f; 
            Rectangle btnAddHeader = {
                customUI.HeaderBox.x + customUI.HeaderBox.width - btnWidthAdd - marginX,
                customUI.HeaderBox.y + (customUI.HeaderBox.height - btnHeight) / 2.0f,
                btnWidthAdd, btnHeight
            };
            
            bool hoverAdd = CheckCollisionPointRec(ManageBooksSize.Mouse, btnAddHeader);
            DrawRectangleRounded(btnAddHeader, 0.2f, 10, hoverAdd ? TEALBLUE : STEELBLUE);
            
            const char* textAdd = "Thêm Truyện";
            Vector2 sizeAdd = MeasureTextEx(_Font[3], textAdd, fontSizeBtn, 1);
            Vector2 posAdd = {
                btnAddHeader.x + (btnAddHeader.width - sizeAdd.x) / 2.0f,
                btnAddHeader.y + (btnAddHeader.height - sizeAdd.y) / 2.0f
            };
            DrawTextEx(_Font[3], textAdd, posAdd, fontSizeBtn, 1, WHITE);
            
            if (hoverAdd && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                for(int i = 0; i < 8; i++) { 
                    addInputs[i].text[0] = '\0'; 
                    addInputs[i].length = 0; 
                    addInputs[i].isFocused = false; 
                }
                
                int maxId = 0;
                for (int k = 0; k < Books->count; k++) {
                    if (Books->theArray[k].CodeBook[0] == 'T' || Books->theArray[k].CodeBook[0] == 't') {
                        int currentId = atoi(&Books->theArray[k].CodeBook[1]);
                        if (currentId > maxId) maxId = currentId;
                    }
                }
                sprintf(addInputs[0].text, "T%03d", maxId + 1);
                addInputs[0].length = strlen(addInputs[0].text);

                FindBar.isFocused = false;
                prevState = State;
                State = MANAGEBOOKS_Add;
            }
        
            if (requestDeleteIndex != -1) {
                char authorToCheck[256];
                strcpy(authorToCheck, Books->theArray[requestDeleteIndex].AuthorBook);

                // ==========================================
                // ĐOẠN CODE THÊM VÀO ĐỂ XÓA ẢNH TRUYỆN
                // ==========================================
                char imgPath[512];
                // Định dạng đường dẫn file ảnh dựa trên CodeBook của truyện sắp xóa
                sprintf(imgPath, "img/img_books/%s.jpg", Books->theArray[requestDeleteIndex].CodeBook);
                
                // Tiến hành xóa file ảnh vật lý trên ổ đĩa
                if (remove(imgPath) == 0) {
                    printf("Xoa anh thanh cong: %s\n", imgPath);
                } else {
                    printf("Khong the xoa anh (file khong ton tai hoac dang bi khoa): %s\n", imgPath);
                }
                
                for (int k = requestDeleteIndex; k < Books->count - 1; k++) {
                    Books->theArray[k] = Books->theArray[k+1];
                }
                Books->count--;
                Savebooks(*Books); 
                requestDeleteIndex = -1;

                bool hasBooksLeft = false;
                for (int i = 0; i < Books->count; i++) {
                    if (strcmp(Books->theArray[i].AuthorBook, authorToCheck) == 0) {
                        hasBooksLeft = true; 
                        break;
                    }
                }

                if (!hasBooksLeft) {
                    for (int i = 0; i < Authors->count; i++) {
                        if (strcmp(Authors->Author[i].name, authorToCheck) == 0) {
                            for (int j = i; j < Authors->count - 1; j++) {
                                Authors->Author[j] = Authors->Author[j+1];
                            }
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

        switch(State)
        {
            case MANAGEBOOKS_Dashboard:
                totalMatch = CountStrInBooks(Books, NULL, IDLE_STATEFINDBOOK);
                Books->totalPages = (int)ceil((float)totalMatch / Books->QuantityForOnePage);
                if (Books->totalPages < 1) Books->totalPages = 1;
                
                clickedBook = ShowBooks_Panel(ManageBooksSize, customUI, &wheel, _Font, Books, 0, NULL, _role, &showPagination, &requestDeleteIndex);
                if (clickedBook != -1) { 
                    selectedBookIndex = clickedBook; 
                    prevState = State; 
                    State = MANAGEBOOKS_Detail; 
                }
                
                DrawPagination(customUI, Books, _Font, ManageBooksSize, &wheel, NULL, showPagination);
                break;

            case MANAGEBOOKS_Author:
                if (selectedAuthor[0] == '\0') {
                    ShowAuthor_Panel(ManageBooksSize, customUI, &wheel, _Font, Authors, selectedAuthor, Books);
                } else {
                    totalMatch = CountStrInBooks(Books, selectedAuthor, AUTHOR);
                    Books->totalPages = (int)ceil((float)totalMatch / Books->QuantityForOnePage);
                    if (Books->totalPages < 1) Books->totalPages = 1;
                    
                    clickedBook = ShowBooks_Panel(ManageBooksSize, customUI, &wheel, _Font, Books, 1, selectedAuthor, _role, &showPagination, &requestDeleteIndex);
                    if (clickedBook != -1) { 
                        selectedBookIndex = clickedBook; 
                        prevState = State; 
                        State = MANAGEBOOKS_Detail; 
                    }

                    DrawPagination(customUI, Books, _Font, ManageBooksSize, &wheel, NULL, showPagination);
                }
                break;

            case MANAGEBOOKS_Publisher:
                if (selectedPublisher[0] == '\0') {
                    ShowPublisher_Panel(ManageBooksSize, customUI, &wheel, _Font, Publishers, selectedPublisher, Books);
                } else {
                    totalMatch = CountStrInBooks(Books, selectedPublisher, PUBLISHER);
                    Books->totalPages = (int)ceil((float)totalMatch / Books->QuantityForOnePage);
                    if (Books->totalPages < 1) Books->totalPages = 1;
                    
                    clickedBook = ShowBooks_Panel(ManageBooksSize, customUI, &wheel, _Font, Books, 3, selectedPublisher, _role, &showPagination, &requestDeleteIndex);
                    if (clickedBook != -1) { 
                        selectedBookIndex = clickedBook; 
                        prevState = State; 
                        State = MANAGEBOOKS_Detail; 
                    }
                    DrawPagination(customUI, Books, _Font, ManageBooksSize, &wheel, NULL, showPagination);
                }
                break;

            case MANAGEBOOKS_Type:
                if (selectedType[0] == '\0') {
                    ShowType_Panel(ManageBooksSize, customUI, &wheel, _Font, Types, selectedType, Books);
                } else {
                    totalMatch = CountStrInBooks(Books, selectedType, TYPE);
                    Books->totalPages = (int)ceil((float)totalMatch / Books->QuantityForOnePage);
                    if (Books->totalPages < 1) Books->totalPages = 1;
                    
                    clickedBook = ShowBooks_Panel(ManageBooksSize, customUI, &wheel, _Font, Books, 2, selectedType, _role, &showPagination, &requestDeleteIndex);
                    if (clickedBook != -1) { 
                        selectedBookIndex = clickedBook; 
                        prevState = State; 
                        State = MANAGEBOOKS_Detail; 
                    }
                    DrawPagination(customUI, Books, _Font, ManageBooksSize, &wheel, NULL, showPagination);
                }
                break;

            case MANAGEBOOKS_Detail:
                if (selectedBookIndex >= 0 && selectedBookIndex < Books->count) {
                    ShowBookDetail_Panel(ManageBooksSize, customUI, _Font, Books->theArray[selectedBookIndex], &State, prevState);
                }
                break;

            case MANAGEBOOKS_Add:
            {
                int addStatus = ShowAddBook_Panel(ManageBooksSize, customUI, _Font, addInputs);
                if (addStatus == 1) { 
                    if (Books->count < Books->capacity) {
                        Book newBook = {0};
                        strcpy(newBook.CodeBook, addInputs[0].text);
                        strcpy(newBook.NameBook, addInputs[1].text);
                        strcpy(newBook.NormNameBook, addInputs[1].text);
                        strcpy(newBook.AuthorBook, addInputs[2].text);
                        strcpy(newBook.TypeBook, addInputs[3].text);
                        strcpy(newBook.PublisherBook, addInputs[4].text);
                        newBook.YearBook = atoi(addInputs[5].text);
                        newBook.PriceBook = atoi(addInputs[6].text);
                        
                        strcpy(newBook.Description, addInputs[7].text);
                        
                        newBook.StockBook = 100;
                        newBook.TotalImportBook = 100;
                        newBook.TotalBorrowBook = 0;

                        Books->theArray[Books->count] = newBook;
                        Books->count++;
                        Savebooks(*Books);
                    }
                    State = prevState;
                }
                break;
            }

            case MANAGEBOOKS_Find:
                totalMatch = CountStrInBooks(Books, FindBar.text, SEARCH_ALL);
                Books->totalPages = (int)ceil((float)totalMatch / Books->QuantityForOnePage);
                if (Books->totalPages < 1) Books->totalPages = 1;
                
                clickedBook = ShowBooks_Panel(ManageBooksSize, customUI, &wheel, _Font, Books, 4, FindBar.text, _role, &showPagination, &requestDeleteIndex);
                
                if (clickedBook != -1) { 
                    selectedBookIndex = clickedBook; 
                    prevState = State; 
                    State = MANAGEBOOKS_Detail; 
                }
                
                DrawPagination(customUI, Books, _Font, ManageBooksSize, &wheel, NULL, showPagination);
                break;

            case MANAGEBOOKS_Main:
                break;
        }
        
        EndDrawing();
        if (State == MANAGEBOOKS_Main) break;
    }

    for (int i = 0; i < 4; i++) UnloadFont(_Font[i]);
    UnloadTexture(Avatar);
    UnloadTexture(Icon_Find);

    Savebooks(*Books);
    
    if (Books->theArray != NULL) free(Books->theArray);
    free(Books);
    if (Authors->Author != NULL) free(Authors->Author);
    free(Authors);

    if (Types != NULL) {
        for (int i = 0; i < Types->count; i++) free(Types->Type[i]);
        free(Types->Type);
        free(Types);
    }
    
    if (Publishers != NULL) {
        for (int i = 0; i < Publishers->count; i++) free(Publishers->Publisher[i]);
        free(Publishers->Publisher);
        free(Publishers);
    }
}

// =========================================================================
// CÁC PANEL HIỂN THỊ CHI TIẾT VÀ DANH SÁCH
// =========================================================================

void ShowBookDetail_Panel(Size size, ManageBooksUI UI, Font *_Font, Book book, MANAGEBOOKS_STATE *State, MANAGEBOOKS_STATE prevState) {
    Rectangle detailArea = { UI.Panel.x, UI.Panel.y, UI.Panel.width, UI.Panel.height };
    Color bgColor = (Color){ 30, 30, 30, 255 }; 
    DrawRectangleRec(detailArea, bgColor);

    static Texture2D currentCover = { 0 };
    static char currentLoadedCode[256] = "";

    if (strcmp(book.CodeBook, currentLoadedCode) != 0) {
        if (currentCover.id != 0) {
            UnloadTexture(currentCover);
            currentCover.id = 0;
        }
        char imgPath[512];
        sprintf(imgPath, "img/img_books/%s.jpg", book.CodeBook);
        if (FileExists(imgPath)) {
            currentCover = LoadTexture(imgPath);
        }
        strcpy(currentLoadedCode, book.CodeBook);
    }

    float leftX = detailArea.x + 50.0f;
    float startY = detailArea.y + 50.0f;
    
    float targetW = 220.0f;
    float targetH = 300.0f;
    Rectangle destRec = { leftX, startY, targetW, targetH };

    if (currentCover.id != 0 && currentCover.width > 0 && currentCover.height > 0) {
        float scaleX = targetW / (float)currentCover.width;
        float scaleY = targetH / (float)currentCover.height;
        float scale = (scaleX > scaleY) ? scaleX : scaleY; 

        float sourceW = targetW / scale;
        float sourceH = targetH / scale;
        float sourceX = ((float)currentCover.width - sourceW) / 2.0f;
        float sourceY = ((float)currentCover.height - sourceH) / 2.0f;

        Rectangle sourceRec = { sourceX, sourceY, sourceW, sourceH };
        DrawTexturePro(currentCover, sourceRec, destRec, (Vector2){0,0}, 0.0f, WHITE);
    } else {
        DrawRectangleRec(destRec, DARKGRAY);
        DrawTextEx(_Font[0], "NO IMAGE", (Vector2){ leftX + 45, startY + 140 }, 25, 1, LIGHTGRAY);
    }
    DrawRectangleLinesEx(destRec, 2.0f, DARKGRAY);

    float rightX = leftX + targetW + 50.0f;
    float currentY = startY;

    DrawTextEx(_Font[0], book.NameBook, (Vector2){ rightX, currentY }, 38, 1, WHITE);
    currentY += 55;

    int fontSizeInfo = 22;
    int spacingY = 38;
    Color labelCol = GRAY;
    Color valueCol = LIGHTGRAY;
    char buffer[256];

    DrawTextEx(_Font[0], "+ Tên khác:", (Vector2){ rightX, currentY }, fontSizeInfo, 1, labelCol);
    DrawTextEx(_Font[0], book.NormNameBook, (Vector2){ rightX + 250, currentY }, fontSizeInfo, 1, valueCol);
    currentY += spacingY;

    DrawTextEx(_Font[0], "+ Tác giả:", (Vector2){ rightX, currentY }, fontSizeInfo, 1, labelCol);
    DrawTextEx(_Font[0], book.AuthorBook, (Vector2){ rightX + 250, currentY }, fontSizeInfo, 1, valueCol);
    currentY += spacingY;

    DrawTextEx(_Font[0], "+ Thể loại:", (Vector2){ rightX, currentY }, fontSizeInfo, 1, labelCol);
    DrawTextEx(_Font[0], book.TypeBook, (Vector2){ rightX + 250, currentY }, fontSizeInfo, 1, (Color){ 255, 165, 0, 255 }); 
    currentY += spacingY;
    
    DrawTextEx(_Font[0], "+ Năm Sản xuất:", (Vector2){ rightX, currentY }, fontSizeInfo, 1, labelCol);
    sprintf(buffer, "%d", book.YearBook);
    DrawTextEx(_Font[0], buffer, (Vector2){ rightX + 250, currentY }, fontSizeInfo, 1, valueCol);
    currentY += spacingY;

    DrawTextEx(_Font[0], "+ Số lượt xem:", (Vector2){ rightX, currentY }, fontSizeInfo, 1, labelCol);
    sprintf(buffer, "%d", book.TotalBorrowBook * 135); 
    DrawTextEx(_Font[0], buffer, (Vector2){ rightX + 250, currentY }, fontSizeInfo, 1, valueCol);
    currentY += spacingY;

    DrawTextEx(_Font[0], "+ Giá thuê (VND/ngày):", (Vector2){ rightX, currentY }, fontSizeInfo, 1, labelCol);
    FormatPriceToVND(book.PriceBook, buffer);
    DrawTextEx(_Font[0], buffer, (Vector2){ rightX + 250, currentY }, fontSizeInfo, 1, valueCol);
    currentY += spacingY;

    float introY = currentY + 30;
    DrawTextEx(_Font[0], "GIỚI THIỆU", (Vector2){ rightX, introY }, 26, 1, ORANGE);
    DrawLineEx((Vector2){rightX, introY + 35}, (Vector2){detailArea.x + detailArea.width - 50, introY + 35}, 2.0f, DARKGRAY);
    
    Rectangle descHitbox = { rightX, introY + 50, detailArea.width - rightX - 50, detailArea.height - (introY + 60) };
    const char *descText = (book.Description[0] != '\0') ? book.Description : "Truyện này chưa có nội dung giới thiệu.";
    DrawTextAutoWrap(_Font[0], descText, descHitbox, 20.0f, 1.0f, valueCol);
}

int ShowAddBook_Panel(Size size, ManageBooksUI UI, Font *_Font, InputBox *inputs) {
    int result = 0; 
    Rectangle panelArea = { 
        UI.Panel.x, 
        UI.Panel.y, 
        UI.Panel.width, 
        UI.Panel.height 
    };
    DrawRectangleRec(panelArea, (Color){ 30, 30, 30, 255 });

    // Biến static để lưu trạng thái lỗi (có hiển thị cảnh báo hay không)
    static bool showValidationError = false;

    float titleFontSize = panelArea.height * 0.045f;
    if (titleFontSize < 25.0f) titleFontSize = 25.0f;
    if (titleFontSize > 45.0f) titleFontSize = 45.0f;

    DrawTextEx(_Font[0], "THÊM TRUYỆN MỚI", (Vector2){panelArea.x + panelArea.width * 0.05f, panelArea.y + panelArea.height * 0.03f}, titleFontSize, 1, GOLDACCENT);
    DrawLineEx((Vector2){panelArea.x + panelArea.width * 0.05f, panelArea.y + panelArea.height * 0.09f}, 
               (Vector2){panelArea.x + panelArea.width * 0.95f, panelArea.y + panelArea.height * 0.09f}, 3, GRAY);

    const char *labels[] = {
        "Mã Truyện (Tự động):", "Tên Truyện:", "Tác Giả:", "Thể Loại:", 
        "Nhà Xuất Bản:", "Năm Sản Xuất:", "Giá thuê (VND/ngày):"
    };
    
    float widthRatio = 0.95f - 0.3f * (panelArea.width / 2500.0f);
    if (widthRatio < 0.5f) widthRatio = 0.5f;
    if (widthRatio > 0.9f) widthRatio = 0.9f;
    
    float contentWidth = panelArea.width * widthRatio;
    float offsetX = panelArea.x + (panelArea.width - contentWidth) / 2.0f; 
    float colGap = contentWidth * 0.06f; 
    float colWidth = (contentWidth - colGap) / 2.0f;
    
    float startY = panelArea.y + panelArea.height * 0.13f;
    float rowSpacing = panelArea.height * 0.11f; 
    
    float boxHeight = panelArea.height * 0.055f;
    if (boxHeight < 35.0f) boxHeight = 35.0f; 
    if (boxHeight > 55.0f) boxHeight = 55.0f; 

    float labelFontSize = boxHeight * 0.45f;
    if (labelFontSize < 18.0f) labelFontSize = 18.0f;
    float inputTextSize = labelFontSize;

    bool clickedAnywhere = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    // Ẩn thông báo lỗi nếu người dùng click để sửa dữ liệu
    if (clickedAnywhere) {
        showValidationError = false;
    }

    for (int i = 0; i < 8; i++) {
        if (i < 7) {
            int row = i / 2;
            int col = i % 2;
            float xPos = offsetX + col * (colWidth + colGap);
            float yPos = startY + row * rowSpacing;

            DrawTextEx(_Font[0], labels[i], (Vector2){xPos, yPos}, labelFontSize, 1, LIGHTGRAY);
            inputs[i].box = (Rectangle){ xPos, yPos + labelFontSize + 6.0f, colWidth, boxHeight };
        } else {
            float lastRowBottom = startY + 3 * rowSpacing + labelFontSize + 6.0f + boxHeight;
            float descY = lastRowBottom + panelArea.height * 0.02f; 
            float buttonAreaTop = panelArea.y + panelArea.height * 0.88f; 
            float descBoxHeight = buttonAreaTop - descY - labelFontSize - 20.0f; 
            
            if (descBoxHeight < boxHeight * 1.5f) descBoxHeight = boxHeight * 1.5f;

            DrawTextEx(_Font[0], "Mô Tả Truyện:", (Vector2){offsetX, descY}, labelFontSize + 2.0f, 1, ORANGE);
            inputs[i].box = (Rectangle){ offsetX, descY + labelFontSize + 8.0f, contentWidth, descBoxHeight };
        }

        if (clickedAnywhere) {
            if (i == 0) inputs[i].isFocused = false;
            else inputs[i].isFocused = CheckCollisionPointRec(size.Mouse, inputs[i].box);
        }

        float roundness = (i == 7) ? 0.05f : 0.2f; 

        Color boxBgColor;
        Color boxOutlineColor;
        float outlineThick = 2.0f;

        if (i == 0) {
            boxBgColor = Fade(DARKGRAY, 0.4f);
            boxOutlineColor = GRAY;
        } else {
            bool hasText = (strlen(inputs[i].text) > 0);
            // Bôi đỏ viền nếu đang báo lỗi và ô này đang trống
            bool isErrorBox = (showValidationError && !hasText);
            
            boxBgColor = (inputs[i].isFocused || hasText) ? WHITESMOKE : GRAY;
            boxOutlineColor = isErrorBox ? RED : (inputs[i].isFocused ? TEALBLUE : DARKGRAY);
            outlineThick = inputs[i].isFocused ? 3.0f : (isErrorBox ? 2.5f : 2.0f);
        }

        DrawRectangleRounded(inputs[i].box, roundness, 10, boxBgColor);
        DrawRectangleRoundedLinesEx(inputs[i].box, roundness, 10, outlineThick, boxOutlineColor);

        if (inputs[i].isFocused && i != 0) {
            UpdateInputBox(&inputs[i]);
            if (IsKeyPressed(KEY_TAB)) {
                inputs[i].isFocused = false;
                if (i < 7) inputs[i+1].isFocused = true;
                else inputs[1].isFocused = true; 
            }
        }

        if (i < 7) {
            BeginScissorMode((int)inputs[i].box.x + 10, (int)inputs[i].box.y, (int)inputs[i].box.width - 20, (int)inputs[i].box.height);
            float textW = MeasureTextEx(_Font[3], inputs[i].text, inputTextSize, 1).x;
            float textX = inputs[i].box.x + 12;
            if (textW > inputs[i].box.width - 24) textX = inputs[i].box.x + 12 - (textW - (inputs[i].box.width - 24));
            
            float textY = inputs[i].box.y + (inputs[i].box.height - inputTextSize) / 2.0f;
            Color textColor = (i == 0) ? ORANGE : BLACK;
            DrawTextEx(_Font[3], inputs[i].text, (Vector2){textX, textY}, inputTextSize, 1, textColor);
            
            if (inputs[i].isFocused && ((int)(GetTime() * 2) % 2 == 0)) {
                DrawRectangle(textX + textW + 2, textY, 2, inputTextSize + 2, BLACK);
            }
            EndScissorMode();
        } else {
            BeginScissorMode((int)inputs[i].box.x, (int)inputs[i].box.y, (int)inputs[i].box.width, (int)inputs[i].box.height);
            char tempText[8192]; 
            strcpy(tempText, inputs[i].text);
            if (inputs[i].isFocused && ((int)(GetTime() * 2) % 2 == 0)) strcat(tempText, "|");
            
            Rectangle textHitbox = { inputs[i].box.x + 15, inputs[i].box.y + 15, inputs[i].box.width - 30, inputs[i].box.height - 30 };
            DrawTextAutoWrap(_Font[3], tempText, textHitbox, inputTextSize, 1.0f, BLACK);
            EndScissorMode();
        }
    }

    float btnHeight = panelArea.height * 0.07f;
    if (btnHeight < 40.0f) btnHeight = 40.0f;
    if (btnHeight > 60.0f) btnHeight = 60.0f;
    float btnWidth = panelArea.width * 0.25f;
    if (btnWidth < 200.0f) btnWidth = 200.0f;
    if (btnWidth > 350.0f) btnWidth = 350.0f;

    Rectangle btnSave = { 
        panelArea.x + (panelArea.width - btnWidth) / 2.0f, 
        panelArea.y + panelArea.height - btnHeight - panelArea.height * 0.04f, 
        btnWidth, 
        btnHeight 
    };

    bool hoverSave = CheckCollisionPointRec(size.Mouse, btnSave);
    DrawRectangleRounded(btnSave, 0.3f, 10, hoverSave ? LIME : SUCCESSGREEN);
    
    float wSave = MeasureTextEx(_Font[0], "LƯU TRUYỆN", inputTextSize + 2.0f, 1).x;
    DrawTextEx(_Font[0], "LƯU TRUYỆN", (Vector2){btnSave.x + (btnSave.width - wSave)/2, btnSave.y + (btnSave.height - (inputTextSize + 2.0f))/2}, inputTextSize + 2.0f, 1, WHITE);
    
    // --- BỘ XÁC MINH DỮ LIỆU ---
    if (hoverSave && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        bool isValid = true;
        
        // Quét từ ô 1 (Tên truyện) đến ô 7 (Mô tả)
        for (int i = 1; i < 8; i++) {
            bool hasValidChar = false;
            
            // Kiểm tra xem chuỗi có ký tự nào khác khoảng trắng/xuống dòng không
            for (int j = 0; j < inputs[i].length; j++) {
                if (inputs[i].text[j] != ' ' && inputs[i].text[j] != '\n') {
                    hasValidChar = true;
                    break;
                }
            }
            
            // Nếu phát hiện có ô trống hoặc chỉ chứa khoảng trắng
            if (!hasValidChar) {
                isValid = false;
                break; 
            }
        }

        if (isValid) {
            showValidationError = false;
            result = 1; // Cho phép Lưu
        } else {
            showValidationError = true; // Kích hoạt hiển thị cảnh báo
        }
    }

    // --- HIỂN THỊ DÒNG CHỮ CẢNH BÁO ---
    if (showValidationError) {
        float distance = panelArea.y + panelArea.height - btnSave.y - btnSave.height;
        const char *errorMsg = "* Lỗi: Vui lòng nhập đầy đủ thông tin vào tất cả các ô!";
        float errFontSize = distance * 0.65f;
        float errWidth = MeasureTextEx(_Font[0], errorMsg, errFontSize, 1).x;
        Vector2 errPos = { 
            panelArea.x + (panelArea.width - errWidth) / 2.0f,
            btnSave.y + btnSave.height + distance * 0.2f
        };
        DrawTextEx(_Font[0], errorMsg, errPos, errFontSize, 1, RED);
    }

    return result;
}

int ShowBooks_Panel(Size size, ManageBooksUI UI, float *wheel, Font *_Font, BookList *Books, int filterMode, const char *filterValue, Role _role, bool *showPagination, int *requestDeleteIndex) {
    int clickedIndex = -1;
    bool isMouseInPanel = CheckCollisionPointRec(size.Mouse, UI.Panel);
    
    Rectangle hitbox = { 
        UI.Panel.x + UI.Panel.width * 0.05f, 
        *wheel + UI.Panel.y + UI.Panel.height * 0.03f, 
        UI.Panel.width * 0.9f, 
        UI.Panel.height * 0.08f * (pow(0.3, size.Scale.x) + 0.6) 
    };

    float Distance_Books = DISTANCE_BOOKS;
    int startIndex = (Books->currentPage - 1) * Books->QuantityForOnePage;
    int matchCount = 0;
    int displayCount = 0;
    int renderList[QUANTITYFORONEPAGE];

    for (int i = 0; i < Books->count && displayCount < Books->QuantityForOnePage; i++) {
        int flag = 0;
        
        if (filterMode == 1) { 
            if (filterValue != NULL && strcmp(filterValue, Books->theArray[i].AuthorBook) == 0) flag = 1; 
        } else if (filterMode == 2) { 
            if (filterValue != NULL && strcmp(filterValue, Books->theArray[i].TypeBook) == 0) flag = 1; 
        } else if (filterMode == 3) { 
            if (filterValue != NULL && strcmp(filterValue, Books->theArray[i].PublisherBook) == 0) flag = 1; 
        } else if (filterMode == 4) {
            if (ContextualBookSearch(Books->theArray[i], filterValue)) {
                flag = 1;
            }
        } else {
            flag = 1; 
        }

        if (flag) {
            if (matchCount >= startIndex) {
                renderList[displayCount] = i;
                displayCount++;
            }
            matchCount++;
        }
    }

    float bottomPadding = UI.PaginationBox.height + 20.0f;
    float TotalContentHeight = (hitbox.height + Distance_Books) * displayCount + bottomPadding;
    
    if (TotalContentHeight > UI.Panel.height) {
        float minWheel = UI.Panel.height - TotalContentHeight;
        if (GetMouseWheelMove() == 1) *wheel += 40.0f;
        else if (GetMouseWheelMove() == -1) *wheel -= 40.0f;
        if (*wheel > 0) *wheel = 0;
        if (*wheel < minWheel) *wheel = minWheel;
        
        if (showPagination != NULL) *showPagination = (*wheel <= minWheel + 5.0f);
    } else {
        *wheel = 0;
        if (showPagination != NULL) *showPagination = true;
    }

    BeginScissorMode(UI.Panel.x, UI.Panel.y, UI.Panel.width, UI.Panel.height);

    for (int idx = 0; idx < displayCount; idx++) {
        int i = renderList[idx];
        hitbox.y = *wheel + UI.Panel.y + UI.Panel.height * 0.03f + (hitbox.height + Distance_Books) * idx;

        bool isHoveredBook = CheckCollisionPointRec(size.Mouse, hitbox) && isMouseInPanel;
        float radius = FindRoundness(hitbox.width * 0.005f, hitbox.width, hitbox.height);
        
        DrawRectangleRounded(hitbox, radius, 50, isHoveredBook ? Fade(BLACK, 0.8f) : BLACK);

        float padding = hitbox.width * 0.02f;
        float rightAnchor = hitbox.x + hitbox.width - padding;
        bool clickedTrash = false;
        
        if (_role == ADMINISTRATOR) {
            float trashSize = hitbox.height * 0.6f;
            Rectangle trashBox = { rightAnchor - trashSize, hitbox.y + (hitbox.height - trashSize) / 2, trashSize, trashSize };
            
            if (CheckCollisionPointRec(size.Mouse, trashBox) && isMouseInPanel) {
                clickedTrash = true;
                DrawRectangleRounded(trashBox, 0.2f, 10, MAROON);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) *requestDeleteIndex = i;
            } else {
                DrawRectangleRounded(trashBox, 0.2f, 10, ERRORRED);
            }
            DrawLineEx((Vector2){trashBox.x + trashSize*0.25f, trashBox.y + trashSize*0.25f}, (Vector2){trashBox.x + trashSize*0.75f, trashBox.y + trashSize*0.75f}, 3.0f, WHITE);
            DrawLineEx((Vector2){trashBox.x + trashSize*0.75f, trashBox.y + trashSize*0.25f}, (Vector2){trashBox.x + trashSize*0.25f, trashBox.y + trashSize*0.75f}, 3.0f, WHITE);
            rightAnchor = trashBox.x - padding;
        }

        if (isHoveredBook && !clickedTrash && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) clickedIndex = i;

        int PriceFontSize = hitbox.height * 0.45f;
        char priceStr[32];
        FormatPriceToVND(Books->theArray[i].PriceBook, priceStr);
        float priceWidth = MeasureTextEx(_Font[0], priceStr, PriceFontSize, 1).x;
        
        Vector2 PricePos = { rightAnchor - priceWidth, hitbox.y + (hitbox.height - PriceFontSize) / 2 };
        DrawTextEx(_Font[0], priceStr, PricePos, PriceFontSize, 1, GOLDACCENT);

        rightAnchor = PricePos.x - padding;

        Rectangle code_hitbox = { hitbox.x, hitbox.y, hitbox.width * 0.12f, hitbox.height };
        radius = FindRoundness(hitbox.width * 0.005f, code_hitbox.width, code_hitbox.height);
        DrawRectangleRounded(code_hitbox, radius, 50, WHITE);
        
        int CodeFontSize = FindFontSize(code_hitbox.width * 0.8f, _Font[0], 1, Books->theArray[i].CodeBook);
        DrawTextEx(_Font[0], Books->theArray[i].CodeBook, (Vector2) { code_hitbox.x + (code_hitbox.width - MeasureTextEx(_Font[0], Books->theArray[i].CodeBook, CodeFontSize, 1).x)/2, code_hitbox.y + (code_hitbox.height - CodeFontSize) / 2 }, CodeFontSize, 1, BLACK);
        
        int NameFontSize = hitbox.height * 0.6f;
        Vector2 NamePos = { code_hitbox.x + code_hitbox.width + padding, hitbox.y + (hitbox.height - NameFontSize) / 2 };
        
        float nameWidth = MeasureTextEx(_Font[0], Books->theArray[i].NameBook, NameFontSize, 1).x;
        int AuthorFontSize = NameFontSize * 0.65f;
        Vector2 AuthorPos = { NamePos.x + nameWidth + padding, hitbox.y + (hitbox.height - AuthorFontSize) / 2 + NameFontSize*0.1f };
        
        float availableWidthText = rightAnchor - NamePos.x;
        Rectangle textRect = { NamePos.x, hitbox.y, availableWidthText, hitbox.height };
        Rectangle panelRect = { UI.Panel.x, UI.Panel.y, UI.Panel.width, UI.Panel.height };
        Rectangle clipRect = GetCollisionRec(textRect, panelRect);

        BeginScissorMode((int)clipRect.x, (int)clipRect.y, (int)clipRect.width, (int)clipRect.height);
        DrawTextEx(_Font[0], Books->theArray[i].NameBook, NamePos, NameFontSize, 1, isHoveredBook ? TEALBLUE : WHITE);
        DrawTextEx(_Font[3], TextFormat("- %s", Books->theArray[i].AuthorBook), AuthorPos, AuthorFontSize, 1, Fade(WHITE, 0.5f));
        EndScissorMode(); 

        BeginScissorMode((int)UI.Panel.x, (int)UI.Panel.y, (int)UI.Panel.width, (int)UI.Panel.height);
    }

    if (TotalContentHeight > UI.Panel.height) {
        float scrollTrackWidth = 12.0f;
        Rectangle scrollTrack = { UI.Panel.x + UI.Panel.width - scrollTrackWidth - 5, UI.Panel.y + 5, scrollTrackWidth, UI.Panel.height - 10 };
        DrawRectangleRounded(scrollTrack, 1.0f, 10, Fade(GRAY, 0.3f));

        float handleHeight = (UI.Panel.height / TotalContentHeight) * scrollTrack.height;
        if (handleHeight < 30.0f) handleHeight = 30.0f; 
        
        float scrollableWheel = TotalContentHeight - UI.Panel.height;
        float scrollRatio = -(*wheel) / scrollableWheel;
        float handleY = scrollTrack.y + scrollRatio * (scrollTrack.height - handleHeight);
        
        Rectangle scrollHandle = { scrollTrack.x, handleY, scrollTrackWidth, handleHeight };

        static bool isDraggingBooks = false;
        static float dragOffsetBooksY = 0.0f;
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(size.Mouse, scrollHandle)) {
            isDraggingBooks = true;
            dragOffsetBooksY = size.Mouse.y - scrollHandle.y;
        }
        
        if (isDraggingBooks) {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                float newHandleY = size.Mouse.y - dragOffsetBooksY;
                if (newHandleY < scrollTrack.y) newHandleY = scrollTrack.y;
                if (newHandleY > scrollTrack.y + scrollTrack.height - handleHeight) newHandleY = scrollTrack.y + scrollTrack.height - handleHeight;
                
                float newRatio = (newHandleY - scrollTrack.y) / (scrollTrack.height - handleHeight);
                *wheel = -(newRatio * scrollableWheel);
                scrollHandle.y = newHandleY; 
            } else {
                isDraggingBooks = false;
            }
        }
        
        bool isHoveredScroll = CheckCollisionPointRec(size.Mouse, scrollHandle) || isDraggingBooks;
        DrawRectangleRounded(scrollHandle, 1.0f, 10, isHoveredScroll ? GRAY : Fade(DARKGRAY, 0.8f));
    }

    EndScissorMode();
    return clickedIndex;
}

void ShowAuthor_Panel(Size size, ManageBooksUI UI, float *wheel, Font *_Font, AuthorList *Authors, char *selectedAuthor, BookList *Books) {
    bool isMouseInPanel = CheckCollisionPointRec(size.Mouse, UI.Panel);
    float itemHeight = 60.0f;
    float distance = 15.0f;
    int displayCount = Authors->count;
    float TotalContentHeight = (itemHeight + distance) * displayCount;

    if (TotalContentHeight > UI.Panel.height) {
        if (GetMouseWheelMove() == 1) *wheel += 40.0f;
        else if (GetMouseWheelMove() == -1) *wheel -= 40.0f;
        if (*wheel > 0) *wheel = 0;
        if (*wheel < UI.Panel.height - TotalContentHeight - distance) *wheel = UI.Panel.height - TotalContentHeight - distance;
    } else *wheel = 0;

    BeginScissorMode(UI.Panel.x, UI.Panel.y, UI.Panel.width, UI.Panel.height);

    for (int i = 0; i < displayCount; i++) {
        Rectangle hitbox = { UI.Panel.x + UI.Panel.width * 0.08f, *wheel + UI.Panel.y + 20.0f + (itemHeight + distance) * i, UI.Panel.width * 0.84f, itemHeight };
        bool isHovered = CheckCollisionPointRec(size.Mouse, hitbox) && isMouseInPanel;
        DrawRectangleRounded(hitbox, 0.2f, 20, isHovered ? Fade(TEALBLUE, 0.8f) : Fade(BLACK, 0.7f));

        DrawTextEx(_Font[0], Authors->Author[i].code, (Vector2){hitbox.x + 20, hitbox.y + 15}, 30, 1, isHovered ? GOLDACCENT : LIGHTGRAY);
        DrawTextEx(_Font[0], Authors->Author[i].name, (Vector2){hitbox.x + 150, hitbox.y + 15}, 30, 1, WHITE);

        int bookCount = CountStrInBooks(Books, Authors->Author[i].name, AUTHOR);
        char countStr[32]; sprintf(countStr, "%d Truyện", bookCount);
        float countW = MeasureTextEx(_Font[0], countStr, 25, 1).x;
        DrawTextEx(_Font[0], countStr, (Vector2){hitbox.x + hitbox.width - countW - 20, hitbox.y + 18}, 25, 1, isHovered ? WHITE : GRAY);

        if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            strcpy(selectedAuthor, Authors->Author[i].name);
            *wheel = 0;
            Books->currentPage = 1;
        }
    }

    if (TotalContentHeight > UI.Panel.height) {
        float scrollTrackWidth = 12.0f;
        Rectangle scrollTrack = { UI.Panel.x + UI.Panel.width - scrollTrackWidth - 5, UI.Panel.y + 5, scrollTrackWidth, UI.Panel.height - 10 };
        DrawRectangleRounded(scrollTrack, 1.0f, 10, Fade(GRAY, 0.3f));
        
        float handleHeight = (UI.Panel.height / TotalContentHeight) * scrollTrack.height;
        if (handleHeight < 30.0f) handleHeight = 30.0f; 
        
        float scrollableWheel = TotalContentHeight + distance - UI.Panel.height;
        float scrollRatio = -(*wheel) / scrollableWheel;
        float handleY = scrollTrack.y + scrollRatio * (scrollTrack.height - handleHeight);
        
        Rectangle scrollHandle = { scrollTrack.x, handleY, scrollTrackWidth, handleHeight };

        static bool isDraggingAuthors = false;
        static float dragOffsetAuthorsY = 0.0f;
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(size.Mouse, scrollHandle)) {
            isDraggingAuthors = true;
            dragOffsetAuthorsY = size.Mouse.y - scrollHandle.y;
        }
        
        if (isDraggingAuthors) {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                float newHandleY = size.Mouse.y - dragOffsetAuthorsY;
                if (newHandleY < scrollTrack.y) newHandleY = scrollTrack.y;
                if (newHandleY > scrollTrack.y + scrollTrack.height - handleHeight) newHandleY = scrollTrack.y + scrollTrack.height - handleHeight;
                
                float newRatio = (newHandleY - scrollTrack.y) / (scrollTrack.height - handleHeight);
                *wheel = -(newRatio * scrollableWheel);
                scrollHandle.y = newHandleY; 
            } else isDraggingAuthors = false;
        }
        bool isHoveredScroll = CheckCollisionPointRec(size.Mouse, scrollHandle) || isDraggingAuthors;
        DrawRectangleRounded(scrollHandle, 1.0f, 10, isHoveredScroll ? GRAY : Fade(DARKGRAY, 0.8f));
    }
    EndScissorMode();
}

void ShowPublisher_Panel(Size size, ManageBooksUI UI, float *wheel, Font *_Font, PublisherList *Publishers, char *selectedPublisher, BookList *Books) {
    bool isMouseInPanel = CheckCollisionPointRec(size.Mouse, UI.Panel);
    float itemHeight = 60.0f;
    float distance = 15.0f;
    int displayCount = Publishers->count;
    float TotalContentHeight = (itemHeight + distance) * displayCount;

    if (TotalContentHeight > UI.Panel.height) {
        if (GetMouseWheelMove() == 1) *wheel += 40.0f;
        else if (GetMouseWheelMove() == -1) *wheel -= 40.0f;
        if (*wheel > 0) *wheel = 0;
        if (*wheel < UI.Panel.height - TotalContentHeight - distance) *wheel = UI.Panel.height - TotalContentHeight - distance;
    } else *wheel = 0;

    BeginScissorMode(UI.Panel.x, UI.Panel.y, UI.Panel.width, UI.Panel.height);

    for (int i = 0; i < displayCount; i++) {
        Rectangle hitbox = { UI.Panel.x + UI.Panel.width * 0.08f, *wheel + UI.Panel.y + 20.0f + (itemHeight + distance) * i, UI.Panel.width * 0.84f, itemHeight };
        bool isHovered = CheckCollisionPointRec(size.Mouse, hitbox) && isMouseInPanel;
        DrawRectangleRounded(hitbox, 0.2f, 20, isHovered ? Fade(TEALBLUE, 0.8f) : Fade(BLACK, 0.7f));

        DrawTextEx(_Font[0], Publishers->Publisher[i], (Vector2){hitbox.x + 30, hitbox.y + 15}, 30, 1, isHovered ? GOLDACCENT : WHITE);

        int bookCount = CountStrInBooks(Books, Publishers->Publisher[i], PUBLISHER);
        char countStr[32]; sprintf(countStr, "%d Truyện", bookCount);
        float countW = MeasureTextEx(_Font[0], countStr, 25, 1).x;
        DrawTextEx(_Font[0], countStr, (Vector2){hitbox.x + hitbox.width - countW - 20, hitbox.y + 18}, 25, 1, isHovered ? WHITE : GRAY);

        if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            strcpy(selectedPublisher, Publishers->Publisher[i]);
            *wheel = 0; Books->currentPage = 1;
        }
    }

    if (TotalContentHeight > UI.Panel.height) {
        float scrollTrackWidth = 12.0f;
        Rectangle scrollTrack = { UI.Panel.x + UI.Panel.width - scrollTrackWidth - 5, UI.Panel.y + 5, scrollTrackWidth, UI.Panel.height - 10 };
        DrawRectangleRounded(scrollTrack, 1.0f, 10, Fade(GRAY, 0.3f));
        
        float handleHeight = (UI.Panel.height / TotalContentHeight) * scrollTrack.height;
        if (handleHeight < 30.0f) handleHeight = 30.0f; 
        
        float scrollableWheel = TotalContentHeight + distance - UI.Panel.height;
        float scrollRatio = -(*wheel) / scrollableWheel;
        float handleY = scrollTrack.y + scrollRatio * (scrollTrack.height - handleHeight);
        
        Rectangle scrollHandle = { scrollTrack.x, handleY, scrollTrackWidth, handleHeight };

        static bool isDraggingPubs = false;
        static float dragOffsetPubsY = 0.0f;
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(size.Mouse, scrollHandle)) {
            isDraggingPubs = true;
            dragOffsetPubsY = size.Mouse.y - scrollHandle.y;
        }
        
        if (isDraggingPubs) {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                float newHandleY = size.Mouse.y - dragOffsetPubsY;
                if (newHandleY < scrollTrack.y) newHandleY = scrollTrack.y;
                if (newHandleY > scrollTrack.y + scrollTrack.height - handleHeight) newHandleY = scrollTrack.y + scrollTrack.height - handleHeight;
                
                float newRatio = (newHandleY - scrollTrack.y) / (scrollTrack.height - handleHeight);
                *wheel = -(newRatio * scrollableWheel);
                scrollHandle.y = newHandleY; 
            } else isDraggingPubs = false;
        }
        bool isHoveredScroll = CheckCollisionPointRec(size.Mouse, scrollHandle) || isDraggingPubs;
        DrawRectangleRounded(scrollHandle, 1.0f, 10, isHoveredScroll ? GRAY : Fade(DARKGRAY, 0.8f));
    }
    EndScissorMode();
}

void ShowType_Panel(Size size, ManageBooksUI UI, float *wheel, Font *_Font, Type *Types, char *selectedType, BookList *Books) {
    bool isMouseInPanel = CheckCollisionPointRec(size.Mouse, UI.Panel);
    float itemHeight = 60.0f;
    float distance = 15.0f;
    int displayCount = Types->count;
    float TotalContentHeight = (itemHeight + distance) * displayCount;

    if (TotalContentHeight > UI.Panel.height) {
        if (GetMouseWheelMove() == 1) *wheel += 40.0f;
        else if (GetMouseWheelMove() == -1) *wheel -= 40.0f;
        if (*wheel > 0) *wheel = 0;
        if (*wheel < UI.Panel.height - TotalContentHeight - distance) *wheel = UI.Panel.height - TotalContentHeight - distance;
    } else *wheel = 0;

    BeginScissorMode(UI.Panel.x, UI.Panel.y, UI.Panel.width, UI.Panel.height);

    for (int i = 0; i < displayCount; i++) {
        Rectangle hitbox = { UI.Panel.x + UI.Panel.width * 0.08f, *wheel + UI.Panel.y + 20.0f + (itemHeight + distance) * i, UI.Panel.width * 0.84f, itemHeight };
        bool isHovered = CheckCollisionPointRec(size.Mouse, hitbox) && isMouseInPanel;
        DrawRectangleRounded(hitbox, 0.2f, 20, isHovered ? Fade(TEALBLUE, 0.8f) : Fade(BLACK, 0.7f));

        DrawTextEx(_Font[0], Types->Type[i], (Vector2){hitbox.x + 30, hitbox.y + 15}, 30, 1, isHovered ? GOLDACCENT : WHITE);

        int bookCount = CountStrInBooks(Books, Types->Type[i], TYPE);
        char countStr[32]; sprintf(countStr, "%d Truyện", bookCount);
        float countW = MeasureTextEx(_Font[0], countStr, 25, 1).x;
        DrawTextEx(_Font[0], countStr, (Vector2){hitbox.x + hitbox.width - countW - 20, hitbox.y + 18}, 25, 1, isHovered ? WHITE : GRAY);

        if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            strcpy(selectedType, Types->Type[i]);
            *wheel = 0; 
            Books->currentPage = 1;
        }
    }

    if (TotalContentHeight > UI.Panel.height) {
        float scrollTrackWidth = 12.0f;
        Rectangle scrollTrack = { UI.Panel.x + UI.Panel.width - scrollTrackWidth - 5, UI.Panel.y + 5, scrollTrackWidth, UI.Panel.height - 10 };
        DrawRectangleRounded(scrollTrack, 1.0f, 10, Fade(GRAY, 0.3f)); 

        float handleHeight = (UI.Panel.height / TotalContentHeight) * scrollTrack.height;
        if (handleHeight < 30.0f) handleHeight = 30.0f; 

        float scrollableWheel = TotalContentHeight + distance - UI.Panel.height;
        float scrollRatio = -(*wheel) / scrollableWheel;
        float handleY = scrollTrack.y + scrollRatio * (scrollTrack.height - handleHeight);
        
        Rectangle scrollHandle = { scrollTrack.x, handleY, scrollTrackWidth, handleHeight };

        static bool isDraggingTypes = false;
        static float dragOffsetTypesY = 0.0f;
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(size.Mouse, scrollHandle)) {
            isDraggingTypes = true;
            dragOffsetTypesY = size.Mouse.y - scrollHandle.y;
        }
        
        if (isDraggingTypes) {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                float newHandleY = size.Mouse.y - dragOffsetTypesY;
                if (newHandleY < scrollTrack.y) newHandleY = scrollTrack.y;
                if (newHandleY > scrollTrack.y + scrollTrack.height - handleHeight) newHandleY = scrollTrack.y + scrollTrack.height - handleHeight;
                
                float newRatio = (newHandleY - scrollTrack.y) / (scrollTrack.height - handleHeight);
                *wheel = -(newRatio * scrollableWheel);
                scrollHandle.y = newHandleY; 
            } else isDraggingTypes = false;
        }
        bool isHoveredScroll = CheckCollisionPointRec(size.Mouse, scrollHandle) || isDraggingTypes;
        DrawRectangleRounded(scrollHandle, 1.0f, 10, isHoveredScroll ? GRAY : Fade(DARKGRAY, 0.8f)); 
    }

    EndScissorMode();
}

// =========================================================================
// CÁC HÀM TIỆN ÍCH UI, SỰ KIỆN, VÀ DỮ LIỆU FILE
// =========================================================================

void LoadManageBooksUI (ManageBooksUI *UI, Size size){
    UI->TitleBox = (Rectangle) { 0, 0, size.Screen.x, size.Screen.y * 0.08f * (float) pow(0.6, (double)size.Scale.x) };
    UI->HeaderBox = (Rectangle) { UI->TitleBox.width * 0.15f, UI->TitleBox.height, UI->TitleBox.width * 0.7f, UI->TitleBox.height };
    
    float totalPanelHeight = size.Screen.y - (UI->HeaderBox.height + UI->TitleBox.height);
    float paginationHeight = 70.0f;

    UI->Panel = (Rectangle) { UI->HeaderBox.x, UI->HeaderBox.y + UI->TitleBox.height, UI->HeaderBox.width, totalPanelHeight };
    UI->PaginationBox = (Rectangle) { UI->HeaderBox.x, UI->Panel.y + totalPanelHeight - paginationHeight, UI->HeaderBox.width, paginationHeight };
}

void ManageBooksTitle(Texture2D icon, ManageBooksUI UI){
    Rectangle IconBox = { UI.TitleBox.width * 0.02f, UI.TitleBox.height * 0.5f, UI.TitleBox.width * 0.08f, UI.TitleBox.width * 0.08f };
    DrawRectangleRec(UI.TitleBox, BRIGHTWHITE);
    DrawRectangleRec(UI.HeaderBox, GRAY);
    DrawRectangleRec(UI.Panel, Fade(WHITESMOKE, 0.8f)); 
    DrawIcon(IconBox, icon);
}

// ------------------------------------------------------------------
// BỘ LỌC CHUẨN HÓA TIẾNG VIỆT (KHÔNG DẤU, KHÔNG CÁCH)
// ------------------------------------------------------------------
void NormalizeString(const char* src, char* dest, int max_len) {
    const char* vn_utf8[] = {
        "à","á","ạ","ả","ã","â","ầ","ấ","ậ","ẩ","ẫ","ă","ằ","ắ","ặ","ẳ","ẵ",
        "è","é","ẹ","ẻ","ẽ","ê","ề","ế","ệ","ể","ễ",
        "ì","í","ị","ỉ","ĩ",
        "ò","ó","ọ","ỏ","õ","ô","ồ","ố","ộ","ổ","ỗ","ơ","ờ","ớ","ợ","ở","ỡ",
        "ù","ú","ụ","ủ","ũ","ư","ừ","ứ","ự","ử","ữ",
        "ỳ","ý","ỵ","ỷ","ỹ",
        "đ",
        "À","Á","Ạ","Ả","Ã","Â","Ầ","Ấ","Ậ","Ẩ","Ẫ","Ă","Ằ","Ắ","Ặ","Ẳ","Ẵ",
        "È","É","Ẹ","Ẻ","Ẽ","Ê","Ề","Ế","Ệ","Ể","Ễ",
        "Ì","Í","Ị","Ỉ","Ĩ",
        "Ò","Ó","Ọ","Ỏ","Õ","Ô","Ồ","Ố","Ộ","Ổ","Ỗ","Ơ","Ờ","Ớ","Ợ","Ở","Ỡ",
        "Ù","Ú","Ụ","Ủ","Ũ","Ư","Ừ","Ứ","Ự","Ử","Ữ",
        "Ỳ","Ý","Ỵ","Ỷ","Ỹ",
        "Đ"
    };
    const char ascii_repl[] = {
        'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a',
        'e','e','e','e','e','e','e','e','e','e','e',
        'i','i','i','i','i',
        'o','o','o','o','o','o','o','o','o','o','o','o','o','o','o','o','o',
        'u','u','u','u','u','u','u','u','u','u','u',
        'y','y','y','y','y',
        'd',
        'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a',
        'e','e','e','e','e','e','e','e','e','e','e',
        'i','i','i','i','i',
        'o','o','o','o','o','o','o','o','o','o','o','o','o','o','o','o','o',
        'u','u','u','u','u','u','u','u','u','u','u',
        'y','y','y','y','y',
        'd'
    };
    int num_vn = sizeof(vn_utf8) / sizeof(vn_utf8[0]);
    int i = 0, j = 0;

    while (src[i] != '\0' && j < max_len - 1) {
        // [QUAN TRỌNG] Bỏ qua toàn bộ dấu cách (Không khoảng trắng)
        if (isspace((unsigned char)src[i])) {
            i++;
            continue; 
        }
        
        bool replaced = false;
        // Quét và xóa toàn bộ dấu Tiếng Việt
        for (int k = 0; k < num_vn; k++) {
            int len = strlen(vn_utf8[k]);
            if (strncmp(&src[i], vn_utf8[k], len) == 0) {
                dest[j++] = ascii_repl[k];
                i += len;
                replaced = true;
                break;
            }
        }
        
        // Đưa về chữ cái thường (lowercase)
        if (!replaced) {
            dest[j++] = tolower((unsigned char)src[i]);
            i++;
        }
    }
    dest[j] = '\0';
}

// ------------------------------------------------------------------
// THUẬT TOÁN TÍNH % GIỐNG NHAU (FUZZY MATCH >= 65%)
// ------------------------------------------------------------------
float CalculateSimilarity(const char* query, const char* text) {
    int qLen = strlen(query);
    int tLen = strlen(text);
    if (qLen == 0) return 1.0f;
    if (tLen == 0) return 0.0f;
    
    // Nếu text chứa trọn vẹn query -> Giống 100%
    if (strstr(text, query) != NULL) return 1.0f;
    
    // Từ quá ngắn (1 ký tự) bắt buộc phải giống chính xác
    if (qLen < 2) return 0.0f;
    
    // Cắt đôi từ khóa để đối chiếu chống gõ sai (Thuật toán Bigram)
    int matches = 0;
    for (int i = 0; i < qLen - 1; i++) {
        char bigram[3] = {query[i], query[i+1], '\0'};
        if (strstr(text, bigram) != NULL) {
            matches++;
        }
    }
    return (float)matches / (qLen - 1); // Trả về tỷ lệ % (0.0 đến 1.0)
}

// ------------------------------------------------------------------
// BỘ TÌM KIẾM MẠNH NHẤT (HỖ TRỢ NGOẶC KÉP VÀ DẤU CỘNG)
// ------------------------------------------------------------------
bool ContextualBookSearch(Book book, const char *query) {
    if (query == NULL || strlen(query) == 0) return true;

    char queryCopy[512];
    strncpy(queryCopy, query, sizeof(queryCopy) - 1);
    queryCopy[sizeof(queryCopy) - 1] = '\0';

    char rawPart1[256] = {0};
    char rawPart2[256] = {0};
    bool isExact1 = false;
    bool isExact2 = false;

    // Quét tìm cú pháp Ngoặc kép "..." + "..."
    if (strchr(queryCopy, '"') != NULL) {
        const char *q1_start = strchr(queryCopy, '"');
        const char *q1_end = strchr(q1_start + 1, '"');
        
        if (q1_end) {
            int len1 = q1_end - (q1_start + 1);
            if (len1 >= sizeof(rawPart1)) len1 = sizeof(rawPart1) - 1;
            strncpy(rawPart1, q1_start + 1, len1);
            rawPart1[len1] = '\0';
            isExact1 = true;

            const char *q2_start = strchr(q1_end + 1, '"');
            if (q2_start) {
                const char *q2_end = strchr(q2_start + 1, '"');
                if (q2_end) {
                    int len2 = q2_end - (q2_start + 1);
                    if (len2 >= sizeof(rawPart2)) len2 = sizeof(rawPart2) - 1;
                    strncpy(rawPart2, q2_start + 1, len2);
                    rawPart2[len2] = '\0';
                    isExact2 = true;
                }
            }
        }
    } 
    
    // Cú pháp dấu cộng truyền thống
    if (!isExact1) {
        char *plusPtr = strchr(queryCopy, '+');
        if (plusPtr != NULL) {
            *plusPtr = '\0'; 
            strcpy(rawPart1, queryCopy);
            strcpy(rawPart2, plusPtr + 1);
        } else {
            strcpy(rawPart1, queryCopy);
        }
    }

    char normPart1[256] = {0};
    char normPart2[256] = {0};
    
    // Bình thường hóa: Ép sạch dấu tiếng việt, khoảng trắng, viết hoa
    if (strlen(rawPart1) > 0) NormalizeString(rawPart1, normPart1, sizeof(normPart1));
    if (strlen(rawPart2) > 0) NormalizeString(rawPart2, normPart2, sizeof(normPart2));

    // --- KIỂM TRA VẾ 1 (TÌM THEO NGỮ CẢNH CỦA TAB ĐANG ĐỨNG) ---
    char rawFieldData[512] = {0};
    switch (g_searchContext) {
        case MANAGEBOOKS_Author: strcpy(rawFieldData, book.AuthorBook); break;
        case MANAGEBOOKS_Publisher: strcpy(rawFieldData, book.PublisherBook); break;
        case MANAGEBOOKS_Type: strcpy(rawFieldData, book.TypeBook); break;
        case MANAGEBOOKS_Dashboard:
        default: strcpy(rawFieldData, book.NameBook); break;
    }
    
    char normFieldData[512] = {0};
    NormalizeString(rawFieldData, normFieldData, sizeof(normFieldData));
    
    if (strlen(normPart1) > 0) {
        if (isExact1) {
            if (strstr(normFieldData, normPart1) == NULL) return false;
        } else {
            // Tính toán % giống nhau. >65% (0.65f) là đạt!
            if (CalculateSimilarity(normPart1, normFieldData) < 0.65f) return false;
        }
    }

    // --- KIỂM TRA VẾ 2 (LỌC TRÊN TOÀN BỘ DỮ LIỆU) ---
    if (strlen(normPart2) > 0) {
        char rawCombined[2048];
        sprintf(rawCombined, "%s %s %s %s %s %s", 
            book.CodeBook, book.NameBook, book.NormNameBook, 
            book.AuthorBook, book.TypeBook, book.PublisherBook);
            
        char normCombined[2048] = {0};
        NormalizeString(rawCombined, normCombined, sizeof(normCombined));

        if (isExact2) {
            if (strstr(normCombined, normPart2) == NULL) return false;
        } else {
            if (CalculateSimilarity(normPart2, normCombined) < 0.65f) return false;
        }
    }

    return true;
}

int CountStrInBooks (BookList *Books, const char *Str, StateFindBook state){
    int count = 0;
    for (int i = 0; i < Books->count; i++){
        int match = 0;
        switch (state){
            case CODE: 
                if (strcmp(Books->theArray[i].CodeBook, Str) == 0) match = 1; 
                break;
            case NAME: 
                if (strcmp(Books->theArray[i].NameBook, Str) == 0) match = 1; 
                break;
            case AUTHOR: 
                if (strcmp(Books->theArray[i].AuthorBook, Str) == 0) match = 1; 
                break;
            case TYPE: 
                if (strcmp(Books->theArray[i].TypeBook, Str) == 0) match = 1; 
                break;
            case PUBLISHER: 
                if (strcmp(Books->theArray[i].PublisherBook, Str) == 0) match = 1; 
                break;
            case SEARCH_ALL: 
                if (ContextualBookSearch(Books->theArray[i], Str)) match = 1;
                break;
            default: match = 1; break;
        }
        if (match) count++;
    }
    return count;
}

void DrawPagination(ManageBooksUI UI, BookList *Books, Font *_Font, Size size, float *wheel, char *backTarget, bool isVisible) {
    if (!isVisible || Books->totalPages <= 1) return; 

    DrawRectangleRec(UI.PaginationBox, Fade(SILVERGRAY, 0.95f));

    float btnWidth = 40.0f, btnHeight = 40.0f, spacing = 8.0f;
    int maxVisible = 5;
    int startPage = Books->currentPage - 2;
    if (startPage < 1) startPage = 1;
    
    int endPage = startPage + maxVisible - 1;
    if (endPage > Books->totalPages) {
        endPage = Books->totalPages;
        startPage = endPage - maxVisible + 1;
        if (startPage < 1) startPage = 1;
    }

    float totalWidth = (endPage - startPage + 1) * (btnWidth + spacing) + 2 * (80.0f + spacing);
    float startX = UI.PaginationBox.x + (UI.PaginationBox.width - totalWidth) / 2;
    float startY = UI.PaginationBox.y + (UI.PaginationBox.height - btnHeight) / 2;

    Rectangle firstBtn = { startX, startY, 80, btnHeight };
    
    if (Books->currentPage > 1) {
        if (CheckCollisionPointRec(size.Mouse, firstBtn)) {
            DrawRectangleRounded(firstBtn, 0.2f, 10, Fade(TEALBLUE, 0.6f));
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { Books->currentPage = 1; *wheel = 0; }
        } else DrawRectangleRounded(firstBtn, 0.2f, 10, GRAYBLUE);
        DrawTextEx(_Font[3], "First", (Vector2){firstBtn.x + 15, firstBtn.y + 10}, 20, 1, WHITE);
    }
    startX += 80.0f + spacing;

    for (int p = startPage; p <= endPage; p++) {
        Rectangle pageBtn = { startX, startY, btnWidth, btnHeight };
        
        if (p == Books->currentPage) {
            DrawRectangleRounded(pageBtn, 0.2f, 10, TEALBLUE);
            DrawTextEx(_Font[3], TextFormat("%d", p), (Vector2){pageBtn.x + 13, pageBtn.y + 10}, 20, 1, WHITE);
        } else {
            if (CheckCollisionPointRec(size.Mouse, pageBtn)) {
                DrawRectangleRounded(pageBtn, 0.2f, 10, Fade(TEALBLUE, 0.6f));
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { Books->currentPage = p; *wheel = 0; }
            } else DrawRectangleRounded(pageBtn, 0.2f, 10, GRAYBLUE);
            DrawTextEx(_Font[3], TextFormat("%d", p), (Vector2){pageBtn.x + 13, pageBtn.y + 10}, 20, 1, WHITE);
        }
        startX += btnWidth + spacing;
    }

    Rectangle lastBtn = { startX, startY, 80, btnHeight };
    
    if (Books->currentPage < Books->totalPages) {
        if (CheckCollisionPointRec(size.Mouse, lastBtn)) {
            DrawRectangleRounded(lastBtn, 0.2f, 10, Fade(TEALBLUE, 0.6f));
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { Books->currentPage = Books->totalPages; *wheel = 0; }
        } else DrawRectangleRounded(lastBtn, 0.2f, 10, GRAYBLUE);
        DrawTextEx(_Font[3], "Last", (Vector2){lastBtn.x + 18, lastBtn.y + 10}, 20, 1, WHITE);
    }
}

bool ManageBooksFunc(MANAGEBOOKS_STATE *State, InputBox *FindBar, Texture2D Icon_Find, ManageBooksUI UI, Size size, Font *_Font){
    int _return = 0;
    
    Rectangle IconBox = { UI.TitleBox.width * 0.02f, UI.TitleBox.height * 0.5f, UI.TitleBox.width * 0.08f, UI.TitleBox.width * 0.08f };
    if (CheckCollisionPointRec(size.Mouse, IconBox)) {
        DrawRectangleRec(IconBox, Fade(TEALBLUE, 0.1f));
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){ *State = MANAGEBOOKS_Dashboard; _return = 1; }
    }
    
    Vector2 TextWidth_F;
    
    TextWidth_F = MeasureTextEx(_Font[0], MANAGEBOOKS_Func_1, 24, 2);
    Rectangle AuthorBox = { UI.TitleBox.width * 0.15f, UI.TitleBox.height * 0.9f - TextWidth_F.y, TextWidth_F.x, TextWidth_F.y };
    if (CheckCollisionPointRec(size.Mouse, AuthorBox)){
        DrawTextEx(_Font[0], MANAGEBOOKS_Func_1, (Vector2) { AuthorBox.x - 8, AuthorBox.y - 4 }, 26, 3, GOLDACCENT);
        DrawLineEx((Vector2) { AuthorBox.x, AuthorBox.y + AuthorBox.height }, (Vector2) { AuthorBox.x + AuthorBox.width, AuthorBox.y + AuthorBox.height }, 3, BLACK);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){ *State = MANAGEBOOKS_Author; _return = 1; }
    } else {
        DrawTextEx(_Font[0], MANAGEBOOKS_Func_1, (Vector2) { AuthorBox.x, AuthorBox.y }, 24, 2, BLACK);
        DrawLineEx((Vector2) { AuthorBox.x, AuthorBox.y + AuthorBox.height }, (Vector2) { AuthorBox.x + AuthorBox.width, AuthorBox.y + AuthorBox.height }, 3, BLACK);
    }

    TextWidth_F = MeasureTextEx(_Font[0], MANAGEBOOKS_Func_2, 24, 2);
    Rectangle PublisherBox = { AuthorBox.x + AuthorBox.width + UI.TitleBox.width * 0.03f, AuthorBox.y, TextWidth_F.x, TextWidth_F.y };
    if (CheckCollisionPointRec(size.Mouse, PublisherBox)){
        DrawTextEx(_Font[0], MANAGEBOOKS_Func_2, (Vector2) { PublisherBox.x - 10, PublisherBox.y - 4 }, 26, 3, GOLDACCENT);
        DrawLineEx((Vector2) { PublisherBox.x, PublisherBox.y + PublisherBox.height }, (Vector2) { PublisherBox.x + PublisherBox.width, PublisherBox.y + PublisherBox.height }, 3, BLACK);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){ *State = MANAGEBOOKS_Publisher; _return = 1; }
    } else {
        DrawTextEx(_Font[0], MANAGEBOOKS_Func_2, (Vector2) { PublisherBox.x, PublisherBox.y }, 24, 2, BLACK);
        DrawLineEx((Vector2) { PublisherBox.x, PublisherBox.y + PublisherBox.height }, (Vector2) { PublisherBox.x + PublisherBox.width, PublisherBox.y + PublisherBox.height }, 3, BLACK);
    }

    TextWidth_F = MeasureTextEx(_Font[0], MANAGEBOOKS_Func_3, 24, 2);
    Rectangle TypeBox = { PublisherBox.x + PublisherBox.width + UI.TitleBox.width * 0.03f, PublisherBox.y, TextWidth_F.x, TextWidth_F.y };
    if (CheckCollisionPointRec(size.Mouse, TypeBox)){
        DrawTextEx(_Font[0], MANAGEBOOKS_Func_3, (Vector2) { TypeBox.x - 8, TypeBox.y - 4 }, 26, 3, GOLDACCENT);
        DrawLineEx((Vector2) { TypeBox.x, TypeBox.y + TypeBox.height }, (Vector2) { TypeBox.x + TypeBox.width, TypeBox.y + TypeBox.height }, 3, BLACK);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){ *State = MANAGEBOOKS_Type; _return = 1; }
    } else {
        DrawTextEx(_Font[0], MANAGEBOOKS_Func_3, (Vector2) { TypeBox.x, TypeBox.y }, 24, 2, BLACK);
        DrawLineEx((Vector2) { TypeBox.x, TypeBox.y + TypeBox.height }, (Vector2) { TypeBox.x + TypeBox.width, TypeBox.y + TypeBox.height }, 3, BLACK);
    }

    TextWidth_F = MeasureTextEx(_Font[0], MANAGEBOOKS_Func_4, 24, 2);
    Rectangle MainBox = { TypeBox.x + TypeBox.width + UI.TitleBox.width * 0.03f, TypeBox.y, TextWidth_F.x, TextWidth_F.y };
    if (CheckCollisionPointRec(size.Mouse, MainBox)){
        DrawTextEx(_Font[0], MANAGEBOOKS_Func_4, (Vector2) { MainBox.x - 8, MainBox.y - 4 }, 26, 3, GOLDACCENT);
        DrawLineEx((Vector2) { MainBox.x, MainBox.y + MainBox.height }, (Vector2) { MainBox.x + MainBox.width, MainBox.y + MainBox.height }, 3, BLACK);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){ *State = MANAGEBOOKS_Main; _return = 1; }
    } else {
        DrawTextEx(_Font[0], MANAGEBOOKS_Func_4, (Vector2) { MainBox.x, MainBox.y }, 24, 2, BLACK);
        DrawLineEx((Vector2) { MainBox.x, MainBox.y + MainBox.height }, (Vector2) { MainBox.x + MainBox.width, MainBox.y + MainBox.height }, 3, BLACK);
    }

    float findStartX = MainBox.x + MainBox.width + UI.TitleBox.width * 0.03f;
    float findWidth = UI.TitleBox.width - findStartX - 20.0f;
    if (findWidth < UI.TitleBox.height * 2.5f) findWidth = UI.TitleBox.height * 2.5f;

    FindBar->box = (Rectangle) { findStartX, UI.TitleBox.height * 0.1f, findWidth, UI.TitleBox.height * 0.8f };

    Rectangle IconFindBox = { FindBar->box.x + (FindBar->box.width - FindBar->box.height), FindBar->box.y, FindBar->box.height, FindBar->box.height };
    Rectangle FindBarTextBox = { FindBar->box.x, FindBar->box.y, FindBar->box.width - IconFindBox.width, FindBar->box.height };
    Rectangle FindBarTextPos = { FindBarTextBox.x + 10, FindBarTextBox.y + (FindBarTextBox.height - 20.0f) / 2.0f, FindBarTextBox.width - 15.0f, 20.0f };

    if (CheckCollisionPointRec(size.Mouse, FindBarTextBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        FindBar->isFocused = true;
    } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        FindBar->isFocused = false;
    }

    float roundness = FindRoundness(0.05f * FindBar->box.width, FindBar->box.width, FindBar->box.height);
    DrawRectangleRoundedLinesEx(FindBar->box, roundness, 10, 2.0f, LIGHTGRAY);
    DrawRectangleRounded(FindBar->box, roundness, 10, GRAY);

    if (FindBar->isFocused == true) {
        DrawRectangleRounded(FindBar->box, roundness, 10, WHITESMOKE);
        BeginScissorMode((int)FindBarTextPos.x, (int)FindBarTextPos.y, (int)FindBarTextPos.width, (int)FindBarTextPos.height);
        
        UpdateInputBox(FindBar);
        
        float w = MeasureTextEx(_Font[0], FindBar->text, 20, 2).x;
        float dx = ((w - (FindBarTextPos.width - 5)) > 0) ? w - (FindBarTextPos.width - 5) : 0;
        Vector2 position = { FindBarTextPos.x - dx, FindBarTextPos.y };

        DrawTextEx(_Font[0], FindBar->text, position, 20, 2, GRAY);
        if ((int)(GetTime() * 2) % 2 == 0) DrawRectangle(FindBarTextPos.x + w - dx + 2, FindBarTextPos.y, 2, 20, BLACK);
        EndScissorMode();

        if (IsKeyPressed(KEY_ENTER)) { *State = MANAGEBOOKS_Find; _return = 1; }

    } else if (FindBar->length > 0) {
        DrawRectangleRounded(FindBar->box, roundness, 10, WHITESMOKE);
        BeginScissorMode((int)FindBarTextPos.x, (int)FindBarTextPos.y, (int)FindBarTextPos.width, (int)FindBarTextPos.height);
        
        float w = MeasureTextEx(_Font[0], FindBar->text, 20, 2).x;
        float dx = ((w - FindBarTextPos.width) > 0) ? w - FindBarTextPos.width : 0;
        Vector2 position = { FindBarTextPos.x - dx, FindBarTextPos.y };
        DrawTextEx(_Font[0], FindBar->text, position, 20, 2, GRAY);
        EndScissorMode();
    } else {
        BeginScissorMode((int)FindBarTextPos.x, (int)FindBarTextPos.y, (int)FindBarTextPos.width, (int)FindBarTextPos.height);
        DrawTextEx(_Font[0], "Search", (Vector2){ FindBarTextPos.x, FindBarTextPos.y }, 20, 2, Fade(BLACK, 0.5f));
        EndScissorMode();
    }

    roundness = FindRoundness(0.05f * FindBar->box.width, IconFindBox.width, IconFindBox.height);
    DrawRectangleRounded(IconFindBox, roundness, 10, BRIGHTGRAY);
    DrawIcon(IconFindBox, Icon_Find);
    
    if (CheckCollisionPointRec(size.Mouse, IconFindBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { 
        *State = MANAGEBOOKS_Find; 
        _return = 1; 
    }
    
    return _return;
}

BookList *Loadbooks(const char *filename){
    FILE *file = fopen(filename, "r");
    if (!file) return NULL;

    BookList *bookList = (BookList *)malloc(sizeof(BookList));
    if (!bookList) { fclose(file); return NULL; }

    bookList->theArray = (Book *)malloc(MAX_BOOKS * sizeof(Book));
    if (!bookList->theArray) { free(bookList); fclose(file); return NULL; }

    bookList->capacity = MAX_BOOKS;
    fscanf(file, "Số quyển truyện: %d\n", &(bookList->count));
    fscanf(file, "Số lượng truyện hiện tại: %d\n", &(bookList->stockBooks));
    fscanf(file, "Số lượng truyện gốc: %d\n", &(bookList->totalImportBooks));

    bookList->pos = 0;
    bookList->QuantityForOnePage = QUANTITYFORONEPAGE;
    bookList->currentPage = 1;
    bookList->totalPages = 1;

    Book book;
    int count = 0;
    while (fscanf(file, " | %[^|]| %[^|]| %[^|]| %[^|]| %[^|]| %[^|]| %d | %d | %d | %d | %d |\n", 
        book.CodeBook, book.NormNameBook, book.NameBook, book.AuthorBook, book.TypeBook, book.PublisherBook, 
        &book.YearBook, &book.StockBook, &book.TotalImportBook, &book.TotalBorrowBook, &book.PriceBook) == 11)
    {
        trim(book.CodeBook);
        trim(book.NormNameBook);
        trim(book.NameBook);
        trim(book.AuthorBook);
        trim(book.TypeBook);
        trim(book.PublisherBook);
        book.Description[0] = '\0'; 
        bookList->theArray[count++] = book;
    }

    if (count != bookList->count) { printf ("Failed!!! Read %d of %d\n", count, bookList->count); return NULL; }
    fclose(file);
    return bookList;
}

bool Savebooks(BookList Books){
    FILE *f = fopen(BOOKS_FILE, "w");
    if (f == NULL) return 0;

    fprintf(f, "Số quyển truyện: %d\n", Books.count);
    fprintf(f, "Số lượng truyện hiện tại: %d\n", Books.stockBooks);
    fprintf(f, "Số lượng truyện gốc: %d\n", Books.totalImportBooks);

    Book A;
    for (int i = 0; i < Books.count; i++) {
        A = Books.theArray[i];
        fprintf(f, "| %-*s | %-*s | %-*s | %-*s | %-*s | %-*s | %-*d | %-*d | %-*d | %-*d | %-*d |\n",
            UTF8Width(A.CodeBook, CODE_BOOKS_LENGTH) - 2, A.CodeBook,
            UTF8Width(A.NormNameBook, NORMNAME_BOOKS_LENGTH) - 2, A.NormNameBook,
            UTF8Width(A.NameBook, NAME_BOOKS_LENGTH) - 2, A.NameBook,
            UTF8Width(A.AuthorBook, AUTHOR_BOOKS_LENGTH) - 2, A.AuthorBook,
            UTF8Width(A.TypeBook, TYPE_BOOKS_LENGTH) - 2, A.TypeBook,
            UTF8Width(A.PublisherBook, PUBLISHER_BOOKS_LENGTH) - 2, A.PublisherBook,
            YEAR_BOOKS_LENGTH - 2, A.YearBook,
            STOCK_BOOKS_LENGTH - 2, A.StockBook,
            TOTAL_IMPORT_BOOKS_LENGTH - 2, A.TotalImportBook,
            TOTAL_BORROW_BOOKS_LENGTH - 2, A.TotalBorrowBook,
            PRICE_BOOKS_LENGTH - 2, A.PriceBook 
        );
    }
    fclose(f);
    return 1;
}

AuthorList *LoadAuthor(const char *filename){
    FILE *f = fopen(filename, "r");
    if (f == NULL) return NULL;
    int count;
    fscanf(f, "Tổng: %d\n", &count);
    AuthorList *A = malloc(sizeof(AuthorList));
    if (A == NULL) return NULL;
    A->Author = malloc(sizeof(Author) * count);
    if (A->Author == NULL) { free(A); return NULL; }
    
    A->count = 0;
    while ((fscanf(f, " | %[^|]| %[^|]|\n", A->Author[A->count].code, A->Author[A->count].name) == 2)) {
        trim(A->Author[A->count].code);
        trim(A->Author[A->count].name);
        A->count++;
    }
    fclose(f);
    if (A->count != count) { free(A); return NULL; }
    return A;
}

bool SaveAuthor(AuthorList _Author){
    FILE *f = fopen(AUTHORS_FILE, "w");
    if (f == NULL) return false;

    fprintf(f, "Tổng: %d\n", _Author.count);

    int i;
    for (i = 0; i < _Author.count; i++){
        Author A = _Author.Author[i];
        fprintf(f, "| %*s | %*s |\n", UTF8Width(A.code, AUTHORCODE_AUTHORS_LENGTH), A.code, UTF8Width(A.name, AUTHOR_AUTHORS_LENGTH), A.name);
    }
    fclose(f);
    
    if (i != _Author.count) return false;
    return true;
}

Type *LoadType(BookList *Books){
    if (Books == NULL || Books->theArray == NULL || Books->count <= 0) return NULL;
    Type *type = malloc(sizeof(Type)); 
    if (type == NULL) return NULL;
    type->Type = malloc(sizeof(char *) * Books->count);
    if (type->Type == NULL){ free(type); return NULL; }
    type->count = 0;
    for (int i = 0; i < Books->count; i++){
        int flag = 0;
        for (int j = 0; j < type->count; j++){
            if (strcmp(type->Type[j], Books->theArray[i].TypeBook) == 0){
                flag = 1;
                break;
            }
        }
        if (!flag){
            type->Type[type->count] = malloc(sizeof(char) * 32);
            if (type->Type[type->count] == NULL) continue;
            strcpy(type->Type[type->count], Books->theArray[i].TypeBook);
            type->count++;
        }
    }
    return type;
}

PublisherList *LoadPublisher(BookList *Books){
    if (Books == NULL || Books->theArray == NULL || Books->count <= 0) return NULL;
    PublisherList *pub = malloc(sizeof(PublisherList)); 
    if (pub == NULL) return NULL;
    pub->Publisher = malloc(sizeof(char *) * Books->count);
    if (pub->Publisher == NULL){ free(pub); return NULL; }
    pub->count = 0;
    for (int i = 0; i < Books->count; i++){
        int flag = 0;
        for (int j = 0; j < pub->count; j++){
            if (strcmp(pub->Publisher[j], Books->theArray[i].PublisherBook) == 0){
                flag = 1;
                break;
            }
        }
        if (!flag){
            pub->Publisher[pub->count] = malloc(sizeof(char) * 64);
            if (pub->Publisher[pub->count] == NULL) continue;
            strcpy(pub->Publisher[pub->count], Books->theArray[i].PublisherBook);
            pub->count++;
        }
    }
    return pub;
}

void LoadDescription(BookList *Books, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;

    char line[1024];
    Book *currentBook = NULL;

    while (fgets(line, sizeof(line), f)) {
        if (line[0] == 'T' && isdigit(line[1]) && isdigit(line[2]) && isdigit(line[3])) {
            char code[10];
            sscanf(line, "%s", code);
            currentBook = NULL;
            for (int i = 0; i < Books->count; i++) {
                if (strcmp(Books->theArray[i].CodeBook, code) == 0) {
                    currentBook = &Books->theArray[i];
                    break;
                }
            }
        } else if (currentBook != NULL) {
            strcat(currentBook->Description, line);
        }
    }
    fclose(f);
}