#ifndef EMUL_H
#define EMUL_H

#include <vector>
#include <iostream>
#include "pin_util.h"
//#define MAX_PROPS 16

#define DLOG_CAT_TRAIN 0 //use both values
#define DLOG_CAT_TEST_OK 1 //don't use both values
#define DLOG_CAT_TEST_BAD 2
#define DLOG_CAT_CORRECT 3
typedef struct TIMER_INFO_T {
	int idx; //identifier
	double inst;
} timer_info_t;


typedef struct IDENTIFIER_INFO {
	int place;
	int task_id;
	unsigned int inst_id;
	unsigned int task_rank;
} identifier_info;

typedef struct OUTLIER_DETECTOR_INFO{
	bool accept;
	bool error;
} detector_info;

typedef struct LOG_INFO_T {
	pin_task_info_t pin;
	detector_info det;
	identifier_info id;
} log_info_t;

struct LOG_NODE_T;
typedef struct LOG_NODE_T {
	log_info_t data;
	struct LOG_NODE_T * NEXT;
	struct LOG_NODE_T * PREV;
} log_node_t;

	
typedef struct LDETECTOR_PROP_T{
	const char name[128];
	double value;
} ldetector_prop_t;

typedef struct LDETECTOR_ID_T{
	int task_id;
	int task_rank;
	int task_inst;
	int out_id;
} ldetector_id_t;

typedef struct LDETECTOR_INFO_T{
	ldetector_id_t id; // id
	double value;
	double cvalue;
	bool is_accept;
	bool is_corr;
	double * props; //specific properties
	bool * prop_mask;
} ldetector_info_t;

struct DET_LOG_NODE_T;

typedef struct DET_LOG_NODE_T {
	ldetector_info_t data;
	struct DET_LOG_NODE_T * NEXT;
} ldetector_node_t;



class TimerInfo {
	public:
	virtual void start(int idx) = 0;
	virtual void stop(int idx) = 0;
	virtual void on() = 0;
	virtual void off() = 0;
	virtual void print() = 0;
	virtual void stop_active()=0;
	virtual void start_active()=0;
};



class RealTimerInfo : public TimerInfo{
	#define N_TIMERS 10
	#define N_HISTS 10
	struct TIMER_NODE_T;
	typedef struct TIMER_NODE_T {
		timer_info_t data;
		struct TIMER_NODE_T * NEXT;
	} timer_node_t;
	
	timer_node_t * timers; 
	timer_node_t* TIMER_LAST; 
	int TIMER_SIZE;
	bool active[N_TIMERS];
	int n_stops ;
	bool stopped[N_HISTS][N_TIMERS];
	FILE * file; // file to write to
	int ndumps;
	bool TIMER_OFF;
	
	
	void init_timers(); 
	void add_timer(timer_info_t t);
	timer_info_t * last_timer();
	void del_timers();
	void light_save(pin_timer_info_t * p);
	void light_load();
	public:
	RealTimerInfo(const char * filename);
	~RealTimerInfo();
	void off();
	void on();
	void stop_active();
	void start_active();
	void start(int idx);
	void stop(int idx);
	void print();
	//print log, timers
	
};

class DummyTimerInfo : public TimerInfo {
	void stop_active(){
		
	}
	void start_active(){
		
	}
	void start(int idx){
		
	}
	void stop(int idx){
		
	}
	void print(){
		
	}
	
	void off(){
		
	}
	void on(){
		
	}
	int getid(){
		return -1;
	}
};
class LogInfo {
	public:
	LogInfo(const char * path){}
	~LogInfo(){}
	virtual void start(int place, int task_id, int task_inst, int task_rank) = 0;
	virtual void stop() = 0; // updates emulator info
	
	virtual void aug_clear() = 0;
	virtual void aug_task_accept(bool accept) = 0;
	virtual void aug_task_error(bool has_error) = 0;
	//get info for packing data
	
	//doing pack, unpack
	virtual int get_pack_size() = 0;
	virtual void pack(int place, int task_id, int task_inst, int task_rank, char * t) = 0;
	virtual void unpack(int place, int task_id, int task_inst, int task_rank, char * t) = 0;
	//start,stop timers
	virtual void print() = 0;
};
class DummyLogInfo : public LogInfo {
	int task_id;
	public:
	DummyLogInfo(const char * path) : LogInfo(path){}
	~DummyLogInfo(){}
	void start(int place, int task_id, int task_inst, int task_rank);
	void stop();
	void aug_clear(){}
	void aug_task_accept(bool accept){}
	void aug_task_error(bool has_error){}
	int get_pack_size(){return 0;}
	void pack(int place, int task_id, int task_inst, int task_rank, char * t){}
	void unpack(int place, int task_id, int task_inst, int task_rank, char * t){}
	void print(){}
};

class RealLogInfo: public LogInfo {
	protected:
	//emulator info
	log_node_t * tasks; log_node_t * TASK_LAST; int TASK_SIZE;
	bool running;
	bool CACHE_ACC;
	bool CACHE_ERR;
	int ndumps;
	FILE * file;
	
	void init_logs();
	void add_log(log_info_t l);
	log_info_t * last_log();
	void del_logs();
	
	
	public:
	RealLogInfo(const char * path);
	~RealLogInfo();
	void start(int place, int task_id, int task_inst, int task_rank);
	void stop(); // updates emulator info
	
	void aug_clear();
	void aug_task_accept(bool accept);
	void aug_task_error(bool has_error);
	//get info for packing data
	
	//doing pack, unpack
	int get_pack_size();
	void pack(int place, int task_id, int task_inst, int task_rank, char * t);
	void unpack(int place, int task_id, int task_inst, int task_rank, char * t);
	//start,stop timers
	void print();
};

class DetectorLogInfo {
	protected:
		static ldetector_id_t CURR_ID;
		static bool IS_ACCEPTED;
	public:
		static void setAccepted(bool a){DetectorLogInfo::IS_ACCEPTED = a;}
		static void setTaskId(int a){DetectorLogInfo::CURR_ID.task_id = a;}
		static void setRank(int a){DetectorLogInfo::CURR_ID.task_rank = a;}
		static void setIID(int a){DetectorLogInfo::CURR_ID.task_inst = a;}
		
		static bool getAccepted(){return DetectorLogInfo::IS_ACCEPTED;}
		static int getTaskId(){return DetectorLogInfo::CURR_ID.task_id;}
		static int getRank(){return DetectorLogInfo::CURR_ID.task_rank;}
		static int getIID(){return DetectorLogInfo::CURR_ID.task_inst;}
		
		virtual void start() =0;
		virtual void start_entry(int task_id, int inst_id, int task_rank, int out_id, bool is_outlier, bool is_error, double val, double cval) = 0; //topaz
		virtual void end_entry() = 0;
		virtual void set(int idx,const char * prop, double value) =0;
		virtual void stop() =0;
		virtual void print() =0;
};
class DummyDetectorLogInfo : public DetectorLogInfo {
	public:
	DummyDetectorLogInfo(){
		
	}
	void start(){
		
	}
	void start_entry(int task_id, int inst_id, int task_rank, int out_id, bool is_outlier, bool is_error, double val, double cval){
		
	}
	void end_entry(){
		
	}
	void set(int idx, const char * prop, double value){
		
	}
	void stop(){
		
	}
	void print(){
		
	}
};

class RealDetectorLogInfo : public DetectorLogInfo {
	protected:
	//data
	int ndumps;
	ldetector_id_t id_info;
	char** prop_hdrs; 
	ldetector_node_t * ldetectors; 
	ldetector_node_t * LDET_LAST;
	long int LDET_SIZE; 
	FILE * file;
	//internal functions
	void init_logs();
	void add_log(ldetector_info_t l);
	ldetector_info_t * last_log();
	void del_logs();
	int n;
	public:
	RealDetectorLogInfo(const char * prop, int nprops);
	~RealDetectorLogInfo();
	void start_entry(int task_id, int inst_id, int task_rank, int out_id, bool is_outlier, bool is_error, double val, double cval);
	void end_entry();
	void start();
	void set(int idx, const char * prop, double value);
	void stop();
	void print();
};



class CommunicationInfo {
	public:
		CommunicationInfo(){}
		virtual void set_taskset(int id, int i)=0;
		virtual void send(int amt)=0;
		virtual void recv(int amt)=0;
};

class DummyCommunicationInfo : public CommunicationInfo {
	public:
		DummyCommunicationInfo(){}
		~DummyCommunicationInfo(){}
		void set_taskset(int id, int i){}
		void send(int amt){}
		void recv(int amt){}
};

class RealCommunicationInfo : public CommunicationInfo {
		FILE * file;
		int taskset;
		int task;
		int sent;
		int recvd;
		void write_line();
	public:
		RealCommunicationInfo(const char * filename);
		~RealCommunicationInfo();
		void set_taskset(int id, int i);
		void send(int amt);
		void recv(int amt);
};
#endif
