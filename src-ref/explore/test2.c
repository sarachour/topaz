#include "mpi.h"
#include "stdio.h"
/* topaz runtime
 * tasks_out
 * task_map (id -> fxn ptrs)
 * -> each task id maps to: task fxn, unpack fxn, pack fxn
 * 
 */
/* task_header
 * task_id 
 * task_index
 */
int main(int argc, char ** argv){
	int rank, size;
	char hostname[MPI_MAX_PROCESSOR_NAME];
	int len;
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // rank is place index
	MPI_Comm_size(MPI_COMM_WORLD, &size); // size is total number of places.
	MPI_Get_processor_name(hostname, &len);
	
	
	if(rank == 0){
		MPI_Status status;
		printf("[MAIN] Rank: %s::[%d], Size: %d\n", hostname, rank, size);
		int array[20];
		int i=0;
		for(i=0; i < 20; i++){
			array[i] = i*5;
			printf("%d ", array[i]);
		}
		printf("\n");
		
		int task_id = 1;
		for(i=0; i < 20; i++){
			MPI_Send(&array[i], 1 /*count*/, MPI_INT, i%(size-1)+1 /*dest*/, task_id/*tag*/, MPI_COMM_WORLD /*communication handle*/ );
		}
		for(i=0; i < 20; i++){
			/*Irecv: Non-blocking recieve*/
			MPI_Recv(&array[i], 1 /*count*/, MPI_INT, MPI_ANY_SOURCE /*dest*/, task_id /*tag*/, MPI_COMM_WORLD /*communication handle*/, &status );
			printf("%d ", array[i]);
		}
		printf("\n");
	}
	else{
		MPI_Status status;
		printf("[WORKER] Rank: %s::[%d], Size: %d\n", hostname, rank, size);
		int num;
		int task_id = 1;
		while(1){
			MPI_Recv(&num, 1 /*count*/, MPI_INT, 0 /*dest*/, task_id /*tag*/, MPI_COMM_WORLD /*communication handle*/, &status );
			num = num-1;
			MPI_Send(&num, 1 /*count*/, MPI_INT, 0 /*dest*/, task_id/*tag*/, MPI_COMM_WORLD /*communication handle*/ );
			
		}
	}
	MPI_Finalize();
	return 0;

}
