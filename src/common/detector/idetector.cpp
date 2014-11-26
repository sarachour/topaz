#include "detector.h"
#include "detectors.h"
#include "util.h"
#include "stdio.h"
#include "topaz.h"

IDetectorData::IDetectorData(int n){
	this->init(n);
}
IDetectorData::IDetectorData(){
	this->comp_targets = NULL;
}
IDetectorData::~IDetectorData(){
	if(this->comp_targets != NULL) delete this->comp_targets;
}
void IDetectorData::print(int i){
	printf("target: %f\n", this->comp_targets[i]); 
}
void IDetectorData::init(int n){
	this->n = n;
	this->comp_targets = new double[n];
}
void IDetectorData::set_target(double target){
	this->target = target;
}
void IDetectorData::update(int idx, double reexec){
	
} // get a reexecution rate
void IDetectorData::resolve(){
	
}
double IDetectorData::get_target(int idx){
		return this->comp_targets[idx];
}  // get the current reexecution rate for an element.

		
IDetector::IDetector(int siz, int nelems){
	this->init(siz, nelems);
}
 // ouypuyd
IDetector::IDetector(){
	this->detectors = NULL;
	this->idetector = NULL;
	this->mappings = NULL;
}

IDetector::~IDetector(){
	if(this->detectors != NULL) delete this->detectors;
	if(this->idetector != NULL) delete this->idetector;
	if(this->mappings != NULL){
		for(int i=0; i < this->n; i++){
			if(this->mappings[i] != NULL){
				delete this->mappings[i];
			}
		}
		delete this->mappings;
	}
}

void IDetector::init(int nouts, int nelems){
	this->n = nouts;
	this->detectors = new Detector[nouts];
	if(Topaz::topaz->config.CROSSDETECTOR_ENABLED){
		this->idetector = new ControlIDetectorData(nelems);
	}
	else{
		this->idetector = new UniformIDetectorData(nelems);
	}
	this->mappings = new int*[nouts];
	for(int i=0; i < nouts; i++){
		this->mappings[i] = NULL;
	}
}

void IDetector::print(){
	printf("#inter-element detector\n");
	for(int i=0; i < this->n; i++){
		int siz = this->size(i);
		for(int j=0; j < siz; j++){
			printf("## output %d, index %d\n", i ,j);
			this->idetector->print(this->get_id(i,j));
		}
	}
	printf("#intra-element detectors\n\n");
	for(int i=0; i < this->n; i++){
		this->detectors[i].print();
	}
}

int IDetector::get_id(int outid, int siz){
	return this->mappings[outid][siz+1];
}
void IDetector::set_target(double targ){
	printf("set target to: %f\n", targ);
	this->idetector->set_target(targ); 
	
	//initially set
	for(int i=0; i < this->n; i++){
		int n = this->size(i);
		for(int j=0; j < n; j++){
			int id = this->get_id(i, j);
			double targ_e = this->idetector->get_target(id);
			printf("set component target to %f\n", targ_e);
			this->detectors[i].set_target(j, targ_e);
		}
	}
}
 //set the target for the task.
void IDetector::add(int outid, int siz){
	this->detectors[outid].init(siz);
	this->mappings[outid] = new int[siz+1];
	this->mappings[outid][0] = siz;
	int q = 0;
	for(int i=0; i < this->n; i++){
		if(this->mappings[i] != NULL){
			int n = this->mappings[i][0];
			for(int j=1; j <= n; j++){
				this->mappings[i][j] = q;
				q++;
			}
			
		}
	}
}
 // set the ith input to size siz
int IDetector::size(int outid){
	return this->mappings[outid][0];

}

double IDetector::test(int outid, int idx, double val){
	int id = this->get_id(outid, idx);
	double test_res = this->detectors[outid].test(idx, val);
	this->idetector->update(id, test_res);
	return test_res;
}
 //test value, update reexecution rate, update target.
void IDetector::train(int outid, int idx, double val, double oval){
	if(this->idetector->train(this->get_id(outid,idx)))
		this->detectors[outid].train(idx, val, oval);
}
 //train data
void IDetector::complete_test(){
	//resolve all joint stuff
	this->idetector->resolve(); //updates all re-execution rate
	//update reexecution rates
	for(int i=0; i < this->n; i++){
		int n = this->size(i);
		for(int j=0; j < n; j++){
			int id = this->get_id(i, j);
			this->detectors[i].set_target(j, this->idetector->get_target(id));
		}
	}
}
 //move to next timestep, update correlation, change reexecution rates accordinly.
void IDetector::complete_train(){

}

