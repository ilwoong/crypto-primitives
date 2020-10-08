CC = gcc
CFLAGS = -O2
LDFLAGS = -lgomp
TARGETS = aes aeslut1 aeslut2 aeslut3 aesni print_tables

.PHONY: all clean

all: $(TARGETS)

aes : aes.c sbox.c gf256.c aes_test.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

aeslut1 : aes.lut1.c aes_test.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

aeslut2 : aes.lut2.c aes_test.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

aeslut3 : aes.lut3.c aes_test.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

aesni: aes.ni.c aes_test.c
	$(CC) $(CFLAGS) -maes $^ -o $@ $(LDFLAGS)

print_tables : print_tables.c mds.c sbox.c gf256.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf *.o
	rm -rf $(TARGETS)