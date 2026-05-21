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

typedef struct {
    Rectangle PanelBox;
    Rectangle DecribeBox;
    Rectangle FuncBox;
    Rectangle ShowFuncBox;

} MainContainers;

void MainLoadContainers(MainContainers *Containers, Size MainSize);
void MainInitParticles(Size MainSize);
void MainUpdateParticlesPosition(Size MainSize);
void MainDrawParticle(void);
void MainDrawConnection(void);
Color MainAnimatedBackground(void);
void MainDrawPanel(MainContainers Containers);
bool MainFunc(Rectangle ShowFuncBox, Vector2 Mouse);


#endif // MAIN_H