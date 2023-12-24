CC = g++
INCLUDES = include
SOURCES = source/*.cpp
BINARY = ./main
PROG = main.cpp

DFLAGS = -g3 -Og -fsanitize=address -fstack-protector-all \
 -fstack-clash-protection -fasynchronous-unwind-tables -D_FORTIFY_SOURCE=2

CFLAGS = -g -lm -march=native -fomit-frame-pointer -O3 -ffp-contract=fast\
 -faggressive-loop-optimizations -ftree-vectorize -std=c++20\
 -Wall -Wextra -Wpedantic -pedantic -Wno-unused-parameter -Wno-unused-variable 

VFLAGS = --show-leak-kinds=all --track-origins=yes --leak-check=full -s

all:
	@$(CC) -o $(BINARY) $(PROG) $(SOURCES) -I$(INCLUDES) $(CFLAGS)
run: 
	@$(BINARY) ./source/pyplot.py
valgrind: all clear 
	valgrind $(VFLAGS) $(BINARY) 
zip:
	zip -r TI.zip main.c include source makefile
clean:
	rm -f $(BINARY); rm -f *.zip
clear:
	clear
debcompile: 
	@$(CC) -o $(BINARY) $(PROG) $(SOURCES) -I$(INCLUDES) $(DFLAGS)
debug: debcompile clear
	$(BINARY)
