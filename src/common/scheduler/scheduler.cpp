
#include "machine.h"
#include "task.h"
#include "scheduler.h"

Scheduler::Scheduler(MachineNetwork * network){
	this->size = network->getSize();
	this->nodes = new ScheduleNode[this->size];
	int main_id = network->getMain().getId();
	this->nodes[main_id].setBaselineCost(1.0);
	
}
//dummy scheduler
int Scheduler::schedule(Task * t){
	float cost = 1.0;
	float mach = -1;
	//find the cheapest machine
	for(int i=0; i < this->size; i++){
		float mcost = this->nodes[i].getCost(t);
		if(mcost <= cost || mach == -1){
			cost = mcost;
			mach = i;
		}	
	}
	return mach;
}
void Scheduler::addTask(TASK_HANDLE t){
	for(int i=0; i < this->size; i++){
		this->nodes[i].addTask(t);
	}
}
