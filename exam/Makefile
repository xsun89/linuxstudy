.PHONY:all clean
CC=gcc
CFLAGS=-Wall -g
BIN=exam3
all:$(BIN)
%.o:%.c
	$(CC) $(CFLAG) -c $< -o $@
clean:
	rm -f *.o $(BIN)

