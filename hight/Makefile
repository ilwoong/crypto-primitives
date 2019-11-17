CC = gcc
CFLAGS = -O2
LDFLAGS = -lgomp
TARGET = hight_ref hight

.PHONY: all clean

all : $(TARGET)

hight: hight.c hight_test.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

hight_ref: hight.ref.c hight_test.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm $(TARGET) -rf
