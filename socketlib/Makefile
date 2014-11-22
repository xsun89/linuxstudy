.PHONY:all clean
CC=gcc
CFLAGS=-Wall -g
BIN=testclient testserver
all:$(BIN)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

testclient:testclient.o commsocket.o
    $(CC) $(CFLAGS) $^ -o $@
testserver:testserver.o commcocket.o
    $(CC) $(CFLAGS) $^ -o $@
clean:
	rm -f *.o $(BIN)

