.PHONY: all clean

all: receiver sender

receiver: receiver.c
	gcc receiver.c -o receiver

sender: sender.c
	gcc sender.c -o sender

run: receiver
	./receiver