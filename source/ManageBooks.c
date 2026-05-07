#include "ManageBooks.h"
#include "raylib.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

int InitManageBooks() {
    
    SetWindowTitle("Manage Books");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Manage Books Screen", 20, 20, 40, DARKGRAY);
        EndDrawing();
    }

    return 1;
}