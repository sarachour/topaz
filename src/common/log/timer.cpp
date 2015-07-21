#include "logger.h"
#include "string.h"

pthread_mutex_t lock;

#define MAX_SIZE (300/(sizeof(timer_info_t)))

RealTimerInfo::RealTimerInfo(const char * base){
	strcpy(this->filename,base);
	printf("INITIALIZE TIMER\n");
	this->file = fopen(base, "w");
	fprintf(this->file, "INDEX\tINSTRUCTIONS\n");
	this->ndumps = 0;
	this->TIMER_OFF = false;
	this->n_stops = 0;
	for(int i=0; i < N_TIMERS; i++){
		this->active[i] = false;
		for(int j=0; j < N_HISTS; j++){
			this->stopped[j][i] = false;
		}
	}
	init_timers();
}
RealTimerInfo::~RealTimerInfo(){
	del_timers();
	fclose(this->file);
}
void  RealTimerInfo::stop_active(){
	int j = this->n_stops;
	for(int i=0; i < N_TIMERS; i++){
		this->stopped[j][i] = this->active[i]; //stop any active timers.
		if(this->active[i]){
			this->stop(i);
		}
	}
	if(this->n_stops <  N_HISTS-1){
		this->n_stops++;
	}
}
void  RealTimerInfo::start_active(){
	int j = this->n_stops-1;
	if(j < 0) return;
	for(int i=0; i < N_TIMERS; i++){
		if(this->stopped[j][i]){
			this->stopped[j][i] = false;
			this->start(i);
		}
	}
	this->n_stops--;
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
}
void RealTimerInfo::dump(){
	this->stop_active();
	this->print();
	this->del_timers();
	this->ndumps++;
	this->start_active();
}
void print_timer(FILE * fp, timer_info_t d){
	fprintf(fp, "%u\t%f\n", d.idx, d.inst);
	
}

void RealTimerInfo::stop(int c){
	if(this->TIMER_OFF || !this->active[c])
		return;
	pin_timer_info_t p;
	pin_stop_timer(c, &p);
	this->active[c] = false;
	timer_info_t pt;
	pt.idx = c;
	pt.inst = p.inst;
	//print_timer(this->file,pt);
	
	if(this->TIMER_SIZE > MAX_SIZE){
		//this->dump();
	}
	this->add_timer(pt);
	
}
void RealTimerInfo::print(){
	FILE * fp = this->file;
	timer_node_t * n = this->timers;
	while(n != NULL){
		timer_info_t d = n->data;
		fprintf(fp, "%u\t%f\n", d.idx, d.inst);
		n=n->NEXT;
	}
	fflush(fp);
	
}

RealCommunicationInfo::RealCommunicationInfo(const char * filename){
	//file = fopen(filename, "w");
	//fprintf(file, "taskset\ttask\tsent\trecvd\n");
	taskset = -1;
	task = -1;
	sent = 0;
	recvd = 0;
}

void RealCommunicationInfo::write_line(){
	//fprintf(file,"%d\t%d\t%d\t%d\n", taskset, task, sent, recvd);
}
RealCommunicationInfo::~RealCommunicationInfo(){
	//fclose(file);
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
