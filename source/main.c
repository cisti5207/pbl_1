#include <stdio.h>
#include "ManageBooks.h"
#include "raylib.h"
#include "login.h"
#include "main.h"
#include <string.h>
#include <math.h>

AppState APP_STATE = LOGINAPP;

int main()
{
    InitWindow(1200, 800, "Login");
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(1000, 600);

    Size MainSize;
    LoadSize(
        &MainSize,
        (Vector2) { 
            GetMonitorWidth(0), 
            GetMonitorHeight(0) 
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

    // Khởi tạo hạt động 1 lần duy nhất từ thư viện libmanage
    InitParticles(MainSize);

    Account _Account;
    if (InitLogin(&_Account) == LOGIN_SUCCESS) {
        APP_STATE = HOME;
    } else {
        printf("Login failed or window closed.\n");
        CloseWindow();
        return 1;
    }

    Role _role;
    if (strcmp(_Account.role, "Administrator") == 0)
        _role = ADMINISTRATOR;
    else 
        _role = STAFF;

    MainContainers Containers;
    MainLoadContainers(&Containers, MainSize);
    
    while (!WindowShouldClose()){
        // Cập nhật vị trí hạt động từ thư viện
        UpdateParticlesPosition(MainSize);
        
        MainSize.Mouse = GetMousePosition();
        LoadSize(
            &MainSize,
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
    
        MainLoadContainers(&Containers, MainSize);

        BeginDrawing();
        // Gọi nền động từ thư viện
        ClearBackground(AnimatedBackground());
        DrawBackgroundParticles();

        MainDrawPanel(Containers);
        MainFunc(Containers.ShowFuncBox, MainSize.Mouse);
        
        EndDrawing();

        switch(APP_STATE){
            case HOME:
                SetWindowTitle("Home");
                break;
            
            case LOGINAPP:
                SetWindowTitle("Login");
                
                _Account = (Account) {0};
                if (InitLogin(&_Account) == LOGIN_SUCCESS) {
                    APP_STATE = HOME;
                    if (strcmp(_Account.role, "Administrator") == 0)
                        _role = ADMINISTRATOR;
                    else 
                        _role = STAFF;
                } else {
                    printf("Login failed or window closed.\n");
                    goto EXIT_APP; 
                }

                printf("| %s | %s | %s | %s | %s | %s |\n",
                    _Account.username, _Account.realName, _Account.password,
                    _Account.cccd, _Account.dateOfBirth, _Account.role
                );
                break;

            case MANAGEBOOKS:
                SetWindowTitle("Manage Books");
                InitManageBooks(_role);
                APP_STATE = HOME;
                break;

            case MANAGEUSER:
                SetWindowTitle("Manage User");
                APP_STATE = HOME; 
                break;

            case MANAGEBORROWING:
                SetWindowTitle("Manage Borrow");
                APP_STATE = HOME; 
                break;
        }
    }

EXIT_APP:
    printf("Exiting application...\n");
    CloseWindow(); 
    return 0;
}

void MainLoadContainers(MainContainers *Containers, Size MainSize){
    Containers->PanelBox = (Rectangle) {
        MainSize.Screen.x * 0.15f * pow(0.9, (double) MainSize.Scale.x),
        MainSize.Screen.y * 0.1f * pow(0.9, (double) MainSize.Scale.y),
        MainSize.Screen.x - 2 * (MainSize.Screen.x * 0.15f * pow(0.9, (double) MainSize.Scale.x)),
        MainSize.Screen.y - (MainSize.Screen.y * 0.1f * pow(0.9, (double) MainSize.Scale.y))
    };

    Containers->DecribeBox = (Rectangle) {
        Containers->PanelBox.x,
        Containers->PanelBox.y,
        Containers->PanelBox.width,
        Containers->PanelBox.height * 0.2f
    };

    Containers->FuncBox = (Rectangle) {
        Containers->PanelBox.x,
        Containers->DecribeBox.y + Containers->DecribeBox.height + Containers->PanelBox.height * 0.02f,
        Containers->PanelBox.width,
        Containers->PanelBox.height * 0.05f
    };

    Containers->ShowFuncBox = (Rectangle) {
        Containers->PanelBox.x,
        Containers->FuncBox.y + Containers->FuncBox.height + Containers->PanelBox.height * 0.02f,
        Containers->PanelBox.width,
        (Containers->PanelBox.height + Containers->PanelBox.y - (Containers->FuncBox.y + Containers->FuncBox.height + Containers->PanelBox.height * 0.02f)) + Containers->PanelBox.width * 0.01f
    };
}

void MainDrawPanel(MainContainers Containers){
    float rounded = Containers.PanelBox.width * 0.01f;
    
    DrawRectangleRounded(Containers.PanelBox, FindRoundness(rounded, Containers.PanelBox.width, Containers.PanelBox.height), 10, Fade(SOFTWHITE, 0.1f));
    DrawRectangleRounded(Containers.DecribeBox, FindRoundness(rounded, Containers.DecribeBox.width, Containers.DecribeBox.height), 10, BRIGHTWHITE);
    DrawRectangleRounded(Containers.FuncBox, FindRoundness(rounded, Containers.FuncBox.width, Containers.FuncBox.height), 10, BRIGHTWHITE);
    DrawRectangleRounded(Containers.ShowFuncBox, FindRoundness(rounded, Containers.ShowFuncBox.width, Containers.ShowFuncBox.height), 10, BRIGHTWHITE);
}

bool MainFunc(Rectangle ShowFuncBox, Vector2 Mouse){
    int i = 0;
    float ratioDistance = 0.1f;
    float widthScissor = ShowFuncBox.width - ShowFuncBox.width/4 * ratioDistance;
    float widthCard = widthScissor / 4;
    
    Rectangle ManageBooksBox = { 
        ShowFuncBox.x + widthCard * ratioDistance + (float) i++ * widthCard, 
        ShowFuncBox.y + ShowFuncBox.height * 0.1f, 
        widthCard * (1.0f - ratioDistance), 
        ShowFuncBox.height * 0.7f 
    };
    
    Rectangle ManageUserBox = { 
        ShowFuncBox.x + widthCard * ratioDistance + (float) i++ * widthCard, 
        ShowFuncBox.y + ShowFuncBox.height * 0.1f, 
        widthCard * (1.0f - ratioDistance), 
        ShowFuncBox.height * 0.7f 
    };
    
    Rectangle ManageBorrowBox = { 
        ShowFuncBox.x + widthCard * ratioDistance + (float) i++ * widthCard, 
        ShowFuncBox.y + ShowFuncBox.height * 0.1f, 
        widthCard * (1.0f - ratioDistance), 
        ShowFuncBox.height * 0.7f 
    };
    
    Rectangle Login = { 
        ShowFuncBox.x + widthCard * ratioDistance + (float) i++ * widthCard, 
        ShowFuncBox.y + ShowFuncBox.height * 0.1f, 
        widthCard * (1.0f - ratioDistance), 
        ShowFuncBox.height * 0.7f 
    };

    float roundness = widthCard * 0.1f;
    float WidthText;

    DrawRectangleRounded(ManageBooksBox, FindRoundness(roundness, ManageBooksBox.width, ManageBooksBox.height), 10, BLACK);
    WidthText = MeasureText("Manage Books", 20);
    Vector2 ManageBooksText = { ManageBooksBox.x + (ManageBooksBox.width - WidthText) * 0.5f, ManageBooksBox.y + ManageBooksBox.height + 50 };
    DrawText("Manage Books", (int) ManageBooksText.x, (int) ManageBooksText.y, 20, BLACK);
    
    DrawRectangleRounded(ManageUserBox, FindRoundness(roundness, ManageUserBox.width, ManageUserBox.height), 10, BLACK);
    WidthText = MeasureText("Manage User", 20);
    Vector2 ManageUserText = { ManageUserBox.x + (ManageUserBox.width - WidthText) * 0.5f, ManageUserBox.y + ManageUserBox.height + 50 };
    DrawText("Manage User", (int) ManageUserText.x, (int) ManageUserText.y, 20, BLACK);

    DrawRectangleRounded(ManageBorrowBox, FindRoundness(roundness, ManageBorrowBox.width, ManageBorrowBox.height), 10, BLACK);
    WidthText = MeasureText("Manage Borrow", 20);
    Vector2 ManageBorrowText = { ManageBorrowBox.x + (ManageBorrowBox.width - WidthText) * 0.5f, ManageBorrowBox.y + ManageBorrowBox.height + 50 };
    DrawText("Manage Borrow", (int) ManageBorrowText.x, (int) ManageBorrowText.y, 20, BLACK);

    DrawRectangleRounded(Login, FindRoundness(roundness, Login.width, Login.height), 10, BLACK);
    WidthText = MeasureText("Logout", 20);
    Vector2 LoginText = { Login.x + (Login.width - WidthText) * 0.5f, Login.y + Login.height + 50 };
    DrawText("Logout", (int) LoginText.x, (int) LoginText.y, 20, BLACK);

    if (CheckCollisionPointRec(Mouse, ManageBooksBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        APP_STATE = MANAGEBOOKS; return 1;
    }
    if (CheckCollisionPointRec(Mouse, ManageUserBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        APP_STATE = MANAGEUSER; return 1;
    }
    if (CheckCollisionPointRec(Mouse, ManageBorrowBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        APP_STATE = MANAGEBORROWING; return 1;
    }
    if (CheckCollisionPointRec(Mouse, Login) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        APP_STATE = LOGINAPP; return 1;
    }

    return 0;
}