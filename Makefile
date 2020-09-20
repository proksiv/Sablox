SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin
CC=gcc
CPPFLAGS=-Ilib/allegro/include
CFLAGS=-Wall -std=c99 -pedantic
LDFLAGS=-Llib/allegro/lib
LIBS=-lallegro -lallegro_font -lallegro_image
EXEC=$(BIN_DIR)/sablox

all: $(EXEC)

$(EXEC): $(SRC_DIR)/main.c
	$(CC) -o $@ $< $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(LIBS)