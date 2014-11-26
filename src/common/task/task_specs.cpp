#include "task.h"


TaskSpecs::TaskSpecs(int maxTasks){
	this->specs = new TaskSpec[maxTasks];
	this->nspecs = 0;
	this->__ibuf_size = 0;
	this->__obuf_size = 0;
}
TaskSpecs::~TaskSpecs(){
	delete this->specs;
}
TASK_HANDLE TaskSpecs::add(int id,  TaskSpec task){
	this->nspecs++;
	this->specs[id] = task;
	int ibufsiz = task.getInputBufferSize();
	int obufsiz = task.getOutputBufferSize();
	if(ibufsiz > this->__ibuf_size){
		this->__ibuf_size = ibufsiz;
	}
	if(obufsiz > this->__obuf_size){
		this->__obuf_size = obufsiz;	
	}
	this->nspecs++;
	return id;
}
int TaskSpecs::getInputBufferSize(){
	return this->__ibuf_size;
}
int TaskSpecs::getOutputBufferSize(){
	return this->__obuf_size;
}
void TaskSpecs::execute(TASK_HANDLE task, Task * inputs, Task * outputs){
	this->specs[task].execute(inputs, outputs);
}
