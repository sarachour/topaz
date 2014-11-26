#include <sys/time.h>
#include <instrument.h>
#include "stdlib.h"
#include "stdio.h"
inst_timer create_timer(){
	inst_timer t;
	t.elapsed.tv_sec = t.total.tv_sec = 0;
	t.elapsed.tv_usec = t.total.tv_usec = 0;
	return t;
}

void start_timer(inst_timer * t){
	gettimeofday(&t->INTERNAL_TIMER, NULL);
}
void end_timer(inst_timer * t){
	const int SEC = 1000000;
	struct timeval prev = t->INTERNAL_TIMER;
	struct timeval next;
	gettimeofday(&t->INTERNAL_TIMER, NULL);
	next = t->INTERNAL_TIMER;
	
	if(next.tv_usec < prev.tv_usec){
		int nsec = (prev.tv_usec - next.tv_usec) / SEC + 1;
		prev.tv_usec -= SEC * nsec;
		prev.tv_sec += nsec;
	}
	if(next.tv_usec - prev.tv_usec > SEC){
		int nsec = (next.tv_usec - prev.tv_usec)/SEC;
		prev.tv_usec += SEC*nsec;
		prev.tv_sec -= nsec;
	}
	t->elapsed.tv_sec = next.tv_sec - prev.tv_sec;
	t->elapsed.tv_usec = next.tv_usec - prev.tv_usec;
	t->total.tv_sec += t->elapsed.tv_sec;
	t->total.tv_usec += t->elapsed.tv_usec;
	if(t->total.tv_usec > SEC){
		t->total.tv_sec += t->total.tv_usec/SEC;
		t->total.tv_usec = t->total.tv_usec%SEC;
	}
}

void print_timer(inst_timer * t){
	const float SEC = 1000000.0;
	float elapsed = t->elapsed.tv_sec + t->elapsed.tv_usec/SEC;
	float total = t->total.tv_sec + t->total.tv_usec/SEC;
	printf("elapsed: %f sec\ntotal: %f sec\n", elapsed, total);
}
