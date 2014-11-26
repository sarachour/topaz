#include "topaz.h"
#include "const.h"
#include "mpi.h"
#include "stdio.h"
#include "task.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"
#ifdef INJECT_FAULTS
#include "fault.h"
#endif
#ifdef TIME_PROGRAM
#include "timeutil.h"
#endif
#include <signal.h>
#include <sys/syscall.h>
#include <setjmp.h>

int curr_task_id;
int curr_task_rank;
topaz_info * topaz;

#define SIGCNT 4
int SIGNAL_LIST[SIGCNT] = {SIGSEGV, SIGFPE, SIGSYS, SIGILL};

struct sigaction worker_sig_action;

static jmp_buf  context;
void exception_handler(int sig, struct siginfo *info, void *uap){
  struct sigaction backup_sig_action;
  #ifdef DEBUG
  psignal(sig, "[WORKER] caught error");
  #endif
  __topaz_worker_send_failed_task_response(topaz, curr_task_id, curr_task_rank);
  sigrelse(sig);
  /*
  if(sigaction(SIGSEGV, &worker_sig_action, NULL)==-1){
    perror("[WORKER] error message:");
    printf("[WORKER] failed to reattach signal");
  }
  */
  longjmp(context, 1);
}
void bind_signals(){
  int i;
  struct sigaction sa;
  sa.sa_handler = (__sighandler_t) exception_handler;
  sigemptyset(&sa.sa_mask);
  //sa.sa_flags = SA_RESTART;
  sa.sa_flags = SA_RESTART | SA_SIGINFO;
  worker_sig_action = sa;
  for(i=0; i < NSIG; i++){
    sigaction(SIGNAL_LIST[i], &worker_sig_action, NULL);
  }


}


typedef struct MPI_POLLING_DATA{
  MPI_Status status;
  int flag;
  MPI_Request req;
} mpi_polling_unit;

void crash_state(){
  //char restart_signal;
  //mpi_polling_unit restart_signal_meta;
  while(1){
    sleep(1);
  }
}

int __PIN_INSTRUMENT_HARDWARE_FAULT(int index){
  return index;
}

void __topaz_worker_jump_handler();
inline void __topaz_worker_invoke_task(task_message * in, task_message * out){
  int task_id = in->task_id;
  int task_rank = in->task_rank;
  fxnptr task_fxn = topaz_get_task(topaz, task_id);
  curr_task_id = task_id;
  curr_task_rank = task_rank;
  task_fxn(topaz, in, out);
  //rank and id should not change.
  out->task_rank = task_rank;
  out->task_id = task_id;
}
#ifdef INJECT_FAULTS
inline void worker_state(fault_info * finfo){
#else
inline void worker_state(){
#endif
  data_message data_header;
  task_message in_task;
  task_message out_task;
  char kill_signal;
  mpi_polling_unit data_header_meta;
  mpi_polling_unit data_body_meta[TOPAZ_DATA_HEADER_NELEMS];
  mpi_polling_unit task_meta;
  mpi_polling_unit kill_signal_meta;

  task_init_task(&out_task, topaz->tasks.max_output_buffer);
  task_init_task(&in_task, topaz->tasks.max_input_buffer);

  __topaz_worker_try_receive_data_header(topaz, &data_header, &data_header_meta.req);
  __topaz_worker_try_receive_task(topaz, &in_task, &task_meta.req);
  __topaz_worker_try_receive_kill_signal(topaz, &kill_signal, &kill_signal_meta.req);

  if(setjmp(context)!=0){
    /*
    printf("[WORKER] <jump>\n");
    int * deref = NULL;
    int boo = *deref;
    printf("[WORKER] </jump>\n");
    */
    goto CATCH_TASK_FAIL_LABEL;
  }
  while(1){
    #ifdef INJECT_FAULTS
    if(fault_run_time_fail(finfo) == true){
      fprintf(stderr, "[WORKER] !! WORKER CRASH\n");
      crash_state();
    }
    #endif

    MPI_Test(&data_header_meta.req, &data_header_meta.flag, &data_header_meta.status);
    MPI_Test(&task_meta.req, &task_meta.flag, &task_meta.status);
    MPI_Test(&kill_signal_meta.req, &kill_signal_meta.flag, &kill_signal_meta.status);
    if(data_header_meta.flag){
      #ifdef DEBUG
      fprintf(stderr, "[WORKER] Receiving data header [%d].\n", data_header.nelems);
      #endif
      int i,reqs_pending=0;
      for(i=0; i < data_header.nelems; i++){
        int siz = data_header.elems[i].size;
        topaz_ptr ptr = data_header.elems[i].ptr;
        void * dat;
        if(topaz_deref(topaz, ptr) == 0){
          dat = malloc(siz);
          topaz_memstore(topaz, ptr, dat, siz);
        }
        else if(topaz_memsize(topaz, ptr) < siz){
          topaz_free(topaz, ptr);
          dat = malloc(siz);
          topaz_memstore(topaz, ptr, dat, siz);
        }
        else{
          dat = topaz_deref(topaz, ptr);
        }
        //setup recieve object
        #ifdef DEBUG
        fprintf(stderr, "[WORKER] Creating local data ptr [%d] (%d)\n", ptr, siz);
        #endif
        __topaz_worker_try_receive_data_body(topaz, (char *) dat, siz, ptr, &data_body_meta[i].req);
        reqs_pending++;
      }
      #ifdef DEBUG
      fprintf(stderr, "[WORKER] Waiting for data body.\n");
      #endif
      while(reqs_pending > 0){
        reqs_pending=0;
        for(i=0; i < data_header.nelems; i++){
          if(data_body_meta[i].req != NULL){
            MPI_Test(&data_body_meta[i].req, &data_body_meta[i].flag, &data_body_meta[i].status);
            if(data_body_meta[i].flag){
              data_body_meta[i].req = NULL;
              #ifdef DEBUG
              fprintf(stderr, "[WORKER] Received data chunk [%d].\n", data_header.elems[i].ptr);
              #endif
            }
            else
              reqs_pending++;
          }
        }
      }
      __topaz_worker_try_receive_data_header(topaz, &data_header, &data_header_meta.req);
    }
    if(task_meta.flag){
      __topaz_worker_process_task(&in_task);
      #ifdef DEBUG
      fprintf(stderr, "[WORKER] Recieved Task[%d], Rank[%d].\n", in_task.task_id, in_task.task_rank);
      #endif
      #ifdef TIME_PROGRAM
      topaz->tasks.task_list[in_task.task_id].n_calls++;
      WKernelStart();
      #endif
      __topaz_worker_invoke_task(&in_task, &out_task);
      #ifdef INJECT_FAULTS
      #ifdef DEBUG
      fprintf(stderr, "[WORKER] !! Injecting Faults.\n");
      #endif
      fault_corrupt_task_message(finfo, &out_task, topaz->tasks.max_output_buffer);
      #endif
      #ifdef TIME_PROGRAM
      WKernelStop();
      #endif
      __topaz_worker_send_task_response(topaz, &out_task);
CATCH_TASK_FAIL_LABEL:
			__topaz_worker_try_receive_task(topaz, &in_task, &task_meta.req);
			
		}
		
		if(kill_signal_meta.flag){
			#ifdef DEBUG
			fprintf(stderr, "[WORKER] Recieved exit signal. Exiting worker...\n");
			#endif
			MPI_Cancel(&task_meta.req);
			MPI_Cancel(&data_header_meta.req);
			//MPI_Request_free(&kill_signal_meta.req);
			MPI_Wait(&data_header_meta.req, &data_header_meta.status);
			MPI_Wait(&task_meta.req, &task_meta.status);
			MPI_Wait(&kill_signal_meta.req, &kill_signal_meta.status);
			break;
		}
		//sleep(1);
	}

	#ifdef DEBUG
	printf("[WORKER] Finishing\n");
	#endif
	MPI_Finalize();
	#ifdef DEBUG
	printf("[WORKER] Finished\n");
	#endif

}


int main(int argc, char ** argv){
  topaz_info dtopaz;
  int retval;
  topaz = &dtopaz;
  MPI_Init(&argc, &argv);

  init_topaz_info(&dtopaz, argc, argv);
  #ifdef INJECT_FAULTS
  fault_info finfo;
  fault_init_fm(&finfo);
  #endif

  #ifdef DEBUG
  fprintf(stderr, "PROCESSOR: %s: %d/%d, main:%c, pid:%d\n",
    dtopaz.machine.hostname,
    dtopaz.machine.rank, dtopaz.machine.size,
    dtopaz.machine.main_proc == dtopaz.machine.rank ? 'y' : 'n',
    getpid());
  #endif
  if(dtopaz.machine.rank==dtopaz.machine.main_proc){
    __PIN_INSTRUMENT_HARDWARE_FAULT(dtopaz.machine.rank);
    topaz_main_init(&dtopaz);
    #ifdef TIME_PROGRAM
    MainStart();
    #endif
    retval = topaz_main(&dtopaz, argc, argv);
    #ifdef TIME_PROGRAM
    MainStop();
    #endif
    int i;
	for(i=0; i < dtopaz.tasks.size; i++){
      printf("Task %d/%d: %d true calls, %d re-executions\n", i, dtopaz.tasks.size, 
		dtopaz.tasks.task_list[i].n_calls, 
		dtopaz.tasks.task_list[i].n_rexec_calls);
    }
    printf("finishing...\n");
    topaz_main_finish(&dtopaz);
    topaz_finish(&dtopaz);
    MPI_Finalize();
  }
  else{
    __PIN_INSTRUMENT_HARDWARE_FAULT(dtopaz.machine.rank);
    bind_signals();
    __topaz_worker_init();
    topaz_worker(&dtopaz, argc, argv);
    __topaz_worker_destroy();
    #ifdef TIME_PROGRAM
    WorkerStart();
    #endif
    #ifdef INJECT_FAULTS
    worker_state(&&finfo);
    #else
    worker_state();
    #endif
    #ifdef TIME_PROGRAM
    WorkerStop(dtopaz.machine.rank);
    int i;
    for(i=0; i < dtopaz.tasks.size; i++){
      printf("Task %d/%d: %d calls\n", i, dtopaz.tasks.size, dtopaz.tasks.task_list[i].n_calls);
    }
    #endif
    destroy_topaz_info(&dtopaz);

  }
	return retval;
}

