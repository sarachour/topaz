#include "topaz.h"
#include "test.h"
#include "task.h"

#include "stdio.h"
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
	printf("remote execution\n");
	execute_task_remote(tpz);
	return 0;
}

int topaz_worker(Topaz * tpz, int argc, char ** argv){
	printf("worker\n");
	return 0;
}
