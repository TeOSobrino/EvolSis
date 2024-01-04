CC = g++

INCLUDE_DIRECTORY = include
BUILD_DIRECTORY := build
SOURCE_DIRECTORY := source
SOURCES := $(shell find $(SOURCE_DIRECTORY) -name '*.cpp')

OBJECTS := $(patsubst source/%.cpp,build/%.o,$(SOURCES))

DEPENDENCIES := $(OBJECTS:.o=.d)

TARGET_EXECUTABLE = evolved_being
MAIN = main.cpp

DFLAGS = -g3 -Og -fsanitize=address -fstack-protector-all \
 -fstack-clash-protection -fasynchronous-unwind-tables -D_FORTIFY_SOURCE=2

CPPFLAGS = -g -lm -march=native -fomit-frame-pointer -O3 -ffp-contract=fast\
 -faggressive-loop-optimizations -ftree-vectorize -std=c++20\
 -Wall -Wextra -Wpedantic -pedantic -Wno-unused-parameter -Wno-unused-variable 

VFLAGS = --show-leak-kinds=all --track-origins=yes --leak-check=full -s

all: $(BUILD_DIRECTORY)/$(TARGET_EXEC)
#	@$(CC) -o $(TARGET_EXECUTABLE) $(PROG) $(SOURCES) -I$(INCLUDES) $(CFLAGS)

# The final build step.
$(BUILD_DIRECTORY)/$(TARGET_EXEC): $(OBJECTS)
	$(CC) $(CPPFLAGS) $(MAIN) $(OBJECTS) -o $@ -I$(INCLUDE_DIRECTORY)

# Build step for C++ source
$(BUILD_DIRECTORY)/%.o: $(SOURCE_DIRECTORY)/%.cpp #$(INCLUDE_DIRECTORY)/%.hpp 
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -c $< -o $@ -I$(INCLUDE_DIRECTORY)

run: 
	@$(BUILD_DIRECTORY)/$(TARGET_EXECUTABLE)

valgrind: all clear 
	valgrind $(VFLAGS) $(TARGET_EXECUTABLE) 

zip:
	zip -r TI.zip main.c include source makefile

.PHONY: clean
clean:
	rm -r $(BUILD_DIRECTORY); rm -f *.zip

clear:
	clear

debcompile: 
	@$(CC) -o $(TARGET_EXECUTABLE) $(PROG) $(SOURCES) -I$(INCLUDES) $(DFLAGS)

debug: debcompile clear
	 $(BUILD_DIRECTORY)/$(TARGET_EXECUTABLE)
