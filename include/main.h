#ifndef MAIN_H
#define MAIN_H

#include "raylib.h"

#define MAX_PARTICLE 256



typedef enum{
    LOGINAPP,
    HOME,
    MANAGEBOOKS,
    MANAGEUSER,
    MANAGEBORROWING,
} AppState;

void MAIN_LoadPanel(Rectangle *PanelBox, Rectangle *DecribeBox, Rectangle *FuncBox, Rectangle *ShowFuncBox);
void MAIN_LoadSize();
void MAIN_InitParticles(void);
void MAIN_UpdateParticlesPosition(void);
void MAIN_DrawParticle(void);
void MAIN_DrawConnection(void);
Color MAIN_AnimatedBackground(void);
void MAIN_DrawPanel(Rectangle PanelBox, Rectangle DecribeBox, Rectangle FuncBox, Rectangle ShowFuncBox);
void MAIN_Func(Rectangle ShowFuncBox);


#endif // MAIN_H