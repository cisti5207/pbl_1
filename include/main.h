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
    STATISTIC,
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
void LoadUserAvatar(Account acc, Texture2D *avatar);
void MainDrawDescribeBox(Rectangle box, Account acc, Texture2D avatar, Font fontLarge, Font fontSmall);
#endif // MAIN_H