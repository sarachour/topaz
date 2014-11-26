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

#undef SPEEDSHOP_OPT

extern "C" void 
print_stat() {
#if defined(SPEEDSHOP_OPT)
  ssrt_caliper_point(2*num_print_reset_stat+1);
#endif
  num_print_reset_stat++;
#if defined(SPEEDSHOP_OPT)
  ssrt_caliper_point(2*num_print_reset_stat);
#endif
}

extern "C" void 
reset_stat() {
#if defined(SPEEDSHOP_OPT)
  ssrt_caliper_point(2*num_print_reset_stat+1);
#endif
  num_print_reset_stat++;
#if defined(SPEEDSHOP_OPT)
  ssrt_caliper_point(2*num_print_reset_stat);
#endif
}
