
PATHSRC = libs_src

.PHONY: objects libs 
all: objects libs clear

objects:
	gcc -g -fPIC -c $(PATHSRC)/operation_add.c 
	gcc -g -fPIC -c $(PATHSRC)/operation_sub.c 
	gcc -g -fPIC -c $(PATHSRC)/operation_mul.c 
	gcc -g -fPIC -c $(PATHSRC)/operation_div.c 
	gcc -g -fPIC -c $(PATHSRC)/operation_square_cube.c 
	gcc -g -fPIC -c $(PATHSRC)/operation_cos.c 
libs:
	gcc -shared operation_add.o -o liboperation_add.so
	gcc -shared operation_sub.o -o liboperation_sub.so
	gcc -shared operation_mul.o -o liboperation_mul.so
	gcc -shared operation_div.o -o liboperation_div.so
	gcc -shared operation_square_cube.o -o liboperation_square_cube.so
	gcc -shared operation_cos.o -o liboperation_cos.so -lm
clear:
	-rm *.o