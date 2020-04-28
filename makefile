CC=gcc
CFLAGS="-O3"
INCLUDEDIR="-I./include"

all:
	@$(CC) $(CFLAGS) $(INCLUDEDIR) ./src/vigener.c -o ./bin/vigener
