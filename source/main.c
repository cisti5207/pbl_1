#include <stdio.h>
#include "ManageBooks.h"
#include "raylib.h"
#include "login.h"
#include "main.h"
#include <string.h>

AppState                    APP_STATE           =           LOGINAPP;
int main()
{
    InitWindow(1200, 800, "Login");
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(1000, 600);

    if (InitLogin() == LOGIN_SUCCESS) {
        APP_STATE = HOME;
    } else {
        printf("Login failed.\n");
        return 1;
    }

    InitManageBooks();

    printf("Exiting login screen...\n");

    return 0;
}


