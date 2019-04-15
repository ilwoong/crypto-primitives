CC = gcc
CFLAGS = 
LDFLAGS = -lgomp
TARGET = lea learef

.PHONY: all clean

all : lea learef

lea: lea.c lea_test.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

learef: lea.ref.c lea_test.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm $(TARGET) -rf
