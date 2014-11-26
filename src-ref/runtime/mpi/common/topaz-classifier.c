#include "topaz.h"
#include "mem.h"
#include "mpi.h"
#include "stdlib.h"
#include "stdio.h"
#include "const.h"
#include "logger.h"

#include "classifier.h"

#include <stdarg.h>

void topaz_cls_init(topaz_info * tpz, int task_id, int arg_id, const char * c){
	int i;
	classifier_info * d = (classifier_info *) malloc(sizeof(classifier_info)*tpz->machine.size);
	tpz->tasks.task_list[task_id].failure_ratio = 0;
	tpz->tasks.task_list[task_id].outputs[arg_id].autocheckers = d;
	int siz = tpz->tasks.task_list[task_id].outputs[arg_id].size;
	for(i=0; i < tpz->machine.size; i++){
			cls_init(&d[i], c, i, siz);
	}
}


void topaz_cls_print(topaz_info * tpz, int task_id, int arg_id){
	int i;
	classifier_info * d = tpz->tasks.task_list[task_id].outputs[arg_id].autocheckers;
	if(d == NULL) return;
	for(i=0; i < tpz->machine.size; i++){
			cls_print(&d[i]);
	}
}



void topaz_cls_destroy(topaz_info * tpz, int task_id, int arg_id){
	int i;
	classifier_info * d = (classifier_info *) malloc(sizeof(classifier_info)*tpz->machine.size);
	tpz->tasks.task_list[task_id].outputs[arg_id].autocheckers = d;
	int siz = tpz->tasks.task_list[task_id].outputs[arg_id].size;
	for(i=0; i < tpz->machine.size; i++){
			cls_destroy(&d[i]);
	}
}


int topaz_cls_predict_int(topaz_info * tpz, int task_id, int arg_id, int place, int * data){
	classifier_info * d = &tpz->tasks.task_list[task_id].outputs[arg_id].autocheckers[place];
	if(d == NULL) return true;
	double conf;
	int res = cls_predict_int(d, data, &conf);
	res = res < 0 ? false : true;
	#ifdef FILL_HOLE
	if(res == false){
		cls_draw_int(d, data);
		res = true;
	}
	#endif
	return res;
}
int topaz_cls_predict_double(topaz_info * tpz, int task_id, int arg_id, int place, double * data){
	classifier_info * checkers = tpz->tasks.task_list[task_id].outputs[arg_id].autocheckers;
	if(checkers == NULL || tpz->config.en_logging) return true;
	classifier_info * d = &checkers[place];
	if(d == NULL) return true;
	double conf;
	int res = cls_predict_double(d, data, &conf);
	#ifdef FILL_HOLE
	if(res == false){
		cls_draw_double(d, data);
		res = true;
	}
	#endif
	return res;
}
/*
 * predict if something is true, otherwise replace with a decent prediction.
 */
int topaz_cls_predict_float(topaz_info * tpz, int task_id, int arg_id, int place, float * data){
	classifier_info * d = &tpz->tasks.task_list[task_id].outputs[arg_id].autocheckers[place];
	if(d == NULL || tpz->config.en_logging) return true;
	double conf;
	int res = cls_predict_float(d, data, &conf);
	#ifdef FILL_HOLE
	if(res == false){
		cls_draw_float(d, data);
		res = true;
	}
	#endif
	return res;
}
int topaz_cls_predict_char(topaz_info * tpz, int task_id, int arg_id, int place, char * data){
	classifier_info * d = &tpz->tasks.task_list[task_id].outputs[arg_id].autocheckers[place];
	if(d == NULL || tpz->config.en_logging) return true;
	double conf;
	int res = cls_predict_char(d, data, &conf);

	return res;
}


int topaz_cls_update_int(topaz_info * tpz, int task_id, int arg_id, int place, int * data){
	classifier_info * d = tpz->tasks.task_list[task_id].outputs[arg_id].autocheckers;
	if(d == NULL) return true;
	double conf;
	int res = cls_update_int(d, data, &conf);
	return res;
}
int topaz_cls_update_double(topaz_info * tpz, int task_id, int arg_id, int place, double * data){
	classifier_info * checkers = tpz->tasks.task_list[task_id].outputs[arg_id].autocheckers;
	if(checkers == NULL) return true;
	classifier_info * d = &checkers[place];
	if(d == NULL) return true;
	double conf;
	int res = cls_update_double(d, data, &conf);
	return res;
}
/*
 * predict if something is true, otherwise replace with a decent prediction.
 */
int topaz_cls_update_float(topaz_info * tpz, int task_id, int arg_id, int place, float * data){
	classifier_info * d = &tpz->tasks.task_list[task_id].outputs[arg_id].autocheckers[place];
	if(d == NULL) return true;
	double conf;
	int res = cls_update_float(d, data, &conf);
	printf("-> %f\n", data[0]);
	return res;
}

//
int topaz_cls_decide(topaz_info * tpz, int task_id, int isOk){
	tpz->tasks.task_list[task_id].failure_ratio*=0.9;
	if(isOk) tpz->tasks.task_list[task_id].failure_ratio+=0.1*0.0;
	else tpz->tasks.task_list[task_id].failure_ratio+=0.1*1.0;
	return (isOk == false);
	//return (tpz->tasks.task_list[task_id].failure_ratio > 0.3 && isOk == false ? true : false);
}
