CC = gcc
CFLAGS = -O2
LDFLAGS = -lgomp
TARGET = hight_ref

.PHONY: all clean

all : $(TARGET)

hight_ref: hight.ref.c hight_test.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm $(TARGET) -rf
