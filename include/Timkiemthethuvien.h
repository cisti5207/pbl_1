#ifndef TIMKIEMTHE_H
#define TIMKIEMTHE_H
#include "raylib.h"
#include "ManageUser.h"
typedef struct {
    InputBox_BD nhap;
    float scroll;
} FormTimKiemThe;

void InitTimKiemThe(FormTimKiemThe *Form);
void UpdateTimKiemThe(FormTimKiemThe *Form);
void DrawTimKiemThe(FormTimKiemThe *Form, Font font, BanDoc *headThe); 
#endif 
