#include <stdio.h>
#include "ManageBooks.h"
#include "raylib.h"
#include "login.h"
#include "main.h"
#include <string.h>
#include <math.h>

Particle MAIN_Particles[MAX_PARTICLE];

AppState APP_STATE = LOGINAPP;
int main()
{
    InitWindow(1200, 800, "Login");
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(1000, 600);

    Account _Account;
    if (InitLogin(&_Account) == LOGIN_SUCCESS) {
        APP_STATE = HOME;
    } else {
        printf("Login failed.\n");
        return 1;
    }

    Role _role;
    if (strcmp(_Account.role, "Administrator") == 0)
        _role = ADMINISTRATOR;
    else 
        _role = STAFF;

    Size MainSize;
    MainContainers Containers;
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
            GetScreenWidth() / GetMonitorWidth(0),
            GetScreenHeight() / GetMonitorHeight(0)
        },
        GetMousePosition()
    );

    MainLoadContainers(
        &Containers,
        MainSize
    );

    
    MainInitParticles(MainSize);
    MainUpdateParticlesPosition(MainSize);
    
    while (!WindowShouldClose()){
        MainUpdateParticlesPosition(MainSize);
        MainSize.Mouse = GetMousePosition();
        if (IsWindowResized()){
            LoadSize(
                &MainSize,
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
        }
        
        MainLoadContainers(
            &Containers,
            MainSize
        );

        BeginDrawing();
        ClearBackground(MainAnimatedBackground());

        MainDrawParticle();
        MainDrawConnection();

        MainDrawPanel(
            Containers
        );

        MainFunc(Containers.ShowFuncBox, MainSize.Mouse);
        
        EndDrawing();

        switch(APP_STATE){
            case HOME:
                SetWindowTitle("Home");
                break;
            
            case LOGINAPP:
                SetWindowTitle("Login");
                
                _Account = (Account) {0};
                if (InitLogin(&_Account) == LOGIN_SUCCESS)
                    APP_STATE = HOME;
                else {
                    printf("Login failed.\n");
                    return 1;
                }

                printf ("| %s | %s | %s | %s | %s | %s |\n",
                    _Account.username,
                    _Account.realName,
                    _Account.password,
                    _Account.cccd,
                    _Account.dateOfBirth,        
                    _Account.role
                );

                break;

            case MANAGEBOOKS:
                SetWindowTitle("Manage Books");
                
                InitManageBooks(_role);
                
                APP_STATE = HOME;
                
                break;

            case MANAGEUSER:
                SetWindowTitle("Manage User");
                
                break;

            case MANAGEBORROWING:
                SetWindowTitle("Manage Borrow");
                
                break;
        }

    }

    printf("Exiting login screen...\n");

    return 0;
}

void MainLoadContainers(MainContainers *Containers, Size MainSize){
    Containers->PanelBox = (Rectangle) {
        MainSize.Screen.x * 0.15f * pow(0.9, (double) MainSize.Scale.x),
        MainSize.Screen.y * 0.1f * pow(0.9, (double) MainSize.Scale.y),
        MainSize.Screen.x - 2 * Containers->PanelBox.x,
        MainSize.Screen.y - Containers->PanelBox.y 
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
        (Containers->PanelBox.height + Containers->PanelBox.y - Containers->ShowFuncBox.y) + Containers->PanelBox.width * 0.01f
    };
}
void MainInitParticles(Size MainSize){
    for (int i = 0; i < MAX_PARTICLE; i++){
        MAIN_Particles[i].Position = (Vector2) {
            (float)(GetRandomValue(0, MainSize.Monitor.x)),
            (float)(GetRandomValue(0, MainSize.Monitor.y))
        };

        MAIN_Particles[i].Velocity = (Vector2) {
            (GetRandomValue(-50, 50) / 100.0f - 0.5f) * 0.3f,
            -0.2f - GetRandomValue(-50, 50) / 500.0f
        };

        MAIN_Particles[i].radius = 1.0f + GetRandomValue(0, 4);
        MAIN_Particles[i].alpha = 0.1f + GetRandomValue(0, 100) / 200.0f;
    }
}
void MainUpdateParticlesPosition(Size MainSize){
    for (int i = 0; i < MAX_PARTICLE; i++){
        MAIN_Particles[i].Position.x += MAIN_Particles[i].Velocity.x;
        MAIN_Particles[i].Position.y += MAIN_Particles[i].Velocity.y;

        if (MAIN_Particles[i].Position.x < -10) 
            MAIN_Particles[i].Position.x = MainSize.Monitor.x + 10;

        if (MAIN_Particles[i].Position.x > MainSize.Monitor.x + 10)
            MAIN_Particles[i].Position.x = -10;

        if (MAIN_Particles[i].Position.y < -10){
            MAIN_Particles[i].Position.y = MainSize.Monitor.y + 10;
            MAIN_Particles[i].Position.x = (float)(GetRandomValue(0, MainSize.Monitor.x));
        }
    }
}
void MainDrawParticle(void){
    for (int i = 0; i < MAX_PARTICLE; i++){
        DrawCircleV(
            MAIN_Particles[i].Position,
            MAIN_Particles[i].radius,
            Fade(SKYBLUE, MAIN_Particles[i].alpha)
        );
    }
}
void MainDrawConnection(void){
    for (int i = 0; i < MAX_PARTICLE; i++){
        for (int j = i + 1; j < MAX_PARTICLE; j++)
        {
            float dx = MAIN_Particles[i].Position.x - MAIN_Particles[j].Position.x;
            float dy = MAIN_Particles[i].Position.y - MAIN_Particles[j].Position.y;

            float distance = sqrtf (dx * dx + dy * dy);

            if (distance < 120.0f) {
                float alpha = 1.0f - (distance / 120.0f);
                float thickness = alpha * 1.5f;
                DrawLineEx(
                    MAIN_Particles[i].Position,
                    MAIN_Particles[j].Position,
                    thickness,
                    Fade(WHITESMOKE, alpha * 0.15f)
                );
            }
        }
    }
}
Color MainAnimatedBackground(void){
    float t = GetTime();

    int r = 10 + (int)(sin(t * 0.4f) * 10.0f);
    int g = 20 + (int)(sin(t * 0.7f) * 15.0f);
    int b = 35 + (int)(sin(t * 0.3f) * 15.0f);

    return (Color){ r, g, b, 255 };
}
void MainDrawPanel(MainContainers Containers){
    float rounded = Containers.PanelBox.width * 0.01f;

    DrawRectangleRounded (
        Containers.PanelBox,
        FindRoundness(
            rounded, 
            Containers.PanelBox.width, 
            Containers.PanelBox.height
        ),
        10,
        Fade(SOFTWHITE, 0.1f)
    );

    DrawRectangleRounded(
        Containers.DecribeBox,
        FindRoundness(
            rounded, 
            Containers.DecribeBox.width, 
            Containers.DecribeBox.height
        ),
        10,
        BRIGHTWHITE
    );
    
    DrawRectangleRounded (
        Containers.FuncBox,
        FindRoundness(
            rounded, 
            Containers.FuncBox.width, 
            Containers.FuncBox.height
        ),
        10,
        BRIGHTWHITE
    );

    DrawRectangleRounded (
        Containers.ShowFuncBox,
        FindRoundness(
            rounded, 
            Containers.ShowFuncBox.width, 
            Containers.ShowFuncBox.height
        ),
        10,
        BRIGHTWHITE
    );
}
void MainFunc(Rectangle ShowFuncBox, Vector2 Mouse){
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

    Rectangle  Login = {
        ShowFuncBox.x + widthCard * ratioDistance + (float) i++ * widthCard,
        ShowFuncBox.y + ShowFuncBox.height * 0.1f,
        widthCard * (1.0f - ratioDistance),
        ShowFuncBox.height * 0.7f
    };

    float roundness = widthCard * 0.1f;

    float WidthText;

    DrawRectangleRounded(
        ManageBooksBox,
        FindRoundness(roundness, ManageBooksBox.width, ManageBooksBox.height),
        10,
        BLACK
    );

    WidthText = MeasureText ("Manage Books", 20);
    Vector2 ManageBooksText = {
        ManageBooksBox.x + (ManageBooksBox.width - WidthText) * 0.5f,
        ManageBooksBox.y + ManageBooksBox.height + 50 
    };

    DrawText("Manage Books", (int) ManageBooksText.x, (int) ManageBooksText.y, 20, BLACK);
    
    DrawRectangleRounded(
        ManageUserBox,
        FindRoundness(roundness, ManageUserBox.width, ManageUserBox.height),
        10,
        BLACK
    );
    
    DrawRectangleRounded(
        ManageBorrowBox,
        FindRoundness(roundness, ManageBorrowBox.width, ManageBorrowBox.height),
        10,
        BLACK
    );

    DrawRectangleRounded(
        Login,
        FindRoundness(roundness, Login.width, Login.height),
        10,
        BLACK
    );

    if (CheckCollisionPointRec(Mouse, ManageBooksBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        APP_STATE = MANAGEBOOKS;
        return;
    }
    
    if (CheckCollisionPointRec(Mouse, ManageUserBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        APP_STATE = MANAGEUSER;
        return;
    }

    if (CheckCollisionPointRec(Mouse, ManageBorrowBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        APP_STATE = MANAGEBORROWING;
        return;
    }
    
    if (CheckCollisionPointRec(Mouse, Login) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        APP_STATE = LOGINAPP;
        return;
    }

    return;
}
