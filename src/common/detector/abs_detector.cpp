#include "abs_detectors.h"
#include "util.h"
#include "stdio.h"
#include "topaz.h"

int AbsDetector::MODE = ABS_DETECTOR_TEST;
bool AbsDetector::compare(){
	//printf("cmp: ");
	for(int i=0; i < this->n; i++){
		if(fabs(this->data_key[i] - this->data[i]) > this->thresh){
			return false;
		}
	}
	//printf("\n");
	return true;
}
void AbsDetector::setMode(int m){
	AbsDetector::MODE = m;
}
int AbsDetector::getMode(){
	return AbsDetector::MODE;
}

AbsDetectorManager::AbsDetectorManager(){
	this->n = -1; this->detectors = NULL;
}
AbsDetectorManager::~AbsDetectorManager(){
	for(int i=0; i < this->n; i++){
		if(this->detectors[i] != NULL)
			this->detectors[i]->clean();
			delete this->detectors[i];
	}
	delete this->detectors;
}
void AbsDetectorManager::print(){
	for(int i=0; i < this->n; i++){
		printf("DETECTOR %d\n",i);
		if(this->detectors[i] != NULL) 
			this->detectors[i]->print();
	}
}
void AbsDetectorManager::init(int siz){
	this->n = siz; 
	this->detectors = new AbsDetector*[siz];
	for(int i=0; i < siz; i++){
		this->detectors[i] = NULL;
	}
}
void AbsDetectorManager::add(int taskid, int nouts){
	switch(Topaz::topaz->config.DETECTOR_TYPE){
		case DETECTOR_TYPE_SOLID:
			this->detectors[taskid] = new AbsSolidRegionDetector(nouts);
			break;
		case DETECTOR_TYPE_SCAR:
			this->detectors[taskid] = new AbsScarRegionDetector(nouts);
			break;
		case DETECTOR_TYPE_AUTOSCAR:
			this->detectors[taskid] = new AbsAutoScarRegionDetector(nouts);
			break;
		default:
			this->detectors[taskid] = new AbsSolidRegionDetector(nouts);
			break;
	}
}
