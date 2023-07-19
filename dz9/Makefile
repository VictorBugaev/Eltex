CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lncurses

SRCS = main.c
OBJS = $(SRCS:.c=.o)
TARGET = file_manager

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)