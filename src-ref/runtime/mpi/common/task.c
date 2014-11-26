#include "task.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

void task_report_error(const char * c){
	fprintf(stderr, "ERROR (task.c): %s", c);
	exit(1);
}

void task_init_task(task_message * t, int size){
	t->data = (char *) malloc(size+sizeof(task_message));
}
void task_destroy_task(task_message * t){
	free(t->data);
}
void task_pack_task(task_message * e, task_data_accessor * d, int task_id, int task_rank){
	task_repack_task(e,task_id,task_rank);
	d->data = e->data;
	d->offset = sizeof(task_message);
	d->size = 0;
	d->max_size=-1;
}
void task_repack_task(task_message * e, int task_id, int task_rank){
	task_message * v = (task_message *) e->data;
	e->task_id = task_id;
	e->task_rank = task_rank;
	v[0] = *e;
}
void task_pack_data_header(data_message * msg, hdr_data_accessor * acc, int n){
	msg->trans_type = TOPAZ_TRANSACTION_DATA_HEADER;
	msg->nelems = n;
	acc->data = msg->elems;
	acc->offset = 0;
	acc->size=0;
	acc->max_size=n;
}
void task_pack_data_header_elem(hdr_data_accessor * d, topaz_ptr p, int size){
	int i = d->size;
	if(i < d->max_size){
		d->data[i].size = size;
		d->data[i].ptr = p;
		d->size++;
		d->offset++;
	}
	else{
		task_report_error("trying to pack too many sends into header");
	}
}

void task_pack_int(task_data_accessor * d, int v){
	int * ptr = (int *) (&d->data[d->offset]);
	ptr[0] = v;
	d->offset += sizeof(int);
	d->size += sizeof(int);
}
void task_pack_char(task_data_accessor * d, char v){
	char * ptr = (char *) (&d->data[d->offset]);
	ptr[0] = v;
	d->offset += sizeof(char);
	d->size += sizeof(char);
}
void task_pack_double(task_data_accessor * d, double v){
	double * ptr = (double *) (&d->data[d->offset]);
	ptr[0] = v;
	d->offset += sizeof(double);
	d->size += sizeof(double);
}

void task_pack_float(task_data_accessor * d, float v){
	float * ptr = (float *) (&d->data[d->offset]);
	ptr[0] = v;
	d->offset += sizeof(float);
	d->size += sizeof(float);
}
void task_pack_ptr(task_data_accessor * d, topaz_ptr v){
	topaz_ptr * ptr = (topaz_ptr *) (&d->data[d->offset]);
	ptr[0] = v;
	d->offset += sizeof(topaz_ptr);
	d->size += sizeof(topaz_ptr);
}

void task_pack_int_array(task_data_accessor * d, const int * v, int siz){
	int * ptr = (int *) (&d->data[d->offset]);
	ptr[0] = siz;
	d->offset += sizeof(int);
	memcpy(&d->data[d->offset], v, siz*sizeof(int));
	d->offset += sizeof(int)*siz;
}
void task_pack_char_array(task_data_accessor * d, const char * v, int siz){
	int * ptr = (int *) (&d->data[d->offset]);
	ptr[0] = siz;
	d->offset += sizeof(int);
	memcpy(&d->data[d->offset], v, siz*sizeof(char));
	d->offset += sizeof(char)*siz;
}
void task_pack_double_array(task_data_accessor * d, const double * v, int siz){
	int * ptr = (int *) (&d->data[d->offset]);
	ptr[0] = siz;
	d->offset += sizeof(int);
	memcpy(&d->data[d->offset], v, siz*sizeof(double));
	d->offset += sizeof(double)*siz;
}
void task_pack_float_array(task_data_accessor * d, const float * v, int siz){
	int * ptr = (int *) (&d->data[d->offset]);
	ptr[0] = siz;
	d->offset += sizeof(int);
	memcpy(&d->data[d->offset], v, siz*sizeof(float));
	d->offset += sizeof(float)*siz;
}
void task_pack_ptr_array(task_data_accessor * d, const topaz_ptr * p, int siz){
	int * ptr = (int *) (&d->data[d->offset]);
	ptr[0] = siz;
	d->offset += sizeof(int);
	memcpy(&d->data[d->offset], p, siz*sizeof(topaz_ptr));
	d->offset += sizeof(topaz_ptr)*siz;
}

void task_unpack_task(task_message * e, task_data_accessor * d){
	d->data = e->data;
	d->offset = sizeof(task_message);
	d->size = 0;
	d->max_size=-1;
}
int task_unpack_int(task_data_accessor * d){
	int v;
	int * ptr = (int *) (&d->data[d->offset]);
	v = ptr[0];
	d->offset += sizeof(int);
	d->size += sizeof(int);
	return v;
}
char task_unpack_char(task_data_accessor * d){
	char v;
	char * ptr = (char *) (&d->data[d->offset]);
	v = ptr[0];
	d->offset += sizeof(char);
	d->size += sizeof(char);
	return v;
}

float task_unpack_float(task_data_accessor * d){
	float v;
	float * ptr = (float *) (&d->data[d->offset]);
	v = ptr[0];
	d->offset += sizeof(float);
	d->size += sizeof(float);
	return v;
}

double task_unpack_double(task_data_accessor * d){
	double v;
	double * ptr = (double *) (&d->data[d->offset]);
	v = ptr[0];
	d->offset += sizeof(double);
	d->size += sizeof(double);
	return v;
}

topaz_ptr task_unpack_ptr(task_data_accessor * d){
	topaz_ptr v;
	topaz_ptr * ptr = (topaz_ptr *) (&d->data[d->offset]);
	v = ptr[0];
	d->offset += sizeof(topaz_ptr);
	d->size += sizeof(topaz_ptr);
	return v;
}

void task_unpack_int_array(task_data_accessor * d, int * v){
	int * ptr; int siz;
	siz=task_quick_unpack_int_array(d,&ptr);
	memcpy(v, ptr, siz*sizeof(int));
}
void task_unpack_char_array(task_data_accessor * d, char * v){
	char * ptr; int siz;
	siz=task_quick_unpack_char_array(d,&ptr);
	memcpy(v, ptr, siz*sizeof(char));
}
void task_unpack_double_array(task_data_accessor * d, double * v){
	double * ptr; int siz;
	siz=task_quick_unpack_double_array(d,&ptr);
	memcpy(v, ptr, siz*sizeof(double));
}
void task_unpack_float_array(task_data_accessor * d, float * v){
	float * ptr; int siz;
	siz=task_quick_unpack_float_array(d,&ptr);
	memcpy(v, ptr, siz*sizeof(float));
}
void task_unpack_ptr_array(task_data_accessor * d, topaz_ptr * v){
	topaz_ptr * ptr; int siz;
	siz=task_quick_unpack_ptr_array(d,&ptr);
	memcpy(v, ptr, siz*sizeof(topaz_ptr));
}

int task_quick_unpack_int_array(task_data_accessor * d, int ** store){
	int * ptr = (int *) (&d->data[d->offset]);
	int siz = ptr[0];
	d->offset += sizeof(int);
	*store = (int *) &d->data[d->offset];
	d->offset += sizeof(int)*siz;
	return siz;
}
int task_quick_unpack_char_array(task_data_accessor * d, char ** store){
	int * ptr = (int *) (&d->data[d->offset]);
	int siz = ptr[0];
	d->offset += sizeof(int);
	*store = (char *) &d->data[d->offset];
	d->offset += sizeof(char)*siz;
	return siz;	
}
int task_quick_unpack_double_array(task_data_accessor * d, double ** store){
	int * ptr = (int *) (&d->data[d->offset]);
	int siz = ptr[0];
	d->offset += sizeof(int);
	*store = (double *) &d->data[d->offset];
	d->offset += sizeof(double)*siz;
	return siz;	
}
int task_quick_unpack_float_array(task_data_accessor * d, float ** store){
	int * ptr = (int *) (&d->data[d->offset]);
	int siz = ptr[0];
	d->offset += sizeof(int);
	*store = (float *) &d->data[d->offset];
	d->offset += sizeof(float)*siz;
	return siz;
}
int task_quick_unpack_ptr_array(task_data_accessor * d, topaz_ptr ** store){
	int * ptr = (int *) (&d->data[d->offset]);
	int siz = ptr[0];
	d->offset += sizeof(int);
	*store = (topaz_ptr *) &d->data[d->offset];
	d->offset += sizeof(topaz_ptr)*siz;
	return siz;
}
