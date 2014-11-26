#include "topaz.h"
#include "test.h"
#include "task.h"

#include "stdio.h"
#include "stdlib.h"
TaskSpec ts;
Task in;
Task out;
void test_task(Topaz * tpz, Task * tin, Task * tout){
	char op;
	float a, b;
	int i;
	int id;
	tpz->unpack_inputs(&id, &i, &op, &a, &b);
	float res = 0;
	switch(op){
		case '+':
			res = a+b; break;
		case '-':
			res = a-b; break;
		case '*':
			res = a*b; break;
		case '/':
			res = a/b; break;
	}
	tpz->pack_outputs(id, i, res);
}

int nelems=15;
float * in1;
float * in2;
float * res;

void execute_task_remote(Topaz * tpz){
	float res;
	int id, rank;
	tpz->send(0, 15, '+', 3.2, 2.1);
	tpz->receive(&id, &rank, &res);
	printf("[%d,%d] Answer %f\n",id,rank, res);
}

int topaz_init(Topaz * tpz, int argc, char ** argv){
	printf("main-init\n");
	ts.setFunction(test_task, 3, 1);
	ts.addInput(0,TaskArgSpec::create(CHAR, 1));
	ts.addInput(1,TaskArgSpec::create(FLOAT, 1));
	ts.addInput(2,TaskArgSpec::create(FLOAT, 1));
	ts.addOutput(0,TaskArgSpec::create(FLOAT, 1));
	tpz->add(ts);
	in.update(0,0,ts.getInputBufferSize());
	out.update(0,0,ts.getOutputBufferSize());
	printf("%d, %d\n", ts.getInputBufferSize(), ts.getOutputBufferSize());
	return 0;
}

int topaz_finish(Topaz * tpz, int argc, char ** argv){
	printf("main-finish\n");
	return 0;
}


int topaz_main(Topaz * tpz, int argc, char ** argv){
	in1 = (float *) malloc(sizeof(float)*nelems);
	in2 = (float *) malloc(sizeof(float)*nelems);
	res = (float *) malloc(sizeof(float)*nelems);
	for(int i=0; i < nelems; i++){
		in1[i] = i;
		in2[i] = i/2*1.843;
	}
	//main task body
	for(int i=0; i < nelems; i++){
		int id, rank;
		tpz->send(0, i, '+', in1[i], in2[i]);
		tpz->receive(&id, &rank, &res[i]);
	}
	
	
	for(int i=0; i < nelems; i++){
		printf("%f\n", res[i]);
	}
	free(in1);
	free(in2);
	free(res);
	return 0;
}

int topaz_worker(Topaz * tpz, int argc, char ** argv){
	printf("worker\n");
	return 0;
}
