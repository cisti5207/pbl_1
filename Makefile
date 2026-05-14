CC = gcc
CFLAGS = -Wall -std=c99 -IC:/raylib/include -Iinclude
LDFLAGS = -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = source/*.c
OUT = app.exe

all:
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS)

run:
	$(OUT)