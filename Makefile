CC = gcc
CFLAGS = -O0 -std=c11 -Wall -Wextra -g
LDFLAGS =

TARGET = h-vm
SRCS = h-vm.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c h-vm.h h-utils.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
