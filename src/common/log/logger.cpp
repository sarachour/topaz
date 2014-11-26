#include "logger.h"
#include "pin_util.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;


void DummyLogInfo::start(int place, int task_id, int task_inst, int task_rank){
	this->task_id = task_id;
	pin_start_task(task_id);
}
void DummyLogInfo::stop(){
	pin_task_info_t pt;
	pin_stop_task(this->task_id, &pt);
}
	
#define MAX_SIZE (300*1000000/(sizeof(log_info_t)))

//#define MAX_SIZE (100*1000/(sizeof(log_info_t)))


RealLogInfo::RealLogInfo(const char * path) : LogInfo(path){
	init_logs();
	this->running = false;
	this->file =  fopen(path, "w");
	this->ndumps = 0;
}
RealLogInfo::~RealLogInfo(){
	del_logs();
	fclose(this->file);
}

void RealLogInfo::start(int place, int task_id, int task_inst, int task_rank){
	log_info_t p;
	p.id.task_id = task_id;
	p.id.inst_id = task_inst;
	p.id.task_rank = task_rank;
	p.id.place = place;
	
	if(this->TASK_SIZE > MAX_SIZE){
		this->print();
		this->del_logs();
		this->ndumps++;
	}
	
	this->add_log(p);
	pin_start_task(p.id.task_id);
	this->running = true;
}
void RealLogInfo::stop(){
	if(this->running == false) return; // can't stop a stopped task again.
	pin_task_info_t pt;
	log_info_t * p = this->last_log();
	pin_stop_task(p->id.task_id, &pt);
	p->pin = pt;
	this->running = false;
	
}
void RealLogInfo::aug_clear(){
	this->CACHE_ACC = false;
	this->CACHE_ERR = false;
}
void RealLogInfo::aug_task_accept(bool accept){
	this->CACHE_ACC = accept;
}
void RealLogInfo::aug_task_error(bool has_error){
	this->CACHE_ERR = has_error;
}
//get info for packing data

//doing pack, unpack
int RealLogInfo::get_pack_size(){
	return (sizeof(pin_task_info_t) + sizeof(double));
}

void RealLogInfo::pack(int place, int task_id, int task_inst, int task_rank, char * t){
	double * it = (double *) t;
	if(this->running){ //stop a running task to pack it.
			this->stop();
	}
	log_info_t * p;
	p = this->last_log();	
	if(p == NULL) return;
	*it = place; it++;
	it = pin_store_task(&p->pin, it);
	
}

//place is place of current unpack
void RealLogInfo::unpack(int place, int task_id, int task_inst, int task_rank, char * t){
	double * it = (double *) t;
	log_info_t p;
	p.id.place = *it; it++;
	p.id.task_id = task_id;
	p.id.inst_id = task_inst;
	p.id.task_rank = task_rank;
	p.det.accept = this->CACHE_ACC;
	p.det.error = this->CACHE_ERR;
	it = pin_load_task(&p.pin, it);
	
	if(this->TASK_SIZE > MAX_SIZE){
		this->print();
		this->del_logs();
		this->ndumps++;
	}
	this->add_log(p);
	
}


void RealLogInfo::print(){
	FILE * fp =this->file;
	log_node_t * n = this->tasks;
	
	if(this->ndumps == 0){
		fprintf(fp, "MACHINE\tTASK\tINSTANCE\tRANK\t");
		fprintf(fp, "ACCEPT\tERROR\t");
		fprintf(fp, "MREAD-ERR\tMREAD-INSTR\tMREAD-TOTAL\t");
		fprintf(fp, "MWRITE-ERR\tMWRITE-INSTR\tMWRITE-TOTAL\t");
		fprintf(fp, "MEM-ERR\tMEM-INSTR\tMEM-TOTAL\t");
		fprintf(fp, "TOTAL\n");
	}
	int i=0;
	while(n != NULL){
		log_info_t d = n->data;
		fprintf(fp, "%d\t%d\t%d\t%d\t", d.id.place, d.id.task_id, d.id.inst_id, d.id.task_rank);
		fprintf(fp, "%d\t%d\t", d.det.accept ? 1:0, d.det.error ? 1 : 0);
		fprintf(fp, "%f\t%f\t%f\t", d.pin.mread.err, d.pin.mread.instr, d.pin.mread.total);
		fprintf(fp, "%f\t%f\t%f\t", d.pin.mwrite.err, d.pin.mwrite.instr, d.pin.mwrite.total);
		fprintf(fp, "%f\t%f\t%f\t", d.pin.mem.err, d.pin.mem.instr, d.pin.mem.total);
		fprintf(fp, "%f\n",d.pin.total);
		n=n->NEXT;
		i++;
	}

}
