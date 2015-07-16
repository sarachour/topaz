#include "topaz.h"
#include "task.h"
#include "scheduler.h"
#include "abs_detectors.h"
#include "logger.h"

#include "pin_util.h"
#include "string.h"
#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include <stdarg.h>
#include <fstream>

//#define DISCARD

pin_task_info_t Topaz::PIN_INFO;
Topaz * Topaz::topaz = NULL;
bool __diff_outputs(Topaz * q, Task * t1, Task * t2, TaskSpecs * tspec);
void __LDET_record_task_outputs(Topaz * q, Task * t, TaskSpecs * tspec);
using namespace std;

Topaz::Topaz(int argc, char ** argv){
	int rank;
	int size;
	
	//basic intialization
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // rank is place index
	MPI_Comm_size(MPI_COMM_WORLD, &size); // size is total number of places.
	
	
	this->config.DETECTOR_ENABLED = false;
	this->config.LOG_DETECTORS_ENABLED = false;
	this->config.SCHEDULE_ENABLED = false;
	this->config.GODMODE_ENABLED = false;
	this->config.DISCARD_TASK = false;
	this->config.TIMERS_ENABLED = false;
	this->config.DETECTOR_TYPE = DETECTOR_TYPE_SOLID;
	this->config.DETECTOR_NBLOCKS = 5;
	this->config.DETECTOR_TARGET = 0.01;
	
	for(int i=0; i < argc; i++){
		if(strcmp(argv[i], "--sched")==0 || strcmp(argv[i], "-s") ==0)
			this->config.SCHEDULE_ENABLED=true;
		if(strcmp(argv[i], "--detect")==0 || strcmp(argv[i], "-d") ==0 ){
			this->config.DETECTOR_ENABLED=true;
			if(i < argc-1){
				i++; 
				if(strcmp(argv[i], "x") == 0) 
					this->config.DETECTOR_TYPE = DETECTOR_TYPE_SOLID;
				else if(strcmp(argv[i], "s") == 0)
					this->config.DETECTOR_TYPE = DETECTOR_TYPE_SCAR;
				else if(strcmp(argv[i], "c") == 0){
					this->config.DETECTOR_TYPE = DETECTOR_TYPE_AUTOSCAR;
				}
				else if(strcmp(argv[i], "d") == 0){
					this->config.DETECTOR_TYPE = DETECTOR_TYPE_DIST;
				}
				else
					i--;
			}
			
		}
		if(strcmp(argv[i], "--detect-target") ==0 || strcmp(argv[i], "-dt") == 0){
			if(i < argc-1){
				i++; 
				this->config.DETECTOR_TARGET = atof(argv[i]);
				i--;
			}
		}
		if(strcmp(argv[i], "--detect-blocks") ==0 || strcmp(argv[i], "-db") == 0){
			if(i < argc-1){
				i++; 
				this->config.DETECTOR_NBLOCKS = atoi(argv[i]);
				i--;
			}
		}
		if(strcmp(argv[i], "--log-detector") ==0 || strcmp(argv[i], "-l") == 0)
			this->config.LOG_DETECTORS_ENABLED=true;
		if(strcmp(argv[i], "--godmode") ==0 || strcmp(argv[i], "-g") == 0)
			this->config.GODMODE_ENABLED=true;
		if(strcmp(argv[i], "--discard") ==0 || strcmp(argv[i], "-d") == 0)
			this->config.DISCARD_TASK=true;
		if(strcmp(argv[i], "--time") ==0 || strcmp(argv[i], "-t") == 0)
			this->config.TIMERS_ENABLED=true;
	}
	if(this->config.TIMERS_ENABLED){
		char name[128];
		sprintf(name, "timer.%d.out", rank);
		this->timer = new RealTimerInfo(name);
		//sprintf(name, "comm.%d.out", rank);
		this->comm = new DummyCommunicationInfo();
	}
	else {
		this->timer = new DummyTimerInfo();
		this->comm = new DummyCommunicationInfo();
	}
	this->timer->stop_active();
	this->timer->start(TOPAZ_TIMER);
	
	if(this->config.DETECTOR_ENABLED){
		this->detector = new AbsDetectorManager(MAX_TASKS);
		if(this->config.LOG_DETECTORS_ENABLED){
			this->logdetector = new RealDetectorLogInfo("ldet.out", 160);
		}
		else{
			this->logdetector = new DummyDetectorLogInfo();
		}
	}
	else{
		this->logdetector = new DummyDetectorLogInfo();
	}
	char name[128];
	sprintf(name, "task.%d.out", rank);
	if(this->config.GODMODE_ENABLED){
		this->log = new RealLogInfo(name);
	}
	else
		this->log = new DummyLogInfo(name);
	this->machines = new MachineNetwork(size);
	this->machines->setMain(0);
	this->machines->setCurrent(rank);
	this->tasks = new TaskSpecs(MAX_TASKS);
	
	//only main node has scheduler
	if(this->isMain()){
		this->scheduler = new Scheduler(this->machines);
	}
	this->input_task = new Task();
	this->output_task = new Task();
	this->isLocalExecute = false;
	
	this->timer->stop(TOPAZ_TIMER);
	this->timer->start_active();
	
}

Topaz::~Topaz(){
	delete this->tasks;
	delete this->machines;
	delete this->scheduler;
	delete this->input_task;
	delete this->output_task;
	delete this->timer;
	delete this->comm;
	if(this->config.GODMODE_ENABLED){
		delete this->log;
	}
	if(this->config.DETECTOR_ENABLED){
		delete this->detector;
	}
	if(this->config.LOG_DETECTORS_ENABLED){
		delete this->logdetector;
	}
}

LogInfo * Topaz::getLog(){
	return this->log;
}
bool Topaz::isMain(){
	return (this->machines->isMain());
}

TASK_HANDLE Topaz::add(int id, fxnptr comp, lrnptr lrn, int nin, int nout, int ntrans,...){
	Topaz::topaz->getTimers()->stop_active();
	this->timer->start(TOPAZ_TIMER);
	va_list arguments;
	va_start(arguments, ntrans);
	int narrs_in=0;
	int narrs_out=0;
	int nout_els=0;
	TaskSpec tspec;
	tspec.setFunction(comp, lrn, nin, nout, ntrans);
	for(int i=0; i < nin; i++){
		bool is_const = false;
		task_arg_type_enum ta_type = (task_arg_type_enum) va_arg(arguments, int);
		type_id type = (type_id) va_arg(arguments, int);
		if(ta_type == TASK_ARG_CONST_ARRAY){
			ta_type = TASK_ARG_ARRAY;
			is_const = true;
		}
		if(ta_type == TASK_ARG_CONST_SCALAR){
			ta_type = TASK_ARG_SCALAR;
			is_const = true;
		}
		int siz = (int) va_arg(arguments, int);
		tspec.addInput(i, TaskArgSpec::create(is_const, ta_type, type, siz));
		if(ta_type == TASK_ARG_ARRAY) narrs_in++;
	}
	for(int i=0; i < nout; i++){
		task_arg_type_enum ta_type = (task_arg_type_enum) va_arg(arguments, int);
		type_id type = (type_id) va_arg(arguments, int);
		if(ta_type == TASK_ARG_CONST_ARRAY){
			ta_type = TASK_ARG_ARRAY;
		}
		if(ta_type == TASK_ARG_CONST_SCALAR){
			ta_type = TASK_ARG_SCALAR;
		}
		int siz = (type_id) va_arg(arguments, int);
		tspec.addOutput(i, TaskArgSpec::create(false, ta_type, type, siz));
		if(ta_type == TASK_ARG_ARRAY) narrs_out++;
		nout_els += siz;
		
	}
	
	if(this->config.DETECTOR_ENABLED){
		this->detector->add(id,ntrans); //add an outlier detector
		this->detector->setTarget(id, Topaz::topaz->config.DETECTOR_TARGET);
	}
	va_end(arguments);
	
	TASK_HANDLE handle = this->tasks->add(id,tspec);
	//EMUL : the sizeof(char) for the metadata
	int inbufsize = this->tasks->getInputBufferSize()+narrs_in*sizeof(int); 
	int outbufsize = this->tasks->getOutputBufferSize()+narrs_out*sizeof(int);
	this->input_task->update(0, 0, 0, inbufsize);
	this->output_task->update(0, 0, 0, outbufsize);
	this->timer->stop(TOPAZ_TIMER);
	Topaz::topaz->getTimers()->start_active();
	return handle;
}
//FIXME: No detector code
TASK_HANDLE Topaz::add(int id, TaskSpec tspec){
	Topaz::topaz->getTimers()->stop_active();
	this->timer->start(TOPAZ_TIMER);
	TASK_HANDLE handle = this->tasks->add(id,tspec);
	this->input_task->update(0, 0, 0, this->tasks->getInputBufferSize());
	this->output_task->update(0, 0, 0, this->tasks->getOutputBufferSize());

	if(this->isMain()){
		//add task node to schedule
		this->scheduler->addTask(handle);
	}
	this->timer->stop(TOPAZ_TIMER);
	Topaz::topaz->getTimers()->start_active();
	return handle;
	
}
void Topaz::finalize(){
	//dump timer info
	Topaz::topaz->getTimers()->off();
	if(this->config.TIMERS_ENABLED){
		this->timer->print();
	}
	
	if(this->isMain()){
		
		if(this->config.DETECTOR_ENABLED){
			this->detector->print();
			if(this->config.LOG_DETECTORS_ENABLED && this->isMain()){
				this->logdetector->print();
			}
		}
	
		if(this->config.GODMODE_ENABLED){
			printf("++ Writing Statistics to Log\n");
			this->log->print();
		}
		
		this->input_task->update(-1, -1, TRIGGER_SHUTDOWN, 0);
		this->input_task->startPack();
		this->machines->sendTo(this->input_task);
		
	}
	else{
		MPI_Finalize();
		printf("++ Worker...Finalizing\n");
		
	}
	if(!this->isMain()){
		printf("++ Worker...Exiting\n");
		exit(0);
	}
	else printf("++ Main...Continuing\n");
	
}

bool Topaz::check(TASK_HANDLE tsk, int rank, ...){
	Topaz::topaz->getTimers()->stop_active();
	TaskSpec sp = this->tasks->get(tsk);
	if(this->output_task->hasFailed()) return true;
	int nabs = sp.getNumAbstractedOutputs(); //abstracted
	va_list arguments;
	
	va_start(arguments, rank);
	//read each abstracted output;
	for(int i=0; i < nabs; i++){
		double val = (double) va_arg(arguments, double);
		this->detector->set(tsk,i, val); //set the argument.
	}
	va_end(arguments);
	//checker.
	Topaz::topaz->getTimers()->start(TOPAZ_TIMER_DET);
	bool res= this->detector->run(tsk); //run train, test, etc
	Topaz::topaz->getTimers()->stop(TOPAZ_TIMER_DET);
	Topaz::topaz->getTimers()->start_active();
	return res;
}
void Topaz::packAllTaskData(bool doit){
	Topaz::topaz->config.PACK_FULL_TASK = doit;
}
void Topaz::start(TASK_HANDLE tid){
	this->tasks->nextIID(tid);
	Topaz::topaz->packAllTaskData(true);
}

bool Topaz::isPackAll(){
	return Topaz::topaz->config.PACK_FULL_TASK;
}

void Topaz::execute(){
	Topaz::topaz->getTimers()->stop_active();
	Topaz::topaz->getTimers()->start(TOPAZ_TIMER); // start whole computation timer
	TASK_HANDLE id;
	this->input_task->startUnpack();
	id = this->input_task->getId();
	//locally execute.
	this->isLocalExecute = true;
	
	if(id >= 0){
		pin_start_task(id); // special task styff
		this->tasks->execute(id, this->input_task, this->output_task);
		pin_stop_task(id, &Topaz::PIN_INFO);
	}
	else{ //special communication message
		error_code_t code = (error_code_t) this->input_task->getRank();
		switch(code){
			//shutdown request
			case TRIGGER_SHUTDOWN:
				printf("[WORKER] Triggering Finalize...\n");
				this->finalize();
				printf("[WORKER] Shutting down...\n");
				break;
		}
	}
	
	Topaz::topaz->getTimers()->stop(TOPAZ_TIMER); // start whole computation timer
	Topaz::topaz->getTimers()->start_active();
}

bool Topaz::send(){
	Topaz::topaz->getTimers()->stop_active();
	
	//set metadata for this communication
	
	if(this->isMain()){
		Task * task = this->input_task;
		this->comm->set_taskset(task->getId(), task->getRank());
		
		int mach = this->scheduler->schedule(task);
		//update emulator with last recieved message
		if(mach == this->machines->getMain().getId()){
			this->execute();
			this->isLocalExecute = true;
		}
		else{
			this->packAllTaskData(false);
			//printf("s: main send to\n");
			this->machines->sendTo(mach, task);
			this->isLocalExecute = false;
		}
	}
	else{
		//printf("s: worker send\n");
		Task * task = this->output_task;
		this->comm->set_taskset(task->getId(), task->getRank());
		//printf("s: worker sending..\n");
		this->machines->sendTo(this->machines->getMain().getId(), task);
		//printf("s: worker sent..\n");
	}
	Topaz::topaz->getTimers()->start_active();
	return true;
}

Task * spare = NULL; 

void Topaz::reexecute_failed(int id, TaskSpec * ts){
	Task * incorr = this->output_task;
	if(spare == NULL) spare = incorr->clone(); //backup task
	//swap task payloads
	this->output_task = spare; 
	spare = incorr;
	this->tasks->execute(id, this->input_task, this->output_task);
}
void Topaz::reexecute(int id, TaskSpec * ts){
	Task * incorr = this->output_task;
	if(spare == NULL) spare = incorr->clone(); //backup task
	//swap task payloads
	this->output_task = spare; 
	spare = incorr;
	this->tasks->execute(id, this->input_task, this->output_task);
	ts->train(this->input_task, incorr, this->output_task);
	//input task, output task
	if(this->config.LOG_DETECTORS_ENABLED) 
		// accepted, input, bad-output, key-output
		ts->log(false,this->input_task, incorr, this->output_task);
}
// by assumption, the detector is enabled.
void Topaz::reexecute_log(int id, TaskSpec * ts){
	Task * incorr = this->output_task;
	if(spare == NULL) spare = incorr->clone(); //backup task
	this->output_task = spare;
	spare = incorr;
	this->tasks->execute(id, this->input_task, this->output_task);
	//change back to incorrect task
	this->output_task = incorr;
	ts->log(true,this->input_task, incorr, this->output_task);
}
bool Topaz::receive(){
	Topaz::topaz->getTimers()->stop_active();
	this->timer->start(TOPAZ_TIMER);
	int id = this->input_task->getId();
	TaskSpec ts = this->tasks->get(id);
	bool status = true;
	
	if(this->isMain()){		
		//printf("r: main waiting for recieve..\n");
		Task * task = this->output_task;
		this->machines->receiveFrom(task);
		Topaz::topaz->getTimers()->dump();
		//printf("r: main recieved..\n");
		//if(this->config.GODMODE_ENABLED) this->log->aug_clear();
		
		this->output_task->startUnpack();
		
		
		if(this->output_task->hasFailed()){
			this->reexecute_failed(id, &ts);
			this->packAllTaskData(true);
			this->timer->stop(TOPAZ_TIMER);
			Topaz::topaz->getTimers()->start_active();
			return true;
		}
		
		//printf("r: main detect\n");
		if(this->config.DETECTOR_ENABLED){
			//handle the outliers
			// outlier | is true error | inject
			status = ts.test(this->input_task, this->output_task);
			//outlier detector detects outlier, and we're not discarding.
			if(status == false && !this->config.DISCARD_TASK){
				//printf("r: main reexecuting\n");
				this->reexecute(id, &ts);
				//printf("r: main reexecuted\n");
				this->packAllTaskData(true);
				status = true;
			}
			//if we're logging, reexecute anyway.
			else if(this->config.LOG_DETECTORS_ENABLED){
				this->reexecute_log(id,&ts);
			}
						
		}
		
		//printf("r: main detected\n");
	}
	else{
		//printf("r: worker receiving..\n");
		Task * task = this->input_task;
		this->machines->receiveFrom(this->machines->getMain().getId(), task);
		Topaz::topaz->getTimers()->dump();
		//printf("r: worker recieved.\n");
	}
	
	this->timer->stop(TOPAZ_TIMER);
	Topaz::topaz->getTimers()->start_active();
	return status;
}

/*
 * unpack argument, find confidence value. if confidence value is less than 
 * 0.5, re-execute computation
 */
