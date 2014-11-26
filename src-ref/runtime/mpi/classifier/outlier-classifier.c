#include "svm.h"
#include "stdio.h"
#include "stdlib.h"
#include "const.h"
#include "classifier.h"
#include "string.h"
#include "float.h"

FILE * __seek_until(FILE * f, char c){
	char chr;
	do{ 
		chr = fgetc(f);
	}while(chr != c && chr != EOF);
	return f;
}
void cls_init(classifier_info * cinfo, const char * name, int place, int size){
		char filename[512];
		char linebuf[1024];
		int i;
		sprintf(filename, "logs/%s.%d.outl", name, place);
		strcpy(cinfo->name, name);
		cinfo->mean = (double *) malloc(sizeof(double)*size);
		cinfo->std = (double *) malloc(sizeof(double)*size);
		cinfo->min = (double *) malloc(sizeof(double)*size);
		cinfo->max = (double *) malloc(sizeof(double)*size);
		cinfo->sumsq = (double *) malloc(sizeof(double)*size);
		cinfo->nelems = (int *) malloc(sizeof(int)*size);
		cinfo->ntasks_total = 0;
		cinfo->ntasks_outl = 0;
		cinfo->nelems_total = 0;
		cinfo->nelems_outl = 0;
		cinfo->size = size;
		cinfo->place = place;
		cinfo->initialized = false;
		FILE * f = fopen(filename, "r");
		if(f == NULL){
			printf("Couldn't open %s\n", filename);
			return;
		}
		printf("reading %s\n", name);
		printf("min: ");
		char chr;
		for(i=0; i < size-1; i++){
				fscanf(f, "%le", &cinfo->min[i]);
				printf("%le	", cinfo->min[i]);
				f=__seek_until(f,'\t');
		}
		fscanf(f, "%le", &cinfo->min[i]);
		printf("%le\n", cinfo->min[i]);
		f=__seek_until(f,'\n');
		
		printf("max: ");
		for(i=0; i < size-1; i++){
			fscanf(f, "%le", &cinfo->max[i]);
			printf("%le	", cinfo->max[i]);
			f=__seek_until(f,'\t');
		}
		fscanf(f, "%le", &cinfo->max[i]);
		printf("%le\n", cinfo->max[i]);
		f=__seek_until(f,'\n');
		
		printf("mean: ");
		for(i=0; i < size-1; i++){
			fscanf(f, "%le", &cinfo->mean[i]);
			printf("%le	", cinfo->mean[i]);
			f=__seek_until(f,'\t');
		}
		fscanf(f, "%le", &cinfo->mean[i]);
		printf("%le\n", cinfo->mean[i]);
		f=__seek_until(f,'\n');
		
		printf("std: ");
		for(i=0; i < size-1; i++){
			fscanf(f, "%le", &cinfo->std[i]);
			printf("%le	", cinfo->std[i]);
			f=__seek_until(f,'\t');
		}
		fscanf(f, "%le", &cinfo->std[i]);
		printf("%le\n", cinfo->std[i]);
		f=__seek_until(f,'\n');
		
		printf("nelems: ");
		for(i=0; i < size-1; i++){
			fscanf(f, "%d", &cinfo->nelems[i]);
			printf("%d	", cinfo->nelems[i]);
			f=__seek_until(f,'\t');
		}
		fscanf(f, "%d", &cinfo->nelems[i]);
		printf("%d\n", cinfo->nelems[i]);
		f=__seek_until(f,'\n');
		
		printf("sumsq: ");
		for(i=0; i < size-1; i++){
			fscanf(f, "%le", &cinfo->sumsq[i]);
			printf("%le	", cinfo->sumsq[i]);
			f=__seek_until(f,'\t');
		}
		fscanf(f, "%le", &cinfo->sumsq[i]);
		printf("%le\n", cinfo->sumsq[i]);
		f=__seek_until(f,'\n');
		
		fscanf(f, "%d", &cinfo->k);
		printf("%d\n", cinfo->k);
		f=__seek_until(f,'\n');
		if(cinfo->k ==0) cinfo->k = 1;
		
		cinfo->initialized = true;
}
void cls_print(classifier_info * cinfo){
	if(cinfo == NULL || cinfo->initialized == false) return;
	
	
}
void cls_destroy(classifier_info * cinfo){
	free(cinfo->mean);
	free(cinfo->std);
	free(cinfo->min);
	free(cinfo->max);
}
int __cls_predict_base(classifier_info * cinfo, double * val){
	
	//prediction = svm_predict(cinfo->model, cinfo->data);
	int i;
	for(i=0; i < cinfo->size; i++){
		if(val[i] < cinfo->min[i] || val[i] > cinfo->max[i]) return false;
	}
	return true;
}
float rand_gauss (void) {
  float v1,v2,s;

  do {
    v1 = 2.0 * ((float) rand()/RAND_MAX) - 1;
    v2 = 2.0 * ((float) rand()/RAND_MAX) - 1;

    s = v1*v1 + v2*v2;
  } while ( s >= 1.0 );

  if (s == 0.0)
    return 0.0;
  else
    return (v1*sqrt(-2.0 * log(s) / s));
}

void  __cls_draw_point(classifier_info * cinfo, double * val){
	int i;
	for(i=0; i < cinfo->size; i++){
		if(val[i] < cinfo->min[i] || val[i] > cinfo->max[i] || isnan(val[i]) || isinf(val[i])){
			cinfo->nelems_outl++;
			val[i] = (rand_gauss()-0.5)*cinfo->std[i]+cinfo->mean[i];
		}
	}
}
void cls_draw_int(classifier_info * cinfo, int * val){
	int i;
	for(i=0; i < cinfo->size; i++){
		if(val[i] < cinfo->min[i] || val[i] > cinfo->max[i] || isnan(val[i]) || isinf(val[i])){
				cinfo->nelems_outl++;
				val[i] = (rand_gauss()-0.5)*cinfo->std[i]+cinfo->mean[i];
		}
	}
}
void cls_draw_float(classifier_info * cinfo, float * val){
	int i;
	for(i=0; i < cinfo->size; i++){
		if(val[i] < cinfo->min[i] || val[i] > cinfo->max[i] || isnan(val[i]) || isinf(val[i])){
				cinfo->nelems_outl++;
				val[i] = (rand_gauss()-0.5)*cinfo->std[i]+cinfo->mean[i];
			}
	}
}

void cls_draw_double(classifier_info * cinfo, double * val){
	int i;
	for(i=0; i < cinfo->size; i++){
		if(val[i] < cinfo->min[i] || val[i] > cinfo->max[i] || isnan(val[i]) || isinf(val[i])){
			cinfo->nelems_outl++;
			val[i] = (rand_gauss()-0.5)*cinfo->std[i]+cinfo->mean[i];
		}
	}
}

int cls_predict_int(classifier_info * cinfo, int * val, double * confidence){
	if(cinfo->initialized == false) return 1;
	//prediction = svm_predict(cinfo->model, cinfo->data);
	int i;
	cinfo->ntasks_total++;
	cinfo->nelems_total+=cinfo->size;
	for(i=0; i < cinfo->size; i++){
		if(val[i] < cinfo->min[i] || val[i] > cinfo->max[i]){
			//printf("outlier detected %le not in (%le, %le)\n", val[i], cinfo->min[i], cinfo->max[i]);
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
		if(val[i] < cinfo->min[i] || val[i] > cinfo->max[i] || isnan(val[i]) || isinf(val[i])){
			//printf("outlier detected %le not in (%le, %le)\n", val[i], cinfo->min[i], cinfo->max[i]);
			//printf("%s: %f not in (%f, %f)\n", cinfo->name, val[i], cinfo->min[i], cinfo->max[i]);
			printf("[%s(%d)] Outlier Detect: %le (%le, %le))\n", cinfo->name, i, val[i], cinfo->min[i], cinfo->max[i]);
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
		if(val[i] < cinfo->min[i] || val[i] > cinfo->max[i]){
			//printf("outlier detected %le not in (%le, %le)\n", val[i], cinfo->min[i], cinfo->max[i]);
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
		if(val[i] < cinfo->min[i] || val[i] > cinfo->max[i] || isnan(val[i]) || isinf(val[i])){
			//printf("outlier detected %le not in (%le, %le)\n", val[i], cinfo->min[i], cinfo->max[i]);
			cinfo->ntasks_outl++;
			return false;
		}
	}
	return true;
}

void __cls_update_cls(classifier_info * cinfo,int i, double val){
	#define DC_RANGE FLT_MAX/10
	cinfo->mean[i] = cinfo->mean[i] + val - cinfo->mean[i]/(cinfo->nelems[i] + 1);
	cinfo->sumsq[i] += val*val;
	//printf("BEFORE	[%s(%d)]	%le +- %le (%le, %le) [%le])\n", cinfo->name,i,cinfo->mean[i],cinfo->std[i], cinfo->min[i], cinfo->max[i], val);
	
	cinfo->std[i] = sqrt((cinfo->nelems[i]+1)*cinfo->sumsq[i] - cinfo->mean[i])/(cinfo->nelems[i]+1);
	if(isnan(cinfo->std[i]) || isnan(cinfo->mean[i]) || isnan(cinfo->sumsq[i]) || 
		isinf(cinfo->std[i]) || isinf(cinfo->mean[i]) || isinf(cinfo->sumsq[i])){
			 cinfo->nelems[i] = 0;
	}
	
	cinfo->min[i] = cinfo->min[i] < val ? cinfo->min[i] : val;
	cinfo->max[i] = cinfo->max[i] > val ? cinfo->max[i] : val;
	//cinfo->min[i] = cinfo->mean[i]-cinfo->std[i]*cinfo->k;
	//cinfo->max[i] = cinfo->mean[i]+cinfo->std[i]*cinfo->k;
	cinfo->nelems[i]++;
	
	//start online learning from beginning
	if(isnan(cinfo->std[i]) || isnan(cinfo->mean[i]) || isnan(cinfo->sumsq[i]) || 
		isinf(cinfo->std[i]) || isinf(cinfo->mean[i]) || isinf(cinfo->sumsq[i])){
			 cinfo->nelems[i] = 1;
			 cinfo->mean[i] = val;
			 cinfo->std[i] = 0;
			 cinfo->sumsq[i] = val*val;
			 cinfo->min[i] = val;
			 cinfo->max[i] = val;
			 //cinfo->min[i] = cinfo->mean[i]-cinfo->std[i]*cinfo->k;
			 //cinfo->max[i] = cinfo->mean[i]+cinfo->std[i]*cinfo->k;
	}
	//printf("UPDATE	[%s(%d)] %d	%le +- %le (%le, %le) [%le])\n",  cinfo->name,i,cinfo->k, cinfo->mean[i],cinfo->std[i], cinfo->min[i], cinfo->max[i], cinfo->sumsq[i]);
	
	//printf("mean: %le, sumsq: %le, std: %le | min: %le, max:%le\n", 
	//	cinfo->mean[i], cinfo->sumsq[i], cinfo->std[i]);
	
	
	
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



