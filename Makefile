CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

SRC = main.c $(wildcard src/*.c)
OUT = main

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

run: all
	./$(OUT)

clean:
	rm -f $(OUT)

.PHONY: all run clean
