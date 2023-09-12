PROGRAM = ../server_type4
SRC = main.c server.c
OBJ = $(patsubst %.c, %.o, $(SRC))
.PHONY: main
main:$(OBJ)
	gcc -o $(PROGRAM) $(OBJ) -lpthread
%.o:%.c
	gcc $< -c