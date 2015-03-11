#include "util.h"
#include "stdlib.h"
#include "stdio.h"

float rand_norm(){
	float val = ((float)rand())/RAND_MAX;
	return val;
}

void panic(const char * message){
	printf("ERROR: %s\n", message);
	exit(1);
}
