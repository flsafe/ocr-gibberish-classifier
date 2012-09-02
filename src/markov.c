#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include "debug.h"

#define LINE_BUFF 80

enum
{
	MC_PREF = 2,     /* prefix length */
	MC_SUFF = 256,   /* number of possible sufixes */
	MC_HASH = 4093,  /* state hash table size */
};

typedef struct MC_State MC_State;
typedef struct MC_Suffix MC_Suffix;

struct MC_Suffix
{
	char c;                   /* char suffix */
	int count;                /* count of this character suffix */
};

struct MC_State 
{
	char *prefix;               /* prefix string */
	MC_Suffix suffix[MC_SUFF];  /* table couting the freq of each suffix char */

    MC_State *next;
};

/* hash table of states */
MC_State *state_tab[MC_HASH];  

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
	for(st = state_tab[h] ; st != NULL ; st = st->next){
		if(0 == strncmp(prefix, st->prefix, MC_PREF)){
			return st;	
		}
	}

	if(create){
		st = calloc(1, sizeof(MC_State));	
		assert(st != NULL && "Failed to allocate mem for MC_State");

		st->prefix = strndup(prefix, n);
		st->next = state_tab[h];
		state_tab[h] = st;
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
}

void remove_whitespace(char *line)
{
	int read = 0, write = 0;
	char c = 0;

	do{
		c = line[read++];
		if(!isspace(c)){
			line[write++] = c;
		}
	} while(c);
}

/* train markov chain on one line of text */
void train_on_line(char *line)
{
	char prefix[MC_PREF+1];
	unsigned char c = 0;
	int i = 0;

	remove_whitespace(line);

	do {
		c = line[i];
		prefix[i] = c;
		i++;
	} while(c && i < MC_PREF);
	prefix[MC_PREF] = '\0';

	if ('\0' == c) return; /* End of line, not enough data to build transition */

	for( ; line[i] != '\0' ; i++){
		c = line[i];

		debug("'%s' -> '%c'\n", prefix, c);
		mc_add_trans(prefix, MC_PREF, c);

		memmove(prefix, prefix + 1, MC_PREF-1);
		prefix[MC_PREF-1] = c;
	}
}

void train_model(char *file_path)
{
	FILE *f = NULL;	
	char line[LINE_BUFF];

	f = fopen(file_path, "rt");
	assert(f != NULL && "Couldn't open file");

	while(fgets(line, LINE_BUFF, f) != NULL){
		train_on_line(line);
	}
}
