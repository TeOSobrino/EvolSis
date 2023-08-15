CC = gcc
BIN = main

clear:
	clear

all:	clear
	$(CC) main.c -o $(BIN)

run:
	./$(BIN)