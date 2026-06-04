CC = gcc

CFLAGS = -Wall -std=c99 -Iinclude

LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm
SRC = source/*.c
OUT = app.exe

all:
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS)

run:
	./$(OUT)

clean:
	del /f $(OUT)