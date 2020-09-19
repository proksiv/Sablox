SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin
CC=gcc
CFLAGS=-Wall -ansi -pedantic
LDFLAGS=
EXEC=$(BIN_DIR)/sablox

all: $(EXEC)

$(EXEC): $(SRC_DIR)/main.c
	$(CC) -o $@ $< $(CFLAGS)