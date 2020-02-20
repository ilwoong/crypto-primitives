CC = gcc
CFLAGS = -O2
TARGET = cham_test

.PHONY: clean

cham_test: cham_test.c cham.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm $(TARGET) -rf
