#include "detector.h"
#include "util.h"
#include "stdio.h"
#include "float.h"


MeanDetectorData::MeanDetectorData(){
	this->mean = 0;
	this->sqsum = 0;
	this->nstd = 2; //FIXME: change back to 2.
	this->code = BAN_ALL;
	this->npts = 0;
	
}
MeanDetectorData::~MeanDetectorData(){
	
}
void MeanDetectorData::accept(detector_code_t code){
	this->code |= code;
}
double MeanDetectorData::accepts(detector_code_t code){
	return ((this->code & code) == code) ? 1.0 : 0.0;
}
double MeanDetectorData::accepts(double v){
	double accepted;
	/*
	printf("?? [%f] n:%d mean: %f, stddev: %f, nan-ok?: %f, inf-ok?: %f, neg-inf-ok?:%f", 
			v, 
			this->npts, 
			this->mean, 
			this->stddev, 
			this->accepts(NAN_OK), 
			this->accepts(INF_OK), 
			this->accepts(NEG_INF_OK));
	*/
	if(isNaN(v)){
		accepted= this->accepts(NAN_OK);
	}
	else if(isInf(v)){
		accepted= this->accepts(INF_OK);
	}
	else if(isNegInf(v)){
		accepted= this->accepts(NEG_INF_OK);
	}
	else {
		double nstds = fabs(v - this->mean)/this->stddev;
		if(ceil(nstds) <= this->nstd || v == this->mean )
			accepted = 1.0;
		else
			accepted = 0;
	}
	//printf(" -> %f\n", accepted);
	return accepted;
}
void MeanDetectorData::accept(double v, double v2){
	if(isNaN(v)){
		this->accept(NAN_OK);
	}
	else if(isInf(v)){
		this->accept(INF_OK);
	}
	else if(isNegInf(v)){
		this->accept(NEG_INF_OK);
	}
	else{
		if(this->npts == 0){
			this->mean = v;
			this->sqsum = v*v;
			this->code = BAN_ALL;
			this->npts++;
		}
		else{
			double n = this->npts;
			//fixed so its numerically stable
			this->mean = this->mean*(n/(n+1.0))+v/(n+1.0);
			this->sqsum += v*v;
			this->stddev = sqrt((n+1)*this->sqsum - (n+1)*(n+1)*this->mean*this->mean)/(n+1);
			this->npts ++;
			
			
		}
	}
	/*
	printf("[%f] n:%d mean: %f, stddev: %f, nan-ok?: %f, inf-ok?: %f, neg-inf-ok?:%f\n", 
			v, 
			this->npts, 
			this->mean, 
			this->stddev, 
			this->accepts(NAN_OK), 
			this->accepts(INF_OK), 
			this->accepts(NEG_INF_OK));
	
	*/
}

