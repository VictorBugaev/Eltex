.PHONY: lib main.o main
all: lib main.o main 

lib:
	gcc -c sum.c 
	gcc -c dif.c 
	gcc -c mul.c 
	gcc -c del.c 
	ar rc liboperations.a sum.o dif.o mul.o del.o

main.o:
	gcc -c main.c -o main.o

main:
	-rm calculator
	gcc -o calculator main.o -L. -loperations

start:
	./calculator

clear:
	-rm *.o
	-rm liboperations.a