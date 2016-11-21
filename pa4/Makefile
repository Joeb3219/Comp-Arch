CC := gcc

all: clean y86emul y86dis y86asmbl

y86emul: y86emul.c
	$(CC) -Wall -g -o y86emul y86emul.c futil.c y86tools.c

y86dis: y86dis.c
	$(CC) -Wall -g -o y86dis y86dis.c y86tools.c futil.c

y86asmbl: y86asmbl.c
	$(CC) -Wall -g -o y86asmbl y86asmbl.c futil.c y86tools.c

clean:
	rm -rf y86emul y86emul.o y86dis.o y86dis y86asmbl y86asmbl.o
