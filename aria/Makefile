CC = gcc
CFLAGS = -O2
TARGET = aria

.PHONY: all clean

all : $(TARGET)

aria: aria.c aria_sbox.c aria_test.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm $(TARGET) -rf
