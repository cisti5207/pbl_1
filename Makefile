CC = gcc
CFLAGS = -Wall -std=c99 -I"C:\Users\HOAN\Downloads\raylib-6.0_win64_mingw-w64\raylib-6.0_win64_mingw-w64\include" -Iinclude
LDFLAGS = -L"C:\Users\HOAN\Downloads\raylib-6.0_win64_mingw-w64\raylib-6.0_win64_mingw-w64\lib" -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = source/*.c
OUT = run.exe

all:
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS)

run:
	$(OUT)
