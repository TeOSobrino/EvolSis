CC = gcc
FLAGS = -g -lm -Wall
DEPS = ga.c
BIN = main

clear:
	clear

all: main

main: ga.o main.c
	@$(CC) ga.o main.c $(FLAGS) -o $(BIN) 

ga.o: ga.c ga.h
	@$(CC) $(FLAGS) -c ga.c -o ga.o 

run:
	./$(BIN)

clean:
	rm ga.o main