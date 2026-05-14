#ifndef LIBMANAGE_H
#define LIBMANAGE_H

#include "raylib.h"

#define MAX_INPUT 256

#define BRIGHTWHITE     (Color) {255, 255, 255, 255}
#define BRIGHTGRAY      (Color) {209, 209, 209, 255}
#define WHITESMOKE      (Color) {242, 242, 242, 255}
#define TEALBLUE        (Color) {21, 96, 130, 255}
#define SILVERGRAY      (Color) {191, 191, 191, 255}
#define NAVYBLUE        (Color){0, 0, 128, 255}
#define RICHBLACK       (Color){13, 17, 23, 255}
#define DARKNAVY        (Color){22, 27, 34, 255}
#define STEELBLUE       (Color){47, 93, 159, 255}
#define BURGUNDY        (Color){128, 0, 32, 255}
#define WINERED         (Color){114, 47, 55, 255}
#define SOFTWHITE       (Color){230, 237, 243, 255}
#define GRAYBLUE        (Color){139, 148, 158, 255}
#define CYBERBLUE       (Color){0, 180, 255, 255}
#define DEEPBLUE        (Color){29, 78, 216, 255}
#define ROYALPURPLE     (Color){124, 58, 237, 255}
#define GOLDACCENT      (Color){212, 160, 23, 255}
#define SUCCESSGREEN    (Color){46, 204, 113, 255}
#define WARNINGORANGE   (Color){243, 156, 18, 255}
#define ERRORRED        (Color){231, 76, 60, 255}
#define DARKBLUE1       (Color){0, 14, 46, 255}

#define QuantityFont 32
#define FONT_SIZE 18

#define Arial "font/arial/arial.ttf"
#define ArialBold "font/arial/arialbd.ttf"
#define Cooper "font/cooper/COOPBL.ttf"
#define Times "font/times/times.ttf"
#define Impact "font/impact/impact.ttf"
#define Roboto "font/roboto/Roboto-Regular.ttf"
#define Roboto_Semibold "font/roboto/Roboto-Semibold.ttf"
#define Gill "font/gill/gillubcd.ttf"

#define Icon_Find_lnk "img/icon5.png"

typedef enum{
    IDLE = 0,
    TRUE,
    FALSE,
    NOT_FOUND,
    FOUND,
    FAILED
} CheckResult;
typedef enum {
    STAFF = 0,
    ADMINISTRATOR
} Role;

typedef struct {
    Rectangle box;
    char text[MAX_INPUT];
    int length;
    bool isFocused;
} InputBox;

typedef struct Particle {
    Vector2 Position;
    Vector2 Velocity;
    float radius;
    float alpha;
} Particle;
typedef struct {
    Vector2 Mouse;
    Vector2 Screen;
    Vector2 Monitor;
    Vector2 Scale;
} Size;

void LoadSize(Size *FormSize, Vector2 monitor, Vector2 screen, Vector2 scale, Vector2 mouse);

Font SetFontUTF8(const char *fontPath, int fontSize);
int lenStringUTF8(const char *str);
void trim(char *str);

void DeleteLastWord(InputBox *input);
void PasteClipboard(InputBox *input);
void DeleteLastChar(InputBox *input);
void UpdateInputBox(InputBox *input);
int UTF8Width(const char *s, int width);

void DrawIcon(Rectangle box, Texture2D icon);

float FindRoundness(float len, float width, float height);



#endif // LIBMANAGE_H