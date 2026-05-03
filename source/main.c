#include <stdio.h>
#include "raylib.h"
#include "login.h"
#include <string.h>

int main()
{
    InitWindow(1200, 800, "Login");\
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(1000, 600);

    InitLogin();

    printf("Exiting login screen...\n");

    return 0;
}