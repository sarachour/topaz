#include "topaz.h"
#include "runner.h"

#include "stdio.h"
#include "string.h"

#include "pin_util.h"

int main(int argc, char ** argv){
	
BREAK1:
	Topaz::topaz = new Topaz(argstp,  argv);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // rank is place index
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
	
}
