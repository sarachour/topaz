#include "detector.h"
#include "util.h"
#include "stdio.h"
#include "topaz.h"
#include "limits.h"
#include "stdlib.h"
// TODO: only update re-executed on train (not in test)
// TODO: bound min, max by data min max. [DONE]
// TODO: detectormanager percolates re-execution rate from detectors downward [DONE]


//double K_I = 0.033;
double K_I = 0.033;
double K_D = 0.33;
double K_E = 0.33;
double DECAY = 0.99;
//double ERR_DECAY = 0.9999999;
double ERR_DECAY = 1.0;
double THRESHHOLD = 1e-20;

double random_float(){
	return ((float) rand())/((float) RAND_MAX);
}
/*
double K_I = 0.00033;
double K_D = 0.0033;
double K_E = 0.00033;
*/

ControlDetectorData::ControlDetectorData(){
	this->left = 0;
	this->right = 0;
	this->mean = 0;
	this->n = 10;
	
	this->rx_targ = 0;
	
	this->lt_freq = 0.5;
	this->lt_obs = this->rx_targ*this->lt_freq;
	this->lt_e = 0; //left error
	this->lt_de = 0; //derivative of error
	this->lt_ie = 0; //integral of error
	this->lt_n = 1;
	
	this->rt_freq = 0.5;
	this->rt_obs = this->rx_targ*this->rt_freq;
	this->rt_e = 0; //left error
	this->rt_de = 0; //derivative of error
	this->rt_ie = 0; //integral of error
	this->rt_n = 1;
	
	this->seed = new MeanDetectorData();
	
}
ControlDetectorData::~ControlDetectorData(){
	delete this->seed;
}
void ControlDetectorData::set_target(float f){
	this->rx_targ = f;
}

void ControlDetectorData::accept(double d, double dold){
	//update data bounds
	
	if(this->seed->getN() > 0){
		this->dat_lbound = this->dat_lbound < d ? this->dat_lbound : d; //lbound
		this->dat_rbound = this->dat_rbound > d ? this->dat_rbound : d; //rbound
	}
	else{
		this->dat_lbound = d;
		this->dat_rbound = d;
	}
	
	
	//update mean, stddev ect
	if(this->seed->getN() < this->n){
		this->seed->accept(d, dold);
		this->right = this->left = this->seed->getStdDev();
		this->mean = this->seed->getMean();
		
	}
	else{
		//update mean on accept
		this->seed->accept(d,dold);
		double omean = this->mean;
		double nmean = this->seed->getMean();
		double diff = nmean - omean;
		
		this->mean = nmean;
		//shift left, right
		this->left += diff;
		this->right -= diff;
		
		//no error
		//printf("%f %f\n", d, dold);
		//update left, right on re-execution
		if(this->reexec ){
			
			if(d <= this->mean)
				update_left(1.0);
			if(d >= this->mean)
				update_right(1.0);
			//this->print(1.0, d);
		}
		
		
	}
	
	this->record(d, DLOG_CAT_TRAIN);
	/*
	else{
		if(this->mean - this->left > d) this->left = this->mean - d;
		if(this->mean + this->right < d) this->right = d - this->mean;
	}
	*/
	
}
//FIXME: Must leave index 0 open for err-value
void ControlDetectorData::record(double d, int cat){
	DetectorLogInfo * l = Topaz::topaz->getDLog();
	int i=1;
	/*
	l->set_category(cat);
	l->set_value(d);
	*/
	//TODO: FIX IMPLEMENTATION
	l->set(i,"targ", this->rx_targ); i++;
	l->set(i,"mean",this->mean); i++;
	
	l->set(i,"left", this->left); i++;
	l->set(i,"left-freq", this->lt_freq); i++;
	l->set(i,"left-rexec", this->lt_obs); i++;
	l->set(i,"left-err", this->lt_e); i++;
	/*
	l->set(i,"lt-err-dt", this->lt_de); i++;
	l->set(i,"lt-err-int", this->lt_ie); i++;
	l->set(i,"lt-err-n", this->lt_n); i++;
	*/
	
	l->set(i,"right", this->right); i++;
	l->set(i,"right-freq", this->rt_freq); i++;
	l->set(i,"rt-rexec", this->rt_obs);  i++;
	l->set(i,"left-err", this->rt_e); i++;
	/*
	l->set(i,"rt-err-dt", this->rt_de); i++;
	l->set(i,"rt-err-int", this->rt_ie);  i++;
	l->set(i,"rt-err-n", this->rt_n); i++;
	*/

	
	
}
void ControlDetectorData::print(){
	/*
	printf("v[%.2f]-> [%.2f, %.2f]  \
			LT=[frac=%.4f obs=%.2f e=%.2f i=%.2f d=%.2f] \
			RT=[frac=%.4f obs=%.2f, e=%.2f, i=%.2f, d=%.2f] %d:%f\n", 
			d, (this->mean- this->left), (this->mean + this->right), 
			this->lt_freq, this->lt_obs, this->lt_e, this->lt_ie, this->lt_de,
			this->rt_freq, this->rt_obs, this->rt_e, this->rt_ie, this->rt_de,
			this->id,  (this->lt_freq*this->lt_obs + this->rt_freq*this->rt_obs));
	*/
	//id left right re-exec
	//return;
	printf("%d\tbnd=[%f\t%f]\tlt=[%f\t%f]\trt=[%f\t%f]\ter=[%f\t%f]\n", this->id, 
		(this->mean- this->left), 
		(this->mean + this->right), 
		(this->lt_freq),
		(this->lt_obs),
		(this->rt_freq),
		(this->rt_obs),
		(this->lt_obs*this->lt_freq + this->rt_obs*this->rt_freq),
		this->rx_targ);
}
#define FACTOR 1.0
int ControlDetectorData::prob_accept(double v){
	const int MAX_STDDEVS = 10;
	double rval = random_float();
	double obs_prob = this->lt_obs*this->lt_freq + this->rt_obs*this->rt_freq;
	double des_prob = this->rx_targ;
	double reexec_err = obs_prob - des_prob;
	
	//if probabilistic drop is disabled, return 0
	if(!Topaz::topaz->config.PROB_DROP_ENABLED) return 0.0;
	//only drop if we're over
	if(reexec_err <= 0){
		return 0.0;
	}
	
	double std_devs;
	if(v < this->mean){
		std_devs = (this->mean - v)/(this->left);
	}
	else {
		std_devs = (v-this->mean)/(this->right);
	}
	//printf("%f %f\n", std_devs, reexec_err);
	
	//if this data point is super outliery, don't probabilistically drop
	if(std_devs > MAX_STDDEVS) return 0.0;
	
	double threshhold = 1.0/std_devs;
	//threshhold *= reexec_err/this->rx_targ;
	if(rval < threshhold){
		return 1.0;
	}
	else return 0.0;
}

void ControlDetectorData::update_left(double reexec){
	//reject left
	int n = this->lt_n;
	//update n
	this->lt_n = (n+1);
	//update frequencies
	this->lt_freq = ((double) this->lt_n)/(this->lt_n + this->rt_n);
	this->rt_freq = 1.0 - this->lt_freq;
	//update observed
	this->lt_obs = (n*this->lt_obs*ERR_DECAY+reexec)/(n+1); //observed rate
	
	//normal energy
	double e = (this->lt_obs-this->rx_targ);
	this->lt_ie = this->lt_ie*DECAY +  e; //integral of errors
	this->lt_de = (e-this->lt_e);
	this->lt_e = e;
	this->left += K_E*this->lt_e + K_I*this->lt_ie + K_D*this->lt_de;
	this->left = this->left < 0 ? 0 : this->left;
	
	//bounding
	if(!Topaz::topaz->config.DISCARD_TASK){
		double diff = (this->mean - this->dat_lbound);
		if(this->left > diff*FACTOR && diff >= 0)
			this->left = diff*FACTOR;
	}
	
	
}
void ControlDetectorData::update_right(double reexec){
	int n = this->rt_n;
	this->rt_n = (n+1);
	this->lt_freq = ((double) this->lt_n)/(this->lt_n + this->rt_n);
	this->rt_freq = 1.0 - this->lt_freq;
	this->rt_obs = (n*this->rt_obs*ERR_DECAY+reexec)/(n+1); //observed rate

	double e = (this->rt_obs - this->rx_targ);
	this->rt_ie = this->rt_ie*DECAY +  e; 
	this->rt_de = (e-this->rt_e);
	this->rt_e = e;
	
	this->right += K_E*this->rt_e + K_I*this->rt_ie + K_D*this->rt_de;
	this->right = this->right < 0 ? 0 : this->right;
	
	if(!Topaz::topaz->config.DISCARD_TASK){
		double diff = (this->dat_rbound - this->mean);
		if(this->right > diff*FACTOR && diff >= 0)
			this->right = diff*FACTOR;
	}
	
}

double ControlDetectorData::accepts(double d){
	double status;
	if(this->seed->getN() < this->n){
		status = 0;
		this->reexec = true;
		if(Topaz::topaz->config.DISCARD_TASK){
			this->seed->accept(d,d);
		}
	}
	else if(isNaN(d) || isInf(d) || isNegInf(d)){
		status = 0;
		this->update_left(1.0);
		this->update_right(1.0);	
		this->reexec = true;
	}
	else {
		double rbound = this->mean + this->right;
		double lbound = this->mean - this->left;
		
		
		if(d <= this->mean && d >= lbound){
			this->update_left(0.0);
			status = 1.0;
			this->reexec = false;
			//update mean
			this->seed->accept(d,d);
			this->mean = this->seed->getMean();
		}
		if(d >= this->mean && d <= rbound){
			this->update_right(0.0);
			status = 1.0;
			this->reexec = false;
			//update mean
			this->seed->accept(d,d);
			this->mean = this->seed->getMean();
		}
		//if we reject
		if(d < lbound || d > rbound){
			if(this->prob_accept(d) == 1.0){
				status = 1.0;
				this->reexec = false;
			}
			else {
				status = 0.0;
				this->reexec = true;
			}
			
			if(Topaz::topaz->config.DISCARD_TASK && status == 0){
				if(d < lbound) this->update_left(1.0);
				if(d > rbound) this->update_right(1.0);	
				this->record(d, DLOG_CAT_TRAIN);
				return status;
				
			}
		}
	}
	//if(status == 1.0) this->print(0.0,d);
	this->record(d, this->reexec ? DLOG_CAT_TEST_BAD : DLOG_CAT_TEST_OK);
	
	//special random ignore
	/*
	 * If we're really far from our goal
	 * 
	 */
	//if we're about to accept
	
	return status;
}
