#include "machine.h"
#include "task.h"
#include "mpi.h"
#include "stdio.h"
#include "topaz.h"

Machine::Machine(){
	this->id = -1;
}
void Machine::sendTo(Task * t){
	Topaz::topaz->getTimers()->stop(TOPAZ_TIMER);
	
	Topaz::topaz->getCLog()->send(t->getEffectiveSize());
	
	Topaz::topaz->getTimers()->start(COMM_TIMER);
	Topaz::topaz->getTimers()->start(COMM_DATA_TIMER);
	
	MPI_Send(t->getData(), t->getEffectiveSize() /*count*/, MPI_CHAR, 
			  this->id /*dest*/, TASK_TAG/*tag*/, MPI_COMM_WORLD);
	Topaz::topaz->getTimers()->stop(COMM_DATA_TIMER);
	Topaz::topaz->getTimers()->stop(COMM_TIMER);
	
	Topaz::topaz->getTimers()->start(TOPAZ_TIMER);
}
void Machine::recieveFrom(Task * t){
	MPI_Status status;
	Topaz::topaz->getTimers()->stop(TOPAZ_TIMER);
	//receive data
	Topaz::topaz->getCLog()->recv(t->getMaxSize());
	
	Topaz::topaz->getTimers()->start(COMM_TIMER);
	Topaz::topaz->getTimers()->start(COMM_DATA_TIMER);
	MPI_Recv(t->getData(), t->getMaxSize(), MPI_CHAR,
		this->id, TASK_TAG, MPI_COMM_WORLD, &status);
	Topaz::topaz->getTimers()->stop(COMM_DATA_TIMER);
	Topaz::topaz->getTimers()->stop(COMM_TIMER);
	
	
	Topaz::topaz->getTimers()->start(TOPAZ_TIMER);
}
