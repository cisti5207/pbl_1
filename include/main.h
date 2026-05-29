#ifndef MAIN_H
#define MAIN_H

#include "raylib.h"
#include "libmanage.h"
#include "login.h"

typedef enum AppState {
    LOGINAPP = 0,
    HOME,
    MANAGEBOOKS,
    APP_MENU,          
    APP_TAO_THE,
    APP_TAO_PHIEU,
    APP_XEM_DANH_SACH,
    APP_LICH_SU,
    APP_TRA_SACH,
    APP_DOANH_THU
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