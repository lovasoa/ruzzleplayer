emcc -Wall -Werror -O3 libcplayerTree.c --pre-js libcplayerTree-preScript.js --post-js libcplayerTree-postScript.js -o cplayerTree-Worker.js -s EXPORTED_FUNCTIONS="['_load_dictfile','_init_grid', '_find_words']"


CC=gcc
EMCC=emcc
CFLAGS=-W -Wall -pedantic -std=gnu99 -O2
LDFLAGS=


js: libcplayerTree.c libcplayerTree-preScript.js
	$(EMCC) $(CFLAGS) libcplayerTree.c --pre-js libcplayerTree-preScript.js -o libcplayerTree-compiled.js -s EXPORTED_FUNCTIONS="['_load_dictfile','_init_grid', '_find_words']";

native: libcplayerTree.c cplayerTree.c
	$(CC) $(CFLAGS) libcplayerTree.c cplayerTree.c -o cplayerTree;

clean:
	rm cplayerTree cplayerTree-Worker.js;
