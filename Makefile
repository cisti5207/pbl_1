# Dùng đường dẫn chuẩn C:/msys64 giống ảnh bro vừa chụp
MSYS2_PATH = C:/Users/HOAN/Downloads/MSYS2/ucrt64

CC = C:/Users/HOAN/Downloads/MSYS2/ucrt64/bin/gcc.exe
# Đường dẫn include và lib
CFLAGS = -Wall -std=c99 -Iinclude -I$(MSYS2_PATH)/include
LDFLAGS = -L$(MSYS2_PATH)/lib -lraylib -lopengl32 -lgdi32 -lwinmm

# File nguồn và file xuất ra
SRC = source/*.c
OUT = app.exe

# Lệnh build (ĐÃ ĐƯỢC CHÈN SẴN DẤU TAB)
all:
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS)

# Lệnh clean (ĐÃ ĐƯỢC CHÈN SẴN DẤU TAB)
clean:
	del $(OUT)