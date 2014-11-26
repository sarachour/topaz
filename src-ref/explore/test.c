#include "mpi.h"
#include "stdio.h"


int main(int argc, char ** argv){
	int rank, size;
	char hostname[MPI_MAX_PROCESSOR_NAME];
	int len;
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // rank is place index
	MPI_Comm_size(MPI_COMM_WORLD, &size); // size is total number of places.
	MPI_Get_processor_name(hostname, &len);
	
	if(rank == 0){
		char ibuf[1];
		char obuf[2];
		int i=0;
		MPI_Status status;
		printf("[MAIN] Rank: %s::[%d], Size: %d\n", hostname, rank, size);
		for(i=1; i < size; i++){
			ibuf[0] = 'a' + i-1;
			MPI_Send(ibuf, 1 /*count*/, MPI_CHAR, i /*dest*/, 0 /*tag*/, MPI_COMM_WORLD /*communication handle*/ );
		}
		obuf[1] = '\0';
		for(i=1; i < size; i++){
			MPI_Recv(obuf, 1 /*count*/, MPI_CHAR, i /*dest*/, 0 /*tag*/, MPI_COMM_WORLD /*communication handle*/, &status );
			printf(">> %s\n", obuf);
		}
	}
	else{
		char ibuf[2];
		char obuf[1];
		MPI_Status status;
		printf("[WORKER] Rank: %s::[%d], Size: %d\n", hostname, rank, size);
		ibuf[1] = '\0';
		MPI_Recv(ibuf, 1 /*count*/, MPI_CHAR, 0 /*src*/, 0 /*tag*/, MPI_COMM_WORLD /*communication handle*/, &status);
		printf("> %s\n", ibuf);
		obuf[0] = ibuf[0] + ('A' - 'a');
		MPI_Send(obuf, 1 /*count*/, MPI_CHAR, 0 /*src*/, 0 /*tag*/, MPI_COMM_WORLD /*communication handle*/);
	}
	MPI_Finalize();
	return 0;

}
