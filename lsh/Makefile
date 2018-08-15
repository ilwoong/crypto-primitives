CC = gcc
TARGET = lsh_test lsh_test_sse4

.PHONY: all clean

all: $(TARGET)

lsh_test: lsh_test.c lsh256.c lsh512.c
	$(CC) $^ -o $@

lsh_test_sse4: lsh_test.c lsh256.sse4.c lsh512.sse4.c
	$(CC) $^ -o $@ -msse4

clean:
	rm $(TARGET) -rf
