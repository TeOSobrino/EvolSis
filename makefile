CC = gcc
INCLUDES = include
SOURCES = source/*.cpp
BINARY = ./main
PROG = main.cpp
DFLAGS = -g3 -O0 -fsanitize=address -fstack-protector-all -fstack-clash-protection -fasynchronous-unwind-tables -D_FORTIFY_SOURCE=2
FLAGS = -g -lm -march=native -O3 -Wall -Wextra -Wpedantic -pedantic -Wno-unused-parameter -Wno-unused-variable
VFLAGS = --show-leak-kinds=all --track-origins=yes --leak-check=full -s -S

all:
	@$(CC) -o $(BINARY) $(PROG) $(SOURCES) -I$(INCLUDES) $(FLAGS)
run: 
	@$(BINARY)
valgrind: all clear 
	valgrind $(VFLAGS) $(BINARY) 
zip:
	zip -r TI.zip main.c include source makefile
clean:
	rm $(BINARY); rm *.zip
clear:
	clear
debcompile: 
	@$(CC) -o $(BINARY) $(PROG) $(SOURCES) -I$(INCLUDES) $(DFLAGS)
debug: debcompile clear
	$(BINARY)
