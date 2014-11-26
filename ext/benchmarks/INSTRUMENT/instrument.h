#ifndef INSTRUMENT_H
#define INSTRUMENT_H
#include "sys/time.h"

typedef struct {
	struct timeval INTERNAL_TIMER;
	struct timeval elapsed;
	struct timeval total;
} inst_timer;

#ifdef __cplusplus 
extern "C" inst_timer create_timer();
extern "C" void start_timer(inst_timer * t);
extern "C" void end_timer(inst_timer * t);
extern "C" void print_timer(inst_timer * t);
#else
inst_timer create_timer();
void start_timer(inst_timer * t);
void end_timer(inst_timer * t);
void print_timer(inst_timer * t);
#endif
#endif
