#include "logger.h"
#include "string.h"
#include "topaz.h"
#include "pin_util.h"


//#define DEBUG
#define MAX_SIZE (300/(sizeof(timer_info_t)))

RealTimerInfo::RealTimerInfo(const char * base){
	pin_disable_timers();
	strcpy(this->filename,base);
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
	pin_enable_timers();
}
RealTimerInfo::~RealTimerInfo(){
	del_timers();
	fclose(this->file);
}
void  RealTimerInfo::stop_active(int cache){
	pin_disable_timers();
	int j = cache;
	#ifdef DEBUG
	printf("[%s,%d] stopping: ", Topaz::topaz == NULL ? "?" : (Topaz::topaz->isMain() ? "m" : "w"),j);
	#endif
	for(int i=0; i < N_TIMERS; i++){
		this->stopped[j][i] = this->active[i]; //stop any active timers.
		if(this->active[i]){
			int ninst = this->_stop(i);
			#ifdef DEBUG
			printf("%d=%d ",i,ninst);
			#endif
		}
	}
	#ifdef DEBUG
	printf("\n");
	#endif
	pin_enable_timers();
}
void  RealTimerInfo::start_active(int cache){
	pin_disable_timers();
	int j = cache;
	#ifdef DEBUG
	printf("[%s,%d] starting: ", Topaz::topaz == NULL ? "?" : (Topaz::topaz->isMain() ? "m" : "w"),j);
	#endif
	for(int i=0; i < N_TIMERS; i++){
		if(this->stopped[j][i]){
			this->stopped[j][i] = false;
			
			this->_start(i);
			
			#ifdef DEBUG
			printf("%d ",i);
			#endif
		}
	}
	#ifdef DEBUG
	printf("\n");
	#endif
	pin_enable_timers();
}
void RealTimerInfo::on(){
	this->TIMER_OFF = false;
}
void RealTimerInfo::off(){
	this->TIMER_OFF = true;
}

//start,stop timers
void RealTimerInfo::_start(int c){
	if(this->TIMER_OFF || this->active[c])
		return;
	this->active[c] = true;
	pin_start_timer(c);
}
void RealTimerInfo::start(int c){
	pin_disable_timers();
	this->_start(c);
	pin_enable_timers();
}
void RealTimerInfo::dump(){
	pin_disable_timers();
	this->print();
	this->del_timers();
	this->ndumps++;
	this->start_active(0);
	pin_enable_timers();
}
void print_timer(FILE * fp, timer_info_t d){
	fprintf(fp, "%u\t%f\n", d.idx, d.inst);
	
}

int RealTimerInfo::_stop(int c){
	if(this->TIMER_OFF || !this->active[c])
		return 0;
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
	return pt.inst;
}
void RealTimerInfo::stop(int c){
	pin_disable_timers();
	this->_stop(c);
	pin_enable_timers();
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
