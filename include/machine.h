#ifndef MACHINE_H
#define MACHINE_H

#include "task.h"

#define TASK_TAG 0

//#define DEBUG

class Machine {
	private:
		int id;
		int ntasks;
	public:
		Machine();
		void setId(int id){
			this->id = id;
		}
		void sendTo(Task * t);
		void recieveFrom(Task * t);
		int getId(){
			return this->id;
		}
};

class MachineNetwork {
  private:
	Machine * main;
	Machine * machines;
	Machine * current;
	int size;
  public:
	MachineNetwork(int size);
	bool isMain();
	void setMain(int id);
	void setCurrent(int id);
	int getSize(){
		return this->size;
	}
	Machine getMain();
	Machine getCurrent();
	void sendTo(int rank, Task * t);
	void receiveFrom(int rank, Task * t);
	void receiveFrom(Task * t);
	void sendTo(Task * t);
};

#endif



