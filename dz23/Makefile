CC = gcc
CFLAGS = -Wall -Wextra

all: server client service

server: server.c
	$(CC) $(CFLAGS) -o server server.c -lrt

client: client.c
	$(CC) $(CFLAGS) -o client client.c -lrt

service: service.c
	$(CC) $(CFLAGS) -o service service.c -lrt

clean:
	rm -f server client service