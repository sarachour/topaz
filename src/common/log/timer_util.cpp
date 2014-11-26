#include "logger.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;


void RealTimerInfo::init_timers(){
	this->timers = NULL;
	this->TIMER_SIZE = 0;
	this->TIMER_LAST = NULL;
}
void RealTimerInfo::del_timers(){
	while(this->timers != NULL){
		timer_node_t * n = this->timers->NEXT;
		free(this->timers);
		this->timers = n;
	}
	this->TIMER_SIZE = 0;
	this->timers = this->TIMER_LAST = NULL;
	
}
void RealTimerInfo::add_timer(timer_info_t e){
	timer_node_t * n = (timer_node_t*) malloc(sizeof(timer_node_t));
	n->NEXT = NULL;
	n->data = e;
	if(this->timers == NULL){
		this->TIMER_LAST = this->timers = n;
	}
	else{
		this->TIMER_LAST->NEXT = n;
		this->TIMER_LAST = n;
	}
	this->TIMER_SIZE++;
}
timer_info_t * RealTimerInfo::last_timer(){
	return &this->TIMER_LAST->data;
}

