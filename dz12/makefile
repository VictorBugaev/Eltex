.PHONY: all clean

all: Program run

Program: main.c
	gcc -Wall -Wextra $< -o $@

run: Program
	./Program > log.txt

clean:
	rm -f Program log.txt