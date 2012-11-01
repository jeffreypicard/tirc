CC=gcc
WARN=-Wall
DEBUG=-g
CFLAGS=$(DEBUG) $(WARN) -lpthread
DAEMON_BIN=tircd

tirc_server: tirc.c tirc.h
	$(CC) $(CFLAGS) $^ -o $(DAEMON_BIN)

.PHONY: clean

clean:
	-rm -rf *.o tircd tirc
