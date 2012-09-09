#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "markov.h"
#include "debug.h"

#define LINE 1023
#define NPREF 3

int mark_line(char *line, char *marked_line)
{
	MC_State *st = NULL;
	MC_Suffix *suf = NULL;
	char prefix[NPREF+1] = {'\0'};
	unsigned char c = 0;
	int i = 0, miss = 0;

	memmove(prefix, line, NPREF);
	if(NPREF != strlen(prefix)) return 0;

	for(i = NPREF ; line[i] ; i++){
		c = line[i];

		st = mc_look_up(prefix, NPREF, 0);

		debug("%s", prefix);
		if(NULL == st){
			debug("%s not found", prefix);
			miss++;	
		} else {
			debug("%s found", prefix);
			suf = &st->suffix[c];
			if(0 == suf->count){
				debug("'%c' suffix not found", c);
				miss++;	
			}
		}

		memmove(prefix, prefix+1, NPREF-1);
		prefix[NPREF-1] = c;
	}

	return miss;
}

int main(int argc, char *argv[])
{
	int misses = 0;
	char line[LINE+1];
	char marked_line[LINE+1];

	printf("Loading corpus\n");
	printf(".");
	mc_train_model("corpus/hg-wells-the-first-men-on-the-moon.txt", NPREF);
	printf(".");
	mc_train_model("corpus/graham.txt", NPREF);
	printf(".");
	mc_train_model("corpus/graham2.txt", NPREF);
	printf("\n");

	while(NULL != fgets(line, LINE, stdin)){
		misses = mark_line(line, marked_line);
		printf("> %d\n", misses);
	}

	return 0;
}
