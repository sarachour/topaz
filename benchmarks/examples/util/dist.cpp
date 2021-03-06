#include "example_util.h"
#include "stdio.h"
#include "pin_util.h"
#include "stdint.h"
void populate_dist(dist_t * d, int dim){
	d->d = dim;
	d->mean = new float[dim];
	d->sigma = new float[dim];
	
	pin_mark_urel(d->mean, sizeof(float)*dim);
	pin_mark_urel(d->sigma, sizeof(float)*dim);
	
	for(int i=0; i < dim; i++){
		d->mean[i] = rand_float(-1000,1000);
		d->sigma[i] = rand_float(0,100);
	}
	printf("   mean: ");
	for(int i=0; i < dim; i++) printf("%f\t", d->mean[i]);
	printf("\n");
	printf("   sigma: ");
	for(int i=0; i < dim; i++) printf("%f\t", d->sigma[i]);
	printf("\n");
}
void populate_dists(dist_set_t * d, int n, int dim){
	seed_rand();
	d->n = n;
	d->d = new dist_t[n];
	for(int i=0; i < n; i++){
		printf("dist %d\n", i);
		populate_dist(&d->d[i], dim);
	}
}
void unpopulate_dist(dist_t * d){
	delete d->mean;
	delete d->sigma;
}
void unpopulate_dists(dist_set_t * d){
	for(int i=0; i < d->n; i++){
		unpopulate_dist(&d->d[i]);
	}
	delete d->d;
}
void sample_dist(dist_t * d, float * store){
	pin_mark_urel(store, d->d*sizeof(float));
	rand_mgauss(d->d, store);
	for(int i=0; i < d->d; i++){
		store[i] = d->mean[i] + d->sigma[i]*store[i];
	}
}
dist_set_t * sample_dists(dist_set_t * d, int n, int dim, float * store){
	if(d == 0){
		d = new dist_set_t[1];
		populate_dists(d,n,dim);
	}
	int i = rand_int(0,d->n);
	sample_dist(&d->d[i], store);
	
	return d;
}
