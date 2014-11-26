#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"
class MachineNetwork;
//this may be moved into detector.
class TaskMetric {
	public:
		float getAverageRuntime();
};
/*
 * Contains scheduling information for every task. Only allocated on main node.
 */
class ScheduleNode {
	private:
		float base_cost;
		TaskMetric * metrics;
	public:
		ScheduleNode();
		void setBaselineCost(float cost);
		void addTask(TASK_HANDLE task);
		float getCost(Task * t);
};

class Scheduler {
  private:
	ScheduleNode * nodes;
	int size;
  public:
	Scheduler(MachineNetwork * network);
	int schedule(Task * t);
	void addTask(TASK_HANDLE t);
};

#endif

