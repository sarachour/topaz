#include "dist_detector.h"
#include "topaz.h"


AbsDistDetector::AbsDistDetector(int n) : AbsDetector(n){
	this->dists = new Distribution[n];
	this->n_dists = 0;
	this->max_dists = n;
	this->targ_fp = Topaz::topaz->config.DETECTOR_TARGET;
}
AbsDistDetector::~AbsDistDetector(){
	delete this->dists;
}
bool AbsDistDetector::test(){
	vector_t pt = Vector::wrap(this->data, this->n);
	//automatically reject if bad.
	for(int i=0; i < pt.n; i++){if(isBad(pt.d[i])) return false;}
	float lowest_score = -1;
	int best_dist = -1;
	//for each distribution
	for(int i=0; i < this->n_dists; i++){
		float sc = this->dists[i].dist(pt);
		float tsc = this->dists[i].thresh();
		if(!isBad(sc) && sc < tsc && (sc < lowest_score || best_dist == -1)){
			best_dist = i;
			lowest_score = sc;
		}
		this->dists[i].decay_use();
	}
	if(best_dist >= 0){
		this->dists[best_dist].update(pt);
		this->dists[best_dist].use();
		return true;
	}
	else return false;
}
float red_mag_diff(float a, float b, float r){
	return r + pow(a - b, 2);
}
bool AbsDistDetector::train(){
	vector_t  pt = Vector::wrap(this->data, this->n);
	vector_t ptk = Vector::wrap(this->data, this->n);
	
	//determine difference
	float diff = sqrt(Vector::reduce(&red_mag_diff, pt, ptk, 0));
	bool is_err = diff > EPS ? true : false;
	
	//add a new distribution
	if(this->n_dists < this->max_dists){
		int nu = this->n_dists;
		//update distribution
		this->dists[nu].init(n);
		this->dists[nu].update(ptk);
		this->dists[nu].set_targ(targ_fp);
		this->n_dists++;
	}
	float score=0;
	float score_key=0;
	int curr_dist = -1;
	int curr_dist_key = -1;
	//find closest existing distribution for key, error
	for(int i=0; i < this->n_dists; i++){
		float c_score = this->dists[i].dist(pt) - this->dists[i].thresh();
		float c_score_k = this->dists[i].dist(ptk) - this->dists[i].thresh();
		
		if(curr_dist < 0 || c_score < score){
			curr_dist = i;
			score = c_score;
		}
		if(curr_dist_key < 0 || c_score_k < score_key){
			curr_dist_key = i;
			score_key = i;
		}
	}
	
	//update distribution with false positive and update control system
	this->dists[curr_dist].decay_fp();
	if(!is_err){
		this->dists[curr_dist].fp();
	}
	this->dists[curr_dist].update_ctrl();
	
	//update key distribution with use key
	this->dists[curr_dist_key].use();
	this->dists[curr_dist_key].update(ptk);
	
	return true;
}

void AbsDistDetector::log(){

}
void AbsDistDetector::print(){
	for(int i=0; i < this->n_dists; i++){
		printf("## DETECTOR %d\n", i);
		this->dists[i].print();
	}
}
