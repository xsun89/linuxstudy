.PHONY:all clean
CC=gcc
CFLAGS=-Wall -g
BIN=demo01 01msg echo_cli echo_srv
all:$(BIN)
%.o:%.c
	$(CC) $(CFLAG) -c $< -o $@
clean:
	rm -f *.o $(BIN)

