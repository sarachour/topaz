#include "example_util.h"
#include "stdlib.h"
#include "math.h"

void seed_rand(){
	srand(0);
}
int rand_int(int LO, int HI){
	return LO + rand()%(HI-LO);
}
float rand_float(float LO, float HI){
	return LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
}
float ex_rand_norm(){
	return rand_float(0,1.0);
}
float rand_gauss(){
	float x1, x2, y1, y2;
	float w;
	do{
		x1 = 2.0 * ex_rand_norm() - 1.0;
		x2 = 2.0 * ex_rand_norm() - 1.0;
		w = x1*x1 + x2*x2;
	} while (w >= 1.0);
	
	w = sqrt( (-2.0 * log(w)) / w);
	y1 = x1 * w;
	y2 = x2 * w;
	return y1;
}
void rand_mgauss(int n, float * store){
	for(int i=0; i < n; i++){
		store[i] = rand_gauss();
	}
}
