SRC = server.c client.c server2.c
.PHONY: $(SRC)
all: $(SRC)
$(SRC):
	gcc -o $(basename $@) $@ -lpthread -Wall -O0
	