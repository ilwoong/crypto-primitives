CC = gcc
CFLAGS = -O2
TARGET = test_seed seed_tool

.PHONY: clean

all : test_seed seed_tool

test_seed: seed.c test_seed.c
	$(CC) $(CFLAGS) $^ -o $@

seed_tool: seed_tool.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm $(TARGET) -rf
