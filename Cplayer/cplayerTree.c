#include "libcplayerTree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_STRLEN 50

int main(int argc, char *argv[]){
	char letters[MAX_STRLEN];
	do {
	    fgets(letters, sizeof letters, stdin);
	} while(init_grid(letters) != 0);

	printf("Saved grid:\n");
	dump_grid();

	char dictfile[256] = "dict.bin";
	if (argc > 1) strncpy(dictfile, argv[1], 256);
	if (load_dictfile(dictfile) != 0){
	    return EXIT_FAILURE;
	}
	find_words();

	return EXIT_SUCCESS;

}
