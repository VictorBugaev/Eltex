SRC = client.c server.c
.PHONY: $(SRC)
all: $(SRC)
$(SRC):
	gcc -o $(basename $@) $@ -lpthread -Wall -O0