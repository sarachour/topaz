#include "abs_detectors.h"
#include "stdio.h"
#include "logger.h"


#define STAT_WINDOW 1000
float STAT_NEW_WEIGHT = (1.0/STAT_WINDOW);
float STAT_OLD_WEIGHT = (1.0-1.0/STAT_WINDOW);

RegionStats::RegionStats(){
	this->p_train_err = this->p_test_err = 0;
	this->p_train_corr = this->p_test_corr = 0;
	this->p_train_total = this->p_test_total = 0;
	this->p_train_n = this->p_test_n = 0;
}
void RegionStats::merge(RegionStats * o){
	this->p_test_err = (this->p_test_err + o->p_test_err);
	this->p_test_corr = (this->p_test_corr + o->p_test_corr);
	this->p_test_n += o->p_test_n;
	this->p_test_total = (this->p_test_total + o->p_test_total);
	
	this->p_train_err = (this->p_train_err + o->p_train_err);
	this->p_train_corr = (this->p_train_corr + o->p_train_corr);
	this->p_train_n += o->p_train_n;
	this->p_train_total = (this->p_train_total + o->p_train_total);
}
void RegionStats::update_accept_rate(bool isset, bool is_acc){
	float STAT_NEW_WEIGHT = (1.0/STAT_WINDOW);
	float STAT_OLD_WEIGHT = (1.0-1.0/STAT_WINDOW);
	
	if(!isset){
		this->p_test_total *= STAT_OLD_WEIGHT;
		return;
	}
	this->p_test_n += 1.0;
	this->p_test_total += STAT_NEW_WEIGHT;
	if(is_acc) this->p_test_corr += 1.0;
	else this->p_test_err += 1.0;
}
void RegionStats::update_accuracy_rate(bool isset, bool is_corr){
	if(!isset){
		this->p_train_total *= STAT_OLD_WEIGHT;
		return;
	}
	this->p_train_n += 1.0;
	this->p_train_total += STAT_NEW_WEIGHT;
	//update with most recent behavior.
	if(is_corr){
		this->p_train_corr += 1.0;
	}
	else{
		this->p_train_err += 1.0;
	}
}
void RegionStats::print(){
	printf("train | n: %f  p(total): %f   p(err): %f   p(out): %f\n", 
			this->p_train_n, this->p_train_total,
			this->p_train_err/this->p_train_n, this->p_train_corr/this->p_train_n);
		printf("test | n: %f  p(total): %f   p(err): %f   p(out): %f\n", 
			this->p_test_n, this->p_test_total,
			this->p_test_err/this->p_test_n, this->p_test_corr/this->p_test_n);
}
int RegionStats::log(DetectorLogInfo * l, int r, int i){
	char name[255];
	float prob, tr_prob, te_prob;
	tr_prob = this->p_train_corr/this->p_train_n; 
	te_prob = this->p_test_corr/this->p_test_n;
	prob = tr_prob*te_prob;
	sprintf(name, "%d.prob", r); 
	l->set(i,name,prob); i++;
	return i;
}



GlobalStats::GlobalStats(){
	this->n_rej = 0;
	this->n_acc = 0;
	this->n_true = 0;
	this->n_false = 0;
	this->n_total_test = 0;
	this->n_total_train = 0;
}

void GlobalStats::update_accuracy_rate(bool is_corr){
	this->n_acc*=STAT_OLD_WEIGHT;
	this->n_rej*=STAT_OLD_WEIGHT;
	this->n_true*=STAT_OLD_WEIGHT;
	this->n_false*=STAT_OLD_WEIGHT;
	this->n_total_train*=STAT_OLD_WEIGHT;
	/*
	if(isacc) 
		this->n_acc+=STAT_NEW_WEIGHT;
	else 
		this->n_rej+=STAT_NEW_WEIGHT;
	*/
	
	if(!is_corr) 
		this->n_true+=STAT_NEW_WEIGHT;
	else 
		this->n_false+=STAT_NEW_WEIGHT;
	this->n_total_train+=STAT_NEW_WEIGHT;
	
}

float GlobalStats::get_reexec(){
	return n_rej/(n_total_test);

}
float GlobalStats::get_frac_rej_tp(){
	return n_true/(n_total_train);
}
void GlobalStats::update_accept_rate(bool isacc){
	this->n_acc*=STAT_OLD_WEIGHT;
	this->n_rej*=STAT_OLD_WEIGHT;
	this->n_total_test*=STAT_OLD_WEIGHT;
	if(isacc) 
		this->n_acc+=STAT_NEW_WEIGHT;
	else 
		this->n_rej+=STAT_NEW_WEIGHT;
	this->n_total_test+=STAT_NEW_WEIGHT;
}
void GlobalStats::print(){
	printf("accept:%f reject%f total:%f\n", this->n_acc/this->n_total_test, this->n_rej/this->n_total_test, this->n_total_test);
	printf("true-rej:%f false-rej:%f total-rej:%f\n", this->n_true/this->n_total_train, this->n_false/this->n_total_train, this->n_total_train);
}
int GlobalStats::log(DetectorLogInfo * l, int i){
	l->set(i,"pct_rej", this->n_acc/this->n_total_test); i++;
	l->set(i,"pct_acc",this->n_rej/this->n_total_test); i++;
	l->set(i,"pct_true_rej",this->n_true/this->n_total_train); i++;
	l->set(i,"pct_false_rej",this->n_false/this->n_total_train); i++;
	return i;
}
