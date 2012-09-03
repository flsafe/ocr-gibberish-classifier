#include <stdio.h>
#include "markov.h"

int main(int argc, char *argv[])
{
	if(argc != 2){
		fprintf(stderr, "Usage: markov <file_path>\n");
		exit(1);
	}

	mc_train_model(argv[1]);

	return 0;
}
