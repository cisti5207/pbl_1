#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "../include/main.h"

void main()
{
    SetConsole();
    Title(46, 20);
    Loading(70, 35);
    showUI_Login(70, 35, Nhappass);
    getchar();
}

void showUI_Login(int x, int y, int state)
{
    int i = 0;
    gotoxy(x, y + i++);     printf ("╔══════════════════════════════════════════════════════════════════════════════════════════════════╗");
    gotoxy(x, y + i++);     printf ("║                                                                                                  ║");
    gotoxy(x, y + i++);     printf ("║                                                                                                  ║");
    gotoxy(x, y + i++);     printf ("║                                                                                                  ║");
    gotoxy(x, y + i++);     printf ("╠══════════════════════════════════════════════════════════════════════════════════════════════════╣");
    gotoxy(x, y + i++);     printf ("║                                                                                                  ║");
    gotoxy(x, y + i++);     printf ("╠══════════════════════════════════════════════════════════════════════════════════════════════════╣");
    gotoxy(x, y + i++);     printf ("║                                                                                                  ║");
    gotoxy(x, y + i++);     printf ("╚══════════════════════════════════════════════════════════════════════════════════════════════════╝");

    if (state == Nhappass)
    {
        gotoxy(x + 45, y + 2);  printf ("LOGIN 👤");
        gotoxy(x + 2, y + 5);   printf ("Tên tài khoản: ");
        gotoxy(x + 2, y + 7);   printf ("Mật khẩu: ");
    }
    else if (state == Changepass)
    {

    }
    else printf ("Eror!!!!");
}

void Loading(int x, int y)
{
    hideCursor();
    gotoxy(x, y);           printf ("█");
    gotoxy(x, y + 1);       printf ("╚");

    for (int i = 1; i < 100; i++)
    {
        gotoxy(x + i, y);           printf ("█");
        gotoxy(x + i, y + 1);       printf ("═");
        if (i <= 50) Sleep(15);
        else if (i <= 75) Sleep(20);
        else Sleep(5);
    }

    gotoxy(x + 100, y);           printf ("╗");
    gotoxy(x + 100, y + 1);       printf ("╝");

    gotoxy(x, y);       printf ("                                                                                                     ");
    gotoxy(x, y + 1);   printf ("                                                                                                     ");
    showCursor();
}

void Title(int x, int y)
{
    gotoxy(x, y);       printf ("████████╗██╗  ██╗██╗   ██╗     ██╗   ██╗██╗███████╗███╗   ██╗     ██╗  ██╗ ██████╗  █████╗ ███╗   ██╗██╗  ██╗ ██████╗  █████╗ ███╗   ██╗ ██████╗ ");
    gotoxy(x, y + 1);   printf ("╚══██╔══╝██║  ██║██║   ██║     ██║   ██║██║██╔════╝████╗  ██║     ██║  ██║██╔═══██╗██╔══██╗████╗  ██║██║  ██║██╔═══██╗██╔══██╗████╗  ██║██╔════╝ ");
    gotoxy(x, y + 2);   printf ("   ██║   ███████║██║   ██║     ██║   ██║██║█████╗  ██╔██╗ ██║     ███████║██║   ██║███████║██╔██╗ ██║███████║██║   ██║███████║██╔██╗ ██║██║  ███╗");
    gotoxy(x, y + 3);   printf ("   ██║   ██╔══██║██║   ██║     ╚██╗ ██╔╝██║██╔══╝  ██║╚██╗██║     ██╔══██║██║   ██║██╔══██║██║╚██╗██║██╔══██║██║   ██║██╔══██║██║╚██╗██║██║   ██║");   
    gotoxy(x, y + 4);   printf ("   ██║   ██║  ██║╚██████╔╝      ╚████╔╝ ██║███████╗██║ ╚████║     ██║  ██║╚██████╔╝██║  ██║██║ ╚████║██║  ██║╚██████╔╝██║  ██║██║ ╚████║╚██████╔╝");
    gotoxy(x, y + 5);   printf ("   ╚═╝   ╚═╝  ╚═╝ ╚═════╝        ╚═══╝  ╚═╝╚══════╝╚═╝  ╚═══╝     ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═══╝ ╚═════╝ ");
}

void gotoxy(int x, int y) 
{
    COORD coord;
    coord.X = x; 
    coord.Y = y; 
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}


void SetConsole() 
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    HWND hwnd = GetConsoleWindow();

    // 1. Phóng to cửa sổ trước
    ShowWindow(hwnd, SW_MAXIMIZE);
    

    // 2. Lấy thông tin chính xác của cửa sổ sau khi đã phóng to
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);

    // Tính toán kích thước hiện tại của cửa sổ (tính bằng số ký tự)
    short winWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    short winHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    // 3. ÉP Buffer phải bằng khít với Window
    // Điều này sẽ triệt tiêu hoàn toàn thanh cuộn
    COORD newSize;
    newSize.X = winWidth;
    newSize.Y = winHeight;
    
    // Thực hiện ép kích thước bộ đệm
    if (!SetConsoleScreenBufferSize(hOut, newSize)) {
        // Nếu lỗi, có thể do winHeight quá lớn so với giới hạn font
        // Ta thử lại với giá trị an toàn hơn từ csbi
        SetConsoleScreenBufferSize(hOut, newSize);
    }

    // 4. Khóa cứng viền không cho kéo giãn
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~WS_THICKFRAME; // Chặn kéo viền
    style &= ~WS_MAXIMIZEBOX; // Chặn nút phóng to (vì đã max rồi)
    SetWindowLong(hwnd, GWL_STYLE, style);

    // Cập nhật thay đổi
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
    
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
}

void hideCursor() 
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursor;
    GetConsoleCursorInfo(h, &cursor);
    cursor.bVisible = FALSE; // ẩn
    SetConsoleCursorInfo(h, &cursor);
}

void showCursor() 
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursor;
    GetConsoleCursorInfo(h, &cursor);
    cursor.bVisible = TRUE; // hiện
    SetConsoleCursorInfo(h, &cursor);
}
