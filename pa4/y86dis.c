#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "futil.h"
#include "y86emul.h"
#include "y86tools.h"

int main(int argc, char **argv){
	if(argc != 2 || strcmp(argv[1], "-h") == 0){
		printf("USAGE: y86dis <filename>\n");
		return 1;
	}

	FILE *file = getFile(argv[1]);
	if(file == 0){
		printf("File not found: %s\n", argv[1]);
		return 1;
	}


	return 0;
}
