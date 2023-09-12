.PHONY: server client
all: server client
server:
	-rm server
	gcc -o server server.c -lrt -lpthread
client:
	-rm client
	gcc -o client client.c -lrt -lpthread
