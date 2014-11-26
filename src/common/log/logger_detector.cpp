#include "logger.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#define MAX_SIZE (1000000/(sizeof(ldetector_info_t)))

ldetector_id_t DetectorLogInfo::CURR_ID;
bool DetectorLogInfo::IS_ACCEPTED;

RealDetectorLogInfo::RealDetectorLogInfo(const char * file, int nprops){
	this->n = nprops;
	this->init_logs();
	this->file = fopen(file, "w");
	this->prop_hdrs = new char*[this->n];
	for(int i=0; i < this->n; i++){
		this->prop_hdrs[i] = NULL;
	}
	this->ndumps = 0;
}
RealDetectorLogInfo::~RealDetectorLogInfo(){
	this->del_logs();
	fclose(this->file);
}
void RealDetectorLogInfo::start(){
	
	//every MAX_LOGS size, print and free
	
}
/*
void RealDetectorLogInfo::set_info(int task_id, int inst_id, int task_rank, int out_id){
	this->id_info.task_id = task_id;
	this->id_info.task_inst = inst_id;
	this->id_info.task_rank = task_rank;
	this->id_info.output_id = out_id;
}
*/
void RealDetectorLogInfo::set(int id, const char * prop, double value){
	if(this->prop_hdrs[id] == NULL){
		this->prop_hdrs[id] = (char *) malloc(sizeof(char)*128);
		strcpy(this->prop_hdrs[id], prop);
	}
	ldetector_info_t * t = this->last_log();
	t->props[id] = value;
	t->prop_mask[id] = true;
}
void RealDetectorLogInfo::stop(){
	
}
void RealDetectorLogInfo::end_entry(){
	
	
}
void RealDetectorLogInfo::start_entry(int task_id, int inst_id, int task_rank, int out_id, bool isaccept, bool iscorr, double val, double cval){
	ldetector_info_t d;
	d.props = new double[this->n];
	d.prop_mask = new bool[this->n];
	d.id.task_id = task_id;
	d.id.task_inst = inst_id;
	d.id.task_rank = task_rank;
	d.id.out_id = out_id;
	d.value = val;
	d.cvalue = cval;
	d.is_corr = iscorr;
	d.is_accept = isaccept;
	
	for(int i=0; i < this->n; i++){
		d.prop_mask[i] = false;
	}
	//dump current log info if exceeded size
	if(this->LDET_SIZE == MAX_SIZE){
		this->print();
		this->del_logs();
	}
	//add new log
	this->add_log(d);
	
}
void float_print(FILE * fp,double v) {
	if(abs(v) < 1e-4) fprintf(fp, "%e\t", v);
	else fprintf(fp,"%f\t",v);
}
void RealDetectorLogInfo::print(){
	FILE * fp = this->file;
	float total = LDET_SIZE;
	ldetector_node_t * n = this->ldetectors;
	
	int nprop = 0;
	for(int i=0; i < this->n; i++){
		if(this->prop_hdrs[i] != NULL){
			nprop=i+1;
		}
	}
	
	if(this->ndumps == 0){
		fprintf(fp, "ID\tINST\tRANK\tOUT-ID\t");
		fprintf(fp, "VAL\tCORR-VAL\tIS-ACCEPT\tIS-CORR\t"); //special properties
		for(int i=0; i < nprop; i++){
			fprintf(fp,"%s\t", prop_hdrs[i] != NULL ? prop_hdrs[i] : "");
		}
		fprintf(fp,"\n");
	}
	
	while(n != NULL){
		ldetector_info_t d = n->data;
		fprintf(fp, "%d\t%d\t%d\t%d\t", 
			d.id.task_id, d.id.task_inst, d.id.task_rank, d.id.out_id);
		float_print(fp,d.value);
		float_print(fp,d.cvalue);
		fprintf(fp, "%c\t%c\t",d.is_accept ? 'y':'n', d.is_corr ? 'y':'n' );
		for(int i=0; i < nprop; i++){
			if(d.prop_mask[i]) float_print(fp,d.props[i]);
			else fprintf(fp, "\t");
		}
		fprintf(fp,"\n");
		
		n=n->NEXT;
	}
	this->ndumps++;
	
}
/*
void RealDetectorLogInfo::set_value(double d){
	ldetector_info_t * t = this->last_log();
	t->value = d;
}
void RealDetectorLogInfo::set_category(int d){
	ldetector_info_t * t = this->last_log();
	t->category = d;
}
*/
