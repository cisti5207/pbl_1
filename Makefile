CC = gcc
CFLAGS = -Wall -std=c99 -I"C:/Users/HOAN/Downloads/Ảnh đồ/raylib-6.0_win64_msvc16/raylib-6.0_win64_msvc16/include" -Iinclude
LDFLAGS = -L"C:/Users/HOAN/Downloads/Ảnh đồ/raylib-6.0_win64_msvc16/raylib-6.0_win64_msvc16/lib" -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = source/*.c
OUT = app.exe

all:
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS)

run:
	$(OUT)