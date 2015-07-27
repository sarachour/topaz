#ifndef TASK_H
#define TASK_H

#include "util.h"
#include "logger.h"

class Topaz;



typedef enum TASK_ARG_TYPE_T{
	TASK_ARG_ARRAY=0,
	TASK_ARG_SCALAR=1,
	TASK_ARG_CONST_ARRAY=2,
	TASK_ARG_CONST_SCALAR=3
} task_arg_type_enum;


class Task {
  private:
	typedef struct PARCEL_T {
		int id;
		int rank;
		int inst;
		char * data;
		char has_failed;
		char is_refresh;
	} parcel_t;
	parcel_t parcel;
	int size;
	int header_size;
	char * __offset;
	char * __checkpoint;
	int __size;
  public:
	Task();
	~Task();
	
	void update(int id, int rank, int inst, int size, bool is_refresh, bool is_fail);
	void update(int id, int rank, int inst, int size);
	bool isRefresh();
	bool hasFailed();
	void setFailed();
	void setRefresh();
	Task * clone();
	int getId();
	int getRank();
	int getInstId();
	int getEffectiveSize();
	int getMaxSize();
	char * getData();
	void startPack();
	void startUnpack();
	void checkpoint();
	void rollback();
	//single
	void pack(double val);
	void pack(int val);
	void pack(char val);
	void pack(float val);
	//array 
	void pack(const int * val, int n);
	void pack(const float * val, int n);
	void pack(const double * val, int n);
	void pack(const char * val, int n);
	
	//unpack scalars
	void unpack(int * val);
	void unpack(float * val);
	void unpack(double * val);
	void unpack(char * val);
	//unpack arrats
	void unpack(int ** val);
	void unpack(float ** val);
	void unpack(double ** val);
	void unpack(char ** val);
	
};

class TaskArgSpec {
	private:
		type_id type;
		task_arg_type_enum ta_type; 
		bool is_const;
		int nelems;
	public: 
		static TaskArgSpec create(bool is_const, task_arg_type_enum ta_type, type_id type, int nelems);
		TaskArgSpec();
		void setInfo(bool is_const, task_arg_type_enum ta_type, type_id type, int nelems);
		bool isConst();
		type_id getType();
		task_arg_type_enum getTaskArgType();
		int getNumberElements();
		int getSize();
		
};
class TaskSpec {
    private:
	   int nInputs;
	   int nOutputs;
	   int nTrans;
	   int iid;
	   int __ibuf_size;
	   int __obuf_size;
	   fxnptr function;
	   lrnptr transform; //output transformation
	   TaskArgSpec * inputs;
	   TaskArgSpec * outputs;
	public:
		TaskSpec();
		void setFunction(fxnptr fxn, lrnptr lrn, int nin, int nout, int ntrans);
		void addInput(int idx, TaskArgSpec in);
		void addOutput(int idx, TaskArgSpec out);
		int getIID(){
			return this->iid;
		}
		void nextIID(){
			this->iid++;
		}
		void abs(Task * inputs, Task * outputs); //invoke this on not-learning
		//determine if you want to learn or not.
		bool test(Task * inputs, Task * outputs); //invoke this on not-learning
		//train output.
		bool train(Task * inputs, Task * outputs, Task * key);
		int getNumAbstractedOutputs(){
			return this->nTrans;
		}
		int getNumOutputs(){
			return this->nOutputs;
		}
		int getNumInputs(){
			return this->nInputs;
		}
		TaskArgSpec getInput(int idx){
			return this->inputs[idx];
		}
		TaskArgSpec getOutput(int idx){
			return this->outputs[idx];
		}
		int getInputBufferSize();
		int getOutputBufferSize();
		void execute(Task * inputs, Task * outputs);
	
};

typedef int TASK_HANDLE;

class TaskSpecs {
    private:
	   TaskSpec * specs;
	   int nspecs;
	   int __ibuf_size;
	   int __obuf_size;
	public:
		TaskSpecs(int MAX_SIZE);
		~TaskSpecs();
		TASK_HANDLE add(int idx, TaskSpec task);
		void execute(TASK_HANDLE task, Task * inputs, Task * outputs);
		TaskSpec get(int idx){
			return this->specs[idx];
		}
		void nextIID(int idx){
			this->specs[idx].nextIID();
		}
		int getInputBufferSize();
		int getOutputBufferSize();
};
#endif

