.PHONY:all clean
CC=gcc
CFLAGS=-Wall -g
BIN=ex01 ex02 ex03
all:$(BIN)
%.o:%.c
	$(CC) $(CFLAG) -c $< -o $@
clean:
	rm -f *.o $(BIN)

