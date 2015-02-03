#include "dist_detector.h"
#include "math.h"
#include "stdio.h"

Distribution::Distribution(){
	
}
Distribution::~Distribution(){
	Vector::dealloc(this->mean);
	DSVector::dealloc(this->sigma);
	DSVector::dealloc(this->sqsum);
}
void Distribution::init(int amt){
	this->d = amt; //dimensions in distribution
	this->n = 0; //number of points
	this->f = 1.0;
	
	this->mean = Vector::create(0.0,d);
	this->sqsum = DSVector::create(0.0,d);
	this->sigma = DSVector::create(0.0,d);
}
//standard distribution functions
float K_update_mean(float mean, float v, float * args){
	float WT = args[0];
	float WT_I = args[1];
	float RES =  WT_I*mean + WT*v;
	return RES;
}

void K_update_sqsum(vector_t mean, vector_t v, ds_vector_t sqsum, float * weights){
	float WT = weights[0];
	float WT_I = weights[1];
	for(int i=0; i < v.n; i++){
		if(v.d[i] >= mean.d[i]){
			sqsum.pos[i] = WT_I*sqsum.pos[i] + WT*v.d[i]*v.d[i]; //square sum
		}
		else{
			sqsum.neg[i] = WT_I*sqsum.pos[i] + WT*v.d[i]*v.d[i]; //square sum
		}	
	}
}
void K_update_sigma(ds_vector_t sqsum, vector_t mean, ds_vector_t sigma){
	for(int i=0; i < mean.n; i++){
		float mi = mean.d[i]*mean.d[i];
		sigma.pos[i] = sqrt(sqsum.pos[i] - mi);
		sigma.neg[i] = sqrt(sqsum.neg[i] - mi);
	}
}
void Distribution::update(vector_t pt){
	float weights[2];
	weights[0] = n < WINDOW_PTS ? 1.0/(n+1) : WT;
	weights[1] = 1.0 - weights[0];
	//update the running mean
	Vector::foreach(K_update_mean, this->mean, pt, this->mean, weights);
	K_update_sqsum(this->mean, pt, this->sqsum, weights);
	K_update_sigma(this->sqsum, this->mean, this->sigma);
	n++;
	
}
/*
 * Distance
 */
float K_dist_mean_diff(float v, float mean){
	return v - mean;
}
float K_dist_sigma_norm(float sig, float diff, float dist){
	return dist + pow(diff/sig,2);
}
float Distribution::dist(vector_t pt){
	float dist = 0;
	for(int i=0; i < pt.n; i++){
		float diff = (pt.d[i] - this->mean.d[i]);
		if(diff>0) dist += fabs(diff)/this->sigma.pos[i];
		else dist += fabs(diff)/this->sigma.neg[i];
	}
	return sqrt(dist);
}

/*
 * Merge Kernel
 * 
 */
float K_merge_weighted(float a, float b,float * args){
	float ac = args[0];
	float bc = args[1];
	return a*ac + b*bc;
}
void Distribution::merge(const Distribution& d){
	float args[2];
	args[0] = this->p_pt/(this->p_pt + d.p_pt);
	args[1] = this->p_pt/(this->p_pt + d.p_pt);
	Vector::foreach(K_merge_weighted, this->mean, d.mean,this->mean, args);
	DSVector::foreach(K_merge_weighted, this->sqsum, d.sqsum,this->sqsum, args);
	K_update_sigma(this->sqsum, this->mean, this->sigma);
	
}

void Distribution::update_ctrl(){
	
}
void Distribution::print(){
	printf("Mean:\n");
	Vector::print(this->mean);
	printf("SqSum:\n");
	DSVector::print(this->sqsum);
	printf("Sigma:\n");
	DSVector::print(this->sigma);
	printf("Stats:\n");
	printf("nstd: %f, percent-pts: %f, percent-fp: %f\n", this->f, this->p_pt, this->p_fp);
}



