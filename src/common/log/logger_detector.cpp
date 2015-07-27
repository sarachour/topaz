#include "logger.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#include "topaz.h"
#include "task.h"
#include "abs_detectors.h"
#include "scar_detector.h"


RealDetectorLogInfo::RealDetectorLogInfo(const char * filename){
	this->file = fopen(filename, "w");
	fprintf(this->file, "ID\tIID\tRANK\tIS-REFRESH\tIS-FAILED\tIS-ACCEPT\t");
	fprintf(this->file, "OUTPUT\tCORR\t");
	fprintf(this->file, "OABS\tCABS\t");
	fprintf(this->file, "MINS\tMAXS\tMEANS\n");
}
RealDetectorLogInfo::~RealDetectorLogInfo(){
	fclose(this->file);
}

void print_abs(FILE * f, AbsDetectorManager * dmgr, int id){
	AbsDetector * d = dmgr->getDetector(id);
	float * vals = d->getOutputVector();
	int n = d->getDim();
	fprintf(f,"[");
	for(int i=0; i < n; i++){
		if(i > 0) fprintf(f,",");
		fprintf(f,"%f",vals[i]);
	}
	fprintf(f,"]");
}
void print_det_stat(FILE * f, AbsDetectorManager * dmgr, int id, char prop){
	AbsDetector * d = dmgr->getDetector(id);
	if(AbsScarRegionDetector* v = dynamic_cast<AbsScarRegionDetector*>(d)) {
	   // old was safely casted to NewType
	   fprintf(f,"[");
	   for(int i=0; i < v->getDim(); i++){
		    if(i > 0) fprintf(f,",[");
		    else fprintf(f,"[");
		    
			for(int j=0; j < v->getNRegions(); j++){
				AbsScarRegionDetector::region_t * r = v->getRegion(j);
				if(j > 0) fprintf(f,",");
				
				switch(prop){
					case 'l':
						fprintf(f,"%f",r->min[i]); break;
					case 'h':
						fprintf(f,"%f",r->max[i]); break;
					case 'c':
						fprintf(f,"%f",r->center[i]); break;
				}
			}
		    fprintf(f,"]");
		}
		fprintf(f,"]");
	}
	
}
void print_task_outputs(FILE * f, TaskSpec * ts, Task * t){
	int idx = 0;
	fprintf(f,"[");
	
	t->startUnpack();
	for(int i=0; i < ts->getNumOutputs(); i++){
		TaskArgSpec tsa = ts->getOutput(i);
		int n = tsa.getNumberElements();
		
		#define print_arr(TYP) { \
			TYP * ptr;\
			t->unpack(&ptr); \
			for(int k=0; k < n; k++){ \
				if(idx > 0) fprintf(f,",%f", static_cast<float>(ptr[k])); \
				else fprintf(f,"%f", static_cast<float>(ptr[k])); \
				idx++; \
			} \
		}
		
		switch(tsa.getType()){
			case FLOAT:
				print_arr(float); break;
			case DOUBLE:
				print_arr(double); break;
			case INT:
				print_arr(int); break;
			case CHAR:
				print_arr(char); break;
			case UNKNOWN:
				print_arr(char); break;
			
		}
		
	}
	fprintf(f,"]");
}
void RealDetectorLogInfo::add_entry(TaskSpec * ts, AbsDetectorManager * det, Task * input, Task * orig, Task * key, bool is_accepted){
	FILE * f = this->file;
	int dim = 0;
	for(int i=0; i < ts->getNumOutputs(); i++){
		int n = ts->getOutput(i).getNumberElements();
		dim += n;
	}
	//begin unpacking 
	input->startUnpack();
	orig->startUnpack();
	
	bool is_ref = input->isRefresh();
	int id = input->getId();
	int iid = input->getInstId()+1;
	int rank = input->getRank();
	bool is_failed = orig->hasFailed();
	fprintf(f,"%d\t%d\t%d\t",id,iid,rank);
	fprintf(f,"%s\t%s\t%s\t",
		is_ref ? "refresh" : "none", 
		is_failed ? "failed" : "none",
		is_accepted ? "accept" : "reject");
		
	print_task_outputs(f,ts,orig);
	fprintf(f,"\t");
	
	print_task_outputs(f,ts,key);
	fprintf(f,"\t");
	
	ts->abs(input, orig);
	print_abs(f,det,id);
	fprintf(f,"\t");
	
	ts->abs(input, key);
	print_abs(f,det,id);
	fprintf(f,"\t");
	
	print_det_stat(f,det,id,'l');
	fprintf(f,"\t");
	print_det_stat(f,det,id,'h');
	fprintf(f,"\t");
	print_det_stat(f,det,id,'c');
	fprintf(f,"\n");
	
}

/*
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
*/
