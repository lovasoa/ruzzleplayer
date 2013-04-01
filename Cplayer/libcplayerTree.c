#include <stdio.h>
#include <string.h>

#define SIZE 4
#define accessgrid(grid, i,j) grid[i+SIZE*j]

#define capitalize(c)   (c&0xDF)
#define is_char(c)      ( 'A'<=capitalize(c) && capitalize(c)<='Z')

#define MAX_STRLEN 50
#define DEBUG 0

char grid[SIZE*SIZE];
FILE *dict;

unsigned int endian_swap(unsigned int x)
{
    return (x>>24) | 
        ((x<<8) & 0x00FF0000) |
        ((x>>8) & 0x0000FF00) |
        (x<<24);
}

void reset_table(char *table, int length) {
    int i;
    for(i=0;i<length;i++)table[i]=0;
}

char word_is_present_recur(char *word,int word_length, int posx, int posy, char *path){
//printf("%s (len:%d, pos=(%d,%d))\n", word, word_length, posx, posy);
//dump_table(path);

    if (word_length==0) return 1;
    char present;
    int pos;
    int x,y,
        xmin = (posx==0) ? posx : posx-1,
        ymin = (posy==0) ? posy :posy-1,
        xmax = (posx==SIZE-1) ? posx : posx+1,
        ymax = (posy==SIZE-1) ? posy : posy+1;
    //printf("xmin:%d ymin:%d xmax:%d ymax:%d\n", xmin,ymin,xmax,ymax);
    for (x=xmin; x<=xmax; x++){
        for (y=ymin; y<=ymax; y++) {
            if (x==posx && y==posy) continue;
            pos = x+SIZE*y;
            //printf("%d , %d (%d)\n",x,y,pos);
            //printf("grid[pos] (%c) == word[0](%c) && path[pos](%d)==0\n",grid[pos],word[0],path[pos]);
            if (grid[pos] == word[0] && path[pos]==0){
                path[pos] = 1;
                present = word_is_present_recur(word+1,word_length-1,x,y,path);
                path[pos] = 0;
                if (present) return 1;
            }
        }
    }
    return 0;
}


char word_is_present (char *word) {
    int x,y;
    int length = strlen(word);
    char path[SIZE*SIZE] = {0};

    if (length==0) return 1;
    for (x=0;x<SIZE;x++){
        for (y=0;y<SIZE;y++){
            if (grid[x+SIZE*y] == word[0]){
                reset_table(path, SIZE*SIZE);
                int pos = x+SIZE*y;
                path[pos]=1;
                if (word_is_present_recur(word+1,length-1,x,y,path)) {
                    return 1;
                }
                path[pos]=0;
            }
        }
    }
    return 0;
}

void found_word (char *word){
    printf("%s\n",word);
}

int read_entry(int pos, char work_str[], unsigned int work_str_length) {
    unsigned char strlen=0;
    unsigned char num_children;
    unsigned int cur_child=0;
    int i;

    if (DEBUG) printf("DBG: read_entry(%x)\n", pos);

    if (fseek(dict, pos, SEEK_SET) != 0) {
        printf("ERROR: invalid dictionary file.");
    };
    strlen = fgetc(dict);

    if (DEBUG) printf("DBG: size of string : %d\n", strlen);

    if (work_str_length+strlen >= MAX_STRLEN){
        printf("ERROR: Invalid dictionary file: too long string at position %x\n",pos);
        return 1;
    }
    fread(work_str+work_str_length, 1, strlen, dict);
    work_str[work_str_length+strlen] = '\0'; //Terminate the string

    if (DEBUG) printf("DBG: word_is_present(%s) = %d\n", work_str , word_is_present(work_str));
    if (work_str_length+strlen==0 || word_is_present(work_str)) {
        if (DEBUG) printf("DBG: Read string : %s\n", work_str+work_str_length);

        num_children = fgetc(dict);
        if (DEBUG) printf("DBG: Number of children : %d\n", num_children);
        if (num_children==0) found_word(work_str);

        for (i=0; i<num_children; i++){
            fseek(dict, pos + 1 + strlen + 1 + 4*i, SEEK_SET);
            if (DEBUG) printf("DBG: Entry %x: Reading at %x\n", pos, (unsigned int)ftell(dict));
            fread(&cur_child, 4, 1, dict);

            cur_child = endian_swap(cur_child);
            if (DEBUG) printf("DBG: Child %d of entry %x : %x\n", i+1, pos, cur_child);

            if (cur_child==0) found_word(work_str);
            else {
                read_entry(cur_child, work_str, work_str_length+strlen);
            }
        }
    }
    return 0;
}

char load_dictfile (char *dictfile){
	dict = fopen(dictfile, "r");
	if (dict==NULL){
	    printf("ERROR: Unable to open %s\n", dictfile);
	    return 1;
	}
	return 0;
}

void find_words () {
    if (DEBUG) printf("DBG: Starting to scan the dictionary.");
    char strbuf[MAX_STRLEN]={0};
    read_entry(0, strbuf, 0);
}

void dump_grid() {
	int i=0,j;
	for(j=0;j<SIZE;j++){printf("___");}printf("\n");
	while(i<SIZE*SIZE){
		if (is_char(grid[i])) printf("| %c", grid[i]);
		else printf("| %d", grid[i]);
		if (++i%SIZE == 0) printf("|\n");
	}
	for(j=0;j<SIZE;j++){printf("___");}printf("\n");
	printf("\n");
}

int init_grid(char *letters) {
    int i = 0;
    if (strlen(letters) < SIZE*SIZE){
        if (DEBUG) printf("ERROR: provided grid is too short.\
        You must give %d letters.", SIZE*SIZE);
        return 1;
    }

    while (letters[i] != '\0' && i<SIZE*SIZE){
        if(is_char(letters[i])){
            grid[i] = capitalize(letters[i]);
            i++;
        }
    }
    if (DEBUG) printf("DBG: Created a grid with %d elements", i);
    return (i==SIZE*SIZE) ? 0 : 1;
}
