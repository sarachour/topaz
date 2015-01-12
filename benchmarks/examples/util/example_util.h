#ifndef EXAMPLE_H
#define EXAMPLE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct T_DIST {
	float * mean;
	float * sigma;
	int d;
} dist_t;

typedef struct T_DIST_SET_T {
	dist_t * d;
	int n;
} dist_set_t;


void seed_rand();
int rand_int(int lo, int hi);
float rand_float(float lo, float hi);
float ex_rand_norm();
float rand_gauss();
void rand_mgauss(int n, float * store);

void populate_dist(dist_t * d, int x);
void populate_dists(dist_set_t * d, int n, int x);
void unpopulate_dist(dist_t * d);
void unpopulate_dists(dist_set_t * d);
void sample_dist(dist_t * d, float * store);
dist_set_t * sample_dists(dist_set_t * d, int n, int x, float * store);

#ifdef __cplusplus
}
#endif
#endif
