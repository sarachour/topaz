#include "fault.h"
#include "timer.h"
#include "random.h"
#include "mpi.h"
#include "stdio.h"
#include "const.h"

void fault_init_fm(fault_info * f){
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	char mtype = get_fault_type(rank-1);
	f->model = get_fault_model(mtype);
	rand_seed();
	timer_init(&f->run_time);
	timer_start(&f->run_time);
}
char get_fault_type(int rank){
	const char * types = "pppp";
	return types[rank];
}
fault_model get_fault_model(char c){
	fault_model fm;
	switch(c){
		case 'c':
			fm.timeout_prob_base = 0.001;
			fm.timeout_prob_grow = 0.00001;
			fm.data_corruption_rate = 0;
			break;
		case 'p':
			fm.timeout_prob_base = 0;
			fm.timeout_prob_grow = 0;
			fm.data_corruption_rate = 0;
			break;
		case 'x':
			fm.timeout_prob_base = 0;
			fm.timeout_prob_grow = 0;
			fm.data_corruption_rate = 0;
			break;
		case 'd':
			fm.timeout_prob_base = 0;
			fm.timeout_prob_grow = 0;
			fm.data_corruption_rate = 0.99;
			break;
		
	}
	return fm;
}
void fault_corrupt_task_message(fault_info * f,task_message * m, int size){
	int i;
	int cnt=0;
	float prob = f->model.data_corruption_rate;
	size += sizeof(task_message);
	for(i=0; i < size; i++){
		if(rand_norm_float() <= prob){
			m->data[i] = rand_char();
			cnt++;
			
		}
	}
	#ifdef DEBUG
	fprintf(stderr, "	++ %d Faults Injected\n", cnt);
	#endif
}

int fault_run_time_fail(fault_info * f){
	float elapsed, prob;
	timer_end(&f->run_time);
	elapsed = timer_get_msecs(&f->run_time);
	prob = f->model.timeout_prob_base + f->model.timeout_prob_grow*elapsed/1000.0;
	timer_start(&f->run_time);
	//rng with prob p.
	if(rand_norm_float() < prob){
		return true;
	}
	else{
		return false;
	}
	
}
