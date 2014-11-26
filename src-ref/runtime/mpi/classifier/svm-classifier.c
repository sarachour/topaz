#include "svm.h"
#include "stdio.h"
#include "stdlib.h"
#include "const.h"
#include "classifier.h"

classifier_info ci;

void cls_init(classifier_info * cinfo, const char * name, int place, int size){
		char filename[512];
		int i;
		int * labels;
		sprintf(filename, "logs/%s.%d.cls", name, place, size);
		cinfo->model = svm_load_model(filename);
		if(cinfo->model == NULL){
			printf("Classifier %s not found\n", filename);
			cinfo->initialized = false;
			return;
		}
		printf("Classifier %s found\n", filename);
		cinfo->n_cls=svm_get_nr_class(cinfo->model);
		cinfo->n_attr=size;
		labels=(int *) malloc(cinfo->n_cls*sizeof(int));
		
		cinfo->data = malloc(sizeof(struct svm_node)*(size+1));
		cinfo->probs = malloc(sizeof(double)*cinfo->n_cls);
		for(i=0; i < size; i++){
			cinfo->data[i].index = i+1;
		}
		cinfo->data[i].index = -1;
		svm_get_labels(cinfo->model,labels);
		for(i=0; i < cinfo->n_cls; i++){
			if(labels[i] >= 0){
				cinfo->pidx = i;
			}
			else{
				cinfo->nidx = i;
			}
		}
		free(labels);
		cinfo->initialized = true;
		
}
void cls_print(classifier_info * cinfo){
	printf("Classifier\n");
}
void cls_destroy(classifier_info * cinfo){
		svm_free_and_destroy_model(&cinfo->model);
		free(cinfo->probs);
		free(cinfo->data);
}
int __cls_predict_base(classifier_info * cinfo, double * confidence){
	int i;
	double prediction;
	if(cinfo->initialized == false){
		return true;
	}
	prediction = svm_predict_probability(cinfo->model, cinfo->data, cinfo->probs);
	//prediction = svm_predict(cinfo->model, cinfo->data);
	
	if(prediction >= 0){
		*confidence = cinfo->probs[cinfo->pidx];
	}
	else{
		*confidence = cinfo->probs[cinfo->nidx];
	}
	printf("confidence %f\n", *confidence);
	if(prediction < 0 && *confidence < 0.5){
		prediction = 1;
	}
	/*
	printf("prediction: %s (%f), confidence: %f, data: [", prediction >= 0 ? "true":"false", prediction, *confidence);
	for(i=0; i < cinfo->n_attr; i++){
		printf("%f ", cinfo->data[i].value);
	}
	printf("]\n");
	*/
	return (prediction >= 0 ? true : false);
}
struct svm_node * __cls_draw_point(classifier_info * cinfo, int * svm_node_len){
	if(cinfo->initialized == false) return NULL;
	int n_support_vect = cinfo->model->l;
	int i;
	struct svm_node * cnode;
	printf("number of support vectors: %d\n", n_support_vect);
	double pred;
	do{
		i=rand()%n_support_vect;
		printf("draw %d\n", i);
		cnode = cinfo->model->SV[i]; //support vectors
		pred= svm_predict(cinfo->model, cnode);
		printf("pred %f (%d)\n", pred, cinfo->nidx);
	}while(pred < 0);
	
	int cnodelen = cinfo->model->nSV[i];
	*svm_node_len = cinfo->n_attr;
	return cnode;
}
void cls_draw_int(classifier_info * cinfo, int * data){
	int i;
	int len;
	struct svm_node * c = __cls_draw_point(cinfo, &len);
	if(c == NULL) return;
	for(i=0; i < len; i++){
		data[c[i].index-1] = (int) c[i].value;
	}
}
void cls_draw_float(classifier_info * cinfo, float * data){
	int i;
	int len;
	struct svm_node * c = __cls_draw_point(cinfo, &len);
	if(c == NULL) return;
	printf("draw: ");
	for(i=0; i < len; i++){
		data[c[i].index-1] = (float) c[i].value;
		printf("%f ", data[c[i].index-1]);
	}
	printf("\n");
}

void cls_draw_double(classifier_info * cinfo, double * data){
	int i;
	int len;
	struct svm_node * c = __cls_draw_point(cinfo, &len);
	if(c == NULL) return;
	for(i=0; i < len; i++){
		data[c[i].index-1] = (double) c[i].value;
	}
}

int cls_predict_int(classifier_info * cinfo, int * d, double * confidence){
	if(cinfo->initialized == false) return 1;
	int i;
	double prediction;
	for(i=0; i < cinfo->n_attr; i++){
		cinfo->data[i].value = (double) d[i];
		cinfo->data[i].index = i+1;
	}
	cinfo->data[i].index = -1;
	return __cls_predict_base(cinfo, confidence);
}

int cls_predict_double(classifier_info * cinfo, double * d, double * confidence){
	if(cinfo->initialized == false) return 1;
	int i;
	double prediction;
	for(i=0; i < cinfo->n_attr; i++){
		cinfo->data[i].value = (double) d[i];
		cinfo->data[i].index = i+1;
	}
	cinfo->data[i].index = -1;
	return __cls_predict_base(cinfo, confidence);
}

int cls_predict_char(classifier_info * cinfo, char * d, double * confidence){
	if(cinfo->initialized == false) return 1;
	int i;
	double prediction;
	for(i=0; i < cinfo->n_attr; i++){
		cinfo->data[i].value = (double) d[i];
		cinfo->data[i].index = i+1;
	}
	cinfo->data[i].index = -1;
	return __cls_predict_base(cinfo, confidence);
}

int cls_predict_float(classifier_info * cinfo, float * d, double * confidence){
	if(cinfo->initialized == false) return 1;
	int i;
	double prediction;
	for(i=0; i < cinfo->n_attr; i++){
		cinfo->data[i].value = (double) d[i];
		cinfo->data[i].index = i+1;
	}
	cinfo->data[i].index = -1;
	return __cls_predict_base(cinfo, confidence);
}
/*
void main(){
	int v = 3;
	double conf;
	int accept;
	cls_init(&ci, "out.lsum", 1, 1);
	accept = cls_predict_int(&ci, &v, &conf);
	if(accept){
		printf("accept: %f\n", conf);
	}
	else{
		printf("reject: %f\n", conf);
	}
}
*/


