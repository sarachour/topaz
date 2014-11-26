#include "dist_detector.h"
#include "math.h"
#include "stdlib.h"
#include "stdio.h"
#include "util.h"
#include "topaz.h"

#define HISTORY 0.00001

//FIXME: Must leave index 0 open for err-value
void DistributionDetectorData::record(double d, int cat){
	DetectorLogInfo * l = Topaz::topaz->getDLog();
	int i=1;
	/*
	l->set_category(cat);
	l->set_value(d);
	*/
	//TODO: FIX IMPLEMENTATION
	l->set(i,"targ", this->targ); i++;
	l->set(i,"obs", this->rejr); i++;
	l->set(i,"n-err", this->dist->getErrorCount()); i++;
	l->set(i,"n-out", this->dist->getOutputCount()); i++;
	l->set(i,"n-oob", this->dist->getOutOfBoundErrorCount()); i++;
	int n = this->dist->getNumberChunks();
	char name[10];
	for(int j=0; j < n; j++){
		chunk_t c = this->dist->get(j);
		sprintf(name, "%d-left", j);
		l->set(i, name, c.left); i++;
		sprintf(name, "%d-right", j);
		l->set(i, name, c.right); i++;
		sprintf(name, "%d-out", j);
		l->set(i, name, c.n_out); i++;
		sprintf(name, "%d-err", j);
		l->set(i, name, c.n_err); i++;
		sprintf(name, "%d-prej", j);
		l->set(i, name, c.p_rej); i++;
	}
	
}

DistributionDetectorData::DistributionDetectorData(){
	this->derr = 0;
	this->ierr = 0;
	this->err = 0;
	this->rejr = 0;
	this->targ = 0;
	this->dist = new Distribution();
}
void DistributionDetectorData::set_target(float f){
	this->targ = f;
	this->param = f;
	this->rejr = 0;
}
DistributionDetectorData::~DistributionDetectorData(){
	delete this->dist;
}
void DistributionDetectorData::accept(double d, double d0){
	float eps = 1e-10;
	if(fabs(d-d0) < eps){ //same value
		this->dist->output(d);
	}
	else{
		this->dist->output(d);
		this->dist->error(d0);
	}
	this->dist->allocate(this->param);
	this->record(d, DLOG_CAT_TRAIN);
}

double DistributionDetectorData::accepts(double d){
	
	
	
	double K_I = 0.33;
	double K_D = 0.33;
	double K_E = 0.33;

	float prob = this->dist->get_prob_reject(this->dist->find_chunk(d),d);
	float amt = ((float)rand())/((float)RAND_MAX);
	int ret = 0;
	this->rejr *= (1-HISTORY);
	if(isNaN(d) || isInf(d) || isNegInf(d)){
		this->rejr+= HISTORY; 
		ret = 0;
	}
	else if(amt <= prob){ // reject
		this->rejr+= HISTORY; 
		ret = 0; //set our confidence this is ok to 0
	}
	else{ //accept
		this->dist->output(d);
		ret = 1;//set our confidence this is ok to 100
	}
	/*
	//This is control system stuff. WE don't need it right now.
	float nerr = this->targ - this->rejr;
	this->derr = (nerr - this->err);
	this->err = nerr;
	this->ierr *= (1-HISTORY);
	this->ierr += this->err*HISTORY;
	this->param += K_E*this->err + K_D*this->derr + K_I*this->ierr;
	
	this->param = this->targ;
	this->dist->allocate(this->param);
	*/
	//this->record(d, ret == 0 ? DLOG_CAT_TEST_BAD : DLOG_CAT_TEST_OK);
	
	return ret;
}
void DistributionDetectorData::print(){
	printf("\nTarget rate: %f\n", this->targ);
	printf("Reexecution rate: %f\n", this->rejr);
	printf("Used rate: %f\n", this->param);
	this->dist->print();
}
