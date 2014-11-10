.PHONY:all clean
CC=gcc
CFLAGS=-Wall -g
BIN=01srv
all:$(BIN)
%.o:%.c
	$(CC) $(CFLAG) -c $< -o $@
clean:
	rm -f *.o $(BIN)

