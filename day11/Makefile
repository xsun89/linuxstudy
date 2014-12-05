.PHONY:all clean
CC=gcc
CFLAGS=-Wall -g
BIN=demo01
all:$(BIN)
%.o:%.c
	$(CC) $(CFLAG) -c $< -o $@
clean:
	rm -f *.o $(BIN)

