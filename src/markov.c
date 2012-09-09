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
unsigned int hash(char *prefix, int np)
{
	unsigned int h = 0;
	int i = 0, mult = 32;

	h = 0;
	for(i = 0 ; i < np ; i++){
		h = mult * h + prefix[i];	
	}

	return h % MC_HASH;
}

/* retrieve the state for the prefix, create if requested */
MC_State *mc_look_up(char *prefix, int np, int create)
{
	unsigned int h = 0;
	MC_State *st = NULL;

	h = hash(prefix, np);
	for(st = mc_state_tab[h] ; st != NULL ; st = st->next){
		if(0 == strncmp(prefix, st->prefix, np)){
			return st;	
		}
	}

	if(create){
		st = calloc(1, sizeof(MC_State));	
		assert(st != NULL && "Failed to allocate mem for MC_State");

		st->prefix = strndup(prefix, np);
		assert(st->prefix != NULL && "Could not allocate mem for the state's copy of prefix");
		st->next = mc_state_tab[h];
		mc_state_tab[h] = st;
	}

	return st;
}

/* increase the counter for the suffix, given the prefix */
void mc_add_trans(char *prefix, int np, unsigned char c)
{
	MC_State *st = NULL;
	MC_Suffix *suf = NULL;

	st = mc_look_up(prefix, np, 1);
	suf = &st->suffix[c];
	suf->c = c;
	suf->count++;
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

/* train markov chain on one line of text, use prefix length np */
void mc_train_on_line(char *line, int np)
{
	char *prefix = NULL;
	unsigned char c = 0;
	int i = 0;

	clean_line(line);

	prefix = calloc(np+1,sizeof(char));
	assert(prefix != NULL && "Could not allocate mem for prefix");

	do {
		c = line[i];
		prefix[i] = c;
		i++;
	} while(c && i < np);

	if ('\0' == c) { free(prefix); return; } /* End of line, not enough data to build transition */

	for( ; line[i] != '\0' ; i++){
		c = line[i];

		mc_add_trans(prefix, np, c);

		memmove(prefix, prefix + 1, np-1);
		prefix[np-1] = c;
	}

	free(prefix);
}

/* train a markov chain model on file text, use
 * the prefix length np */
void mc_train_model(char *file_path, int np)
{
	FILE *f = NULL;	
	char line[LINE_BUFF];

	f = fopen(file_path, "rt");
	assert(f != NULL && "Couldn't open file");

	while(fgets(line, LINE_BUFF, f) != NULL){
		mc_train_on_line(line, np);
	}
}
