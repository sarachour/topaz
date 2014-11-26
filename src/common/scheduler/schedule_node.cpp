
#include "machine.h"
#include "task.h"
#include "scheduler.h"
#include "topaz.h"

ScheduleNode::ScheduleNode(){
	this->base_cost = 0;
	this->metrics = new TaskMetric[MAX_TASKS];
}
void ScheduleNode::setBaselineCost(float cost){
	this->base_cost = cost;
	
}
void ScheduleNode::addTask(TASK_HANDLE t){
	
}

float ScheduleNode::getCost(Task * t){
	int id = t->getId();
	float adjcost = this->base_cost*this->metrics[id].getAverageRuntime();
	return adjcost;
}
