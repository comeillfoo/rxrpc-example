CC=gcc
CFLAGS=--std=c11 -Wall -pedantic -Wextra -Werror

all: server client

server: server.c rxrpc.h
	$(CC) $(CFLAGS) $< -o $@

client: client.c rxrpc.h
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f server client