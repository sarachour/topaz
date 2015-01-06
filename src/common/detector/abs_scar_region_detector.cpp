#include "scar_detector.h"
#include "util.h"
#include "stdio.h"
#include "logger.h"
#include "topaz.h"
#include "ctrl_system.h"
//FIXME: Must leave index 0 open for err-value

void AbsScarRegionDetector::log(){
	DetectorLogInfo * l = Topaz::topaz->getDLog();
	bool isaccepted = DetectorLogInfo::getAccepted();
	bool iscorr = this->compare();
	int tid = DetectorLogInfo::getTaskId();
	int rank = DetectorLogInfo::getRank();
	int iid = DetectorLogInfo::getIID();
	
	for(int v=0; v < this->dim; v++){
		int i=1;
		l->start_entry(tid, iid, rank, v, isaccepted, iscorr, this->data[v], this->data_key[v]); 
		i=this->stats->log(l, i);
		l->set(i,"n-regions", this->n_regions); i++;
		for(int r=0; r < this->n_regions; r++){
			char name[128];
			region_t * q = this->regions[r];
			
			sprintf(name, "%d.min", r); 
			l->set(i,name, q->min[v]); i++;
			sprintf(name, "%d.max", r); 
			l->set(i,name,q->max[v]); i++;
			sprintf(name, "%d.center", r); 
			l->set(i,name,q->center[v]); i++;
			i=q->stats.log(l,r,i);
		}
		l->end_entry();
	}

	
}
AbsScarRegionDetector::AbsScarRegionDetector(int n) : AbsDetector(n){
	this->dim = n;
	this->n_regions = 0;
	this->max_regions = Topaz::topaz->config.DETECTOR_NBLOCKS;
	this->env = new RegionStats[1];
	this->stats = new GlobalStats[1];
	this->ctrl = new PIDControlSystem[1];
	
	this->set_control_target();
	for(int i=0; i < this->max_regions; i++){
		this->regions[i] = NULL; //initialize all to null
	}
}
AbsScarRegionDetector::~AbsScarRegionDetector(){
	for(int i=0; i < this->max_regions; i++){
		if(this->regions[i] != NULL){
			delete this->regions[i];
		}
	}
	delete this->env;
	delete this->stats;
	delete this->ctrl;
	this->clean();
}
bool AbsScarRegionDetector::test(){
	if(AbsDetector::getMode() ==  ABS_DETECTOR_KEY) return true;
	bool res =  this->test_point(this->data);
	//printf("test: %f %s -> pct-rej:%f\n", this->data[0], res ? "accept" : "reject", this->stats.n_rej/this->stats.n_total_test);
	return res;
}
bool AbsScarRegionDetector::train(){
	if(AbsDetector::getMode() ==  ABS_DETECTOR_KEY) return true;
	bool corr = this->compare();
	//printf("train: %f = %f : %s pct_fp:%f\n", this->data[0], this->data_key[0], corr ? "same":"not same", this->stats.n_false/this->stats.n_total_train);
	//insert the training point
	this->insert_point(this->data, corr);
	if(!corr) this->insert_point(this->data_key,true);
	return true;
}
void AbsScarRegionDetector::print(){
	for(int i=0; i < this->n_regions; i++){
		region_t * r = this->regions[i];
		printf("BLOCK %d\n",i);
		r->stats.print();
		printf("min: ");
		for(int j=0; j < this->dim; j++){
			printf("%e ",r->min[j]);
		}
		printf("\n");
		printf("max: ");
		for(int j=0; j < this->dim; j++){
			printf("%e ",r->max[j]);
		}
		printf("\n------\n\n");
	}
	printf("ENVIRONMENT\n");
	this->env->print();
	printf("STATISTICS\n");
	this->stats->print();
	
}



bool AbsScarRegionDetector::is_valid(float * val){
	for(int i=0; i < this->dim; i++){
		if(isNaN(val[i]) || isInf(val[i]) || isNegInf(val[i])) 
			return false;
	}
	return true;
}
void AbsScarRegionDetector::allocate_region(region_t * region, float * d){
	region->min = new float[this->dim];
	region->max = new float[this->dim];
	region->center = new float[this->dim];
	for(int i=0; i < this->dim; i++){
		region->min[i] = region->max[i] = region->center[i] = d[i];
	}
}
void AbsScarRegionDetector::deallocate_region(region_t * region){
	delete region->min;
	delete region->max;
	delete region->center;
}

int AbsScarRegionDetector::find_region(float * d){
	int idx = -1;
	float score = -1;
	for(int i=0; i < this->n_regions; i++){
		region_t * region = this->regions[i];
		bool is_idx = true;
		float tmp_score = 0;
		float dist = 0;
		float area = 0;
		for(int j=0; j < this->dim; j++){
			//compute the distance and scale.
			float ldist = (d[j] - region->center[j]);
			float hdist = (region->max[j] - region->center[j]);
			float absdist = ldist*ldist;
			float scale = hdist*hdist;
			area += scale;
			dist += absdist > scale ? absdist : scale;
			//determine if we're out of the bound
			if(region->min[j] > d[j] || region->max[j] < d[j])
				is_idx = false;
		}
		//used to just be smallest area.
		tmp_score = 1/area;
		if(is_idx && (idx < 0 || tmp_score > score)){
				idx = i;
				score = tmp_score;
		}
	}
	return idx;
}
//merge r2 into r1
void AbsScarRegionDetector::merge_regions(int id1, int id2){
	region_t * r1 =	this->regions[id1];
	region_t * r2 = this->regions[id2];
	for(int i=0; i < this->dim; i++){
		if(r2->min[i] < r1->min[i]){
			r1->min[i] = r2->min[i];
		}
		if(r2->max[i] > r1->max[i]){
			r1->max[i] = r2->max[i];
		}
		r1->center[i] = (r1->center[i] + r1->center[i])/2;
	}
	r1->stats.merge(&r2->stats);
	
	this->deallocate_region(r2);
	//move everything over
	for(int i=id2; i < this->max_regions-1; i++){
		this->regions[i] = this->regions[i+1];
	}
	this->regions[this->max_regions-1] = NULL;
	
}
//find region, ie find the closest region with similar output, error 
//characteristics, returning score.
/* We want to merge nearby regions with a good score
 * 
 */
float AbsScarRegionDetector::score_region(region_t * r1, region_t * r2){
	float distance = 0;
	//compute the distance
	for(int j=0; j < this->dim; j++){
		float d = (r1->center[j] - r2->center[j]);
		distance += d*d;
	}
	float overlap;
	overlap=1;
	for(int i=0; i < this->dim; i++){
		float d1 = r1->max[i] - r1->min[i];
		float d2 = r2->max[i] - r2->min[i];
		float ov;
		if(r1->min[i] < r2->min[i])
			ov = d1 - (r2->min[i] - r1->min[i]);
		else
			ov = d2 - (r1->min[i] - r2->min[i]);
		ov = ov < 0 ? 0 : ov;
		overlap *= ov;
	}
	float score=0;
	//printf("b:%f d:%f o:%f = %f\n", behavior, distance, overlap, score);
	//if we are overlapping with another group, merge regardless.
	if(overlap > 0) return overlap;
	//severely penalize groups where there is an interfering group 
	score -= distance; //negatively impact behavior.
	
	return score;
}
int AbsScarRegionDetector::find_closest_region(int idx, float * score){
	region_t * region = this->regions[idx];
	int id = -1;
	for(int i=0; i < this->n_regions; i++){
		if(i == idx) continue;
		//printf("score [%d,%d]:", idx, i);
		float sc = score_region(region, this->regions[i]);
		if(id < 0 || sc > *score){
			id = i;
			*score = sc;
		}
		
	}
	return id;
}
void AbsScarRegionDetector::adjust_region(region_t * r){
	float amt = this->ctrl->get();
	amt = amt < 0.5 ? 0.5 : (amt > 1 ? 1 : amt);
	for(int i=0; i < this->dim; i++){
		r->min[i] = r->center[i] - (r->center[i] - r->min[i])*amt;
		r->max[i] = r->center[i] + (r->max[i] - r->center[i])*amt;
	}
}

void AbsScarRegionDetector::adjust_control(){
	this->ctrl->update(this->stats->get_reexec());
}
void AbsScarRegionDetector::set_control_target(){
	this->ctrl->goal(Topaz::topaz->config.DETECTOR_TARGET);
}

void AbsScarRegionDetector::update_test_regions(int id, float * val, bool is_accept){
	//forget behavior
	this->adjust_control();
	
	for(int i=0; i < this->n_regions; i++){
		this->regions[i]->stats.update_accept_rate(i == id, is_accept);
		this->adjust_region(this->regions[i]);
	}
	if(id >= 0){
		//update center of mass.
		for(int i=0; i < this->dim; i++){
			this->regions[id]->center[i] *= 1.0 - 1.0/this->CENTER_WINDOW;
			this->regions[id]->center[i] += val[i]*1.0/this->CENTER_WINDOW;
		}
	}
	else 
		this->env->update_accept_rate(id < 0, false);
		
	this->stats->update_accept_rate(is_accept);
}
void AbsScarRegionDetector::update_train_regions(int id, float * val, bool is_correct){
	
	//forget behavior
	for(int i=0; i < this->n_regions; i++){
		this->regions[i]->stats.update_accuracy_rate(id == i, is_correct);
	}
	//we do not have a region.
	this->env->update_accuracy_rate(id < 0, true);
	
	this->stats->update_accuracy_rate(is_correct); //is accept, is error
	
	if(id < 0 || !is_correct) return; //if outside of region, or error.
	//we have a region
	region_t * r = this->regions[id];
	
	//update boundaries to include point.
	//on train update center of mass.
	for(int i=0; i < this->dim; i++){
		if(val[i] > r->max[i]) r->max[i] = val[i];
		else if(val[i] < r->min[i]) r->min[i] = val[i];
		r->center[i] *= 1.0 - 1.0/this->CENTER_WINDOW;
		r->center[i] += val[i]*1.0/this->CENTER_WINDOW;
	}
	
}
bool AbsScarRegionDetector::test_point(float * d){
	int id = find_region(d); // find the closest scoring info.
	//printf("test: %f grp:%d score:%f\n", d[0], id, score);
	//printf("score: %f\n", score);
	if(!this->is_valid(d) || id < 0){
		this->update_test_regions(-1,d,false);
		return false;
	}
	else{
		this->update_test_regions(id,d,true);
		return true;
	}
	
}
void AbsScarRegionDetector::insert_point(float * d, bool is_correct){
	if(!this->is_valid(d)) return;
	/*
	 * try and optimistically find a region the float belongs to
	 */
	int id = find_region(d);
	// case: update an existing a group
	if(id > 0 || !is_correct){
		this->update_train_regions(id,d,is_correct); //updates statistics.
		return;
	}
	// case: create a new group
	
	//create a new node.
	region_t * r = new region_t[1];
	this->allocate_region(r,d);
	this->regions[this->n_regions] = r;
	this->n_regions++;
	this->update_train_regions(this->n_regions-1,d, is_correct);
	//add region
	//if we're full of space, make room for another region.
	if(this->n_regions == this->max_regions){ //if we're full.
		//find the region with the most similar dynamics that isn't
		//completely obstructed by another region.
		int merge_1=-1;
		int merge_2=-1;
		float score = 0;
		for(int i=0; i < this->n_regions; i++){
			float tmp_score=0;
			int tmp_other = find_closest_region(i, &tmp_score);
			if(merge_1 < 0 || tmp_score > score){
				score = tmp_score;
				merge_1 = i;
				merge_2 = tmp_other;
			}
		}
		merge_regions(merge_1, merge_2);
		this->n_regions--;
	}
	/*
	 * 
	 */
	
	
}
