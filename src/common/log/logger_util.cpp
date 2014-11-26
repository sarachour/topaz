#include "logger.h"
#include "stdlib.h"

void RealLogInfo::init_logs(){
	this->tasks = NULL;
	this->TASK_SIZE = 0;
	this->TASK_LAST = NULL;
}
void RealLogInfo::del_logs(){
	log_node_t * curr = this->tasks;
	while(curr != NULL){
		log_node_t * n = curr->NEXT;
		free(curr);
		curr = n;
	}
	this->tasks = NULL;
	this->TASK_SIZE = 0;
	this->TASK_LAST = NULL;
}
log_info_t * RealLogInfo::last_log(){
	if(this->TASK_LAST == NULL){
		return NULL;
	}
	else
		return &this->TASK_LAST->data;
}
void RealLogInfo::add_log(log_info_t e){
	log_node_t * n = (log_node_t*) malloc(sizeof(log_node_t));
	n->NEXT = NULL;
	n->PREV = this->TASK_LAST;
	n->data = e;
	if(this->tasks == NULL){
		this->TASK_LAST = this->tasks = n;
	}
	else{
		this->TASK_LAST->NEXT = n;
		this->TASK_LAST = n;
	}
	this->TASK_SIZE++;
}

