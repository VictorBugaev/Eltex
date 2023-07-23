CC := gcc
CFLAGS := -Wall -Wextra -pthread

SRCS := main.c
TARGET := Program
.PHONY: all clean
all: $(TARGET)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(SRCS:.c=.o)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(SRCS:.c=.o) $(TARGET)
