#ifndef LIBMANAGE_H
#define LIBMANAGE_H

#include "raylib.h"

#define MAX_INPUT 256

typedef struct {
    Rectangle box;
    char text[MAX_INPUT];
    int length;
    bool isFocused;
} InputBox;

void GetSize(int *screenWidth, int *screenHeight, int *formX, int *formY, int *formWidth, int *formHeight);

Font SetFontUTF8(const char *fontPath, int fontSize);
int lenStringUTF8(const char *str);
void trim(char *str);
void DeleteLastChar(InputBox *input);
void UpdateInputBox(InputBox *input);
void DrawIcon(Rectangle box, Texture2D icon);


#endif // LIBMANAGE_H