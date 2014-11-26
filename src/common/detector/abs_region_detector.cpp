#include "abs_detectors.h"
#include "util.h"
#include "stdio.h"
#include "logger.h"
#include "topaz.h"
//FIXME: Must leave index 0 open for err-value

void AbsRegionDetector::log(){
	DetectorLogInfo * l = Topaz::topaz->getDLog();
	bool isaccepted = DetectorLogInfo::getAccepted();
	bool iscorr = this->compare();
	int tid = DetectorLogInfo::getTaskId();
	int rank = DetectorLogInfo::getRank();
	int iid = DetectorLogInfo::getIID();
	stats_t * s = &this->stats;
	
	for(int v=0; v < this->dim; v++){
		int i=1;
		l->start_entry(tid, iid, rank, v, isaccepted, iscorr, this->data[v], this->data_key[v]); 
		l->set(i,"pct_rej", s->n_acc/s->n_total_test); i++;
		l->set(i,"pct_acc",s->n_rej/s->n_total_test); i++;
		l->set(i,"pct_true_rej",s->n_true/s->n_total_train); i++;
		l->set(i,"pct_false_rej",s->n_false/s->n_total_train); i++;
		l->set(i,"n-regions", this->n_regions); i++;
		for(int r=0; r < this->n_regions; r++){
			char name[128];
			region_t * q = this->regions[r];
			float prob, tr_prob, te_prob;
			tr_prob = q->p_train_corr/q->p_train_n; 
			te_prob = q->p_test_corr/q->p_test_n;
			prob = tr_prob*te_prob;
			sprintf(name, "%d.min", r); 
			l->set(i,name, q->min[v]); i++;
			sprintf(name, "%d.max", r); 
			l->set(i,name,q->max[v]); i++;
			sprintf(name, "%d.prob", r); 
			l->set(i,name,prob); i++;
		}
		l->end_entry();
	}

	
}
AbsRegionDetector::AbsRegionDetector(int n) : AbsDetector(n){
	this->dim = n;
	this->n_regions = 0;
	this->environment = new env_t[1];
	this->stats.n_rej = 0;
	this->stats.n_acc = 0;
	this->stats.n_true = 0;
	this->stats.n_false = 0;
	this->stats.n_total_test = 0;
	this->stats.n_total_train = 0;
	for(int i=0; i < this->max_regions; i++){
		this->regions[i] = NULL; //initialize all to null
	}
}
AbsRegionDetector::~AbsRegionDetector(){
	for(int i=0; i < this->max_regions; i++){
		if(this->regions[i] != NULL){
			delete this->regions[i];
		}
	}
	
	this->clean();
}
#define WEIGHT 0.999
bool AbsRegionDetector::test(){
	if(AbsDetector::getMode() ==  ABS_DETECTOR_KEY) return true;
	bool res =  this->test_point(this->data);
	//update running statistics
	this->stats.n_acc*=WEIGHT;
	this->stats.n_rej*=WEIGHT;
	this->stats.n_total_test*=WEIGHT;
	if(res) this->stats.n_acc+=1-WEIGHT;
	else this->stats.n_rej+=1-WEIGHT;
	this->stats.n_total_test+=1-WEIGHT;
	//printf("test: %f %s -> pct-rej:%f\n", this->data[0], res ? "accept" : "reject", this->stats.n_rej/this->stats.n_total_test);
	return res;
}
bool AbsRegionDetector::train(){
	if(AbsDetector::getMode() ==  ABS_DETECTOR_KEY) return true;
	bool corr = this->compare();
	
	this->stats.n_true*=WEIGHT;
	this->stats.n_false*=WEIGHT;
	this->stats.n_total_train*=WEIGHT;
	if(!corr) this->stats.n_true+=1-WEIGHT;
	else this->stats.n_false+=1-WEIGHT;
	this->stats.n_total_train+=1-WEIGHT;
	
	//printf("train: %f = %f : %s pct_fp:%f\n", this->data[0], this->data_key[0], corr ? "same":"not same", this->stats.n_false/this->stats.n_total_train);
	//insert the training point
	this->insert_point(this->data, !corr);
	if(!corr) this->insert_point(this->data_key,true);
	return true;
}
void AbsRegionDetector::print(){
	for(int i=0; i < this->n_regions; i++){
		region_t * r = this->regions[i];
		printf("BLOCK %d\n",i);
		printf("train | n: %f  p(total): %f   p(err): %f   p(out): %f\n", 
			r->p_train_n, r->p_train_total,
			r->p_train_err/r->p_train_n, r->p_train_corr/r->p_train_n);
		printf("test | n: %f  p(total): %f   p(err): %f   p(out): %f\n", 
			r->p_test_n, r->p_test_total,
			r->p_test_err/r->p_test_n, r->p_test_corr/r->p_test_n);
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
	env_t * e = this->environment;
	printf("test | n: %f  p(total): %f   p(err): %f   p(out): %f\n", 
			e->p_test_n, e->p_test_total,
			e->p_test_err/e->p_test_n, e->p_test_corr/e->p_test_n);
	printf("STATISTICS\n");
	stats_t * s = &this->stats;
	printf("accept:%f reject%f total:%f\n", s->n_acc/s->n_total_test, s->n_rej/s->n_total_test, s->n_total_test);
	printf("true-rej:%f false-rej:%f total-rej:%f\n", s->n_true/s->n_total_train, s->n_false/s->n_total_train, s->n_total_train);
}



bool AbsRegionDetector::is_valid(float * val){
	for(int i=0; i < this->dim; i++){
		if(isNaN(val[i]) || isInf(val[i]) || isNegInf(val[i])) 
			return false;
	}
	return true;
}
void AbsRegionDetector::allocate_region(region_t * region, float * d){
	region->min = new float[this->dim];
	region->max = new float[this->dim];
	region->center = new float[this->dim];
	region->p_train_err = region->p_test_err = 0;
	region->p_train_corr = region->p_test_corr = 0;
	region->p_train_total = region->p_test_total = 0;
	region->p_train_n = region->p_test_n = 0;
	for(int i=0; i < this->dim; i++){
		region->min[i] = region->max[i] = region->center[i] = d[i];
	}
}
void AbsRegionDetector::deallocate_region(region_t * region){
	delete region->min;
	delete region->max;
	delete region->center;
}

int AbsRegionDetector::find_region(float * d, float * score){
	int idx = -1;
	*score = 0;
	float size = -1;
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
		tmp_score = sqrt(area/dist);
		if(is_idx){
			if(size < 0 || area < size){
				*score = 1;
				size = area;
				idx = i;
			}
		}
		else{
			//compute distance and normalizing factor.
			//the score is the 1/(distance/scale);
			if(idx == -1 || (*score < tmp_score && size == -1)){
				idx = i;
				*score = tmp_score;
			}
		}
	}
	return idx;
}
//merge r2 into r1
void AbsRegionDetector::merge_regions(int id1, int id2){
	region_t * r1 =	this->regions[id1];
	region_t * r2 = this->regions[id2];
	for(int i=0; i < this->dim; i++){
		if(r2->min[i] < r1->min[i]){
			r1->min[i] = r2->min[i];
		}
		if(r2->max[i] > r1->max[i]){
			r1->max[i] = r2->max[i];
		}
		r1->center[i] = (r1->max[i] + r1->min[i])/2.0;
	}
	r1->p_test_err = (r1->p_test_err + r2->p_test_err);
	r1->p_test_corr = (r1->p_test_corr + r2->p_test_corr);
	r1->p_test_n += r2->p_test_n;
	r1->p_test_total = (r1->p_test_total + r2->p_test_total);
	
	r1->p_train_err = (r1->p_train_err + r2->p_train_err);
	r1->p_train_corr = (r1->p_train_corr + r2->p_train_corr);
	r1->p_train_n += r2->p_train_n;
	r1->p_train_total = (r1->p_train_total + r2->p_train_total);
	
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
float AbsRegionDetector::score_region(region_t * r1, region_t * r2){
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
int AbsRegionDetector::find_closest_region(int idx, float * score){
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
void AbsRegionDetector::update_test_regions(int id, bool iserr){
	//forget behavior
	float factor =  1.0-1.0/WINDOW;
	for(int i=0; i < this->n_regions; i++){
		//probability output falls in this output
		this->regions[i]->p_test_total *= factor;
	}
	this->environment->p_test_total *= factor;
	if(id >= 0){
		region_t * r;
		r = this->regions[id];
		r->p_test_n += 1.0;
		r->p_test_total += 1-factor;
		if(iserr) r->p_test_err += 1.0;
		else r->p_test_corr += 1.0;
	}
	else{
		env_t * e = this->environment;
		e->p_test_err += 1.0;
		e->p_test_n += 1.0;
		e->p_test_total += 1.0/WINDOW;
	}
	
}
void AbsRegionDetector::update_train_regions(int id, float * val, bool iserr){
	
	//forget behavior
	for(int i=0; i < this->n_regions; i++){
		//probability output falls in this output
		this->regions[i]->p_train_total *= 1.0-1.0/WINDOW;
	}
	//we do not have a region.
	if(id < 0){
		env_t * e = this->environment;
		e->p_train_err += 1.0;
		e->p_train_n += 1.0;
		e->p_train_total += 1.0/WINDOW;
		return;
	}
	//we have a region
	region_t * r = this->regions[id];
	r->p_train_n += 1.0;
	r->p_train_total += 1.0/WINDOW;
	//update with most recent behavior.
	if(iserr){
		//probability, given it falls in output
		r->p_train_err += 1.0;
		return;
	}
	else{
		r->p_train_corr += 1.0;
	}
	
	//update boundaries to include point.
	for(int i=0; i < this->dim; i++){
		if(val[i] > r->max[i]) r->max[i] = val[i];
		else if(val[i] < r->min[i]) r->min[i] = val[i];
		r->center[i] = (r->max[i] + r->min[i])/2.0;
	}
}
bool AbsRegionDetector::test_point(float * d){
	float score;
	bool isok = true;
	int id = find_region(d, &score); // find the closest scoring info.
	//printf("test: %f grp:%d score:%f\n", d[0], id, score);
	//printf("score: %f\n", score);
	if(!this->is_valid(d) || id < 0){
		isok = false;
	}
	if(isok &&rand_norm() <= score){
		region_t * r = this->regions[id];
		//probability, given this event happens, that this event is an error.
		//basically, the true positive rate.
		float prob = r->p_train_err/r->p_train_n*r->p_test_err/r->p_test_n;
		if(isNaN(prob)) prob = 0;
		else if(isInf(prob)) prob = 1;
		//printf("train| test-err:%f	train-err:%f min:%f , max:%f\n",r->p_test_err/r->p_test_n, r->p_train_err/r->p_train_n, r->min[0],r->max[0]);
		if(rand_norm() > prob){
			/*
			printf("accept %f (id=%d, bnd=[%f,%f] prob=%f, score=%f)\n",data[0],id,
				regions[id]->min[0], regions[id]->max[0],prob, score);
			*/
			isok= true;
		}
		else {
			isok= false;
		}
		this->update_test_regions(id,!isok);
	}
	else {
		id=-1; //we decided it's not part of the distribution.
		isok = false; //reject because this point does not belong anywhere.
		this->update_test_regions(-1,!isok);
	}
		
	return isok;
}
void AbsRegionDetector::insert_point(float * d, bool iserr){
	if(!this->is_valid(d)) return;
	/*
	 * try and optimistically find a region the float belongs to
	 */
	float score;
	int id = find_region(d, &score);
	//printf("train: %f grp:%d score:%f\n", d[0], id, score);
	/*v
	 * Probabilistically choose whether we should extend an existing group 
	 * or create a new group.
	 */
	// case: update an existing a group
	if(id >= 0 && (score == 1.0 || rand_norm() <= score)){
		this->update_train_regions(id,d,iserr); //updates statistics.
		return;
	}
	if(iserr){
		this->update_train_regions(-1,d,iserr); //updates statistics.
		return;
	}
	// case: create a new group
	
	//create a new node.
	region_t * r = new region_t[1];
	this->allocate_region(r,d);
	this->regions[this->n_regions] = r;
	this->n_regions++;
	this->update_train_regions(this->n_regions-1,d, iserr);
	//add region
	//if we're full of space, make room for another region.
	if(this->n_regions == this->max_regions){ //if we're full.
		//find the region with the most similar dynamics that isn't
		//completely obstructed by another region.
		int merge_1=-1;
		int merge_2=-1;
		score = 0;
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
