#include <markov.c>
#include "minunit.h"

char *has_prefix_and_suf_count(char *pre, int n, unsigned char c, int count)
{
	MC_State *st = NULL;
	MC_Suffix *suf = NULL;

	st = mc_look_up(pre, n, 0);
	mu_assert(st != NULL, "No state for prefix");

	suf = &st->suffix[c];
	mu_assert(suf->c == c, "prefix did not have correct suffix");
	mu_assert(suf->count == count, "prefix did not have suffix count");

	return NULL;
}

char *test_train_model()
{
	char *error = NULL;

	mc_train_model("./tests/test_training_corpus");
	
	error = has_prefix_and_suf_count("th", MC_PREF, 'e', 1);
	if(error != NULL) return error;

	error = has_prefix_and_suf_count("he", MC_PREF, 'c', 1);
	if(error != NULL) return error;

	error = has_prefix_and_suf_count("ec", MC_PREF, 'a', 1);
	if(error != NULL) return error;

	error = has_prefix_and_suf_count("ec", MC_PREF, 'a', 1);
	if(error != NULL) return error;

	error = has_prefix_and_suf_count("ca", MC_PREF, 't', 1);
	if(error != NULL) return error;

	error = has_prefix_and_suf_count("at", MC_PREF, 's', 1);
	if(error != NULL) return error;

	error = has_prefix_and_suf_count("ts", MC_PREF, 'a', 1);
	if(error != NULL) return error;

	error = has_prefix_and_suf_count("sa", MC_PREF, 't', 1);
	if(error != NULL) return error;

	return NULL;
}

char *all()
{
	mu_suite_start();

	mu_run_test(test_train_model);

	return NULL;
}

RUN_TESTS(all)
