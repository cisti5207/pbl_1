#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <stdlib.h>

// ================= MENU =============================
void Menu();

// Khóa console mặc định và scroll
void FixAndLockConsole();

// Chuyển vị trí con trỏ
void gotoxy(int x, int y);

int main() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);    

    FixAndLockConsole();
    
    Menu();
    gotoxy(13, 70);

    char choose;
    scanf ("%c", &choose);
    switch(choose)
    {
        case '1':
            system("cls");
            system("build\\dataTruyen_menu.exe");
            return 0;
        case '2':
        case '3':
        case '4':
        default: break;
    }
    return 0;
}

//================== Menu =========================
void Menu()
{
    int x = 236;
    int y = x - 74;
    int z = x - 48; 
    printf("╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s████████╗██████╗ ██╗   ██╗██╗   ██╗███████╗███╗   ██╗   █████╗   █████╗ %*s║\n", y/2, "", y/2, "");
    printf("║%*s╚══██╔══╝██╔══██╗██║   ██║╚██╗ ██╔╝██╔════╝████╗  ██║  ██╔══██╗ ██╔══██╗%*s║\n", y/2, "", y/2, "");
    printf("║%*s   ██║   ██████╔╝██║   ██║ ╚████╔╝ █████╗  ██╔██╗ ██║  ╚█████╔╝ ╚█████╔ %*s║\n", y/2, "", y/2, "");
    printf("║%*s   ██║   ██╔══██╗██║   ██║  ╚██╔╝  ██╔══╝  ██║╚██╗██║  ██╔══██╗ ██╔══██╗%*s║\n", y/2, "", y/2, "");
    printf("║%*s   ██║   ██║  ██║╚██████╔╝   ██║   ███████╗██║ ╚████║  ╚█████╔╝ ╚█████╔╝%*s║\n", y/2, "", y/2, "");
    printf("║%*s   ╚═╝   ╚═╝  ╚═╝ ╚═════╝    ╚═╝   ╚══════╝╚═╝  ╚═══╝   ╚════╝   ╚════╝ %*s║\n", y/2, "", y/2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║%*s║\n", x - 2, "");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║%*s║\n", x - 2, "");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║%*s║\n", x - 2, "");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║%*s║\n", x - 2, "");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s╔════════════════════════════════════════════╗%*s║\n", z/2, "", z/2);
    printf("║%*s║   HỆ THỐNG QUẢN LÝ CHO THUÊ TRUYỆN TRANH   ║%*s║\n", z/2, "", z/2);
    printf("║%*s╠════════════════════════════════════════════╣%*s║\n", z/2, "", z/2);
    printf("║%*s║ [1] 📚 Dữ liệu truyện                      ║%*s║\n", z/2, "", z/2);
    printf("║%*s║ [2] 👤 Dữ liệu người thuê truyện           ║%*s║\n", z/2, "", z/2);
    printf("║%*s║ [3] 📊 Thống kê                            ║%*s║\n", z/2, "", z/2);
    printf("║%*s║ [4] ❌ Exit                                ║%*s║\n", z/2, "", z/2);
    printf("║%*s╚════════════════════════════════════════════╝%*s║\n", z/2, "", z/2);
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("║%*s║\n", x - 2, "");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║%*s║\n", x - 2, "");
    printf("╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║ 🟩 Choose: %*s║\n", x - 14, "");
    printf("╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n");
}


void gotoxy(int x, int y) 
{
    COORD coord;
    coord.X = x; 
    coord.Y = y; 
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void FixAndLockConsole() 
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    HWND hwnd = GetConsoleWindow();

    // 1. Phóng to cửa sổ trước
    ShowWindow(hwnd, SW_MAXIMIZE);
    
    // Đợi 1 chút để Windows ổn định kích thước cửa sổ mới
    Sleep(200); 

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
}