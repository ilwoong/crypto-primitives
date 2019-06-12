CC = gcc
CFLAGS = -O2
LDFLAGS = -lgomp
TARGET = lea lea_ref lea_avx2

.PHONY: all clean

all : $(TARGET)

lea: lea.keyschedule.c lea.c lea_test.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

lea_ref: lea.keyschedule.c lea.ref.c lea_test.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

lea_avx2: lea.keyschedule.c lea.avx2.c lea_test.avx2.c
	$(CC) $(CFLAGS) -mavx2 $^ -o $@ $(LDFLAGS)

clean:
	rm $(TARGET) -rf
