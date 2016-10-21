CC := gcc

SOURCES := android_compat.cpp image_processing.cpp helpers.cpp simulator.cpp

all: clean calc format

calc: calc.c
	$(CC) -Wall -g -o calc calc.c strutil.c

format: format.c
	$(CC) -Wall -g -o format format.c strutil.c

clean:
	rm -rf calc.o calc format.o format
