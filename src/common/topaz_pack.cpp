#include "topaz.h"
#include "task.h"
#include "scheduler.h"
#include "logger.h"

#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include <stdarg.h>

void __pack_inputs(Topaz * that, TASK_HANDLE taskid, int rank, va_list args);
void __pack_outputs(Topaz * that, TASK_HANDLE taskid, int rank, va_list args);
void __unpack_inputs(Topaz * that, TASK_HANDLE* TASKID, int * RANK, va_list args);
void __unpack_outputs(Topaz * that, TASK_HANDLE* TASKID, int * RANK, va_list args);


void Topaz::pack_inputs(TASK_HANDLE taskid, int rank, ...){
	Topaz::topaz->getTimers()->stop_active(3);
	
	va_list arguments;
	va_start(arguments, rank);
	__pack_inputs(this, taskid, rank, arguments);
	va_end(arguments);
	
	Topaz::topaz->getTimers()->start_active(3);
}


void Topaz::pack_outputs(TASK_HANDLE taskid, int rank, ...){
	Topaz::topaz->getTimers()->stop_active(3);
	//printf("pack inputs\n");
	
	va_list arguments;
	va_start(arguments, rank);
	__pack_outputs(this, taskid, rank, arguments);
	va_end(arguments);
	
	Topaz::topaz->getTimers()->start_active(3);
}


void Topaz::unpack_inputs(TASK_HANDLE taskid, int * rank, ...){
	Topaz::topaz->getTimers()->stop_active(3);
	//printf("unpack inputs\n");
	int __dummy;
	va_list arguments;
	va_start(arguments, rank);
	__unpack_inputs(this, &__dummy, rank, arguments);
	va_end(arguments);
	
	Topaz::topaz->getTimers()->start_active(3);
}
void Topaz::unpack_outputs(TASK_HANDLE taskid, int * rank, ...){
	int __dummy;
	Topaz::topaz->getTimers()->stop_active(3);
	
	va_list arguments;
	va_start(arguments, rank);
	__unpack_outputs(this, &__dummy, rank, arguments);
	va_end(arguments);
	
	Topaz::topaz->getTimers()->start_active(3);
}
void Topaz::unpack_inputs(Task * t, TASK_HANDLE taskid, int * rank, ...){
	int __dummy;
	Topaz::topaz->getTimers()->stop_active(3);
	
	Task * back = this->input_task;
	this->input_task = t;
	va_list arguments;
	va_start(arguments, rank);
	__unpack_inputs(this, &__dummy, rank, arguments);
	this->input_task = back;
	va_end(arguments);
	
	Topaz::topaz->getTimers()->start_active(3);
}
void Topaz::unpack_outputs(Task *t, TASK_HANDLE taskid, int * rank, ...){
	int __dummy;
	Topaz::topaz->getTimers()->stop_active(3);
	
	Task * back = this->output_task;
	this->output_task = t;
	va_list arguments;
	va_start(arguments, rank);
	__unpack_outputs(this, &__dummy, rank, arguments);
	this->output_task = back;
	va_end(arguments);
	
	Topaz::topaz->getTimers()->start_active(3);
}



bool Topaz::receive(TASK_HANDLE TASKID, int * RANK, ...){
	bool ok = this->receive();
	int __dummy;
	Topaz::topaz->getTimers()->stop_active(3);
	
	va_list arguments;
	va_start(arguments, RANK);
	
	if(ok){
		if(this->isMain()){
			__unpack_outputs(this, &__dummy, RANK, arguments);
		}
		else{
			__unpack_inputs(this, &__dummy, RANK, arguments);
		}
	}
	va_end(arguments);
	Topaz::topaz->getTimers()->start_active(3);
	return ok;
}

bool Topaz::send(TASK_HANDLE TASKID, int RANK, ...){
	Topaz::topaz->getTimers()->stop_active(3);
	va_list arguments;
	va_start(arguments, RANK);
	
	if(this->isMain()){
		__pack_inputs(this, TASKID, RANK, arguments);
	}
	else{
		__pack_outputs(this, TASKID, RANK, arguments);
	}
	
	va_end(arguments);
	Topaz::topaz->getTimers()->start_active(3);
	return this->send();
}
/*
void __PIN_LOAD_INFO(char * mask){}

//EMUL
inline void __pack_error_info(Topaz * q, Task * t){
	char error_mask=0;
	__PIN_LOAD_INFO(&error_mask);
	t->pack(error_mask);
	

}
*/
inline void __unpack_entry(Task * t, va_list arguments, TaskArgSpec targ){
	switch(targ.getType()){
			case FLOAT: 
				if(targ.getTaskArgType() == TASK_ARG_SCALAR)
					t->unpack((float*) va_arg(arguments, double*)); 
				else 
					t->unpack((float**) va_arg(arguments, double**));
				break; 
			case DOUBLE: 
				if(targ.getTaskArgType() == TASK_ARG_SCALAR)
					t->unpack((double*) va_arg(arguments, double*)); 
				else 
					t->unpack((double**) va_arg(arguments, double**));
				break; 
			case INT: 
				if(targ.getTaskArgType() == TASK_ARG_SCALAR)
					t->unpack((int*) va_arg(arguments, int*)); 
				else 
					t->unpack((int**) va_arg(arguments, int**));
				break; 
			case CHAR: 
				if(targ.getTaskArgType() == TASK_ARG_SCALAR)
					t->unpack((char*) va_arg(arguments, int*)); 
				else 
					t->unpack((char**) va_arg(arguments, int**));
				break; 
			default:
				break;
		}
}
inline void __pack_entry(Task * t, va_list arguments, TaskArgSpec targ){
	switch(targ.getType()){
		case FLOAT: 
			if(targ.getTaskArgType() == TASK_ARG_SCALAR) 
				t->pack((float) va_arg(arguments, double)); 
			else 
				t->pack((float*) va_arg(arguments, double*), targ.getNumberElements());
			break; 
		case DOUBLE: 
			if(targ.getTaskArgType() == TASK_ARG_SCALAR)
				t->pack((double) va_arg(arguments, double)); 
			else 
				t->pack((double*) va_arg(arguments, double*), targ.getNumberElements());
			break; 
		case INT: 
			if(targ.getTaskArgType() == TASK_ARG_SCALAR)
				t->pack((int) va_arg(arguments, int)); 
			else 
				t->pack((int*) va_arg(arguments, int*), targ.getNumberElements());
			break; 
		case CHAR: 
			if(targ.getTaskArgType() == TASK_ARG_SCALAR)
				t->pack((char) va_arg(arguments, int)); 
			else 
				t->pack((char*) va_arg(arguments, int*), targ.getNumberElements());
			break; 
		default:
			break;
	}
}

inline void __pack_inputs(Topaz * that, TASK_HANDLE taskid, int rank, va_list arguments){
	TaskSpec tspec = that->tasks->get(taskid);
	Topaz::topaz->getTimers()->start(TOPAZ_TIMER_SERIALIZE);
	int n = tspec.getNumInputs();
	Task * t = that->input_task;
	//send inputs
	t->update(taskid, tspec.getIID(), rank, 0,false,Topaz::topaz->isPackAll());
	t->startPack();
	
	for(int i=0; i < n; i++){
		TaskArgSpec targ = tspec.getInput(i);
		if(targ.isConst()) continue;
		__pack_entry(t, arguments, targ);
	}
	if(Topaz::topaz->isPackAll()){
		for(int i=0; i < n; i++){
			TaskArgSpec targ = tspec.getInput(i);
			if(!targ.isConst()) continue;
			__pack_entry(t, arguments, targ);
		}
	}
	Topaz::topaz->getTimers()->stop(TOPAZ_TIMER_SERIALIZE);
	
}
/*
 * 
 */


inline void __pack_outputs(Topaz * that, TASK_HANDLE taskid, int rank, va_list arguments){
	Topaz::topaz->getTimers()->start(TOPAZ_TIMER_SERIALIZE);
	TaskSpec tspec = that->tasks->get(taskid);
	int n = tspec.getNumOutputs();
	Task * t = that->output_task;
	//send inputs
	
	//FIX ME
	t->update(taskid, -1, rank, 0);
	t->startPack();
	
	
	
	for(int i=0; i < n; i++){
		TaskArgSpec targ = tspec.getOutput(i);
		__pack_entry(t, arguments, targ);
	}
	Topaz::topaz->getTimers()->stop(TOPAZ_TIMER_SERIALIZE);
}


inline void __unpack_inputs(Topaz * that, TASK_HANDLE* TASKID, int * RANK, va_list arguments){
	Topaz::topaz->getTimers()->start(TOPAZ_TIMER_SERIALIZE);
	Task * t = that->input_task;
	t->startUnpack();
	*TASKID = t->getId();
	*RANK= t->getRank();
	TaskSpec tspec = that->tasks->get(*TASKID);
	int n = tspec.getNumInputs();
	
	//if we're not on the main machine and this is a refresh packet, refresh the dram.
	/*
	if(t->isRefresh() && !that->isMain()){
		pin_refresh_dram();
	}
	*/
	for(int i=0; i < n; i++){
		TaskArgSpec targ = tspec.getInput(i);
		if(targ.isConst()) continue;
		__unpack_entry(t, arguments,targ);
	}
	for(int i=0; i < n; i++){
		TaskArgSpec targ = tspec.getInput(i);
		if(!targ.isConst()) continue;
		__unpack_entry(t, arguments,targ);
	}
	Topaz::topaz->getTimers()->stop(TOPAZ_TIMER_SERIALIZE);
}


inline void __unpack_outputs(Topaz * that, TASK_HANDLE* TASKID, int * RANK, va_list arguments){
	Topaz::topaz->getTimers()->start(TOPAZ_TIMER_SERIALIZE);
	Task * t = that->output_task;
	t->startUnpack();
	*TASKID = t->getId();
	*RANK= t->getRank();
	
	TaskSpec tspec = that->tasks->get(*TASKID);
	int n = tspec.getNumOutputs();
	
	
	
	for(int i=0; i < n; i++){
		TaskArgSpec targ = tspec.getOutput(i);
		__unpack_entry(t, arguments, targ);
	}
	Topaz::topaz->getTimers()->stop(TOPAZ_TIMER_SERIALIZE);
}
