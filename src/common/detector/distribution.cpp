#include "dist_detector.h"
#include "math.h"

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
	
	this->mean = Vector::create(0.0,d);
	this->sqsum = DSVector::create(0.0,d);
	this->sigma = DSVector::create(0.0,d);
}
//standard distribution functions
float K_update_mean(float mean, float v, float * args){
	float WT = args[0];
	float WT_I = args[1];
	return WT_I*mean + WT*v;
}
float K_update_sqsum(float sqsum, float v, float * args){
	float WT = args[0];
	float WT_I = args[1];
	return WT_I*sqsum + WT*v*v; //square sum
}
float K_update_sigma(float sqsum, float mean){
	return sqrt(sqsum - pow(mean,2));
}
void Distribution::update(vector_t pt){
	float weights[2];
	weights[0] = n < WINDOW_PTS ? 1.0/n : WT;
	weights[1] = 1.0 - weights[0];
	//update the running mean
	Vector::foreach(K_update_mean, this->mean, pt, this->mean, weights);
	DSVector::foreach(K_update_sqsum, this->sqsum, pt, this->sqsum, weights);
	//update the running standard deviation
	DSVector::foreach(K_update_sigma, this->sqsum, this->mean, this->sigma);
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
	vector_t tmp = Vector::create(d);
	Vector::foreach(K_dist_mean_diff,pt,this->mean,tmp);
	float dist = DSVector::reduce(K_dist_sigma_norm,this->sigma,tmp,0);
	Vector::dealloc(tmp);
	
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
float K_merge_sigma(float ssq, float mn){
	return sqrt(ssq - pow(mn,2));
}
void Distribution::merge(const Distribution& d){
	float args[2];
	args[0] = this->p_pt/(this->p_pt + d.p_pt);
	args[1] = this->p_pt/(this->p_pt + d.p_pt);
	Vector::foreach(K_merge_weighted, this->mean, d.mean,this->mean, args);
	DSVector::foreach(K_merge_weighted, this->sqsum, d.sqsum,this->sqsum, args);
	DSVector::foreach(K_merge_sigma,this->sqsum,this->mean,this->sigma);
}

void Distribution::update_ctrl(){
	
}
