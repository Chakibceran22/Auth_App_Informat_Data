CC = gcc
CFLAGS = -Wall
LIBS = -lssl -lcrypto

# Detect OS
ifeq ($(OS),Windows_NT)
    TARGET = app.exe
    LIBS += -lws2_32
else
    TARGET = app
endif

$(TARGET): main.c auth.c utils.c validation.c
	$(CC) $(CFLAGS) main.c auth.c utils.c validation.c -o $(TARGET) $(LIBS)

clean:
	rm -f app app.exe *.o