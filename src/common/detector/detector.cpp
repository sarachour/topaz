#include "detector.h"
#include "util.h"
#include "stdio.h"
#include "topaz.h"
#include "stdlib.h"

int DetectorData::ID = 0;

DetectorData::DetectorData(){
	this->id = DetectorData::ID;
	DetectorData::ID++;
}
DetectorData::~DetectorData(){
	
}

void DetectorData::print(){
	
}
void DetectorData::set_target(float val){
	
}

void DetectorData::accept(double d, double d0){
	
}
double DetectorData::accepts(double d){
	return 1.0;
}

Detector::Detector(int siz){
	 this->init(siz);
}

Detector::Detector(){
	this->n = 0;
	this->data = NULL;
	
}

Detector::~Detector(){
	if(this->data != NULL) delete this->data;
}

void Detector::print(){
	for(int i=0; i < this->n; i++){
		printf("idx[%d]\n", i);
		this->data[i]->print();
	}
}
void Detector::set_target(int idx, float val){
	this->data[idx]->set_target(val);
}

void Detector::init(int siz){
	this->n = siz;
	this->data = (DetectorData**) malloc(sizeof(DetectorData *)*siz);
	if(Topaz::topaz->config.DETECTOR_TYPE == DETECTOR_MEAN){
		for(int i=0; i < this->n; i++){
			this->data[i] = new MeanDetectorData();
		}
	}
	else if(Topaz::topaz->config.DETECTOR_TYPE == DETECTOR_CONTROL){
		for(int i=0; i < this->n; i++){
			this->data[i] = new ControlDetectorData();
		}
	}
	else{
		printf("UNKNOWN TYPE: DID NOT UNDERSTAND TYPE OF DETECTOR\n");
	}
}


void Detector::train(int idx, double val, double val2){
	this->data[idx]->accept(val, val2);
}


//confidence it belongs is returned.
double Detector::test(int idx, double val){
	return this->data[idx]->accepts(val);
}
