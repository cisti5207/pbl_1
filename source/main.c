#include <stdio.h>
#include "ManageBooks.h"
#include "raylib.h"
#include "login.h"
#include "main.h"
#include <string.h>
#include <math.h>

Particle MAIN_Particles[MAX_PARTICLE];


Rectangle MAIN_Screen;
Rectangle MAIN_Monitor;
Vector2 MAIN_Scale;
Vector2 MAIN_Mouse;

AppState                    APP_STATE           =           LOGINAPP;
int main()
{
    InitWindow(1200, 800, "Login");
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(1000, 600);

    MAIN_Monitor = (Rectangle) {
        0,
        0,
        GetMonitorWidth(0),
        GetMonitorHeight(0)
    };
    MAIN_LoadSize();
    MAIN_InitParticles();

    Rectangle PanelBox;
    Rectangle DecribeBox;
    Rectangle FuncBox;
    Rectangle ShowFuncBox;

    Account _Account;

    if (InitLogin(&_Account) == LOGIN_SUCCESS) {
        APP_STATE = HOME;
    } else {
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

    while (!WindowShouldClose()){
        MAIN_UpdateParticlesPosition();
        MAIN_LoadSize();
        MAIN_LoadPanel(&PanelBox, &DecribeBox, &FuncBox, &ShowFuncBox);

        BeginDrawing();
        ClearBackground(MAIN_AnimatedBackground());

        MAIN_DrawParticle();
        MAIN_DrawConnection();

        MAIN_DrawPanel(PanelBox, DecribeBox, FuncBox, ShowFuncBox);
        MAIN_Func(ShowFuncBox);

        switch(APP_STATE){
            case HOME:
                break;
            
            case LOGINAPP:
                if (InitLogin(&_Account) == LOGIN_SUCCESS) {
                    APP_STATE = HOME;
                } else {
                    printf("Login failed.\n");
                    return 1;
                }
                break;

            case MANAGEBOOKS:
                InitManageBooks();
                APP_STATE = HOME;
                break;

            case MANAGEUSER:
                break;

            case MANAGEBORROWING:
                break;
        }

        EndDrawing();
    }

    

    printf("Exiting login screen...\n");

    return 0;
}

void MAIN_LoadPanel(Rectangle *PanelBox, Rectangle *DecribeBox, Rectangle *FuncBox, Rectangle *ShowFuncBox){
    *PanelBox = (Rectangle) {
        MAIN_Screen.width * 0.15f * pow(0.9, (double) MAIN_Scale.x),
        MAIN_Screen.height * 0.1f * pow(0.9, (double) MAIN_Scale.y),
        MAIN_Screen.width - 2 * PanelBox->x,
        MAIN_Screen.height - PanelBox->y 
    };

    *DecribeBox = (Rectangle) {
        PanelBox->x,
        PanelBox->y,
        PanelBox->width,
        PanelBox->height * 0.2f
    };

    *FuncBox = (Rectangle) {
        PanelBox->x,
        DecribeBox->y + DecribeBox->height + PanelBox->height * 0.02f,
        PanelBox->width,
        PanelBox->height * 0.05f
    };

    *ShowFuncBox = (Rectangle) {
        PanelBox->x,
        FuncBox->y + FuncBox->height + PanelBox->height * 0.02f,
        PanelBox->width,
        (PanelBox->height + PanelBox->y - ShowFuncBox->y) + PanelBox->width * 0.01f
    };
}

void MAIN_LoadSize(void){
    MAIN_Screen = (Rectangle) {
        0,
        0,
        GetScreenWidth(),
        GetScreenHeight()
    };

    MAIN_Scale = (Vector2) {
        MAIN_Screen.width / MAIN_Monitor.width,
        MAIN_Screen.height / MAIN_Monitor.height
    };


    MAIN_Mouse = GetMousePosition();
}

void MAIN_InitParticles(void){
    for (int i = 0; i < MAX_PARTICLE; i++){
        MAIN_Particles[i].Position = (Vector2) {
            (float)(GetRandomValue(0, MAIN_Monitor.width)),
            (float)(GetRandomValue(0, MAIN_Monitor.height))
        };

        MAIN_Particles[i].Velocity = (Vector2) {
            (GetRandomValue(-50, 50) / 100.0f - 0.5f) * 0.3f,
            -0.2f - GetRandomValue(-50, 50) / 500.0f
        };

        MAIN_Particles[i].radius = 1.0f + GetRandomValue(0, 4);
        MAIN_Particles[i].alpha = 0.1f + GetRandomValue(0, 100) / 200.0f;
    }
}

void MAIN_UpdateParticlesPosition(void){
    for (int i = 0; i < MAX_PARTICLE; i++){
        MAIN_Particles[i].Position.x += MAIN_Particles[i].Velocity.x;
        MAIN_Particles[i].Position.y += MAIN_Particles[i].Velocity.y;

        if (MAIN_Particles[i].Position.x < -10) 
            MAIN_Particles[i].Position.x = MAIN_Monitor.width + 10;

        if (MAIN_Particles[i].Position.x > MAIN_Monitor.width + 10)
            MAIN_Particles[i].Position.x = -10;

        if (MAIN_Particles[i].Position.y < -10){
            MAIN_Particles[i].Position.y = MAIN_Monitor.height + 10;
            MAIN_Particles[i].Position.x = (float)(GetRandomValue(0, MAIN_Monitor.width));
        }
    }
}

void MAIN_DrawParticle(void){
    for (int i = 0; i < MAX_PARTICLE; i++){
        DrawCircleV(
            MAIN_Particles[i].Position,
            MAIN_Particles[i].radius,
            Fade(SKYBLUE, MAIN_Particles[i].alpha)
        );
    }
}

void MAIN_DrawConnection(void){
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

Color MAIN_AnimatedBackground(void){
    float t = GetTime();

    int r = 10 + (int)(sin(t * 0.4f) * 10.0f);
    int g = 20 + (int)(sin(t * 0.7f) * 15.0f);
    int b = 35 + (int)(sin(t * 0.3f) * 15.0f);

    return (Color){ r, g, b, 255 };
}

void MAIN_DrawPanel(Rectangle PanelBox, Rectangle DecribeBox, Rectangle FuncBox, Rectangle ShowFuncBox){
    float rounded = PanelBox.width * 0.01f;

    DrawRectangleRounded (
        PanelBox,
        FindRoundness(rounded, PanelBox.width, PanelBox.height),
        10,
        Fade(SOFTWHITE, 0.1f)
    );

    DrawRectangleRounded(
        DecribeBox,
        FindRoundness(rounded, DecribeBox.width, DecribeBox.height),
        10,
        BRIGHTWHITE
    );
    
    DrawRectangleRounded (
        FuncBox,
        FindRoundness(rounded, FuncBox.width, FuncBox.height),
        10,
        BRIGHTWHITE
    );

    DrawRectangleRounded (
        ShowFuncBox,
        FindRoundness(rounded, ShowFuncBox.width, ShowFuncBox.height),
        10,
        BRIGHTWHITE
    );
}

void MAIN_Func(Rectangle ShowFuncBox){
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

    DrawRectangleRounded(
        ManageBooksBox,
        FindRoundness(roundness, ManageBooksBox.width, ManageBooksBox.height),
        10,
        BLACK
    );
    
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

    if (CheckCollisionPointRec(MAIN_Mouse, ManageBooksBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        APP_STATE = MANAGEBOOKS;
        return;
    }
    
    if (CheckCollisionPointRec(MAIN_Mouse, ManageUserBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        APP_STATE = MANAGEUSER;
        return;
    }

    if (CheckCollisionPointRec(MAIN_Mouse, ManageBorrowBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        APP_STATE = MANAGEBORROWING;
        return;
    }
    
    if (CheckCollisionPointRec(MAIN_Mouse, ManageBooksBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        APP_STATE = LOGINAPP;
        return;
    }


    return;
}
