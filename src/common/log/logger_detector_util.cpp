#include "logger.h"
#include "stdlib.h"


void RealDetectorLogInfo::init_logs(){
	this->LDET_SIZE = 0;
	this->LDET_LAST = NULL;
	this->ldetectors = NULL;
}
void RealDetectorLogInfo::add_log(ldetector_info_t l){
	ldetector_node_t * n = new ldetector_node_t();
	n->NEXT = NULL;
	n->data = l;
	if(this->ldetectors == NULL){
		this->LDET_LAST = this->ldetectors = n;
	}
	else{
		this->LDET_LAST->NEXT = n;
		this->LDET_LAST = n;
	}
	this->LDET_SIZE++;
}
ldetector_info_t * RealDetectorLogInfo::last_log(){
	return &this->LDET_LAST->data;
}
void RealDetectorLogInfo::del_logs(){
	while(this->ldetectors != NULL){
		ldetector_node_t * n = this->ldetectors->NEXT;
		free(this->ldetectors->data.props);
		free(this->ldetectors->data.prop_mask);
		free(this->ldetectors);
		this->ldetectors = n;
	}
	this->LDET_SIZE = 0;
	this->LDET_LAST = NULL;
	this->ldetectors = NULL;
}
