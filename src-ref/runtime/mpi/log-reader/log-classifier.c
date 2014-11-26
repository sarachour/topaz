
#include "stdio.h"
#include "stdlib.h"
#include "logger.h"
#include "classifier.h"
#include "math.h"


int cls_get_esize(entry_type type){
	int esize;
	switch(type){
		case ENTRY_TYPE_CHAR:
			esize = sizeof(char);
			break;
		case ENTRY_TYPE_INT:
			esize = sizeof(int);
			break;
		case ENTRY_TYPE_FLOAT:
			esize = sizeof(float);
			break;
		case ENTRY_TYPE_DOUBLE:
			esize = sizeof(double);
			break;
	}
	return esize;
}
void cls_read_existing_logfile(log_info * l, FILE * file){
	int nentries = -1, i;
	printf("Beginning read\n");
	fread(&nentries, sizeof(int), 1, file);
	fprintf(stderr, "LOG[READ]: number of entries %d\n", nentries);
	l->nentries = nentries;
	l->entries = (log_entry *) malloc(sizeof(log_entry)*nentries);
	for(i=0; i < nentries; i++){
		fread(l->entries[i].name, sizeof(char), NAME_LEN, file);
		fread(&l->entries[i].type, sizeof(int), 1, file);
		fread(&l->entries[i].size, sizeof(int), 1, file);
		l->entries[i].history = malloc(cls_get_esize(l->entries[i].type)*l->entries[i].size*LOG_BUFFER_SIZE);
		fprintf(stderr, "LOG[READ]: entry [%s, type=%d, size=%d] discovered\n", 
			l->entries[i].name, l->entries[i].type, l->entries[i].size);
		
	}
	l->idx = nentries;
	#ifdef DEBUG
	fprintf(stderr, "LOG[READ]: done\n");
	#endif
}
double pct_diff(double * a, double * b, int siz){
	int i;
	double sqdiff, sq;
	sqdiff = sq = 0;
	for(i=0; i < siz; i++){
		sqdiff+=(a[i]-b[i])*(a[i]-b[i]);
		sq +=a[i]*a[i];
	}
	return (sqrt(sqdiff));
}
int main(int argc, char ** argv){
	char line[2048];
	int i;
	if(argc < 5){
		printf("USAGE: log-classifier field_name place nelems/entry pct-tolerance\n");
		exit(1);
	}
	char * clsname = argv[1];
	int place = atoi(argv[2]);
	int size = atoi(argv[3]);
	double ptol = atof(argv[4]);
	

	classifier_info c;
	log_info l;
	printf("Initializing Classifier %s @ %d with %d\n", clsname, place, size);
	cls_init(&c, clsname, place, size);
	double * answer = (double*) malloc(sizeof(double)*size);
	double * value = (double*) malloc(sizeof(double)*size);
	int flag;
	int true_pos=0;
	int false_pos=0;
	int true_neg=0;
	int false_neg=0;
	do{
		for(i=0; i < size-1; i++){
			flag = scanf("%le,", &value[i]);
		}
		flag = scanf("%le	", &value[i]);
		for(i=0; i < size-1; i++){
			flag = scanf("%le,", &answer[i]);
		}
		flag = scanf("%le\n", &answer[i]);
		//predict
		int prediction = cls_predict_double(&c, value, NULL);
		double pdiff = pct_diff(answer, value, size);
		if(prediction == false){
			if(pdiff < ptol){
				false_pos++;
			}
			else{
				true_pos++;
			}
			/*
			for(i=0; i < size; i++){
				printf("%le,", value[i]);
			}
			printf("	");
			for(i=0; i < size; i++){
				printf("%le,", answer[i]);
			}
			printf("	p:%f\n", pdiff);
			*/
		}
		else{
			if(pdiff < ptol){
				true_neg++;
			}
			else{
				false_neg++;
			}
			
		}
	}while(flag > 0);
	double total = true_neg+false_neg+true_pos+false_pos;
	double total_pos = true_pos + false_pos;
	double total_neg = true_neg + false_neg;
	printf("Classification	Number Elems	Relative Frac	Cumulative Frac\n");
	printf("true-outl	%d	%f	%f\n", true_pos, ((double)true_pos)/total_pos*100, ((double)true_pos)/total*100);
	printf("fals-outl	%d	%f	%f\n", true_pos, ((double)false_pos)/total_pos*100, ((double)false_pos)/total*100);
	printf("true-ok	%d	%f	%f\n", true_neg, ((double)true_neg)/total_neg*100, ((double)true_neg)/total*100);
	printf("fals-ok	%d	%f	%f\n", false_neg, ((double)false_neg)/total_neg*100, ((double)false_neg)/total*100);
}
