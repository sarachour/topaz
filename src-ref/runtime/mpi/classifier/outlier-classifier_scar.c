#include "stdio.h"
#include "stdlib.h"
#include "const.h"
#include "classifier.h"
#include "string.h"
#include "float.h"

void cls_print(classifier_info * cinfo){
	
}
void cls_init(classifier_info * cinfo, const char * name, int place, int size){
		int i;
		strcpy(cinfo->name, name);
		int bufsiz = 1000;
		
		cinfo->buffer = (sorted_array *) malloc(sizeof(sorted_array)*(size));
		cinfo->size = size;
		for(i=0; i < size; i++){
			cinfo->buffer[i].n = 0;
			cinfo->buffer[i].size = bufsiz;
			cinfo->buffer[i].data = (double *) malloc(sizeof(double)*(cinfo->buffer[i].size+1));
			cinfo->buffer[i].k = 4;
			cinfo->buffer[i].mean = 0;
			cinfo->buffer[i].stdev = 0;
			cinfo->buffer[i].sqsum = 0;
		}
		
		cinfo->initialized = true;
}
void cls_destroy(classifier_info * cinfo){
	int i;
	for(i=0; i < cinfo->size; i++){
		free(cinfo->buffer[i].data);
	}
}
int __cls_predict_base(classifier_info * cinfo, double * val){
	if(cinfo->initialized == false) return 1;
	//prediction = svm_predict(cinfo->model, cinfo->data);
	int i;
	cinfo->ntasks_total++;
	cinfo->nelems_total+=cinfo->size;
	
	for(i=0; i < cinfo->size; i++){
		int n = cinfo->buffer[i].n;
		if(n < 2 || val[i] < cinfo->buffer[i].data[0] || val[i] > cinfo->buffer[i].data[n-1] ) {
			cinfo->ntasks_outl++;
			return false;
		}
	}
	return true;
}


void  __cls_draw_point(classifier_info * cinfo, double * val){


}
void cls_draw_int(classifier_info * cinfo, int * val){


}
void cls_draw_float(classifier_info * cinfo, float * val){


}

void cls_draw_double(classifier_info * cinfo, double * val){


}

int cls_predict_int(classifier_info * cinfo, int * val, double * confidence){
	if(cinfo->initialized == false) return 1;
	//prediction = svm_predict(cinfo->model, cinfo->data);
	int i;
	cinfo->ntasks_total++;
	cinfo->nelems_total+=cinfo->size;
	
	for(i=0; i < cinfo->size; i++){
		if(cinfo->buffer[i].n < 2 || val[i] < cinfo->buffer[i].min || val[i] > cinfo->buffer[i].max ) {
			cinfo->ntasks_outl++;
			return false;
		}
	}
	return true;
}

int cls_predict_double(classifier_info * cinfo, double * val, double * confidence){
	if(cinfo->initialized == false) return 1;
	//prediction = svm_predict(cinfo->model, cinfo->data);
	int i;
	cinfo->ntasks_total++;
	cinfo->nelems_total+=cinfo->size;
	
	for(i=0; i < cinfo->size; i++){
		if(cinfo->buffer[i].n < 2 || val[i] < cinfo->buffer[i].min || val[i] > cinfo->buffer[i].max || isnan(val[i]) || isinf(val[i]) ) {
			printf("[%s(%d)] Outlier Detect: %le (%le, %le))\n", cinfo->name, i, (double) val[i], cinfo->buffer[i].min, cinfo->buffer[i].max);
			cinfo->ntasks_outl++;
			return false;
		}
	}
	return true;
}

int cls_predict_char(classifier_info * cinfo, char * val, double * confidence){
	if(cinfo->initialized == false) return 1;
	//prediction = svm_predict(cinfo->model, cinfo->data);
	int i;
	cinfo->ntasks_total++;
	cinfo->nelems_total+=cinfo->size;
	
	for(i=0; i < cinfo->size; i++){
		if(cinfo->buffer[i].n < 2 || val[i] < cinfo->buffer[i].min || val[i] > cinfo->buffer[i].max ) {
			cinfo->ntasks_outl++;
			return false;
		}
	}
	return true;
}

int cls_predict_float(classifier_info * cinfo, float * val, double * confidence){
	if(cinfo->initialized == false) return 1;
	//prediction = svm_predict(cinfo->model, cinfo->data);
	int i;
	cinfo->ntasks_total++;
	cinfo->nelems_total+=cinfo->size;
	
	for(i=0; i < cinfo->size; i++){
		if(cinfo->buffer[i].n < 2 || val[i] < cinfo->buffer[i].min || val[i] > cinfo->buffer[i].max  || isnan(val[i]) || isinf(val[i]) ) {
			printf("[%s(%d)] fOutlier Detect: %le [%le %le] (%le, %le))\n", cinfo->name, i, (double) val[i], 
				cinfo->buffer[i].mean, cinfo->buffer[i].stdev, cinfo->buffer[i].min, cinfo->buffer[i].max);
			cinfo->ntasks_outl++;
			return false;
		}
	}
	return true;
}

void __cls_update_cls(classifier_info * cinfo,int i, double val){
	int k=0; 
	int min, max;
	double * data = cinfo->buffer[i].data;
	int n = cinfo->buffer[i].n;
	int size = cinfo->buffer[i].size;
	/*
	int pos = (n+1)%size;
	data[pos] = val;
	
	if(n < size) n++;
	
	min =  max = data[0];
	for(k=0; k < n; k++){
		if(data[k] < min) min = data[k];
		if(data[k] > max) max = data[k];
	}
	*/
	if(n > 0){
		//update accordingly
		//cinfo->buffer[i].min = cinfo->buffer[i].min < val ? cinfo->buffer[i].min : val;
		//cinfo->buffer[i].max = cinfo->buffer[i].max > val ? cinfo->buffer[i].max : val;
		int n = (cinfo->buffer[i].n+1);
		cinfo->buffer[i].sqsum += val*val;
		cinfo->buffer[i].sum += val;
		cinfo->buffer[i].mean = cinfo->buffer[i].sum/n;
		cinfo->buffer[i].stdev = sqrt(n*cinfo->buffer[i].sqsum - (cinfo->buffer[i].mean*cinfo->buffer[i].mean))/((double) n);
		//printf("mean: %le, std: %le\n",  cinfo->buffer[i].mean,  cinfo->buffer[i].stdev);
		cinfo->buffer[i].min = cinfo->buffer[i].mean - cinfo->buffer[i].stdev*cinfo->buffer[i].k;
		cinfo->buffer[i].max = cinfo->buffer[i].mean + cinfo->buffer[i].stdev*cinfo->buffer[i].k;
		
	}
	else{
		cinfo->buffer[i].min = val;
		cinfo->buffer[i].max = val;
		cinfo->buffer[i].mean = val;
		cinfo->buffer[i].stdev = 0;
		cinfo->buffer[i].sum = val;
		cinfo->buffer[i].sqsum = val*val;
	}
	
	cinfo->buffer[i].n += 1;
	
	
	
}
int cls_update_int(classifier_info * cinfo, int * val, double * confidence){
	if(cinfo->initialized == false) return 1;
	//prediction = svm_predict(cinfo->model, cinfo->data);
	int i;
	for(i=0; i < cinfo->size; i++){
		__cls_update_cls(cinfo, i, (double) val[i]);
	}
	return true;
}

int cls_update_double(classifier_info * cinfo, double * val, double * confidence){
	if(cinfo->initialized == false) return 1;
	//prediction = svm_predict(cinfo->model, cinfo->data);
	int i;
	for(i=0; i < cinfo->size; i++){
		__cls_update_cls(cinfo, i, (double) val[i]);
	}
	return true;
}

int cls_update_char(classifier_info * cinfo, char * val, double * confidence){
	if(cinfo->initialized == false) return 1;
	int i;
	for(i=0; i < cinfo->size; i++){
		__cls_update_cls(cinfo, i, (double) val[i]);
	}
	return true;
}

int cls_update_float(classifier_info * cinfo, float * val, double * confidence){
	if(cinfo->initialized == false) return 1;
	//prediction = svm_predict(cinfo->model, cinfo->data);
	int i;
	for(i=0; i < cinfo->size; i++){
		__cls_update_cls(cinfo, i, (double) val[i]);
	}
	return true;
}



