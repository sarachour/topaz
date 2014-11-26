#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/param.h>

#define GRANULARITY_TIME 1000
#define GRANULARITY_MASK 0xfffff
#define TICKS_PER_SECOND_TIME GRANULARITY_TIME
 
extern "C" void get_ticks_time(int *ret) {
  *ret = TICKS_PER_SECOND_TIME;
}

extern "C" void get_time(int *ret) 
{
  double t;
  struct timeval _tp;
  struct timezone _tzp;
  gettimeofday(&_tp,&_tzp);
  t = _tp.tv_sec & GRANULARITY_MASK;
  t = t*GRANULARITY_TIME;
  t += (_tp.tv_usec) / (1000000 / GRANULARITY_TIME);
  *ret = (int)t;
}

extern "C" double square(double v1, double v2){
  return v1 * v2;
}


/*
double  sign(double a, double b){
  if(b < 0) {
    if(a < 0)
      return a;
    return -a;
  } else {
    if(a < 0)
      return -a;
    return a; 
  }
}
*/
/*
#include <sys/sysmp.h>
 
void
pin(int p) {
  int np, tp;
  np = sysmp(MP_NPROCS);
  tp = np - p - 1;
  sysmp(MP_MUSTRUN, tp);
}
*/
