#include "machine.h"
#include "mpi.h"
#include "stdio.h"
#include "topaz.h"

MachineNetwork::MachineNetwork(int size){
	this->machines = new Machine[size];
	this->size = size;
	for(int i=0; i < size; i++){
		this->machines[i].setId(i);
	}
}
void MachineNetwork::setMain(int id){
	this->main = &this->machines[id];
}
void MachineNetwork::setCurrent(int id){
	this->current = &this->machines[id];
}
bool MachineNetwork::isMain(){
	return (this->current == this->main);
}
Machine MachineNetwork::getMain(){
	return (*this->main);
}
Machine MachineNetwork::getCurrent(){
	return (*this->current);
}
void MachineNetwork::sendTo(int rank, Task * t){
	#ifdef DEBUG
	printf("[%d->%d] sending: %d\n", this->current->getId(), rank, t->getEffectiveSize());
	#endif
	this->machines[rank].sendTo(t);
}
void MachineNetwork::receiveFrom(int rank, Task * t){
	#ifdef DEBUG
	printf("[%d->%d] recieving: %d\n", rank, this->current->getId(), t->getMaxSize());
	#endif
	this->machines[rank].recieveFrom(t);
}
void MachineNetwork::receiveFrom(Task * t){
	MPI_Status status;
	#ifdef DEBUG
	printf("[?->%d] recieving: %d\n", this->current->getId(), t->getMaxSize());
	#endif
	Topaz::topaz->getTimers()->stop(TOPAZ_TIMER);
	Topaz::topaz->getTimers()->start(COMM_TIMER);
	Topaz::topaz->getTimers()->start(COMM_DATA_TIMER);
	
	MPI_Recv(t->getData(), t->getMaxSize(), MPI_CHAR,
		MPI_ANY_SOURCE, TASK_TAG, MPI_COMM_WORLD, &status);
		
	Topaz::topaz->getTimers()->stop(COMM_DATA_TIMER);
	Topaz::topaz->getTimers()->stop(COMM_TIMER);
	Topaz::topaz->getTimers()->start(TOPAZ_TIMER);
}
void MachineNetwork::sendTo(Task * t){
	#ifdef DEBUG
	printf("[%d->?] recieving: %d\n", this->current->getId(), t->getMaxSize());
	#endif
	Topaz::topaz->getTimers()->stop(TOPAZ_TIMER);
	Topaz::topaz->getTimers()->start(COMM_TIMER);
	Topaz::topaz->getTimers()->start(COMM_DATA_TIMER);
	
	for(int i=0; i < this->size; i++){
		if(i != this->current->getId()){
			MPI_Send(t->getData(), t->getEffectiveSize(), MPI_CHAR,
				this->machines[i].getId(), TASK_TAG, MPI_COMM_WORLD);
		}
	}
	
	Topaz::topaz->getTimers()->stop(COMM_DATA_TIMER);
	Topaz::topaz->getTimers()->stop(COMM_TIMER);
	Topaz::topaz->getTimers()->start(TOPAZ_TIMER);
}
