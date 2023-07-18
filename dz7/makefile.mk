LIB = LD_LIBRARY_PATH=$$PWD:$$LD_LIBRARY_PATH 

.PHONY: main
all: main


main:
	gcc -o calculator -g -ldl main.c -rdynamic
	./calculator ./liboperation_add.so ./liboperation_sub.so ./liboperation_mul.so ./liboperation_div.so ./liboperation_square_cube.so ./liboperation_cos.so
#	./calculator
