CC := gcc

SOURCES := android_compat.cpp image_processing.cpp helpers.cpp simulator.cpp

all: calc format

calc: calc.c
	$(CC) -Wall -g -o calc calc.c

format: format.c
	$(CC) -Wall -g -o format format.c -lm

clean:
	rm -rf calc.o calc format.o format
