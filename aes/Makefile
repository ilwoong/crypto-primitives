CC = gcc
CFLAGS = -maes
TARGETS = aesni

.PHONY: all clean

all: $(TARGETS)

aesni: aes.ni.c aes_test.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf *.o
	rm -rf $(TARGETS)