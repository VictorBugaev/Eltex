
.PHONY: all
all: manager

manager: main.c
gcc -g -o manager main.c -lncurses

clean:
rm -f manager

run:
./manager test.txt
