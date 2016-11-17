CC := gcc

all: clean calc format

y86emul: y86emul.c
	$(CC) -Wall -g -o y86emul y86emul.c futil.c y86tools.c

y86dis: y86dis.c
	$(CC) -Wall -g -o y86dis y86dis.c y86tools.c futil.c

clean:
	rm -rf y86emul y86emul.o y86dis.o y86dis
