#include "task.h"
#include "topaz.h"
#include "abs_detectors.h"

TaskSpec::TaskSpec(){
	this->__ibuf_size =0;
	this->__obuf_size =0;
	this->nInputs = 0;
	this->nOutputs = 0;
	this->iid = 0;
	this->function =NULL;
	this->inputs = NULL;
	this->outputs = NULL;
}
void TaskSpec::setFunction(fxnptr fxn, lrnptr lrn, int nin, int nout, int ntrans){
	this->function = fxn;
	if(this->inputs != NULL) delete this->inputs;
	if(this->outputs != NULL) delete this->outputs;
	this->nInputs = nin;
	this->nOutputs = nout;
	this->inputs = new TaskArgSpec[nin];
	this->outputs = new TaskArgSpec[nout];
	this->transform = lrn;
	this->nTrans = ntrans;
}

void TaskSpec::addInput(int idx, TaskArgSpec in){
	this->__ibuf_size -= this->inputs[idx].getSize();
	this->inputs[idx] = in;
	this->__ibuf_size += this->inputs[idx].getSize();
}
void TaskSpec::addOutput(int idx, TaskArgSpec out){
	this->__obuf_size -= this->outputs[idx].getSize();
	this->outputs[idx] = out;
	this->__obuf_size += this->outputs[idx].getSize();
}
int TaskSpec::getInputBufferSize(){
	return this->__ibuf_size;
}
int TaskSpec::getOutputBufferSize(){
	return this->__obuf_size;
}
bool TaskSpec::test(Task * inputs, Task * outputs){
	if(this->transform != NULL){
		//Topaz::topaz->getTimers()->start(TASK_TIMER);
		AbsDetector::setMode(ABS_DETECTOR_TEST);
		Topaz::topaz->getTimers()->stop_active(5);
		Topaz::topaz->getTimers()->start(TOPAZ_TIMER_ABS); 
		bool res= this->transform(inputs, outputs);
		Topaz::topaz->getTimers()->stop(TOPAZ_TIMER_ABS);
		Topaz::topaz->getTimers()->start_active(5);
		return res;
		//Topaz::topaz->getTimers()->stop(TASK_TIMER);
	}
	return true;
}
bool TaskSpec::train(Task * inputs, Task * outputs, Task * key){
	bool res;
	if(this->transform != NULL){
		//set detector info to key
		Topaz::topaz->getTimers()->stop_active(5);
		Topaz::topaz->getTimers()->start(TOPAZ_TIMER_ABS); 
		AbsDetector::setMode(ABS_DETECTOR_KEY);
		this->transform(inputs, key); //record key.
		//set detector info to train
		AbsDetector::setMode(ABS_DETECTOR_TRAIN);
		res = this->transform(inputs, outputs); //record outputs.
		AbsDetector::setMode(ABS_DETECTOR_TEST);
		Topaz::topaz->getTimers()->stop(TOPAZ_TIMER_ABS);
		Topaz::topaz->getTimers()->start_active(5);
	}
	return res;
}
void TaskSpec::log(bool is_reexec, Task * inputs, Task * outputs, Task * key){
	if(this->transform != NULL){
		//set detector info to key
		Topaz::topaz->getTimers()->stop_active(5);
		Topaz::topaz->getTimers()->start(TOPAZ_TIMER_ABS); 
		AbsDetector::setMode(ABS_DETECTOR_KEY);
		this->transform(inputs, key); //record key.
		//set detector info to train
		inputs->startUnpack();
		DetectorLogInfo::setTaskId(inputs->getId());
		DetectorLogInfo::setRank(inputs->getRank());
		DetectorLogInfo::setAccepted(is_reexec);
		DetectorLogInfo::setIID(0);
		
		AbsDetector::setMode(ABS_DETECTOR_LOG);
		this->transform(inputs, outputs); //record outputs.
		AbsDetector::setMode(ABS_DETECTOR_TEST);
		Topaz::topaz->getTimers()->stop(TOPAZ_TIMER_ABS);
		Topaz::topaz->getTimers()->start_active(5);
	}
}
void TaskSpec::execute(Task * inputs, Task * outputs){
	if(this->function != NULL){
		Topaz::topaz->getTimers()->stop_active(4);
		Topaz::topaz->getTimers()->start(TASK_TIMER);
		this->function(inputs, outputs);
		Topaz::topaz->getTimers()->stop(TASK_TIMER);
		Topaz::topaz->getTimers()->start_active(4);
	}
}

