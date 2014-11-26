#include "svm.h"
#include "stdio.h"
#include "stdlib.h"

struct svm_model * model;
void main(){
	double prediction;
	int i;
	printf("loading model\n");
	model = svm_load_model("out.lsum.1.cls");
	if(model == 0){
		printf("Failed to load model file\n");
		exit(1);
	}
	if(svm_check_probability_model(model)==0)
	{
		fprintf(stderr,"Model does not support probabiliy estimates\n");
		exit(1);
	}
	
	int svm_type=svm_get_svm_type(model);
	int nr_class=svm_get_nr_class(model);
	int nr_attr =1;
	double *prob_estimates=NULL;
	struct svm_node * x;
	printf("There are %d classes and %d attributes\n", nr_class, nr_attr);
	prob_estimates = (double *) malloc(nr_class*sizeof(double));
	x = (struct svm_node *) malloc(nr_attr*sizeof(struct svm_node));
	x[0].index = 1;
	x[0].value = 4;
	printf("predicting prob model\n");
	prediction = svm_predict_probability(model, x, prob_estimates);
	
	int *labels=(int *) malloc(nr_class*sizeof(int));
	svm_get_labels(model,labels);
	
	printf("Predicted %f\n", prediction);
	for(i=0; i < nr_class; i++){
		printf("Class[%d]  Prob %f\n", labels[i], prob_estimates[i]);
	}
  free(labels);
	svm_free_and_destroy_model(&model);
		
}


