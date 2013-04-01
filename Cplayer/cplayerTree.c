#include "libcplayerTree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#if EMSCRIPTEN
#include <emscripten.h>
#endif

#define capitalize(c)   (c&0xDF)
#define is_letter(c)      ( 'A'<=capitalize(c) && capitalize(c)<='Z')

#define MIN(a,b) ((a<b)?a:b)

#define MAX_STRLEN 50

static unsigned int min_display_length = 5; //Words shorter than this won't be displayed
static SDL_Surface *ecran = NULL;
static SDL_Rect grid_rect = {10,10,0,0}; //The rectangle in which the grid will be displayed 10x10 margins
static SDL_Surface *letter_box = NULL;
static TTF_Font *font = NULL;
static TTF_Font *smallfont = NULL;
static Uint32 background_color;

void found_word_callback(char *word) {
	static int word_num = 0;
	int maxnum;
 	SDL_Surface *text = NULL;
    SDL_Rect word_rect;
    SDL_Color color = {0, 0, 0, 0};

	if (word == NULL) { //Reinitialize
		word_num = 0;
		word_rect.x = ecran->w/2 + 20;
		word_rect.y = 10;
		word_rect.w = ecran->w - word_rect.x;
		word_rect.h = ecran->h - word_rect.y;
		SDL_FillRect(ecran, &word_rect, background_color);
		SDL_Flip(ecran);
		return;
	}

	printf("%s\n", word);
	if (strlen(word) >= min_display_length) {//Display long words only
		text = TTF_RenderText_Blended(smallfont, word, color);
		maxnum = (ecran->h-10)/(text->h+10);
		word_rect.x = ecran->w/2 + 20 + (word_num/maxnum)*150;
		word_rect.y = 10 + (text->h+10)*(word_num % maxnum);
		word_rect.w = text->w;
		word_rect.h = text->h;

		word_num++;
		if (word_rect.x < ecran->w) {
			SDL_FillRect(ecran, &word_rect, background_color);
			SDL_BlitSurface(text, NULL, ecran, &word_rect);
			SDL_Flip(ecran);
		}
	}
}

void draw_letters (char *letters) {
 	SDL_Surface *text = NULL;
    SDL_Color color = {0, 0, 0, 0};
    char letter_str[2] = "a";

    SDL_Rect position;

	for (int i=0; letters[i]!='\0'; i++) {
		position.y = 10 + (i/SIZE)*(letter_box->h+10);
		position.x = 10 + (i%SIZE)*(letter_box->w+10);
		
		letter_str[0] = letters[i];
	    SDL_BlitSurface(letter_box, NULL, ecran, &position);
    	text = TTF_RenderText_Blended(font, letter_str, color);
    	
		position.x += letter_box->w/2 - text->w/2;
    	position.y += letter_box->h/2 - text->h/2;

        SDL_BlitSurface(text, NULL, ecran, &position); /* Blit du texte */
	}
    SDL_Flip(ecran);
}

void one_iter (char *continuer) {
	static char letters[MAX_STRLEN]={0}, cur_letter;
	static int letters_index = 0;
	char letters_changed=0, new_grid=0, do_search=0;
    SDL_Event event;
	while(SDL_PollEvent(&event)) {
    	switch(event.type) {
            case SDL_QUIT:
                *continuer = 0;
                break;
            case SDL_KEYDOWN:

                switch(event.key.keysym.sym)
                {
                    case SDLK_UP:
                    	min_display_length++;
                    	do_search = 1;
                    	break;
                    case SDLK_DOWN:
                    	if (min_display_length>2) min_display_length --;
                    	do_search=1;
                    	break;
                    case SDLK_BACKSPACE:
                    	if (letters_index > 0){
	                        letters_index--;
                        	letters[letters_index] = '\0';
							letters_changed=1;
                    	}
                        break;
                    case SDLK_RETURN:
                    	new_grid = 1;
                    	break;
                    default:
                    	if (letters_index == 0) SDL_FillRect(ecran, NULL, background_color);
	                	cur_letter = capitalize((char)event.key.keysym.unicode);
						if (is_letter(cur_letter) && letters_index <  MIN(MAX_STRLEN, SIZE*SIZE)){
							letters_changed=1;
							letters[letters_index] = cur_letter;
							letters[letters_index+1] = '\0';
							letters_index ++;
						}
						break;
                }
                break;
        }
		if (new_grid) {
			new_grid=0;
			letters_index = 0;
			letters[0] = '\0';
			SDL_FillRect(ecran, NULL, background_color);
			SDL_Flip(ecran);
		}
        if (letters_changed){
        	letters_changed = 0;
        	if (letters_index == SIZE*SIZE) do_search=1;
            SDL_FillRect(ecran, &grid_rect, background_color);
        	draw_letters(letters);
        }
        if (do_search) {
        	do_search = 0;
        	if (letters_index == SIZE*SIZE) {
	        	init_grid(letters);
        		found_word_callback(NULL);
    			find_words();
        	}
        }
	}
}

#define TICK_INTERVAL    50
Uint32 time_to_next_frame(void)
{
    static Uint32 next_time = 0;
    Uint32 now;

    now = SDL_GetTicks();
    if ( next_time <= now ) {
        next_time = now+TICK_INTERVAL;
        return(0);
    }
    return(next_time-now);
}

int main(int argc, char *argv[]){
	SDL_Surface *text=NULL;
	SDL_Rect position;
    SDL_Color instructions_color = {200, 10, 10, 0};

	//Loading the dictionary
	char dictfile[256] = "assets/dict.bin";
	if (argc > 1) strncpy(dictfile, argv[1], 256);
	if (load_dictfile(dictfile) != 0){
	    return EXIT_FAILURE;
	}

	SDL_Init(SDL_INIT_VIDEO);

    letter_box = SDL_LoadBMP("assets/letter_box.bmp");
    if (letter_box == NULL) {
    	printf("Unable to load image.\n");
    	exit(1);
    }
    grid_rect.w = (letter_box->w+10)*SIZE;
    grid_rect.h = (letter_box->h+10)*SIZE;
    
    /* Chargement de la police */
    TTF_Init();
    font = TTF_OpenFont("assets/Ubuntu mono.ttf", letter_box->h-10);
    smallfont = TTF_OpenFont("assets/Ubuntu mono.ttf", 20);
    if (font == NULL || smallfont==NULL) {
    	printf("Unable to load font.\n");
    	exit(1);
    }

    ecran = SDL_SetVideoMode(10+(letter_box->w+10)*SIZE * 2, 10+(letter_box->h+10)*SIZE, 32, SDL_HWSURFACE | SDL_DOUBLEBUF); /* Double Buffering */
    if (ecran == NULL) {
    	printf("Unable to display a window.\n");
    	exit(1);
    }

    background_color = SDL_MapRGB(ecran->format, 205, 105, 255);

    SDL_WM_SetCaption("Ruzzle solver by Ophir", NULL);

	//Display instructions
    SDL_FillRect(ecran, NULL, background_color);
	text = TTF_RenderText_Blended(font, "Ruzzle solver", instructions_color);
	position.x = ecran->w/2 - text->w/2;
	position.y = ecran->h/2 - text->h/2;
    SDL_BlitSurface(text, NULL, ecran, &position);
	position.y += text->h + 10;
	text = TTF_RenderText_Blended(smallfont, "Type letters to fill the grid, then press ENTER.\t(by Ophir)", instructions_color);
	position.x = ecran->w/2 - text->w/2;
    SDL_BlitSurface(text, NULL, ecran, &position);
    SDL_Flip(ecran);

    /* Enable Unicode translation */
    SDL_EnableUNICODE( SDL_ENABLE );

	#ifdef EMSCRIPTEN
	  emscripten_set_main_loop(one_iter, 1000/TICK_INTERVAL, 1);
	#else
		char continuer = 1;
	  while (continuer) {
	  	one_iter(&continuer);
		SDL_Delay(time_to_next_frame());
	  }
	#endif


    TTF_CloseFont(font);
    TTF_Quit();

    SDL_Quit();
	return EXIT_SUCCESS;

}
