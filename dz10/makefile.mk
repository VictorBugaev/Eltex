.PHONY: all p1 p2 
all: p1 p2

p1:
	gcc -o p1 p1.c
	./p1
p2:
	gcc -o p2 p2.c	

