#include "topaz.h"
#include "runner.h"

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

#include "pin_util.h"

int main(int argc, char ** argv){
	int npages = 5;
	SharedMemory shared_buffer("shared.dat", 4*1024*npages);
	Buffer<int> test(1);
	
	pid_t pid = fork();
	if(pid == -1){
		panic("fork failed");
	}
	else if(pid == 0){
		printf("hello from the child process\n");
		
	}
	else {
		printf("hello from the parent process\n");
		
	}
/*
	Topaz::topaz = new Topaz(argstp,  argv);
	int rank;
	pin_bind_model(rank);
	argc -= argstp;
	argv += argstp;
	if(Topaz::topaz->isMain()){
		central_routine(argc, argv);
	}
	else{
		worker_routine(argc,argv);
	}
	
	MPI_Finalize();
	return ret;
*/

}
