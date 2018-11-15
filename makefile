CC=gcc
CFLAGS="-O2"
INCLUDEDIR="-I./include"

all:
	@$(CC) $(CFLAGS) $(INCLUDEDIR) ./src/vigener.c -o ./bin/vigener
