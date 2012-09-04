#include <stdio.h>
#include <stdlib.h>
#include "markov.h"

int main(int argc, char *argv[])
{
	if(argc != 3){
		fprintf(stderr, "Usage: markov <file_path> <prefix_length>\n");
		exit(1);
	}

	mc_train_model(argv[1], atoi(argv[2]));

	return 0;
}
