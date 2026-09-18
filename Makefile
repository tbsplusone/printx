CC = gcc-14
GIT_VERSION := $(shell git symbolic-ref -q --short HEAD || git describe --tags --exact-match)
CFLAGS = -std=c2x -Wall -Wextra -g -DGIT_VERSION=\"$(GIT_VERSION)\"
BUILD_DIR = bin
TARGET = $(BUILD_DIR)/printx
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

.PHONY: test
test: $(TARGET)
	./test/bats/bin/bats -F pretty ./test/tests.bats

clean:
	rm -f $(TARGET)