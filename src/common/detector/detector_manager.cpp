#include "detectors.h"
#include "util.h"
#include "topaz.h"
#include "stdio.h"

DetectorManager::DetectorManager(int siz){
	this->init(siz);
}
DetectorManager::DetectorManager(){
	this->classes = NULL;
	this->nclasses = 0;
}
void DetectorManager::print(){
	for(int i=0; i < this->nclasses; i++){
		printf("Taskset[%d]\n", i);
		this->classes[i].print();
		printf("\n");
	}
}
DetectorManager::~DetectorManager(){
	for(int i=0; i < this->nclasses; i++){
		printf("Taskset[%d]\n", i);
		this->classes[i].print();
		printf("\n");
	}
	
	delete this->classes;
}
void DetectorManager::init(int siz){
	this->classes = new IDetector[siz];
	this->nclasses = siz;
}
void DetectorManager::add(int tidx, int nouts, int tnelems){
	this->classes[tidx].init(nouts, tnelems);
}
void DetectorManager::add_output(int tidx, int oidx, int nelems){
	this->classes[tidx].add(oidx, nelems);
}

#define EXEC_TRAIN(taskid, outid, oval, nval) \
	int siz = this->classes[taskid].size(outid); \
	for(int i=0; i < siz; i++){ \
		this->classes[taskid].train(outid, i, (double) oval[i], (double) nval[i]); \
	} 

#define EXEC_TEST(taskid, outid, val) \
	int siz = this->classes[taskid].size(outid); \
	double sum = 0; \
	for(int i=0; i < siz; i++){ \
		sum += this->classes[taskid].test(outid, i, (double) val[i]); \
	} \
	return (sum/siz);
	
void DetectorManager::train(int taskid, int outid, double * oval, double * nval){
	EXEC_TRAIN(taskid, outid, oval, nval);
}

void DetectorManager::train(int taskid, int outid, float * oval, float * nval){
	EXEC_TRAIN(taskid, outid, oval, nval);
}

void DetectorManager::train(int taskid, int outid, int * oval, int * nval){
	EXEC_TRAIN(taskid, outid, oval, nval);
}

void DetectorManager::train(int taskid, int outid, char * oval, char * nval){
	EXEC_TRAIN(taskid, outid, oval, nval);
}

double DetectorManager::test(int taskid, int outid, double * val){
	EXEC_TEST(taskid, outid, val);
}

double DetectorManager::test(int taskid, int outid, int * val){
	EXEC_TEST(taskid, outid, val);
}

double DetectorManager::test(int taskid, int outid, float * val){
	EXEC_TEST(taskid, outid, val);
}

double DetectorManager::test(int taskid, int outid, char * val){
	EXEC_TEST(taskid, outid, val);
}


void DetectorManager::set_target(int i, float amt){
	this->classes[i].set_target(amt);
}

void DetectorManager::complete_test(int i){
	this->classes[i].complete_test();
}
void DetectorManager::complete_train(int i){
	this->classes[i].complete_train();
}


/*
Detectors* DetectorManager::get(int idx){
	if(idx >= this->nclasses){
		return NULL;
	}
	return &this->classes[idx];
}
*/
