.PHONY: lib main.o main
all: lib main.o main clear

lib:
	gcc -fPIC -c sum.c 
	gcc -fPIC -c dif.c 
	gcc -fPIC -c mul.c 
	gcc -fPIC -c del.c 
	gcc -shared sum.o dif.o mul.o del.o -o liboperations.so
main.o:
	gcc -c main.c -o main.o
main:
	-rm calculator
	gcc -o calculator main.o -L. -loperations
#	запуск	
#	make -f makefile_dinamic.mk
#	LD_LIBRARY_PATH=$PWD:$LD_LIBRARY_PATH ./calculator
clear:
	-rm *.o