#include "checker.h"
#include "stdlib.h"
#include "const.h"
//svm

void svm_init(checker_t * c){
	int i=0;
	c->impl.svm.center = (double*) malloc(sizeof(double)*c->ndims);
	for(i=0; i < c->ndims; i++){
		c->impl.svm.center[i] = 0;
	}
}

void svm_destroy(checker_t * c){
	free(c->impl.svm.center);
}
int svm_test(checker_t * c, double * t){
	return true;
}

void checker_set_init(checker_set_t * t, unsigned int nplaces, unsigned int ntasks){
	t->checkers = malloc(nplaces*ntasks*sizeof(checker_t));
	t->nplaces = nplaces;
	t->ntasks = ntasks;
}
void checker_set_destroy(checker_set_t * t){
	int i;
	for(i=0; i < t->ntasks*t->nplaces; i++){
		checker_destroy(&t->checkers[i]);
	}
	free(t->checkers);
}
checker_t * checker_set_get(checker_set_t * t, unsigned int place, unsigned int task){
	return &t->checkers[t->nplaces*task + place];
}

void checker_init(checker_t * c, int ndims){
	c->ndims = ndims;
	c->f_pos = 0;
	c->f_neg = 0;
	c->t_pos = 0;
	c->t_neg = 0;
	c->npts = 0;
	svm_init(c);
}
void checker_destroy(checker_t * c){
	svm_destroy(c);
}
int checker_test_int(checker_t * c, int * v){
	return svm_test(c, (double*) v);
}
int checker_test_float(checker_t * c, float * v){
	return svm_test(c, (double*) v);
}
int checker_test_double(checker_t * c, double * v){
	return svm_test(c, (double*) v);
}
int checker_test_char(checker_t * c, char * v){
	return svm_test(c, (double*) v);
}
