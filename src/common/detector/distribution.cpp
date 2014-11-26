#include "dist_detector.h"
#include "util.h"
#include "dist_detector.h"
#include "stdio.h"

chunk_t * Distribution::allocate_chunk(){
	chunk_t * c = new chunk_t();
	c->left = 0;
	c->right = 0;
	c->n_err = 0;
	c->n_out = 0;
	c->p_rej = 0;
	c->next = NULL;
	c->prev = NULL;
	this->oob_prob = 1.0;
	return c;
	
}
Distribution::Distribution(){
	this->chunks = NULL;
	this->n = 
		this->n_vals = 
		this->n_outs = 
		this->oob_err =
		this->n_errs = 0;
}
Distribution::~Distribution(){
	chunk_t * CHUNK  = this->chunks;
	while(CHUNK != NULL){
		chunk_t * tmp = CHUNK;
		CHUNK = CHUNK->next;
		delete tmp;
	}
}
void Distribution::merge_one(){
	//we want to merge like nodes that are closely clustered.
	//must complete at least one merge.
	chunk_t * merge_1=NULL; //first node
	chunk_t * merge_2=NULL;
	float dscore = -1;
	bool shrt=  false;
	/*
	 * I want to merge adjascent purely errorneous states.
	 * 
	 */
	for(chunk_t * c = this->chunks; c != NULL && !shrt; c=c->next){
		chunk_t * prev = c->prev;
		if(prev !=  NULL){
			float tmp_dscore = c->left - prev->right; //distance between clusters
			if(dscore < 0 || (tmp_dscore <= dscore)){
				dscore = tmp_dscore;
				merge_1 = prev;
				merge_2 = c;
			}
		}
	}
	if(merge_1 != NULL){
		/*
		this->print();
		printf("merge [%f %f: %f %f] [%f %f: %f %f]\n\n", 
		merge_1->left,merge_1->right,  merge_1->n_out, merge_1->n_err,
		merge_2->left, merge_2->right, merge_2->n_out, merge_2->n_err);
		*/
		merge_1->n_out += merge_2->n_out;
		merge_1->n_err += merge_2->n_err;
		merge_1->p_rej += merge_2->p_rej;
		merge_1->p_rej*=0.5;
		merge_1->right = merge_2->right;
		
		merge_1->next = merge_2->next;
		if(merge_2->next != NULL) merge_2->next->prev = merge_1;
		
		delete (merge_2);
		n--;
	}
	
	
}
void Distribution::merge(){
	//merge exactly one
	for(int i=0; i < 1; i++){
		this->merge_one();
	}
}
void Distribution::insert_value(float i, bool iserr){
	int incr_out = iserr ? 0 : 1;
	int incr_err = iserr ? 1 : 0;
	
	this->n_outs+=incr_out;
	this->n_errs+=incr_err;
	this->n_vals++;
	if(this->n >= MAX_NODES){
		this->merge();
	}
	//first element
	if(this->n == 0){
		this->chunks = allocate_chunk(); 
		this->chunks->left = i;
		this->chunks->right = i;
		this->chunks->n_out+= incr_out; //increment the number of outputs
		this->chunks->n_err+= incr_err; //increment the number of outputs
		this->n++;
		
	}
	else {
		chunk_t * curr = NULL; 
		chunk_t * prev = NULL;
		for(curr = this->chunks; curr != NULL && curr->left <= i; curr = curr->next){
			prev = curr;
		}
		//if this is part of an interval, increment
		if(prev != NULL && i <= prev->right){ // inside the previous element
			prev->n_out+= incr_out; //increment the number of outputs
			prev->n_err+= incr_err; //increment the number of outputs
		}
		else { //in between the previous and next element
			if(iserr){
				this->oob_err++;
				return;
			}
			chunk_t * n = allocate_chunk();
			n->n_out+= incr_out; //increment the number of outputs
			n->n_err+= incr_err; //increment the number of outputs
			n->prev = prev;
			n->next = curr;
			n->left = i;
			n->right = i;
			if(prev != NULL) prev->next = n;
			else this->chunks = n;
			if(curr != NULL) curr->prev = n;
			this->n++;
		}
	}
	
}
void Distribution::output(float i){
	this->insert_value(i, false);
}
void Distribution::error(float i){
	this->insert_value(i, true);
}
chunk_t * Distribution::find_chunk(float f){
	chunk_t * prev = NULL;
	chunk_t * curr;
	for(curr = this->chunks; curr != NULL && curr->left <= f; curr = curr->next){
		prev = curr;
	}
	if(prev != NULL && prev->right >= f) return prev;
	else{
		//return the chunk you're closest to.
		if(prev != NULL && curr != NULL){
			float l = prev->right;
			float r = curr->left;
			if(fabs(f - l) < fabs(f - r)){
				return prev;
			}
			else{
				return curr;
			}
		}
		else if(prev != NULL){
			return prev;
		}
		else if(curr != NULL){
			return curr;
		}
		
	}
	return NULL;
}
float Distribution::get_output_prob(chunk_t *  c, float f){
	if(c != NULL && c->left <= f && c->right>=f) return c->n_out/this->n_vals;
	else return 0;
	
}
float Distribution::get_error_prob(chunk_t * c, float f){
	if(c != NULL && c->left <= f && c->right>=f) return (c->n_err/this->n_vals);
	else return 0;
	
}
float Distribution::get_error_cum_prob(){
	return (this->n_errs/this->n_vals);
}
float Distribution::get_output_cum_prob(){
	return (this->n_outs/this->n_vals);
}
float Distribution::get_error_cond_prob(chunk_t * c, float f){
	if(c != NULL && c->left <= f && c->right>=f) return c->n_err/(c->n_err+c->n_out);
	else return 0;
	
}
float Distribution::get_output_cond_prob(chunk_t * c, float f){
	if(c != NULL && c->left <= f && c->right>=f) return c->n_err/(c->n_err+c->n_out);
	else return 0;
	
}
float Distribution::get_prob_reject(chunk_t * c, float f){
	if(c != NULL){
		if(f >= c->left && f <= c->right){
			return c->p_rej;
		}
		else{
			float half  = (c->right - c->left)/2;
			float dist;
			//determine the distance from the chunk border
			if(f < c->left) dist = c->left - f;
			else if(f > c->right) dist = f-c->right;
			
			if(dist > half) 
				return this->oob_prob;
			else 
				return (dist/half)*this->oob_prob + (1-dist/half)*c->p_rej;
		}
		
	}
	else return this->oob_prob;
	
}

void Distribution::clear(){
	chunk_t * CHUNK  = this->chunks;
	while(CHUNK != NULL){
		chunk_t * tmp = CHUNK;
		CHUNK = CHUNK->next;
		delete tmp;
	}
	this->chunks = NULL;
	this->n = 
		this->n_vals = 
		this->n_outs = 
		this->n_errs = 0;
}

void Distribution::print(){
	chunk_t * curr;
	int i=0;
	printf("oob errs: %f\n", this->oob_err);
	printf("oob prob: %f\n", this->oob_prob);
	printf("id\tleft\tright\t\touts\terrs\tprej\n");
	for(curr = this->chunks; curr != NULL; curr = curr->next){
		printf("%d\t%3f\t%3f\t\t%f\t%f\t%f\n",
			i,
			curr->left,
			curr->right,
			curr->n_out,
			curr->n_err,
			curr->p_rej);
		i++;
	}
}

chunk_t Distribution::get(int i){
	int k=0;
	chunk_t * curr;
	for(curr = this->chunks; curr != NULL; curr = curr->next){
		if(k = i) return (*curr);
		k++;
	} 
	chunk_t c;
	return c;
}
//we want to reject the nodes with the highest reject rate.
void Distribution::allocate(float amt){
	/*
	 * Find the highest probability error
	 * 
	 */
	chunk_t * curr;
	double ded;
	for(curr = this->chunks; curr != NULL; curr = curr->next){
		curr->p_rej = 0;
	}
	ded = this->oob_err/this->n_vals;
	amt -= ded;
	
	while(amt > 0){
		int i=0, idx;
		chunk_t * best = NULL;
		for(curr = this->chunks; curr != NULL; curr = curr->next){
			//either we haven't found anything, or we found the lowest probability
			//output or the highest probability error range.
			if(best == NULL || curr->p_rej == 0){
				best = curr;
				idx = i;
			}
			else if(curr->p_rej == 0){
				if(best->n_err <= curr->n_err && 
					curr->n_err - curr->n_out <= best->n_err - best->n_out){
					best = curr;
					idx = i;
				}
				else if(best->n_err ==0 && curr->n_err == 0 &&
					best->n_out > curr->n_out)
				{
					best = curr;
					idx = i;
				}
			}
			i++;
		}
		if(best != NULL){
			ded = (best->n_err+best->n_out)/this->n_vals;
			if(amt >= ded){
				amt -= ded;
				best->p_rej = 1.0;
			}
			else{
				float prob = (amt)/ded;
				amt -= ded*prob;
				best->p_rej = prob;
			}
			
		}
	}
}


