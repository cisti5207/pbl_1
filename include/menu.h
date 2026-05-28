#ifndef MENUCHINH_H
#define MENUCHINH_H

#include "raylib.h"

// Trạng thái menu trả về cho main
typedef enum {
    APP_MENU = 0,
    APP_TAO_THE,
    APP_TAO_PHIEU,
    APP_XEM_DANH_SACH,
    APP_LICH_SU,
    APP_TRA_SACH
} AppState;

AppState DrawAndHandleMenu(Font font, float screenW, float screenH);

#endif