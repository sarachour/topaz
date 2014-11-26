#include "scheduler.h"
#include "stdlib.h"
#include "const.h"

void scheduler_init_place(place_info_t * p,int ntask){
	p->tasks = (cost_info_t *) malloc(sizeof(cost_info_t)*ntask);
	p->ntasks = ntask;
}
void scheduler_init(scheduler_t * sc, int nplaces, int ntasks){
	int i;
	sc->places = (place_info_t*) malloc(nplaces*sizeof(place_info_t));
	for(i=0; i < nplaces; i++){
		sc->places[i].running = 0;
		sc->places[i].disabled = false;
		scheduler_init_place(&sc->places[i], ntasks);
	}
	sc->nplaces = nplaces;
	sc->ntasks =ntasks;
}
void scheduler_disable_place(scheduler_t * sc, int placeidx){
	sc->places[placeidx].disabled = true;
}
void scheduler_destroy(scheduler_t * sc){
	int i;
	for(i=0; i < sc->nplaces; i++){
		scheduler_destroy_place(&sc->places[i]);
	}
	free(sc->places);
}
void scheduler_destroy_place(place_info_t * p){
	free(p->tasks);
}

void scheduler_set_cost(scheduler_t * sc,int place, int task, int cost){
	sc->places[place].tasks[task].cost = cost;
}

int __schedule_workload_balance(scheduler_t * sc){
	int i=0;
	int min_running=-1;
	int min_running_idx=-1;
	for(i=0; i < sc->nplaces; i++){
		place_info_t * p = &sc->places[i];
		if(p->disabled == false){
			min_running = min_running < 0 || p->running < min_running? 
							p->running : min_running;
			min_running_idx = min_running < 0 || p->running < min_running? 
							i : min_running_idx;
		}
	}
	sc->places[min_running_idx].running++;
	return min_running_idx;
}
int scheduler_get_place(scheduler_t * sc, int taskid){
	return __schedule_workload_balance(sc);
}
