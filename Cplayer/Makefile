emcc -Wall -Werror -O3 libcplayerTree.c --pre-js libcplayerTree-preScript.js --post-js libcplayerTree-postScript.js -o cplayerTree-Worker.js -s EXPORTED_FUNCTIONS="['_load_dictfile','_init_grid', '_find_words']"


CC=gcc
EMCC=emcc
CFLAGS=-W -pedantic -std=gnu99
SDLFLAGS=-lSDL -lSDL_ttf
LDFLAGS=


js: libcplayerTree.c libcplayerTree-preScript.js
	$(EMCC) $(CFLAGS) -O2 libcplayerTree.c --pre-js libcplayerTree-preScript.js -o libcplayerTree-compiled.js -s EXPORTED_FUNCTIONS="['_load_dictfile','_init_grid', '_find_words']";

native: libcplayerTree.c cplayerTree.c
	$(CC) $(CFLAGS) -O3 -I /usr/include/SDL -o cplayerTree libcplayerTree.c cplayerTree.c $(SDLFLAGS);

html: libcplayerTree.c cplayerTree.c assets/dict.bin assets/Ubuntu\ mono.ttf assets/letter_box.bmp
	$(EMCC) -O3 -s WARN_ON_UNDEFINED_SYMBOLS=1 --preload-file assets $(CFLAGS) -o cplayerTree.html libcplayerTree.c cplayerTree.c;

debug:
	clang $(CFLAGS) -Wall -Werror -D DEBUG=1 -O0 -g -I /usr/include/SDL -o cplayerTree libcplayerTree.c cplayerTree.c $(SDLFLAGS);
	
clean:
	rm cplayerTree cplayerTree-Worker.js;
