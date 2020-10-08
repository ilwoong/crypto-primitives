CC = gcc
CFLAGS = -O2
LDFLAGS = -lgomp
TARGET = aria arialut print_tables

.PHONY: all clean

all : $(TARGET)

aria: aria.c aria_sbox.c aria_test.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

arialut: aria.lut.c aria_sbox.c aria_test.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

print_tables : print_tables.c aria_sbox.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm $(TARGET) -rf
