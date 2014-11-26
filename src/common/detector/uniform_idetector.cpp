#include "detectors.h"
#include "stdio.h"

#define HISTORY 0.00001

UniformIDetectorData::UniformIDetectorData(int n):
	IDetectorData(n){
	this->n = n;
	this->scratch = new int[n];
	this->rejr = 0;
}

UniformIDetectorData::~UniformIDetectorData(){
	delete this->scratch;
}

void UniformIDetectorData::print(int i){
	printf("target: %f\n", this->target); 
	printf("observed: %f\n", this->rejr);
}

void UniformIDetectorData::set_target(double t){
	printf("uniform target set\n");
	IDetectorData::set_target(t);
	for(int i=0; i < this->n; i++){
		this->comp_targets[i] = (this->target/this->n);
	}
}

void UniformIDetectorData::update(int idx, double reexec){
		if(reexec < 0.5)
			this->scratch[idx] = 1;
		else
			this->scratch[idx] = 0;
} 
void UniformIDetectorData::resolve(){
		this->rejr *= (1-HISTORY);
		for(int i=0; i < this->n; i++){
			if(this->scratch[i] == 1){
				this->rejr += HISTORY;
				return;
			}
		}
}
