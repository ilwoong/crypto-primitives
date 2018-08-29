CC = gcc
TARGET = lsh_test lsh_test_sse4 lsh_test_avx2

.PHONY: all clean

all: $(TARGET)

lsh_test: lsh_test.c lsh256.c lsh512.c
	$(CC) $^ -o $@

lsh_test_sse4: lsh_test.c lsh256.sse4.c lsh512.sse4.c
	$(CC) $^ -o $@ -msse4

lsh_test_avx2: lsh_test.c lsh256.avx2.c lsh512.avx2.c
	$(CC) $^ -o $@ -mavx2

clean:
	rm $(TARGET) -rf
