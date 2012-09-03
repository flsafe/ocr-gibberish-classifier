#ifndef _MARKOV_H
#define _MARKOV_H

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

void mc_train_model(char *file_path);                     /* Train markov model on file text */
MC_State *mc_look_up(char *prefix, int n, int create);    /* Look up state for prefix of length n, is created if specified */
extern MC_State *mc_state_tab[MC_HASH];                   /* Table of states */

#endif
