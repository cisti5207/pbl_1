#ifndef MAIN_H
#define MAIN_H

#include "raylib.h"
#include "libmanage.h"

typedef enum{
    LOGINAPP,
    HOME,
    MANAGEBOOKS,
    MANAGEUSER,
    MANAGEBORROWING,
} AppState;

typedef struct {
    Rectangle PanelBox;
    Rectangle DecribeBox;
    Rectangle FuncBox;
    Rectangle ShowFuncBox;
} MainContainers;

void MainLoadContainers(MainContainers *Containers, Size MainSize);
void MainDrawPanel(MainContainers Containers);
bool MainFunc(Rectangle ShowFuncBox, Vector2 Mouse);

#endif // MAIN_H