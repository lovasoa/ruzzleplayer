#include <stdio.h>
#include <string.h>

#define SIZE 4
#define accessgrid(grid, i,j) grid[i+SIZE*j]

#define capitalize(c)   (c&0xDF)
#define is_char(c)      ( 'A'<=capitalize(c) && capitalize(c)<='Z')

struct grid {
    char *grid;
    int size;
};

void dump_table(char *table) {
	int i=0,j;
	for(j=0;j<SIZE;j++){printf("___");}printf("\n");
	while(i<SIZE*SIZE){
		if (is_char(table[i])) printf("| %c", table[i]);
		else printf("| %d", table[i]);
		if (++i%SIZE == 0) printf("|\n");
	}
	for(j=0;j<SIZE;j++){printf("___");}printf("\n");
	printf("\n");
}

void reset_table(char *table, int length) {
    int i;
    for(i=0;i<length;i++)table[i]=0;
}

char word_is_present_recur(char *grid, char *word,int word_length, int posx, int posy, char *path){
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
                present = word_is_present_recur(grid,word+1,word_length-1,x,y,path);
                path[pos] = 0;
                if (present) return 1;
            }
        }
    }
    return 0;
}


char word_is_present (char *grid, char *word) {
    int x,y;
    int length = strlen(word);
    char path[SIZE*SIZE] = {0};

    if (length==0) return 0;
    for (x=0;x<SIZE;x++){
        for (y=0;y<SIZE;y++){
            if (grid[x+SIZE*y] == word[0]){
                reset_table(path, SIZE*SIZE);
                int pos = x+SIZE*y;
                path[pos]=1;
                if (word_is_present_recur(grid,word+1,length-1,x,y,path)) {
                    return 1;
                }
                path[pos]=0;
            }
        }
    }
    return 0;
}

void readline (FILE *f, ssize_t buflen, char *buf) {
    int i;
    int tmpchar;
    for(i=0; i<buflen-1; i++){
        tmpchar = fgetc(f);
        if (tmpchar==EOF || (char)tmpchar =='\n') break;
        buf[i] = (char) tmpchar;
    }
    buf[i] = 0;
}

int main(int argc, char *argv[]){
	char grid[SIZE*SIZE];
	char tmpchar;
	int i=0;
	while (i < SIZE*SIZE) {
		tmpchar = getchar();
        if(is_char(tmpchar)){
            grid[i] = tmpchar;
            i++;
        }
	}
	printf("Grille enregistrée:\n");
	dump_table(grid);
	
	char word[256]={0};
	char dictfile[256] = "dico.txt";
	if (argc > 1) strncpy(dictfile, argv[1], 256);
	FILE* dico = fopen(dictfile, "r");
	if (dico==NULL){
	    printf("Impossible d'ouvrir %s\n", dictfile);
	    return 1;
	}
	do{
	    readline(dico, 256, word);
	    if (strlen(word)>4 && word_is_present(grid,word)==1) printf("%s\n",word);
	}while (strlen(word)>0);
	return 0;

}
