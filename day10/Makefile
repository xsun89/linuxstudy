.PHONY:all clean
CC=gcc
CFLAGS=-Wall -g
BIN=p2psrv p2pclt
all:$(BIN)
%.o:%.c
	$(CC) $(CFLAG) -c $< -o $@
clean:
	rm -f *.o $(BIN)

