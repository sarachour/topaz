#include "sys/time.h"               // for gettimeofday()
#include "timer.h"
void timer_init(timer * t){
	t->cu_msecs = 0;
	t->msecs = 0;
}
void timer_start(timer * t){
	gettimeofday(&t->INTERNAL_start, 0);
}
void timer_end(timer * t){
	double elapsedTime;
	gettimeofday(&t->INTERNAL_end, 0);
	elapsedTime = (t->INTERNAL_end.tv_sec - t->INTERNAL_start.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t->INTERNAL_end.tv_usec - t->INTERNAL_start.tv_usec) / 1000.0;   // us to ms
    t->cu_msecs += elapsedTime;
    t->msecs = elapsedTime;
}
float timer_get_msecs(timer * t){
	return t->msecs;
}
float timer_get_cu_msecs(timer * t){
	return t->cu_msecs;
}

/*
int main()
{
    timeval t1, t2;
    double elapsedTime;

    // start timer
    gettimeofday(&t1, NULL);

    // do something
    // ...

    // stop timer
    gettimeofday(&t2, NULL);

    // compute and print the elapsed time in millisec
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    cout << elapsedTime << " ms.\n";

    return 0;
}
*/
