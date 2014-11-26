#define EXIT_BEFORE_PRINT_STAT_OPT 2
#define EXIT_BEFORE_PRINT_STAT_OPT 2
#define EXIT_BEFORE_PRINT_STAT_OPT 2
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/param.h>
#if defined(PROF_STAT_OPT)
#include <cmplrs/mon.h>
#endif

int num_print_reset_stat = 0;

extern "C" void 
print_stat() {
  num_print_reset_stat++;
}

extern "C" void 
reset_stat() {
  num_print_reset_stat++;
}
