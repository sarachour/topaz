#include "logger.h"

#define MAX_SIZE (10*1000/(sizeof(timer_info_t)))

RealTimerInfo::RealTimerInfo(const char * base){
	/*
	this->file = fopen(base, "w");
	this->ndumps = 0;
	*/
	this->TIMER_OFF = false;
	for(int i=0; i < N_TIMERS; i++){
		this->active[i] = false;
		for(int j=0; j < N_HISTS; j++){
			this->stopped[j][i] = false;
		}
	}
	//init_timers();
}
RealTimerInfo::~RealTimerInfo(){
	del_timers();
	/*
	fclose(this->file);
	*/
}
void  RealTimerInfo::stop_active(){
	int j = 0;
	for(int i=0; i < N_TIMERS; i++){
		this->stopped[j][i] |= this->active[i]; //stop any active timers.
		if(this->active[i]){
			this->stop(i);
		}
	}
}
void  RealTimerInfo::start_active(){
	int j = 0;
	for(int i=0; i < N_TIMERS; i++){
		if(this->stopped[j][i]){
			this->stopped[j][i] = false;
			this->start(i);
		}
	}
}
void RealTimerInfo::on(){
	this->TIMER_OFF = false;
}
void RealTimerInfo::off(){
	this->TIMER_OFF = true;
}

//start,stop timers
void RealTimerInfo::start(int c){
	if(this->TIMER_OFF || this->active[c])
		return;
	this->active[c] = true;
	pin_start_timer(c);
	/*
	*/
}
void RealTimerInfo::dump(){

}
void print_timer(FILE * fp, timer_info_t d){
	fprintf(fp, "%u\t%f\n", d.idx, d.inst);
	
}

void RealTimerInfo::stop(int c){
	if(this->TIMER_OFF || !this->active[c])
		return;
	pin_stop_timer(c);
	this->active[c] = false;
}
void RealTimerInfo::print(){

}

RealCommunicationInfo::RealCommunicationInfo(const char * filename){
	file = fopen(filename, "w");
	fprintf(file, "taskset\ttask\tsent\trecvd\n");
	taskset = -1;
	task = -1;
	sent = 0;
	recvd = 0;
}

void RealCommunicationInfo::write_line(){
	fprintf(file,"%d\t%d\t%d\t%d\n", taskset, task, sent, recvd);
}
RealCommunicationInfo::~RealCommunicationInfo(){
	fclose(file);
}
void RealCommunicationInfo::dump(){
	
	
}
void RealCommunicationInfo::set_taskset(int id, int i){
	if(taskset >= 0) this->write_line();
	taskset = id;
	task = i;
	sent = 0;
	recvd = 0;
}
void RealCommunicationInfo::send(int amt){
	sent += amt;
	
}
void RealCommunicationInfo::recv(int amt){
	recvd += amt;
}
