#ifndef TOPAZ_H
#define TOPAZ_H

#include "task.h"
#include "machine.h"
#include "scheduler.h"
#include "logger.h"
#include "logger.h"
#include "pin_util.h"

typedef enum DETECTOR_TYPE_T{
	DETECTOR_MEAN=0,
	DETECTOR_CONTROL,
	DETECTOR_DISTRIBUTION
} detector_type_t;
typedef struct CONFIG_T_M{
	bool DETECTOR_ENABLED;
	bool LOG_DETECTORS_ENABLED;
	bool SCHEDULE_ENABLED;
	bool GODMODE_ENABLED;
	detector_type_t DETECTOR_TYPE;
	float DETECTOR_TARGET;
	bool DISCARD_TASK;
	bool TIMERS_ENABLED;
	bool PROB_DROP_ENABLED;
	bool CROSSDETECTOR_ENABLED;
	bool SCAR_DETECTOR_ENABLED;
	bool PACK_FULL_TASK;
} config_t;


class AbsDetectorManager;
#define MAX_TASKS 10

class Topaz {
  private:
	MachineNetwork * machines;
	Scheduler * scheduler;
	//DetectorManager * detector;
	AbsDetectorManager * detector;
	LogInfo * log;
	TimerInfo * timer;
	DetectorLogInfo * logdetector;
	typedef enum ERROR_CODES {
		TRIGGER_SHUTDOWN,
	} error_code_t;
	//EMUL
	bool isLocalExecute;
	void packAllTaskData(bool doit);
	void reexecute(int id, TaskSpec * ts);
	void reexecute_log(int id, TaskSpec * ts);
	void reexecute_failed(int id, TaskSpec * ts);
	static pin_task_info_t PIN_INFO;
  public:	
	config_t config;
	static Topaz * topaz;
	//public for implementation reasons. Don't touch outside of cpp file.
	Task * input_task;
	Task * output_task;
	TaskSpecs * tasks;
	Topaz(int argc, char ** argv);
	~Topaz();
	bool isPackAll();
	bool isMain();
	static pin_task_info_t getPinInfo(){return PIN_INFO;};
	static void setPinInfo(pin_task_info_t pt){PIN_INFO = pt;};
	int getMachineId(){return this->machines->getCurrent().getId();};
	//add new task
	TASK_HANDLE add(int id, TaskSpec tspec);
	TASK_HANDLE add(int id, fxnptr comp, lrnptr lrn, int nin, int nout, int nchk, ...);
	//send data
	void pack_inputs(TASK_HANDLE taskid, int rank, ...);
	void pack_outputs(TASK_HANDLE taskid, int rank, ...);
	void unpack_inputs(TASK_HANDLE TASKID, int * RANK, ...);
	void unpack_outputs(TASK_HANDLE TASKID, int * RANK, ...);
	void unpack_inputs(Task * t, TASK_HANDLE TASKID, int * RANK, ...);
	void unpack_outputs(Task * t, TASK_HANDLE TASKID, int * RANK, ...);
	bool check(TASK_HANDLE TASKID, int RANK, ...); //check info.
	void start(TASK_HANDLE taskid);
	
	LogInfo* getLog();
	DetectorLogInfo * getDLog(){
		return logdetector;
	}
	TimerInfo * getTimers(){
		return this->timer;
	}
	bool send(TASK_HANDLE taskid, int rank, ...);
	bool send();
	//recieve data 
	bool receive(TASK_HANDLE TASKID, int * RANK, ...);
	bool receive();
	
	void execute();
	void finalize();
	
	
};
int topaz_init(int argc, char ** argv);
int topaz_finish(int argc, char ** argv);
int topaz_main(int argc, char ** argv);
int topaz_worker(int argc, char ** argv);

#endif
