#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include "debug.h"
#include "markov.h"

#define LINE_BUFF 80

/* hash table of states */
MC_State *mc_state_tab[MC_HASH];  

/* returns hash of prefix */
unsigned int hash(char *prefix, int n)
{
	unsigned int h = 0;
	int i = 0, mult = 32;

	h = 0;
	for(i = 0 ; i < n ; i++){
		h = mult * h + prefix[i];	
	}

	return h % MC_HASH;
}

/* retrieve the state for the prefix, create if requested */
MC_State *mc_look_up(char *prefix, int n, int create)
{
	unsigned int h = 0;
	MC_State *st = NULL;

	h = hash(prefix, MC_PREF);
	for(st = mc_state_tab[h] ; st != NULL ; st = st->next){
		if(0 == strncmp(prefix, st->prefix, MC_PREF)){
			return st;	
		}
	}

	if(create){
		st = calloc(1, sizeof(MC_State));	
		assert(st != NULL && "Failed to allocate mem for MC_State");

		st->prefix = strndup(prefix, n);
		st->next = mc_state_tab[h];
		mc_state_tab[h] = st;
	}

	return st;
}

/* increase the counter for the suffix, given the prefix */
void mc_add_trans(char *prefix, int n, unsigned char c)
{
	MC_State *st = NULL;
	MC_Suffix *suf = NULL;

	st = mc_look_up(prefix, n, 1);
	suf = &st->suffix[c];
	suf->c = c;
	suf->count++;

	debug("'%s'->'%c' : %d\n", prefix, c, suf->count);
}

/* transform to lowercase */
void clean_line(char *line)
{
	int read = 0, write = 0;
	char c = 0;

	do{
		c = line[read++];
		line[write++] = tolower(c);
	} while(c);
}

/* train markov chain on one line of text */
void mc_train_on_line(char *line)
{
	char prefix[MC_PREF+1];
	unsigned char c = 0;
	int i = 0;

	clean_line(line);

	do {
		c = line[i];
		prefix[i] = c;
		i++;
	} while(c && i < MC_PREF);
	prefix[MC_PREF] = '\0';

	if ('\0' == c) return; /* End of line, not enough data to build transition */

	for( ; line[i] != '\0' ; i++){
		c = line[i];

		mc_add_trans(prefix, MC_PREF, c);

		memmove(prefix, prefix + 1, MC_PREF-1);
		prefix[MC_PREF-1] = c;
	}
}

void mc_train_model(char *file_path)
{
	FILE *f = NULL;	
	char line[LINE_BUFF];

	f = fopen(file_path, "rt");
	assert(f != NULL && "Couldn't open file");

	while(fgets(line, LINE_BUFF, f) != NULL){
		mc_train_on_line(line);
	}
}
