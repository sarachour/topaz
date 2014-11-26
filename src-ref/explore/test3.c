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
#define IN_TASK_SIZE (sizeof(int))
#define OUT_TASK_SIZE (sizeof(int))

typedef struct TOPAZ_RUNTIME_T{
	int n_running_procs;
} topaz_runtime;
typedef struct IN_TASK_INFO_T{
	int task_id;
	int task_rank;
	int data[IN_TASK_SIZE];
} in_task_info;

typedef struct OUT_TASK_INFO_T{
	int task_id;
	int task_rank;
	int data[OUT_TASK_SIZE];
} out_task_info;


int main(int argc, char ** argv){
	int rank, size;
	char hostname[MPI_MAX_PROCESSOR_NAME];
	int len;
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // rank is place index
	MPI_Comm_size(MPI_COMM_WORLD, &size); // size is total number of places.
	MPI_Get_processor_name(hostname, &len);
	
	topaz_runtime topaz;
	in_task_info input;
	out_task_info output;
	
	topaz.n_running_procs = 0;
	
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
			int p;
			p = array[i];
			input.task_id = 2;
			input.task_rank = i;
			input.data[0] = p;
			MPI_Send(&input, 2+1 /*count*/, MPI_INT, i%(size-1)+1 /*dest*/, task_id/*tag*/, MPI_COMM_WORLD /*communication handle*/ );
			topaz.n_running_procs++;
		}
		for(i=0; i < 20; i++){
			/*Irecv: Non-blocking recieve*/
			int p;
			MPI_Recv(&output, 2+1 /*count*/, MPI_INT, MPI_ANY_SOURCE /*dest*/, task_id /*tag*/, MPI_COMM_WORLD /*communication handle*/, &status );
			topaz.n_running_procs--;
			array[output.task_rank] = output.data[0];
		}
		for(i=0; i < 20; i++){
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
			MPI_Recv(&input, 2+1 /*count*/, MPI_INT, 0 /*dest*/, task_id /*tag*/, MPI_COMM_WORLD /*communication handle*/, &status );
			output.task_rank = input.task_rank;
			output.task_id = input.task_id;
			if(input.task_id == 1){
				output.data[0] = input.data[0]-1;
			}
			else if(input.task_id == 2){
				output.data[0] = input.data[0]/2;
			}
			MPI_Send(&output, 2+1 /*count*/, MPI_INT, 0 /*dest*/, task_id/*tag*/, MPI_COMM_WORLD /*communication handle*/ );
			
		}
	}
	MPI_Finalize();
	return 0;

}
