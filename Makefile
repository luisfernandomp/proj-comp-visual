CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -g
TARGET = proj1

CFLAGS += $(shell pkg-config --cflags sdl3 sdl3-image sdl3-ttf)
LDFLAGS = $(shell pkg-config --libs sdl3 sdl3-image sdl3-ttf)

ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
endif

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean
