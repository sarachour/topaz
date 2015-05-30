#include "topaz.h"
#include "runner.h"
#include "signal.h"

#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "mpi.h"
#include "pin_util.h"

#include <setjmp.h>
struct sigaction continue_exec;
sigjmp_buf sig_escape;

void main_loop(){
	while(1){
			if(sigsetjmp(sig_escape, 1)){
				Topaz::topaz->output_task->setFailed();
				Topaz::topaz->output_task->startPack();
			}
			//printf("[%d] executed. Returning data.\n", rank);
			Topaz::topaz->send();
			//get the task
			//printf("[%d] Waiting for data.\n", rank);
			Topaz::topaz->receive();
			//printf("[%d] Recieved data. Executing.\n", rank);
			Topaz::topaz->execute();
	}
}

void segfault_sigaction(int signal, siginfo_t * si, void * arg){
	PIN_STOP_INJECT_ERRORS();
	siglongjmp(sig_escape,1);
}

int worker_routine(int argc, char ** argv){
	int ret = 0;
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	topaz_init(argc, argv);
	topaz_worker(argc, argv);
	
	
	memset(&continue_exec, 0, sizeof(struct sigaction));
	sigemptyset(&continue_exec.sa_mask);
	continue_exec.sa_sigaction = segfault_sigaction;
	continue_exec.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &continue_exec, NULL);
	sigaction(SIGABRT, &continue_exec, NULL);
	sigaction(SIGBUS, &continue_exec, NULL);
	Topaz::topaz->getTimers()->start(MAIN_TIMER); // start whole computation time
	Topaz::topaz->receive();
	Topaz::topaz->execute();
	main_loop();
	topaz_finish(argc, argv);
	return ret;
}
