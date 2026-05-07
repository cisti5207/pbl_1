#ifndef MAIN_H
#define MAIN_H

#include "raylib.h"
#include "login.h"

typedef enum{
    LOGINAPP,
    HOME,
    MANAGE_BOOKS,
    MANAGE_MEMBERS,
    MANAGE_BORROWING,
    MANAGE_RETURNING,
} AppState;

#endif // MAIN_H