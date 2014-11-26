#include "time.h"
#include "random.h"
#include "stdlib.h"
void rand_seed(){
	srand(time(NULL));
}
void rand_seed_with(int i){
	srand(i);
}

float rand_norm_float(){
	float flt = rand()/((float) RAND_MAX);
	return flt;
}

int rand_int(){
	return rand();
}
char rand_char(){
	return rand()%255;
}
