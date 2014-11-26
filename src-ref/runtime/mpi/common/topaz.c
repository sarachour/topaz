#include "topaz.h"
#include "mem.h"
#include "mpi.h"
#include "stdlib.h"
#include "stdio.h"
#include "const.h"
#include "logger.h"

#include <stdarg.h>
#define DATA_HEADER_ID 1
#define TASK_ID 2
#define RET_TASK_ID 3
#define KILL_PROC_ID 4
#define FAILED_TASK_ID 5
#define DATA_BODY_ID 6

#ifdef TIME_PROGRAM
#include "timeutil.h"
#endif


void __init_topaz_config(topaz_info * t, int argc, char ** argv){
	char * envvar = getenv("TOPAZ_ENABLE_LOGGING");
	int en_logging = envvar == NULL ? 0 : atoi(envvar);
	if(t->machine.rank == t->machine.main_proc){
		printf("logging enabled? %d\n", en_logging);
		t->config.en_logging = en_logging == 1 ? true : false;
	}
	else{
		t->config.en_logging = false;
	}
}

void __topaz_main_thrd_invoke_task(topaz_info * topaz, task_message * in, task_message * out){
	int task_id = in->task_id;
	int task_rank = in->task_rank;
	fxnptr task_fxn = topaz_get_task(topaz, task_id);
	task_fxn(topaz, in, out);
	//rank and id should not change.
	out->task_rank = task_rank;
	out->task_id = task_id;
}

void init_topaz_info(topaz_info * t, int argc, char ** argv){
	int len, i;
	machine_info mach = t->machine;
	task_list tasks = t->tasks;
	mach.hostname = (char *) malloc(MPI_MAX_PROCESSOR_NAME*sizeof(char));
	MPI_Comm_rank(MPI_COMM_WORLD, &mach.rank); // rank is place index
	MPI_Comm_size(MPI_COMM_WORLD, &mach.size); // size is total number of places.
	MPI_Get_processor_name(mach.hostname, &len);
	
	
	mach.INTERNAL_last_ran_proc = 1;
	mach.nrunning=0;
	mach.procs = (proc_info *) malloc(mach.size*sizeof(proc_info));
	/*
	 * for now, assume proc 0 = main, other are unreliable workers.
	 */
	for(i=0; i < mach.size; i++){
		mach.procs[i].n_running_tasks=0;
	}
	mach.main_proc = 0;
	
	tasks.task_list = (task_info *) malloc(TOPAZ_DEFAULT_TASK_LIST_SIZE*sizeof(task_info));
	tasks.max_size = TOPAZ_DEFAULT_TASK_LIST_SIZE;
	tasks.size = 0;
	tasks.max_output_buffer = 0;
	tasks.max_input_buffer = 0;
	
	init_mem_info(&t->mem);
	t->machine = mach;
	t->tasks = tasks;
	
	__init_topaz_config(t, argc,argv);
	
}
void destroy_topaz_info(topaz_info * t){
	int i;
	free(t->machine.hostname);
	free(t->machine.procs);
	for(i=0; i < t->tasks.size; i++){
		free(t->tasks.task_list[i].inputs);
		free(t->tasks.task_list[i].outputs);
	}
	free(t->tasks.task_list);
	destroy_mem_info(&t->mem);
}
void topaz_dump(topaz_info * t){
	int i;
	for(i=0; i < t->tasks.size; i++){
		task_info * curr = &t->tasks.task_list[i];
		int j;
		for(j=0; j < curr->out_nargs; j++){
			topaz_cls_print(t, i, j);
		}
	}
}
void topaz_finish(topaz_info * t){
	int i;
	char c = 't';
	for(i=0; i < t->machine.size; i++){
		if(t->machine.main_proc != i){
			MPI_Send(&c, 1, MPI_CHAR, i, KILL_PROC_ID, MPI_COMM_WORLD);
		}
	}
	if(t->config.en_logging == true){
		for(i=0; i < t->tasks.size; i++){
			task_info * curr = &t->tasks.task_list[i];
			log_write_out_entries(&curr->log);
			log_destroy(&curr->log);
		}
	}
	destroy_topaz_info(t);
}

int topaz_add_task(topaz_info * t, fxnptr v, int nargs_in, int nargs_out, ...){
	int nbytes_in=0, nbytes_out=0, i, idx;
	int global_ob = t->tasks.max_output_buffer;
	int global_ib = t->tasks.max_input_buffer;
	va_list arguments; 
	task_arg_type ttype; int size;
	task_info info;
	
	idx = t->tasks.size;
	
	info.fxn = v;
	/* Initializing arguments to store all values after num */
  va_start ( arguments, nargs_out );  
	//check if we are going out of bounds
	if(t->tasks.size >= t->tasks.max_size){
		fprintf(stderr, "ERROR (topaz.c): Adding task failed. task list does not support reallocation.");
		exit(1);
	}
	info.in_nargs = nargs_in;
	info.out_nargs = nargs_out;
	info.inputs = (task_info_arg *) malloc(sizeof(task_info_arg)*nargs_in);
	info.outputs = (task_info_arg *) malloc(sizeof(task_info_arg)*nargs_out);
	info.n_rexec_calls= 0;
	info.n_calls = 0;
	if(t->config.en_logging) log_init_task(&info);
	for(i=0; i < nargs_in; i++){
		ttype = va_arg ( arguments, task_arg_type ); 
		size = va_arg ( arguments, unsigned int ); 
		info.inputs[i].type = ttype;
		info.inputs[i].size = size;
		//printf("Adding input [%d]: type: %d, size: %d\n", i, ttype, size);
		switch(ttype){
			case TASK_ARG_TYPE_CHAR:
				nbytes_in += sizeof(char)*size + (size > 1 ? sizeof(int) : 0);
				break;
			case TASK_ARG_TYPE_FLOAT:
				nbytes_in += sizeof(float)*size + (size > 1 ? sizeof(int) : 0);
				break;
			case TASK_ARG_TYPE_DOUBLE:
				nbytes_in += sizeof(double)*size + (size > 1 ? sizeof(int) : 0);
				break;
			case TASK_ARG_TYPE_INT:
				nbytes_in += sizeof(int)*size + (size > 1 ? sizeof(int) : 0);
				break;
			case TASK_ARG_TYPE_PTR:
				nbytes_in += sizeof(topaz_ptr)*size + (size > 1 ? sizeof(int) : 0);
				break;
		}
	}
	for(i=0; i < nargs_out; i++){
		ttype = va_arg ( arguments, task_arg_type ); 
		size = va_arg ( arguments, unsigned int ); 
		info.outputs[i].type = ttype;
		info.outputs[i].size = size;
		info.outputs[i].autocheckers = NULL;
		//printf("Adding output [%d]: type: %d, size: %d\n", i, ttype, size);
		switch(ttype){
			case TASK_ARG_TYPE_CHAR:
				nbytes_out += sizeof(char)*size + (size > 1 ? sizeof(int) : 0);
				break;
			case TASK_ARG_TYPE_FLOAT:
				nbytes_out += sizeof(float)*size + (size > 1 ? sizeof(int) : 0);
				break;
			case TASK_ARG_TYPE_DOUBLE:
				nbytes_out += sizeof(double)*size + (size > 1 ? sizeof(int) : 0);
				break;
			case TASK_ARG_TYPE_INT:
				nbytes_out += sizeof(int)*size + (size > 1 ? sizeof(int) : 0);
				break;
			case TASK_ARG_TYPE_PTR:
				nbytes_out += sizeof(topaz_ptr)*size + (size > 1 ? sizeof(int) : 0);
				break;
		}
	}
	info.ibuf_siz = nbytes_in;
	info.obuf_siz = nbytes_out;
	
	
	t->tasks.task_list[idx] = info;
	t->tasks.size ++;

	t->tasks.max_output_buffer = global_ob > info.obuf_siz ? global_ob : info.obuf_siz; 
	t->tasks.max_input_buffer = global_ib > info.ibuf_siz ? global_ib : info.ibuf_siz; 

	return idx;
}
fxnptr topaz_get_task(topaz_info * t, int task_id){
	task_list ts = t->tasks;
	if(task_id < ts.size && task_id >= 0){
		return ts.task_list[task_id].fxn;
	}
	else{
		fprintf(stderr, "ERROR (topaz.c): task of task-id %d doesn't exist\n", task_id);
		exit(1);
	}
}
int get_task_id(topaz_info * t, void * task){
	task_list ts = t->tasks;
	int i;
	for(i=0; i < ts.size; i++){
		if(ts.task_list[i].fxn == task){
			return i;
		}
	}
	fprintf(stderr, "ERROR (topaz.c): task get failed. no task exists in task map.");
	exit(1);
}

void topaz_log_task(topaz_info * t, int task_id, const char * handle){
	if(t->config.en_logging == true) log_task(&t->tasks.task_list[task_id], handle);
}
void topaz_log_input(topaz_info * t, int task_id, int arg_id, const char * handle){
	if(t->config.en_logging == true) log_task_input(&t->tasks.task_list[task_id], arg_id, handle);
}
void topaz_log_output(topaz_info * t, int task_id, int arg_id, const char * handle){
	if(t->config.en_logging == true) log_task_output(&t->tasks.task_list[task_id], arg_id, handle);
}

int topaz_alloc(topaz_info * t, int size){
	return mem_alloc(&t->mem, size);
}
void topaz_free(topaz_info * t, topaz_ptr p){
	mem_free(&t->mem, p);
}
void * topaz_deref(topaz_info * t, topaz_ptr p){
	return mem_deref(&t->mem, p);
}
int topaz_memsize(topaz_info * t, topaz_ptr p){
	return mem_size(&t->mem, p);
}
void topaz_memdump(topaz_info * t){
	mem_dump(&t->mem);
}
void topaz_memstore(topaz_info * t, topaz_ptr p, void * m, int size){
	mem_store(&t->mem,p,m,size);
}
int get_proc_to_run_on(topaz_info * t){
	int nextproc;
	if(t->config.en_logging)
		nextproc = (t->machine.INTERNAL_last_ran_proc+1)%(t->machine.size-2)+1;
	else
		nextproc = (t->machine.INTERNAL_last_ran_proc+1)%(t->machine.size-1)+1;
	t->machine.INTERNAL_last_ran_proc = nextproc;
	return nextproc;
}
void topaz_send_data_header(topaz_info * t, int place, data_message * e){
	MPI_Send((char*) e, sizeof(data_message) /*count*/, MPI_CHAR, 
			  place /*dest*/, DATA_HEADER_ID/*tag*/, MPI_COMM_WORLD);
}
void topaz_send_data(topaz_info * t, int place, topaz_ptr p){
	int siz = topaz_memsize(t,p);
	void * ptr = topaz_deref(t,p);
	#ifdef TIME_PROGRAM
	MCommStart();
	#endif
	MPI_Send((char*) ptr, siz /*count*/, MPI_CHAR, 
			  place /*dest*/, DATA_BODY_ID+p/*tag*/, MPI_COMM_WORLD);
	#ifdef TIME_PROGRAM
	MCommStop();
	#endif
}	

void topaz_send_data_header_to_all(topaz_info * t, data_message * e){
	int i;
	for(i=0; i < t->machine.size; i++){
		if(i != t->machine.main_proc){
			topaz_send_data_header(t,i,e);
		}
	}
}
void topaz_send_data_to_all(topaz_info * t, topaz_ptr p){
	int i;
	for(i=0; i < t->machine.size; i++){
		if(i != t->machine.main_proc){
			topaz_send_data(t,i,p);
		}
	}
}

typedef struct TM_NODE task_message_node;
unsigned int NODE_IID=0;
struct TM_NODE{
	task_message tm;
	task_message_node * next;
	int place;
	int iid;
};



task_message_node * __SOLN_HEAD = NULL;
void topaz_send_task(topaz_info * t, int place, task_message * e){
	int itask_id = (int) e->task_id;

	t->machine.procs[place].n_running_tasks++;	
	t->machine.nrunning++;	
	#ifdef TIME_PROGRAM
	MCommStart();
	#endif
	MPI_Send((char*) e->data, sizeof(task_message)+t->tasks.task_list[itask_id].ibuf_siz /*count*/, MPI_CHAR, 
			  place /*dest*/, TASK_ID/*tag*/, MPI_COMM_WORLD);
	#ifdef TIME_PROGRAM
	MCommStop();
	#endif
	//send a secondary one
	if(t->config.en_logging){
		//send same task to oracle.
		MPI_Send((char*) e->data, sizeof(task_message)+t->tasks.task_list[itask_id].ibuf_siz /*count*/, MPI_CHAR, 
			  t->machine.size-1 /*dest*/, TASK_ID/*tag*/, MPI_COMM_WORLD);
	}
}


void topaz_send_task_to_any(topaz_info * t, task_message * e){
	int nextproc = get_proc_to_run_on(t);
	topaz_send_task(t,nextproc,e);
}
#include "string.h"
void __topaz_log_recvd_task(topaz_info *t, const task_message * e, int place){
	task_message_node * curr = __SOLN_HEAD;
	task_message_node * prev = NULL;
	int itask_id = (int) e->task_id;
	int itask_rank = (int) e->task_rank;
	int oracle = t->machine.size-1;
	while(curr != NULL){
		if(itask_id == curr->tm.task_id && itask_rank == curr->tm.task_rank){
			if(place == oracle){
				log_task_learning(&t->tasks.task_list[itask_id], &curr->tm, e, curr->place);
			}
			else{
				int dsize = t->tasks.task_list[(int)e->task_id].obuf_siz+sizeof(task_message);
				log_task_learning(&t->tasks.task_list[itask_id], e, &curr->tm, place);
				memcpy(e->data, curr->tm.data, dsize);
				//printf("recieved task after oracle\n");
			}
			if(prev != NULL){
				prev->next = curr->next;
				free(curr->tm.data);
				free(curr);
				curr = prev->next;
			}
			else{
				__SOLN_HEAD = curr->next;
				free(curr->tm.data);
				free(curr);
				curr = __SOLN_HEAD;
				prev = NULL;
			}
			return;
		}
		else{
			prev = curr;
			curr = curr->next;
		}
	}
	if(t->machine.size-1 == place){
		int dsize = t->tasks.task_list[(int)e->task_id].obuf_siz+sizeof(task_message);
		task_message_node * newnode = (task_message_node*) malloc(sizeof(task_message_node));
		newnode->tm.task_id = e->task_id;
		newnode->tm.task_rank = e->task_rank;
		newnode->tm.data = (void *) malloc(dsize*sizeof(char));
		newnode->next = NULL;
		newnode->place = place;
		newnode->iid = NODE_IID++;
		memcpy(newnode->tm.data, e->data, dsize);
		if(__SOLN_HEAD == NULL){
			__SOLN_HEAD = newnode;
		}
		else{
			curr = __SOLN_HEAD;
			while(curr->next != NULL) curr = curr->next;
			curr->next = newnode;
		}
	}
}
inline int __topaz_main_check_for_errors(int place, task_message * dest, task_message * src){
	if(src->task_rank < 0 && src->task_id < 0){
		int taskinfo[2];
		MPI_Status status;
		MPI_Recv(taskinfo, 2, MPI_INT, place, FAILED_TASK_ID, MPI_COMM_WORLD, &status);
		int task_id = taskinfo[0];
		int task_rank = taskinfo[1];
		dest->task_id = task_id;
		dest->task_rank = task_rank;
		src->task_id = task_id;
		src->task_rank = task_rank;
		#ifdef DEBUG
		printf("FAILED [%d]: task=%d, rank=%d\n", place, task_id, task_rank);
		#endif
		return true;
	}
	else return false;
}
void _topaz_receive_task(topaz_info *t, int place, task_message * e){
	MPI_Status status;
	//recieve a task from oracle, log it.
	if(t->config.en_logging){
		MPI_Recv(e->data, sizeof(task_message)+t->tasks.max_output_buffer, MPI_CHAR,
		t->machine.size-1, RET_TASK_ID, MPI_COMM_WORLD, &status);
		task_message tmp = ((task_message *) e->data)[0];
		e->task_rank = tmp.task_rank;
		e->task_id = tmp.task_id;
		int is_failed = __topaz_main_check_for_errors(t->machine.size-1, e, &tmp);
		if(!is_failed) __topaz_log_recvd_task(t, e, t->machine.size-1);
	}
	#ifdef TIME_PROGRAM
	MCommStart();
	#endif
	MPI_Recv(e->data, sizeof(task_message)+t->tasks.max_output_buffer, MPI_CHAR,
		place, RET_TASK_ID, MPI_COMM_WORLD, &status);
	#ifdef TIME_PROGRAM
	MCommStop();
	#endif
	task_message tmp = ((task_message *) e->data)[0];
	e->task_rank = tmp.task_rank;
	e->task_id = tmp.task_id;
	int is_failed = __topaz_main_check_for_errors(place, e, &tmp);
	t->machine.nrunning--;
	t->machine.procs[place].n_running_tasks--;
	if(t->config.en_logging && !is_failed){
		__topaz_log_recvd_task(t, e, place);
	}
	t->tasks.task_list[tmp.task_id].n_calls++;
	//if(t->config.en_logging == true) log_task_message(&t->tasks.task_list[itask_id], e, false, false);
}

//return the place
int topaz_receive_task_from_any(topaz_info *t, task_message * e){
	int i;
	MPI_Status status;
	
	if(t->config.en_logging){
		MPI_Recv(e->data, sizeof(task_message)+t->tasks.max_output_buffer, MPI_CHAR,
		t->machine.size-1, RET_TASK_ID, MPI_COMM_WORLD, &status);
		task_message tmp = ((task_message *) e->data)[0];
		e->task_rank = tmp.task_rank;
		e->task_id = tmp.task_id;
		int is_failed = __topaz_main_check_for_errors(t->machine.size-1, e, &tmp);
		if(!is_failed) __topaz_log_recvd_task(t, e, t->machine.size-1);
	}
	#ifdef TIME_PROGRAM
	MCommStart();
	#endif
	MPI_Recv(e->data, sizeof(task_message)+t->tasks.max_output_buffer, MPI_CHAR,
		MPI_ANY_SOURCE, RET_TASK_ID, MPI_COMM_WORLD, &status);
	#ifdef TIME_PROGRAM
	MCommStop();
	#endif
	task_message tmp = ((task_message *) e->data)[0];
	int place = status.MPI_SOURCE;
	e->task_rank = tmp.task_rank;
	e->task_id = tmp.task_id;
	int is_failed = __topaz_main_check_for_errors(place, e, &tmp);
	t->machine.nrunning--;
	t->machine.procs[place].n_running_tasks--;
	
	if(t->config.en_logging && !is_failed){
		__topaz_log_recvd_task(t, e, place);
	}
	t->tasks.task_list[tmp.task_id].n_calls++;
	return place;
	//if(t->config.en_logging == true) log_task_message(&t->tasks.task_list[itask_id], e, false, false);
}

int topaz_n_tasks_left(topaz_info *t){
	return t->machine.nrunning;
}

int topaz_reexecute(topaz_info * tpz, int task_id, task_message * in, task_message * out){
	
	__topaz_main_thrd_invoke_task(tpz, in, out);
	tpz->tasks.task_list[task_id].n_rexec_calls++;
}


task_message __WORKER_fail_tsk;
void __topaz_worker_init(){
	task_init_task(&__WORKER_fail_tsk, 0);
}
void __topaz_worker_destroy(){
	task_destroy_task(&__WORKER_fail_tsk);
}
void __topaz_worker_send_failed_task_response(topaz_info * t, int task_id, int task_rank){
	task_data_accessor da;
	int taskinfo[2];
	taskinfo[0] = task_id;
	taskinfo[1] = task_rank;
	//initialize error message
	task_pack_task(&__WORKER_fail_tsk, &da, -1, -1);
	MPI_Send(&__WORKER_fail_tsk, sizeof(task_message), MPI_CHAR, 0, RET_TASK_ID, MPI_COMM_WORLD);
	MPI_Send(&taskinfo, 2, MPI_INT, 0, FAILED_TASK_ID, MPI_COMM_WORLD);
}
/*for worker*/
int __topaz_worker_try_receive_data_header(topaz_info * t, data_message * dm, MPI_Request * mr){
	//TODO: topaz->machine.main_proc var instead of is_main mem var
	MPI_Irecv((char *) dm, sizeof(data_message), MPI_CHAR, 0, DATA_HEADER_ID, MPI_COMM_WORLD , mr );
	return 0;
}

/*for worker*/
int __topaz_worker_try_receive_task(topaz_info * t, task_message * th, MPI_Request * mr){
	//TODO: topaz->machine.main_proc var instead of is_main mem var
	MPI_Irecv(th->data, sizeof(task_message) + t->tasks.max_input_buffer, MPI_CHAR, 0, TASK_ID, MPI_COMM_WORLD , mr );
	return 0;
}
void __topaz_worker_process_task(task_message * th){
	task_message v = *((task_message *) th->data);
	th->task_id = v.task_id;
	th->task_rank = v.task_rank;
}

int __topaz_worker_try_receive_data_body(topaz_info * t, char * buffer, int n, topaz_ptr elem, MPI_Request * mr){
	//TODO: topaz->machine.main_proc var instead of is_main mem var
	MPI_Irecv(buffer, n, MPI_CHAR, 0, DATA_BODY_ID + elem, MPI_COMM_WORLD , mr );
	return 0;
}
void __topaz_worker_try_receive_kill_signal(topaz_info * t, char * c,MPI_Request * mr){
	MPI_Irecv(c, 1, MPI_CHAR, 0, KILL_PROC_ID, MPI_COMM_WORLD , mr );
}
void __topaz_worker_send_task_response(topaz_info * t, task_message *th){
	int itask_id = (int) th->task_id;
	#ifdef TIME_PROGRAM
	WCommStart();
	#endif
	#ifdef DEBUG
	printf("[Worker] Sending Task Response\n");
	#endif
	MPI_Send(th->data, sizeof(task_message) + t->tasks.task_list[itask_id].obuf_siz, MPI_CHAR, 0, RET_TASK_ID, MPI_COMM_WORLD);
	#ifdef DEBUG
	printf("[Worker] Sent Task Response\n");
	#endif
	#ifdef TIME_PROGRAM
	WCommStop();
	#endif
}

