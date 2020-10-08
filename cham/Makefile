CC = gcc
CFLAGS = -O2
TARGET = test_cham

.PHONY: clean

test_cham: test_cham.c cham.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm $(TARGET) -rf
