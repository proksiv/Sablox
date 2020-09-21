SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin
CC=gcc
CPPFLAGS=-Ilib/allegro/include
CFLAGS=-Wall -std=c99 -pedantic
LDFLAGS=-Llib/allegro/lib
LIBS=-lallegro -lallegro_font -lallegro_image -lallegro_primitives
EXEC=$(BIN_DIR)/sablox
SRC=$(wildcard $(SRC_DIR)/*.c)
OBJ=$(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(LIBS)

$(OBJ): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -o $@ -c $< $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(LIBS)